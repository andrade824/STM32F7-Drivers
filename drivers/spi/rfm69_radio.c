/**
 * @author Devon Andrade
 * @created 12/26/2019
 *
 * Driver to control an RFM69 radio module.
 *
 * https://cdn-shop.adafruit.com/product-files/3076/RFM69HCW-V1.1.pdf
 */
#include "config.h"
#include "debug.h"
#include "gpio.h"
#include "spi.h"
#include "spi/rfm69_radio.h"
#include "spi/rfm69_radio_reg.h"
#include "system_timer.h"

#include <stdbool.h>
#include <stdint.h>

/* Maximum amount of time to wait when switching radio modes. */
#define RFM69_MODE_CHANGE_TIMEOUT MSECS(50)

/**
 * Return a pointer to the SPI instance used by this rfm69 instance. This is
 * useful for setting a software managed slave select pin or reconfiguring the
 * underlying SPI module between transactions when multiple devices are using
 * the same SPI interface.
 *
 * @param inst The rfm69 instance to extract the SPI instance from.
 *
 * @return The SPI instance used by the passed in rfm69 instance.
 */
SpiInst* rfm69_get_spi_inst(Rfm69Inst *inst)
{
	ASSERT(inst != NULL);

	return &inst->spi;
}

/**
 * Write to an RFM69 register.
 *
 * @param inst The rfm69 instance to wite to.
 * @param addr The register address.
 * @param data The byte to write to the register.
 */
static void rfm69_write_reg(Rfm69Inst *inst, uint8_t addr, uint8_t data)
{
	ASSERT(inst != NULL);

	spi_enable(&inst->spi);

	/* Send address byte with MSB set to indicate write-mode. */
	spi_send_receive(&inst->spi, addr | 0x80);
	spi_send_receive(&inst->spi, data);

	spi_disable(&inst->spi);
}

/**
 * Read from an RFM69 register.
 *
 * @param inst The rfm69 instance to read from.
 * @param addr The register address to read from.
 *
 * @return The data read from the register.
 */
static uint8_t rfm69_read_reg(Rfm69Inst *inst, uint8_t addr)
{
	ASSERT(inst != NULL);

	spi_enable(&inst->spi);

	/* Send address byte with MSB cleared to indicate read-mode. */
	spi_send_receive(&inst->spi, addr & 0x7F);
	uint8_t data = (uint8_t)spi_send_receive(&inst->spi, 0);

	spi_disable(&inst->spi);

	return data;
}

/**
 * Initialize the radio with some default settings.
 *
 * @note Before usage, the GPIOs for the SPI module (alternate function), the
 *       NSS line (output), and the RST (output) line will need to be setup.
 *
 * @note Software management of the slave select line is always enabled. This is
 *       due to the fact that the radio module SPI runs at up to 10MHz, which is
 *       fast enough that the slave select line might not have enough time to
 *       float high between accesses (the SPI module drives the NSS as open-drain).
 *       By managing the slave select in software, it can be driven push-pull
 *       always ensuring the correct value is on the bus.
 *
 * @note If using an RFM69HW/RFM69HCW module then rfm69_set_power_mode() MUST be
 *       called with a mode that uses PA1 or PA1/PA2 otherwise the transmitter
 *       won't work.
 *
 * Default Transmitter Settings:
 * - Bitrate: 115107bps
 * - Fdev: 115173Hz
 * - RF Carrier Frequency: 915MHz
 * - Transmit Power: 13dBm, PA0 enabled (won't work on RFM69HW/RFM69HCW)
 * - Power Amp Ramp Time: 40us
 * - Over Current Protection: 95mA
 *
 * Default Receiver Settings:
 * - Receiver Bandwidth: 200Khz
 * - DC Offset Canceller: 7957.75Hz
 * - Continuous Digital AGC: Enabled
 *
 * Default Packet Settings:
 * - Preamble bytes: 3
 * - Sync Words: 2
 * -- Fixed prefix byte of 0x37, and default network ID of 0xAA
 * - Fixed Length packet size
 * -- Default length of two bytes.
 * - CRC enabled
 * - No address filtering
 * - InterPacketRxDelay: 35us
 * -- Restart RX 35us after the FIFO is cleared.
 * - No encryption
 *
 * @param inst The rfm69 instance to initialize. There should be one instance
 *             per physical radio module.
 * @param spi_reg The underlying SPI module the display module is connected to.
 * @param rst_reg GPIO register used for the reset line.
 * @param rst_pin GPIO pin to be used as the reset line.
 * @param nss_reg GPIO register used for the slave select line.
 * @param nss_pin GPIO pin to be used as the slave select line.
 */
void rfm69_init_radio(
	Rfm69Inst *inst,
	SpiReg *spi_reg,
	GpioReg *rst_reg,
	GpioPin rst_pin,
	GpioReg *nss_reg,
	GpioPin nss_pin)
{
	ASSERT(inst != NULL);
	ASSERT(spi_reg != NULL);
	ASSERT(rst_reg != NULL);
	ASSERT(nss_reg != NULL);

	/* Toggle the reset line before writing registers.  */
#define RFM69_RESET_HOLD (USECS(100))
#define RFM69_RESET_WAIT (MSECS(5))
	gpio_set_output(rst_reg, rst_pin, GPIO_HIGH);
	sleep(RFM69_RESET_HOLD);
	gpio_set_output(rst_reg, rst_pin, GPIO_LOW);
	sleep(RFM69_RESET_WAIT);

	/* SPI2/3 are on APB1, all other SPI modules are on APB2. */
	const uint32_t pclk = spi_get_periph_clock(spi_reg);
	const SpiBaudRateDiv div = (pclk == APB1_HZ) ? SPI_BR_DIV_8 : SPI_BR_DIV_16;

	/* Enforce that the RFM69 serial clock is 10MHz or less. */
	ASSERT((pclk / (1 << (div + 1))) <= 10000000U);

	spi_init(&inst->spi, spi_reg, SPI_CPHA_0, SPI_CPOL_0, div, SPI_MSBFIRST, SPI_DS_8BIT);
	spi_use_software_ss(&inst->spi, nss_reg, nss_pin);

	inst->last_rssi = 0;
	inst->mode = RF_MODE_STANDBY;

	/* Ensure we can communicate with the radio module correctly. */
	ASSERT(rfm69_read_reg(inst, REG_VERSION) == RFM69_DEFAULT_VERSION);

	/* Let the radio auto-sequence between modes and disable listening mode. */
	rfm69_write_reg(inst, REG_OP_MODE,
		SET_RF_OPMODE_SEQUENCEROFF(RF_SEQUENCER_ON) |
		SET_RF_OPMODE_LISTENON(RF_LISTEN_OFF) |
		SET_RF_OPMODE_MODE(RF_MODE_STANDBY)
	);

	/* Packet mode, FSK modulation with no shaping. */
	rfm69_write_reg(inst, REG_DATA_MODUL,
		SET_RF_DATAMODUL_DATAMODE(RF_DATA_MODE_PACKET) |
		SET_RF_DATAMODUL_MODULATIONTYPE(RF_MODUL_TYPE_FSK) |
		SET_RF_DATAMODUL_MODULATIONSHAPING(RF_FSK_SHAPING_NONE)
	);

	/* Set default bitrate to 115107.9bps. */
#define RFM69_DEFAULT_BITRATE_MSB 0x1
#define RFM69_DEFAULT_BITRATE_LSB 0x16
	rfm69_write_reg(inst, REG_BITRATE_MSB, RFM69_DEFAULT_BITRATE_MSB);
	rfm69_write_reg(inst, REG_BITRATE_LSB, RFM69_DEFAULT_BITRATE_LSB);

	/* Set default frequency deviation to 115173Hz. */
#define RFM69_DEFAULT_FDEV_MSB 0x7
#define RFM69_DEFAULT_FDEV_LSB 0x5F
	rfm69_write_reg(inst, REG_FDEV_MSB, RFM69_DEFAULT_FDEV_MSB);
	rfm69_write_reg(inst, REG_FDEV_LSB, RFM69_DEFAULT_FDEV_LSB);

	/* Set default RF Carrier frequency to 915MHz. */
#define RFM69_DEFAULT_FRF_MSB 0xE4
#define RFM69_DEFAULT_FRF_MID 0xC0
#define RFM69_DEFAULT_FRF_LSB 0x00
	rfm69_write_reg(inst, REG_FRF_MSB, RFM69_DEFAULT_FRF_MSB);
	rfm69_write_reg(inst, REG_FRF_MID, RFM69_DEFAULT_FRF_MID);
	rfm69_write_reg(inst, REG_FRF_LSB, RFM69_DEFAULT_FRF_LSB);

	/**
	 * Set power range to be from -18dBm to +13dBm, and default power to 13dBm
	 * on PA0. If using an RFM69HW/RFM69HCW then rfm69_set_power_mode() MUST be
	 * called with a mode that uses PA1 or PA1/PA2 otherwise the transmitter
	 * won't work.
	 */
#define RFM69_DEFAULT_OUTPUT_POWER 31
	rfm69_write_reg(inst, REG_PA_LEVEL,
		SET_RF_PALEVEL_PA0ON(1) |
		SET_RF_PALEVEL_PA1ON(0) |
		SET_RF_PALEVEL_PA2ON(0) |
		SET_RF_PALEVEL_OUTPUTPOWER(RFM69_DEFAULT_OUTPUT_POWER)
	);

	/* Set default power amplifier ramp time to 40us. The InterPacketRxDelay must be close to this. */
#define RFM69_DEFAULT_PARAMP RF_PARAMP_40_US
	rfm69_write_reg(inst, REG_PA_RAMP, SET_RF_PARAMP_PARAMP(RFM69_DEFAULT_PARAMP));

	/* Set default over current protection to 95mA (radio uses 45mA while transmitting in +13dBm mode). */
#define RFM69_DFEAULT_OCPTRIM 10 /* Imax = 45 + 5*OCPTRIM */
	rfm69_write_reg(inst, REG_OCP,
		SET_RF_OCP_OCPON(RF_OCP_ON) |
		SET_RF_OCP_OCPTRIM(RFM69_DFEAULT_OCPTRIM)
	);

	/* Set receiver to automatically adjust the gain. */
	rfm69_write_reg(inst, REG_LNA,
		SET_RF_LNA_LNAZIN(RF_LNA_ZIN_200_OHMS) |
		SET_RF_LNA_LNAGAINSELECT(RF_LNA_GAIN_AGC)
	);

	/* Set receiver bandwidth to 200khz by default. */
#define RFM69_DEFAULT_DCC_FREQ 2 /* 7957.75, supposed to be 4% of RXBW, aka, 8000 */
#define RFM69_DEFAULT_RXBW_MANT RF_RXBW_MANTISSA_20
#define RFM69_DEFAULT_RXBW_EXP 1
	rfm69_write_reg(inst, REG_RX_BW,
		SET_RF_RXBW_DCCFREQ(RFM69_DEFAULT_DCC_FREQ) |
		SET_RF_RXBW_RXBWMANT(RFM69_DEFAULT_RXBW_MANT) |
		SET_RF_RXBW_RXBWEXP(RFM69_DEFAULT_RXBW_EXP)
	);

	/* Disable CLKOUT feature for power savings. */
	rfm69_write_reg(inst, REG_DIO_MAPPING_2, SET_RF_DIO2_CLKOUT(RF_CLKOUT_OFF));

	/* Set default RSSI Threshold for triggering the Rssi interrupt. */
#define RFM69_DEFAULT_RSSI_THRESH 0xE4 /* -RssiThreshold / 2 dBm = 228dBm */
	rfm69_write_reg(inst, REG_RSSI_THRESH, RFM69_DEFAULT_RSSI_THRESH);

	/* Set default preamble bytes to three (the receiver uses these to synchronize the clock). */
#define RFM69_DEFAULT_PREAMBLE_MSB 0x0
#define RFM69_DEFAULT_PREAMBLE_LSB 0x3
	rfm69_write_reg(inst, REG_PREAMBLE_MSB, RFM69_DEFAULT_PREAMBLE_MSB);
	rfm69_write_reg(inst, REG_PREAMBLE_LSB, RFM69_DEFAULT_PREAMBLE_LSB);

	/* Set size of sync word generation to 2 (a prefix byte, and a user-supplied network ID). */
#define RFM69_DEFAULT_SYNC_SIZE 1 /* Real sync size is this value + 1, so 2. */
	rfm69_write_reg(inst, REG_SYNC_CONFIG,
		SET_RF_SYNCCONFIG_SYNC_ON(RF_SYNC_ON) |
		SET_RF_SYNCCONFIG_FIFO_FILL_COND(RF_FIFO_COND_SYNC_ADDR) |
		SET_RF_SYNCCONFIG_SYNC_SIZE(RFM69_DEFAULT_SYNC_SIZE) |
		SET_RF_SYNCCONFIG_SYNC_TOL(0)
	);

	/* Set default Sync words (fixed prefix and user-changeable ID). */
#define RFM69_DEFAULT_SYNC_PREFIX 0x37
#define RFM69_DEFAULT_SYNC_ID 0xAA
	rfm69_write_reg(inst, REG_SYNC_VALUE_1, RFM69_DEFAULT_SYNC_PREFIX);
	rfm69_write_reg(inst, REG_SYNC_VALUE_2, RFM69_DEFAULT_SYNC_ID);

	/* Set default packet configuration (fixed length, no dc-free, CRC enabled, no address filtering). */
	rfm69_write_reg(inst, REG_PACKET_CONFIG_1,
		SET_RF_PACKET1_PACKET_FORMAT(RF_FIXED_LENGTH) |
		SET_RF_PACKET1_DC_FREE(RF_DCFREE_NONE) |
		SET_RF_PACKET1_CRC_ON(RF_CRC_ON) |
		SET_RF_PACKET1_CRC_AUTO_CLEAR_OFF(RF_CRC_AUTO_CLEAR_FIFO) |
		SET_RF_PACKET1_ADDRESS_FILTERING(RF_ADDR_FILT_NONE));

	/* Set default packet length to two bytes. */
#define RFM69_DEFAULT_PAYLOAD_LENGTH 2
	inst->payload_length = RFM69_DEFAULT_PAYLOAD_LENGTH;
	rfm69_write_reg(inst, REG_PAYLOAD_LENGTH, RFM69_DEFAULT_PAYLOAD_LENGTH);

	/* Start packet transmission after the first byte hits the FIFO. */
	rfm69_write_reg(inst, REG_FIFO_THRESH,
		SET_RF_FIFOTHRESH_TX_START_CONDITION(RF_TXSTART_FIFONOTEMPTY) |
		SET_RF_FIFOTHRESH_FIFO_THRESHOLD(0xF));

	/* Restart RX 35us after FIFO empty, no encryption. */
#define RFM69_DEFAULT_INTERPACKET_RX_DELAY 2 /* 35uS delay, should match PA ramp time */
	rfm69_write_reg(inst, REG_PACKET_CONFIG_2,
		SET_RF_PACKET2_INTER_PACKET_RX_DELAY(RFM69_DEFAULT_INTERPACKET_RX_DELAY) |
		SET_RF_PACKET2_AUTO_RX_RESTART_ON(RF_AUTO_RX_RESTART_ON) |
		SET_RF_PACKET2_AES_ON(RF_AES_OFF));

	/* Enable Continuous Digital Automatic Gain Control (DAGC). */
	rfm69_write_reg(inst, REG_TEST_DAGC, RF_TEST_DAGC_LOW_BETA_0);

	/* The IrqFlags and FIFO are cleared out when FifoOverrun is written to. */
	rfm69_write_reg(inst, REG_IRQ_FLAGS_2, SET_RF_IRQ2_FIFO_OVERRUN(1));

	/* Wait for ModeReady to confirm the radio has entered STANDBY mode. */
	ABORT_TIMEOUT(
		GET_RF_IRQ1_MODE_READY(rfm69_read_reg(inst, REG_IRQ_FLAGS_1)) == 1,
		RFM69_MODE_CHANGE_TIMEOUT);
}

/**
 * Set the maximum payload length for both TX and RX. When using variable length
 * packets (length == 0), then the maximum RX payload length defaults to the
 * maximum payload length.
 *
 * @note For fixed length payloads, both the transmitter and receiver need to be
 *       configured with the same payload length.
 *
 * @param inst The radio instance to update.
 * @param length 0 for variable length, >0 for fixed length payloads.
 */
void rfm69_set_payload_length(Rfm69Inst *inst, uint8_t length)
{
	ASSERT(inst != NULL);
	ASSERT(length <= RFM69_MAX_PAYLOAD_LEN);

	inst->payload_length = length;

	uint8_t packet_config = rfm69_read_reg(inst, REG_PACKET_CONFIG_1) & ~RF_PACKET1_PACKET_FORMAT();

	if(length == 0) {
		/* Set packet config to variable length. */
		packet_config |= RF_PACKET1_PACKET_FORMAT();

		/**
		 * The payload_length register has no effect on variable length TX, but
		 * defines max RX length when receiving variable length packets.
		 */
		length = RFM69_MAX_PAYLOAD_LEN;
	}

	rfm69_write_reg(inst, REG_PACKET_CONFIG_1, packet_config);
	rfm69_write_reg(inst, REG_PAYLOAD_LENGTH, length);
}

/**
 * Set the power mode and level of the radio module. This has a direct correlation
 * on the current consumption of the transmitter.
 *
 * @note If using an RFM69HW/RFM69HCW module then this function MUST be called
 *       with a mode that uses PA1 or PA1/PA2 otherwise the transmitter won't
 *       work. PA0 should only be used on RFM69W modules.
 *
 * @param inst The radio instance to update.
 * @param mode Which power amplifiers to enable in the radio. PA0 is only
 *             available on RFM69W modules. Possible modes:
 *             RFM69_PA0:           -18dBm to +13dBm on RFM69W
 *             RFM69_PA1:           -2dBm to +13dBm on RFM69HW/RFM69HCW
 *             RFM69_PA1_PA2:       +2dBm to +17dBm on RFM69HW/RFM69HCW
 *             RFM69_PA1_PA2_BOOST: +5dBm to +20dBm on RFM69HW/RFM69HCW
 * @param level The power level to transmit at. The available levels depends on
 *              mode:
 *              RFM69_PA0:           0 (-18dBm) to 31 (+13dBm)
 *              RFM69_PA1:           16 (-2dBm) to 31 (+13dBm)
 *              RFM69_PA1_PA2:       16 (+2dBm) to 31 (+17dBm)
 *              RFM69_PA1_PA2_BOOST: 16 (+5dBm) to 31 (+20dBm)
 */
void rfm69_set_power_mode(Rfm69Inst *inst, Rfm69PowerMode mode, uint8_t level)
{
	ASSERT(inst != NULL);
	ASSERT(((mode == RFM69_PA0) && (level <= 31)) ||
	       ((mode != RFM69_PA0) && (level >= 16 && level <= 31)));

	/**
	 * Over current protection has to be disabled when operating in >=18dBm, but
	 * disable it in any of the higher power modes so it doesn't get tripped.
	 */
	if((mode == RFM69_PA1_PA2_BOOST) || (mode == RFM69_PA1_PA2)) {
		rfm69_write_reg(inst, REG_OCP, SET_RF_OCP_OCPON(RF_OCP_OFF) | SET_RF_OCP_OCPTRIM(0xF));
	}

	const uint8_t enable_pa0 = (mode == RFM69_PA0) ? 1 : 0;
	const uint8_t enable_pa1 = (mode != RFM69_PA0) ? 1 : 0;
	const uint8_t enable_pa2 = ((mode != RFM69_PA0) && (mode != RFM69_PA1)) ? 1 : 0;
	rfm69_write_reg(inst, REG_PA_LEVEL,
		SET_RF_PALEVEL_PA0ON(enable_pa0) |
		SET_RF_PALEVEL_PA1ON(enable_pa1) |
		SET_RF_PALEVEL_PA2ON(enable_pa2) |
		SET_RF_PALEVEL_OUTPUTPOWER(level)
	);
}

/**
 * Enable or disable the power boost when the power mode is RFM69_PA1_PA2_BOOST
 * (which allows the transmitter to transmit at levels above 17dBm).
 *
 * The power boost must be disabled during RX operation.
 *
 * @param inst The radio instance to power boost.
 * @param enabled Whether to enable the power boost.
 */
static void rfm69_set_power_boost(Rfm69Inst *inst, bool enabled)
{
	rfm69_write_reg(inst, REG_TEST_PA_1, (enabled) ? RF_TEST_PA1_20DBM : RF_TEST_PA1_NORMAL);
	rfm69_write_reg(inst, REG_TEST_PA_2, (enabled) ? RF_TEST_PA2_20DBM : RF_TEST_PA2_NORMAL);
}

/**
 * Switch the radio module to a different mode and wait for the sequencing
 * between modes to finish.
 *
 * @param mode The new mode to switch to.
 */
static void rfm69_switch_mode(Rfm69Inst *inst, Rfm69Mode mode)
{
	ASSERT(inst != NULL);

	if(inst->mode == mode) {
		return;
	}

	/* Power boost (>17dBm output) must be disabled during RX and enabled during TX. */
	if((inst->power_mode == RFM69_PA1_PA2_BOOST) && (mode == RF_MODE_RX)) {
		rfm69_set_power_boost(inst, false);
	} else if((inst->power_mode == RFM69_PA1_PA2_BOOST) && (mode == RF_MODE_TX)) {
		rfm69_set_power_boost(inst, true);
	}

	const uint8_t opmode = rfm69_read_reg(inst, REG_OP_MODE) & ~RF_OPMODE_MODE();
	rfm69_write_reg(inst, REG_OP_MODE, opmode | SET_RF_OPMODE_MODE(mode));

	/* Wait for ModeReady to confirm the radio has entered the new mode. */
	ABORT_TIMEOUT(
		GET_RF_IRQ1_MODE_READY(rfm69_read_reg(inst, REG_IRQ_FLAGS_1)) == 1,
		RFM69_MODE_CHANGE_TIMEOUT);

	inst->mode = mode;
}

/**
 * Send a packet of data over the radio. After the packet is sent, the radio
 * will be in transmit mode.
 *
 * @param inst The radio instance to transit over.
 * @param data The data to send.
 * @param length The length of the payload. If the radio was configured with a
 *               fixed-length payload, then this length must match that. Otherwise,
 *               this is the length sent for variable-length payloads.
 */
void rfm69_send(Rfm69Inst *inst, uint8_t *data, uint8_t length)
{
	ASSERT(inst != NULL);
	ASSERT(data != NULL);
	ASSERT(((inst->payload_length == 0) && (length > 0)) || (inst->payload_length == length));

	if((inst->mode != RF_MODE_STANDBY) && (inst->mode != RF_MODE_TX)) {
		rfm69_switch_mode(inst, RF_MODE_STANDBY);
	}

	/* Fill up the radio's FIFO while in standby or TX mode. */
	spi_enable(&inst->spi);
	spi_send_receive(&inst->spi, REG_FIFO | 0x80);

	/**
	 * In variable length packet mode, the first byte placed into the FIFO is
	 * the length of the payload (not including the length byte).
	 */
	if(inst->payload_length == 0) {
		spi_send_receive(&inst->spi, length);
	}

	for(int i = 0; i < length; ++i) {
		spi_send_receive(&inst->spi, data[i]);
	}
	spi_disable(&inst->spi);

	/* Once the radio is switched to transmit, it will start sending data. */
	if(inst->mode != RF_MODE_TX) {
		rfm69_switch_mode(inst, RF_MODE_TX);
	}

	/* Wait for the packet to send. */
#define RFM69_TRANSMIT_TIMEOUT MSECS(2)
	ABORT_TIMEOUT(
		GET_RF_IRQ2_PACKET_SENT(rfm69_read_reg(inst, REG_IRQ_FLAGS_2)) == 1,
		RFM69_TRANSMIT_TIMEOUT);
}

/**
 * Wait for a packet to be received and return the payload. After the packet is
 * received, the radio wil be in standby mode.
 *
 * @note For variable length payloads, the first byte is always the length of
 *       the length of the payload (not including the length byte).
 *
 * @param inst The radio to receive the packet over.
 * @param data A buffer to fill with the received payload.
 * @param length The maximum amount of bytes to read from the radio.
 *
 * @return The number of bytes read from the radio.
 */
uint8_t rfm69_receive(Rfm69Inst *inst, uint8_t *data, uint8_t length)
{
	ASSERT(inst != NULL);
	ASSERT(data != NULL);

	rfm69_switch_mode(inst, RF_MODE_RX);

	/* Wait for a packet to be received. */
	while(GET_RF_IRQ2_PAYLOAD_READY(rfm69_read_reg(inst, REG_IRQ_FLAGS_2)) == 0);

	rfm69_switch_mode(inst, RF_MODE_STANDBY);

	/* Read the data from the FIFO. */
	uint8_t num_read = 0;
	while(GET_RF_IRQ2_FIFO_NOT_EMPTY(rfm69_read_reg(inst, REG_IRQ_FLAGS_2))) {
		if(num_read < length) {
			data[num_read++] = rfm69_read_reg(inst, REG_FIFO);
		} else {
			/* Clear the FIFO of any outstanding data. */
			rfm69_read_reg(inst, REG_FIFO);
		}
	}

	inst->last_rssi = -(rfm69_read_reg(inst, REG_RSSI_VALUE)) / 2;

	return num_read;
}

/**
 * The RSSI value at the time of the last packet reception.
 *
 * @param inst The radio instance to read the RSSI from.
 *
 * @return The last received RSSI value or 0 if no packets have been received.
 */
int16_t rfm69_get_last_rssi(Rfm69Inst *inst)
{
	ASSERT(inst != NULL);

	return inst->last_rssi;
}

void rfm69_dump_regs(__attribute__((unused)) Rfm69Inst *inst)
{
	dbprintf("REG_OP_MODE: 0x%x\n", rfm69_read_reg(inst, REG_OP_MODE));
	dbprintf("REG_DATA_MODUL: 0x%x\n", rfm69_read_reg(inst, REG_DATA_MODUL));
	dbprintf("REG_BITRATE_MSB: 0x%x\n", rfm69_read_reg(inst, REG_BITRATE_MSB));
	dbprintf("REG_BITRATE_LSB: 0x%x\n", rfm69_read_reg(inst, REG_BITRATE_LSB));
	dbprintf("REG_FDEV_MSB: 0x%x\n", rfm69_read_reg(inst, REG_FDEV_MSB));
	dbprintf("REG_FDEV_LSB: 0x%x\n", rfm69_read_reg(inst, REG_FDEV_LSB));
	dbprintf("REG_FRF_MSB: 0x%x\n", rfm69_read_reg(inst, REG_FRF_MSB));
	dbprintf("REG_FRF_MID: 0x%x\n", rfm69_read_reg(inst, REG_FRF_MID));
	dbprintf("REG_FRF_LSB: 0x%x\n", rfm69_read_reg(inst, REG_FRF_LSB));
	dbprintf("REG_OSC1: 0x%x\n", rfm69_read_reg(inst, REG_OSC1));
	dbprintf("REG_AFC_CTRL: 0x%x\n", rfm69_read_reg(inst, REG_AFC_CTRL));
	dbprintf("REG_LISTEN1: 0x%x\n", rfm69_read_reg(inst, REG_LISTEN1));
	dbprintf("REG_LISTEN2: 0x%x\n", rfm69_read_reg(inst, REG_LISTEN2));
	dbprintf("REG_LISTEN3: 0x%x\n", rfm69_read_reg(inst, REG_LISTEN3));
	dbprintf("REG_VERSION: 0x%x\n", rfm69_read_reg(inst, REG_VERSION));
	dbprintf("REG_PA_LEVEL: 0x%x\n", rfm69_read_reg(inst, REG_PA_LEVEL));
	dbprintf("REG_PA_RAMP: 0x%x\n", rfm69_read_reg(inst, REG_PA_RAMP));
	dbprintf("REG_OCP: 0x%x\n", rfm69_read_reg(inst, REG_OCP));
	dbprintf("REG_LNA: 0x%x\n", rfm69_read_reg(inst, REG_LNA));
	dbprintf("REG_RX_BW: 0x%x\n", rfm69_read_reg(inst, REG_RX_BW));
	dbprintf("REG_RSSI_CONFIG: 0x%x\n", rfm69_read_reg(inst, REG_RSSI_CONFIG));
	dbprintf("REG_RSSI_VALUE: 0x%x\n", rfm69_read_reg(inst, REG_RSSI_VALUE));
	dbprintf("REG_DIO_MAPPING_1: 0x%x\n", rfm69_read_reg(inst, REG_DIO_MAPPING_1));
	dbprintf("REG_DIO_MAPPING_2: 0x%x\n", rfm69_read_reg(inst, REG_DIO_MAPPING_2));
	dbprintf("REG_IRQ_FLAGS_1: 0x%x\n", rfm69_read_reg(inst, REG_IRQ_FLAGS_1));
	dbprintf("REG_IRQ_FLAGS_2: 0x%x\n", rfm69_read_reg(inst, REG_IRQ_FLAGS_2));
	dbprintf("REG_RSSI_THRESH: 0x%x\n", rfm69_read_reg(inst, REG_RSSI_THRESH));
	dbprintf("REG_TIMEOUT_RX_START: 0x%x\n", rfm69_read_reg(inst, REG_TIMEOUT_RX_START));
	dbprintf("REG_TIMEOUT_RSSI_THRESH: 0x%x\n", rfm69_read_reg(inst, REG_TIMEOUT_RSSI_THRESH));
	dbprintf("REG_PREAMBLE_MSB: 0x%x\n", rfm69_read_reg(inst, REG_PREAMBLE_MSB));
	dbprintf("REG_PREAMBLE_LSB: 0x%x\n", rfm69_read_reg(inst, REG_PREAMBLE_LSB));
	dbprintf("REG_SYNC_CONFIG: 0x%x\n", rfm69_read_reg(inst, REG_SYNC_CONFIG));
	dbprintf("REG_SYNC_VALUE_1: 0x%x\n", rfm69_read_reg(inst, REG_SYNC_VALUE_1));
	dbprintf("REG_SYNC_VALUE_2: 0x%x\n", rfm69_read_reg(inst, REG_SYNC_VALUE_2));
	dbprintf("REG_SYNC_VALUE_3: 0x%x\n", rfm69_read_reg(inst, REG_SYNC_VALUE_3));
	dbprintf("REG_SYNC_VALUE_4: 0x%x\n", rfm69_read_reg(inst, REG_SYNC_VALUE_4));
	dbprintf("REG_SYNC_VALUE_5: 0x%x\n", rfm69_read_reg(inst, REG_SYNC_VALUE_5));
	dbprintf("REG_SYNC_VALUE_6: 0x%x\n", rfm69_read_reg(inst, REG_SYNC_VALUE_6));
	dbprintf("REG_SYNC_VALUE_7: 0x%x\n", rfm69_read_reg(inst, REG_SYNC_VALUE_7));
	dbprintf("REG_SYNC_VALUE_8: 0x%x\n", rfm69_read_reg(inst, REG_SYNC_VALUE_8));
	dbprintf("REG_PACKET_CONFIG_1: 0x%x\n", rfm69_read_reg(inst, REG_PACKET_CONFIG_1));
	dbprintf("REG_PAYLOAD_LENGTH: 0x%x\n", rfm69_read_reg(inst, REG_PAYLOAD_LENGTH));
	dbprintf("REG_NODE_ADRS: 0x%x\n", rfm69_read_reg(inst, REG_NODE_ADRS));
	dbprintf("REG_BROADCAST_ADRS: 0x%x\n", rfm69_read_reg(inst, REG_BROADCAST_ADRS));
	dbprintf("REG_AUTO_MODES: 0x%x\n", rfm69_read_reg(inst, REG_AUTO_MODES));
	dbprintf("REG_FIFO_THRESH: 0x%x\n", rfm69_read_reg(inst, REG_FIFO_THRESH));
	dbprintf("REG_PACKET_CONFIG_2: 0x%x\n", rfm69_read_reg(inst, REG_PACKET_CONFIG_2));
	dbprintf("REG_TEST_LNA: 0x%x\n", rfm69_read_reg(inst, REG_TEST_LNA));
	dbprintf("REG_TEST_PA_1: 0x%x\n", rfm69_read_reg(inst, REG_TEST_PA_1));
	dbprintf("REG_TEST_PA_2: 0x%x\n", rfm69_read_reg(inst, REG_TEST_PA_2));
	dbprintf("REG_TEST_DAGC: 0x%x\n", rfm69_read_reg(inst, REG_TEST_DAGC));
}
