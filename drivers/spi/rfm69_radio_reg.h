/**
 * @author Devon Andrade
 * @created 12/26/2019
 *
 * Register map for the RFM69 radio module.
 *
 * https://cdn-shop.adafruit.com/product-files/3076/RFM69HCW-V1.1.pdf
 */
#pragma once

#include "bitfield.h"

/****** Common Configuration Register Definitions ******/

/* FIFO Data Register. */
#define REG_FIFO 0x00U

/* Operating Mode Register. */
#define REG_OP_MODE 0x01U
BIT_FIELD2(RF_OPMODE_MODE,         2, 4);
BIT_FIELD2(RF_OPMODE_LISTENABORT,  5, 5);
BIT_FIELD2(RF_OPMODE_LISTENON,     6, 6);
BIT_FIELD2(RF_OPMODE_SEQUENCEROFF, 7, 7);

/* Possible values for RF_OPMODE_MODE. */
typedef enum {
	RF_MODE_SLEEP      = 0,
	RF_MODE_STANDBY    = 1,
	RF_MODE_FREQ_SYNTH = 2,
	RF_MODE_TX         = 3,
	RF_MODE_RX         = 4
} Rfm69Mode;

/* Possible values for RF_OPMODE_LISTENON. */
typedef enum {
	RF_LISTEN_OFF = 0,
	RF_LISTEN_ON  = 1
} Rfm69ListenOn;

/* Possible values for RF_OPMODE_SEQUENCEROFF. */
typedef enum {
	RF_SEQUENCER_ON  = 0, /* Modes are automatically reached with the sequencer. */
	RF_SEQUENCER_OFF = 1 /* Mode is forced by the user. */
} Rfm69SequencerOff;

/* Data Modulation Register. */
#define REG_DATA_MODUL 0x02U
BIT_FIELD2(RF_DATAMODUL_MODULATIONSHAPING, 0, 1);
BIT_FIELD2(RF_DATAMODUL_MODULATIONTYPE,    3, 4);
BIT_FIELD2(RF_DATAMODUL_DATAMODE,          5, 6);

/* Possible values for RF_DATAMODUL_MODULATIONSHAPING when using FSK. */
typedef enum {
	RF_FSK_SHAPING_NONE         = 0,
	RF_FSK_SHAPING_GAUSSIAN_1_0 = 1,
	RF_FSK_SHAPING_GAUSSIAN_0_5 = 2,
	RF_FSK_SHAPING_GAUSSIAN_0_2 = 3
} Rfm69FskModulationShaping;

/* Possible values for RF_DATAMODUL_MODULATIONTYPE. */
typedef enum {
	RF_MODUL_TYPE_FSK = 0, /* Frequency Shift Keying */
	RF_MODUL_TYPE_OOK = 1  /* On-Off Keying */
} Rfm69ModulationType;

/* Possible values for RF_DATAMODUL_DATAMODE. */
typedef enum {
	RF_DATA_MODE_PACKET          = 0, /* Packet Mode */
	RF_DATA_MODE_CONTINUOUS_SYNC = 2, /* Continuous Mode with bit synchronizer */
	RF_DATA_MODE_CONTINUOUS      = 3 /* Continous Mode without bit synchronizer */
} Rfm69DataMode;

/* Bitrate Registers. */
#define REG_BITRATE_MSB 0x03U
#define REG_BITRATE_LSB 0x04U

/* Frequency Deviation (Fdev) Registers. */
#define REG_FDEV_MSB 0x05U
#define REG_FDEV_LSB 0x06U

/* Carrier Frequency (Frf) Registers. */
#define REG_FRF_MSB 0x07U
#define REG_FRF_MID 0x08U
#define REG_FRF_LSB 0x09U

/* Oscillator Control 1 Register. */
#define REG_OSC1 0x0AU

/* Automatic Frequency Control (AFC) Control Register. */
#define REG_AFC_CTRL 0x0BU

/* Listen Registers. */
#define REG_LISTEN1 0x0DU
#define REG_LISTEN2 0x0EU
#define REG_LISTEN3 0x0FU

/* Version Register. */
#define REG_VERSION 0x10U
BIT_FIELD2(RF_VERSION_MASK_REV, 0, 3); /* Metal mask revision number. */
BIT_FIELD2(RF_VERSION_REV,      4, 7); /* Full revision number. */

#define RFM69_DEFAULT_VERSION 0x24 /* Default value of the VERSION register. */

/****** Transmitter Register Definitions ******/

/* Power Amplifier Level Register. */
#define REG_PA_LEVEL 0x11U
BIT_FIELD2(RF_PALEVEL_OUTPUTPOWER, 0, 4);
BIT_FIELD2(RF_PALEVEL_PA2ON, 5, 5);
BIT_FIELD2(RF_PALEVEL_PA1ON, 6, 6);
BIT_FIELD2(RF_PALEVEL_PA0ON, 7, 7);

/* Power Amplifier Ramp Register. */
#define REG_PA_RAMP 0x12U
BIT_FIELD2(RF_PARAMP_PARAMP, 0, 3);

/* Possible transmitter ramp up/down values for RF_PARAMP_PARAMP. */
typedef enum {
	RF_PARAMP_3_4_MS =  0, /* 3.4ms */
	RF_PARAMP_2_MS   =  1, /* 2ms */
	RF_PARAMP_1_MS   =  2, /* 1ms */
	RF_PARAMP_500_US =  3, /* 500us */
	RF_PARAMP_250_US =  4, /* 250us */
	RF_PARAMP_125_US =  5, /* 125us */
	RF_PARAMP_100_US =  6, /* 100us */
	RF_PARAMP_62_US  =  7, /* 62us */
	RF_PARAMP_50_US  =  8, /* 50us */
	RF_PARAMP_40_US  =  9, /* 40us */
	RF_PARAMP_31_US  = 10, /* 31us */
	RF_PARAMP_25_US  = 11, /* 25us */
	RF_PARAMP_20_US  = 12, /* 20us */
	RF_PARAMP_15_US  = 13, /* 15us */
	RF_PARAMP_12_US  = 14, /* 12us */
	RF_PARAMP_10_US  = 15, /* 10us */
} Rfm69PaRampTime;

/* Overload Current Protection (OCP) Register. */
#define REG_OCP 0x13U
BIT_FIELD2(RF_OCP_OCPTRIM, 0, 3); /* Imax = 45 + 5*OCPTRIM mA */
BIT_FIELD2(RF_OCP_OCPON,   4, 4);

/* Possible values for RF_OCP_OCPON. */
typedef enum {
	RF_OCP_OFF = 0,
	RF_OCP_ON  = 1
} Rfm69OcpOn;

/****** Receiver Register Definitions ******/

/* Low-Noise Amplifier (LNA) Register. */
#define REG_LNA 0x18U
BIT_FIELD2(RF_LNA_LNAGAINSELECT,  0, 2);
BIT_FIELD2(RF_LNA_LNACURRENTGAIN, 3, 5);
BIT_FIELD2(RF_LNA_LNAZIN,         7, 7);

/* Possible values for RF_LNA_LNAGAINSELECT. */
typedef enum {
	RF_LNA_GAIN_AGC = 0, /* Automatic Gain Control */
	RF_LNA_GAIN_G1  = 1, /* Max Gain */
	RF_LNA_GAIN_G2  = 2, /* Max Gain - 6dB */
	RF_LNA_GAIN_G3  = 3, /* Max Gain - 12dB */
	RF_LNA_GAIN_G4  = 4, /* Max Gain - 24dB */
	RF_LNA_GAIN_G5  = 5, /* Max Gain - 36dB */
	RF_LNA_GAIN_G6  = 6  /* Max Gain - 48dB */
} Rfm69LnaGainSelect;

/* Possible values for RF_LNA_LNAGAINSELECT. */
typedef enum {
	RF_LNA_ZIN_50_OHMS  = 0,
	RF_LNA_ZIN_200_OHMS = 1
} Rfm69LnaZin;

/* Receiver Bandwidth Register. */
#define REG_RX_BW 0x19U
BIT_FIELD2(RF_RXBW_RXBWEXP,  0, 2);
BIT_FIELD2(RF_RXBW_RXBWMANT, 3, 4);
BIT_FIELD2(RF_RXBW_DCCFREQ,  5, 7);

/* Possible values for RF_RXBW_RXBWMANT. */
typedef enum {
	RF_RXBW_MANTISSA_16 = 0,
	RF_RXBW_MANTISSA_20 = 1,
	RF_RXBW_MANTISSA_24 = 2
} Rfm69RxBwMantissa;

/* Automatic Frequency Correction Bandwidth Register (same fields as REG_RX_BW). */
#define REG_AFC_BW 0x1AU

/* Automatic Frequency Correction FEI Register. */
#define REG_AFC_FEI 0x1EU
BIT_FIELD2(RF_AFC_START,         0, 0);
BIT_FIELD2(RF_AFC_CLEAR,         1, 1);
BIT_FIELD2(RF_AFC_AUTO_ON,       2, 2);
BIT_FIELD2(RF_AFC_AUTO_CLEAR_ON, 3, 3);
BIT_FIELD2(RF_AFC_DONE,          4, 4);
BIT_FIELD2(RF_AFC_FEI_START,     5, 5);
BIT_FIELD2(RF_AFC_FEI_DONE,      6, 6);

/* RSSI Configuration Register. */
#define REG_RSSI_CONFIG 0x23U
BIT_FIELD2(RF_RSSICONFIG_RSSISTART, 0, 0);
BIT_FIELD2(RF_RSSICONFIG_RSSIDONE,  1, 1);

/* RSSI Value Register. */
#define REG_RSSI_VALUE 0x24U

/****** IRQ and Pin Mapping Registers ******/

/* Dio Mapping 1 Register. See Table 22 for mappings in Packet Mode. */
#define REG_DIO_MAPPING_1 0x25U
BIT_FIELD2(RF_DIO1_DIO3MAPPING, 0, 1);
BIT_FIELD2(RF_DIO1_DIO2MAPPING, 2, 3);
BIT_FIELD2(RF_DIO1_DIO1MAPPING, 4, 5);
BIT_FIELD2(RF_DIO1_DIO0MAPPING, 6, 7);

/* Dio Mapping 2 Register. */
#define REG_DIO_MAPPING_2 0x26U
BIT_FIELD2(RF_DIO2_CLKOUT, 0, 2);
BIT_FIELD2(RF_DIO2_DIO5MAPPING, 4, 5);
BIT_FIELD2(RF_DIO2_DIO4MAPPING, 6, 7);

/* Possible clock output values for RF_DIO2_CLKOUT. */
typedef enum {
	RF_CLKOUT_NO_DIV = 0,
	RF_CLKOUT_DIV_2  = 1,
	RF_CLKOUT_DIV_4  = 2,
	RF_CLKOUT_DIV_8  = 3,
	RF_CLKOUT_DIV_16 = 4,
	RF_CLKOUT_DIV_32 = 5,
	RF_CLKOUT_RC     = 6,
	RF_CLKOUT_OFF    = 7
} Rfm69ClkOut;

/* IRQ Flags 1 Register. */
#define REG_IRQ_FLAGS_1 0x27U
BIT_FIELD2(RF_IRQ1_SYNC_ADDRESS_MATCH, 0, 0);
BIT_FIELD2(RF_IRQ1_AUTO_MODE,          1, 1);
BIT_FIELD2(RF_IRQ1_TIMEOUT,            2, 2);
BIT_FIELD2(RF_IRQ1_RSSI,               3, 3);
BIT_FIELD2(RF_IRQ1_PLLLOCK,            4, 4);
BIT_FIELD2(RF_IRQ1_TX_READY,           5, 5);
BIT_FIELD2(RF_IRQ1_RX_READY,           6, 6);
BIT_FIELD2(RF_IRQ1_MODE_READY,         7, 7);

/* IRQ Flags 2 Register. */
#define REG_IRQ_FLAGS_2 0x28U
BIT_FIELD2(RF_IRQ2_CRC_OK,         1, 1);
BIT_FIELD2(RF_IRQ2_PAYLOAD_READY,  2, 2);
BIT_FIELD2(RF_IRQ2_PACKET_SENT,    3, 3);
BIT_FIELD2(RF_IRQ2_FIFO_OVERRUN,   4, 4);
BIT_FIELD2(RF_IRQ2_FIFO_LEVEL,     5, 5);
BIT_FIELD2(RF_IRQ2_FIFO_NOT_EMPTY, 6, 6);
BIT_FIELD2(RF_IRQ2_FIFO_FULL,      7, 7);

/* RSSI Threshold Register. */
#define REG_RSSI_THRESH 0x29U

/* Receive Timeout Start Register. */
#define REG_TIMEOUT_RX_START 0x2AU

/* RSSI Threshold Timeout Register. */
#define REG_TIMEOUT_RSSI_THRESH 0x2BU

/****** Packet Engine Registers ******/

/* Preamble Size Registers. */
#define REG_PREAMBLE_MSB 0x2CU
#define REG_PREAMBLE_LSB 0x2DU

/* Sync Word Config Register. */
#define REG_SYNC_CONFIG 0x2EU
BIT_FIELD2(RF_SYNCCONFIG_SYNC_TOL,       0, 2);
BIT_FIELD2(RF_SYNCCONFIG_SYNC_SIZE,      3, 5);
BIT_FIELD2(RF_SYNCCONFIG_FIFO_FILL_COND, 6, 6);
BIT_FIELD2(RF_SYNCCONFIG_SYNC_ON,        7, 7);

/* Possible values for RF_SYNCCONFIG_FIFO_FILL_COND. */
typedef enum {
	RF_FIFO_COND_SYNC_ADDR = 0, /* If SyncAddress interrupt occurs. */
	RF_FIFO_COND_FIELD_SET = 1  /* As long as the FifoFillCondition field is set. */
} Rfm69FifoFillCondition;

/* Possible values for RF_SYNCCONFIG_SYNC_ON. */
typedef enum {
	RF_SYNC_OFF = 0,
	RF_SYNC_ON  = 1
} Rfm69SyncOn;

/* Sync Value Registers. */
#define REG_SYNC_VALUE_1 0x2FU
#define REG_SYNC_VALUE_2 0x30U
#define REG_SYNC_VALUE_3 0x31U
#define REG_SYNC_VALUE_4 0x32U
#define REG_SYNC_VALUE_5 0x33U
#define REG_SYNC_VALUE_6 0x34U
#define REG_SYNC_VALUE_7 0x35U
#define REG_SYNC_VALUE_8 0x36U

/* Packet Configuration 1 Register. */
#define REG_PACKET_CONFIG_1 0x37U
BIT_FIELD2(RF_PACKET1_ADDRESS_FILTERING,  1, 2);
BIT_FIELD2(RF_PACKET1_CRC_AUTO_CLEAR_OFF, 3, 3);
BIT_FIELD2(RF_PACKET1_CRC_ON,             4, 4);
BIT_FIELD2(RF_PACKET1_DC_FREE,            5, 6);
BIT_FIELD2(RF_PACKET1_PACKET_FORMAT,      7, 7);

/* Possible values for RF_PACKET1_ADDRESS_FILTERING. */
typedef enum {
	RF_ADDR_FILT_NONE              = 0, /* No address filtering. */
	RF_ADDR_FILT_NODE              = 1, /* Address field must match NodeAddress. */
	RF_ADDR_FILT_NODE_OR_BROADCAST = 2  /* Address field must match NodeAddress or BroadcastAddress. */
} Rfm69AddressFiltering;

/* Possible values for RF_PACKET1_CRC_AUTO_CLEAR_OFF. */
typedef enum {
	RF_CRC_AUTO_CLEAR_FIFO       = 0, /* Clear FIFO and restart new packet reception. */
	RF_CRC_AUTO_CLEAR_DONT_CLEAR = 1  /* Do not clear FIFO. */
} Rfm69CrcAutoClearOff;

/* Possible values for RF_PACKET1_CRC_ON. */
typedef enum {
	RF_CRC_OFF = 0,
	RF_CRC_ON  = 1
} Rfm69CrcOn;

/* Possible values for RF_PACKET1_DC_FREE. */
typedef enum {
	RF_DCFREE_NONE       = 0,
	RF_DCFREE_MANCHESTER = 1,
	RF_DCFREE_WHITENING  = 2
} Rfm69DcFree;

/* Possible values for RF_PACKET1_PACKET_FORMAT. */
typedef enum {
	RF_FIXED_LENGTH    = 0,
	RF_VARIABLE_LENGTH = 1
} Rfm69PacketFormat;

/* Payload Length Register. */
#define REG_PAYLOAD_LENGTH 0x38U

/* Node Address Register. */
#define REG_NODE_ADRS 0x39U

/* Broadcast Address Register. */
#define REG_BROADCAST_ADRS 0x3AU

/* Auto Modes Register. */
#define REG_AUTO_MODES 0x3BU
BIT_FIELD2(RF_AUTOMODE_INTERMEDIATE_MODE, 0, 1);
BIT_FIELD2(RF_AUTOMODE_EXIT_CONDITION,    2, 4);
BIT_FIELD2(RF_AUTOMODE_ENTER_CONDITION,   5, 7);

/* Possible values for RF_AUTOMODE_INTERMEDIATE_MODE. */
typedef enum {
	RF_INTERMODE_SLEEP   = 0,
	RF_INTERMODE_STANDBY = 1,
	RF_INTERMODE_RX      = 2,
	RF_INTERMODE_TX      = 3
} Rfm69IntermediateMode;

/* Possible values for RF_AUTOMODE_EXIT_CONDITION. */
typedef enum {
	RF_INTEREXIT_NONE              = 0,
	RF_INTEREXIT_FALL_FIFONOTEMPTY = 1,
	RF_INTEREXIT_RISE_FIFOLEVEL    = 2,
	RF_INTEREXIT_RISE_CRCOK        = 3,
	RF_INTEREXIT_RISE_PAYLOADREADY = 4,
	RF_INTEREXIT_RISE_SYNCADDR     = 5,
	RF_INTEREXIT_RISE_PACKETSENT   = 6,
	RF_INTEREXIT_RISE_TIMEOUT      = 7
} Rfm69InterModeExitCondition;

/* Possible values for RF_AUTOMODE_ENTER_CONDITION. */
typedef enum {
	RF_INTERENTER_NONE              = 0,
	RF_INTERENTER_RISE_FIFONOTEMPTY = 1,
	RF_INTERENTER_RISE_FIFOLEVEL    = 2,
	RF_INTERENTER_RISE_CRCOK        = 3,
	RF_INTERENTER_RISE_PAYLOADREADY = 4,
	RF_INTERENTER_RISE_SYNCADDR     = 5,
	RF_INTERENTER_RISE_PACKETSENT   = 6,
	RF_INTERENTER_FALL_FIFONOTEMPTY = 7
} Rfm69InterModeEnterCondition;

/* FIFO Threshold Register. */
#define REG_FIFO_THRESH 0x3CU
BIT_FIELD2(RF_FIFOTHRESH_FIFO_THRESHOLD,     0, 6);
BIT_FIELD2(RF_FIFOTHRESH_TX_START_CONDITION, 7, 7);

/* Possible values for RF_FIFOTHRESH_TX_START_CONDITION. */
typedef enum {
	RF_TXSTART_FIFOLEVEL    = 0,
	RF_TXSTART_FIFONOTEMPTY = 1
} Rfm69TxStartCondition;

/* Packet Configuration 2 Register. */
#define REG_PACKET_CONFIG_2 0x3DU
BIT_FIELD2(RF_PACKET2_AES_ON,                0, 0);
BIT_FIELD2(RF_PACKET2_AUTO_RX_RESTART_ON,    1, 1);
BIT_FIELD2(RF_PACKET2_RESTART_RX,            2, 2);
BIT_FIELD2(RF_PACKET2_INTER_PACKET_RX_DELAY, 4, 7);

/* Possible values for RF_PACKET2_AES_ON. */
typedef enum {
	RF_AES_OFF = 0,
	RF_AES_ON  = 1
} Rfm69AesOn;

/* Possible values for RF_PACKET2_AUTO_RX_RESTART_ON. */
typedef enum {
	RF_AUTO_RX_RESTART_OFF = 0,
	RF_AUTO_RX_RESTART_ON  = 1
} Rfm69AutoRxRestartOn;

/* AES Key Registers. */
#define REG_AES_KEY_1  0x3EU
#define REG_AES_KEY_2  0x3FU
#define REG_AES_KEY_3  0x40U
#define REG_AES_KEY_4  0x41U
#define REG_AES_KEY_5  0x42U
#define REG_AES_KEY_6  0x43U
#define REG_AES_KEY_7  0x44U
#define REG_AES_KEY_8  0x45U
#define REG_AES_KEY_9  0x46U
#define REG_AES_KEY_10 0x47U
#define REG_AES_KEY_11 0x48U
#define REG_AES_KEY_12 0x49U
#define REG_AES_KEY_13 0x4AU
#define REG_AES_KEY_14 0x4BU
#define REG_AES_KEY_15 0x4CU
#define REG_AES_KEY_16 0x4DU

/****** Test Registers ******/

/* Test LNA Register. */
#define REG_TEST_LNA 0x58U

/* Possible values for REG_TEST_LNA. */
typedef enum {
	RF_TEST_LNA_NORMAL           = 0x1B,
	RF_TEST_LNA_HIGH_SENSITIVITY = 0x2D
} Rfm69TestLna;

/* Test Power Amplifier 1 Register. */
#define REG_TEST_PA_1 0x5AU

/* Possible values for REG_TEST_PA_1. */
typedef enum {
	RF_TEST_PA1_NORMAL = 0x55,
	RF_TEST_PA1_20DBM  = 0x5D
} Rfm69TestPa1;

/* Test Power Amplifier 2 Register. */
#define REG_TEST_PA_2 0x5CU

/* Possible values for REG_TEST_PA_1. */
typedef enum {
	RF_TEST_PA2_NORMAL = 0x70,
	RF_TEST_PA2_20DBM  = 0x7C
} Rfm69TestPa2;

/* Test Digital Automatic Gain Control (DAGC) Register. */
#define REG_TEST_DAGC 0x6FU

/* Possible values for REG_TEST_DAGC. */
typedef enum {
	RF_TEST_DAGC_NORMAL     = 0x00,
	RF_TEST_DAGC_LOW_BETA_1 = 0x20,
	RF_TEST_DAGC_LOW_BETA_0 = 0x30
} Rfm69TestDagc;
