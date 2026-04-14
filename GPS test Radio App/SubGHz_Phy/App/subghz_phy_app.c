/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    subghz_phy_app.c
  * @author  MCD Application Team
  * @brief   Application of the SubGHz_Phy Middleware
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "platform.h"
#include "sys_app.h"
#include "subghz_phy_app.h"
#include "radio.h"
#include "usart.h"

/* USER CODE BEGIN Includes */
#include "stm32_timer.h"
#include "stm32_seq.h"
#include "utilities_def.h"
#include "app_version.h"
#include "subghz_phy_version.h"
#include "string.h"
#include "stdlib.h"
/* USER CODE END Includes */

/* External variables ---------------------------------------------------------*/
/* USER CODE BEGIN EV */
extern volatile uint16_t gps_rx_len;
extern volatile uint16_t gps_rx_len_ready;
extern volatile uint32_t gps_rx_total_bytes;
extern uint8_t gps_rx_buffer[512];
/* USER CODE END EV */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* Configurations */
/*Timeout*/
#define RX_TIMEOUT_VALUE              3000 //2000
#define TX_TIMEOUT_VALUE              3000
/* Definitions */
#define RX_CONTINUOUS_ON              1
#define RADIO_TX                      0 /* do not change*/
#define RADIO_RX                      1 /* do not change*/
#define PRBS9_INIT                    ( ( uint16_t) 2 )

/* Test Configurations */
/*if TEST_MODE is RADIO_TX, board will send packet indefinitely*/
/*if TEST_MODE is RADIO_RX, board will receive packet indefinitely*/
#define TEST_MODE                     RADIO_RX //JC1-Rx, JC2-Tx

#define GPS_PAYLOAD_ONLY              1
#define GPS_TX_MAX_CHUNK              64
#define GPS_TX_TRACE                  1
#define GPS_BUFFER_TRACE              0
#define GPS_TX_SEND_DELAY_MS          1000

#if (( USE_MODEM_LORA == 1 ) && ( USE_MODEM_FSK == 0 ))
/* 0: Tx Long packet shall be disable when USE_MODEM_LORA*/
#define APP_LONG_PACKET              0
/* Application buffer 255 max when USE_MODEM_LORA */
#define MAX_APP_BUFFER_SIZE              255
#elif (( USE_MODEM_LORA == 0 ) && ( USE_MODEM_FSK == 1 ))
/* 0: Tx Long packet disable*/
/* 1: Tx Long packet enable(payload can be greater than 255bytes. Available on stm32wl revision Y)*/
#define APP_LONG_PACKET               0
/* Application buffer, can be increased further*/
#define MAX_APP_BUFFER_SIZE           1000
#else
#error "Please define a modem in the compiler subghz_phy_app.h."
#endif /* USE_MODEM_LORA | USE_MODEM_FSK */

#if (PAYLOAD_LEN>MAX_APP_BUFFER_SIZE)
#error increase MAX_APP_BUFFER_SIZE
#endif /* (PAYLOAD_LEN>MAX_APP_BUFFER_SIZE) */

#if ((APP_LONG_PACKET==0) && PAYLOAD_LEN>255)
#error in case PAYLOAD_LEN>255, APP_LONG_PACKET shall be defined to 1
#endif /* ((APP_LONG_PACKET==0) && PAYLOAD_LEN>255) */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* Radio events function pointer */
static RadioEvents_t RadioEvents;

/* USER CODE BEGIN PV */
//static uint8_t ch;

static __IO uint32_t RadioTxDone_flag = 0;
static __IO uint32_t RadioTxTimeout_flag = 0;
static __IO uint32_t RadioRxDone_flag = 0;
static __IO uint32_t RadioRxTimeout_flag = 0;
static __IO uint32_t RadioError_flag = 0;
static __IO int16_t last_rx_rssi = 0;
static __IO int8_t last_rx_cfo = 0;

uint8_t data_buffer[MAX_APP_BUFFER_SIZE] UTIL_MEM_ALIGN(4);
uint16_t data_offset = 0;

static __IO uint16_t payloadLen = PAYLOAD_LEN;
#if (TEST_MODE == RADIO_TX)
static uint16_t payloadLenMax = MAX_APP_BUFFER_SIZE;
#endif /* TEST_MODE == RADIO_TX */

#if (( USE_MODEM_LORA == 0 ) && ( USE_MODEM_FSK == 1 ))
static uint8_t syncword[] = { 0xC1, 0x94, 0xC1};
#endif /* USE_MODEM_FSK */

uint32_t count_RxOk = 0;
uint32_t count_RxKo = 0;
uint32_t PER = 0;

static int32_t packetCnt = 0;

/* TxPayloadMode
 * 0: byte Inc e.g payload=0x00, 0x01, ..,payloadLen-1
 * 1: prbs9  */
static __IO uint8_t TxPayloadMode = 0;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/*!
 * @brief Function to be executed on Radio Tx Done event
 */
static void OnTxDone(void);

/**
  * @brief Function to be executed on Radio Rx Done event
  * @param  payload ptr of buffer received
  * @param  size buffer size
  * @param  rssi
  * @param  LoraSnr_FskCfo
  */
static void OnRxDone(uint8_t *payload, uint16_t size, int16_t rssi, int8_t LoraSnr_FskCfo);

/**
  * @brief Function executed on Radio Tx Timeout event
  */
static void OnTxTimeout(void);

/**
  * @brief Function executed on Radio Rx Timeout event
  */
static void OnRxTimeout(void);

/**
  * @brief Function executed on Radio Rx Error event
  */
static void OnRxError(void);

/* USER CODE BEGIN PFP */
/**
  * @brief Packet Error Rate state machine implementation
  */
static void Per_Process(void);

#if (APP_LONG_PACKET != 0)
/**
  * @brief Process next Tx chunk of payload
  * @param buffer
  * @param buffer_size
  */
void TxLongPacketGetNextChunk(uint8_t **buffer, uint8_t buffer_size);

/**
  * @brief Process next Rx chunk of payload
  * @param buffer
  * @param chunk_size
  */
void RxLongPacketChunk(uint8_t *buffer, uint8_t chunk_size);
#endif /* APP_LONG_PACKET != 0 */

#if (TEST_MODE == RADIO_TX)
/**
  * @brief Generates a PRBS9 sequence
  * @retval 0
  */
static int32_t tx_payload_generator(void);

#endif /* TEST_MODE == RADIO_TX */

/* USER CODE END PFP */

/* Exported functions ---------------------------------------------------------*/
void SubghzApp_Init(void)
{
  /* USER CODE BEGIN SubghzApp_Init_1 */


#if  (( USE_MODEM_LORA == 0 ) && ( USE_MODEM_FSK == 1 ) && (TEST_MODE == RADIO_RX))
  RxConfigGeneric_t RxConfig = {0};
#elif (( USE_MODEM_LORA == 0 ) && ( USE_MODEM_FSK == 1 ) && (TEST_MODE == RADIO_TX))
  TxConfigGeneric_t TxConfig;
#else
#endif /* TEST_MODE */
  /* Get SubGHY_Phy APP version*/
  APP_LOG(TS_OFF, VLEVEL_M, "APPLICATION_VERSION: V%X.%X.%X\r\n",
          (uint8_t)(APP_VERSION_MAIN),
          (uint8_t)(APP_VERSION_SUB1),
          (uint8_t)(APP_VERSION_SUB2));

  /* Get MW SubGhz_Phy info */
  APP_LOG(TS_OFF, VLEVEL_M, "MW_RADIO_VERSION:    V%X.%X.%X\r\n",
          (uint8_t)(SUBGHZ_PHY_VERSION_MAIN),
          (uint8_t)(SUBGHZ_PHY_VERSION_SUB1),
          (uint8_t)(SUBGHZ_PHY_VERSION_SUB2));

  APP_LOG(TS_OFF, VLEVEL_M, "---------------\n\r");
#if (( USE_MODEM_LORA == 1 ) && ( USE_MODEM_FSK == 0 ))
  APP_LOG(TS_OFF, VLEVEL_M, "LORA_MODULATION\n\r");
  APP_LOG(TS_OFF, VLEVEL_M, "LORA_BW=%d Hz\n\r", 125000);
#elif (( USE_MODEM_LORA == 0 ) && ( USE_MODEM_FSK == 1 ))
  APP_LOG(TS_OFF, VLEVEL_M, "FSK_MODULATION\n\r");
  APP_LOG(TS_OFF, VLEVEL_M, "FSK_BW=%d Hz\n\r", FSK_BANDWIDTH);
  APP_LOG(TS_OFF, VLEVEL_M, "FSK_DR=%d bits/s\n\r", FSK_DATARATE);
#if (TEST_MODE == RADIO_RX)
  APP_LOG(TS_OFF, VLEVEL_M, "Rx Mode\n\r", FSK_DATARATE);
#elif (TEST_MODE == RADIO_TX)
  APP_LOG(TS_OFF, VLEVEL_M, "Tx Mode\n\r", FSK_DATARATE);
#endif /* TEST_MODE */
#else
#error "Please define a modem in the compiler subghz_phy_app.h."
#endif /* USE_MODEM_LORA | USE_MODEM_FSK */
  /* USER CODE END SubghzApp_Init_1 */

  /* Radio initialization */
  RadioEvents.TxDone = OnTxDone;
  RadioEvents.RxDone = OnRxDone;
  RadioEvents.TxTimeout = OnTxTimeout;
  RadioEvents.RxTimeout = OnRxTimeout;
  RadioEvents.RxError = OnRxError;

  Radio.Init(&RadioEvents);

  /* USER CODE BEGIN SubghzApp_Init_2 */
  /* Radio Set frequency */
  Radio.SetChannel(RF_FREQUENCY);

  data_offset = 0;
#if (TEST_MODE == RADIO_RX)
#if (( USE_MODEM_LORA == 1 ) && ( USE_MODEM_FSK == 0 ))
  APP_TPRINTF("Rx LORA Test\r\n");
  /* RX Continuous */
  Radio.SetRxConfig(MODEM_LORA, LORA_BANDWIDTH, LORA_SPREADING_FACTOR,
                    LORA_CODINGRATE, 0, LORA_PREAMBLE_LENGTH,
                    LORA_SYMBOL_TIMEOUT, LORA_FIX_LENGTH_PAYLOAD_ON,
                    0, true, 0, 0, LORA_IQ_INVERSION_ON, true);

  Radio.SetMaxPayloadLength(MODEM_LORA, MAX_APP_BUFFER_SIZE);
#elif (( USE_MODEM_LORA == 0 ) && ( USE_MODEM_FSK == 1 ))
  APP_TPRINTF("Rx FSK Test\r\n");
  /* RX Continuous */
  RxConfig.fsk.ModulationShaping = RADIO_FSK_MOD_SHAPING_G_BT_05;
  RxConfig.fsk.Bandwidth = FSK_BANDWIDTH;
  RxConfig.fsk.BitRate = FSK_DATARATE; /*BitRate*/
  RxConfig.fsk.PreambleLen = 4; /*in Byte*/
  RxConfig.fsk.SyncWordLength = sizeof(syncword); /*in Byte*/
  RxConfig.fsk.PreambleMinDetect = RADIO_FSK_PREAMBLE_DETECTOR_08_BITS;
  RxConfig.fsk.SyncWord = syncword; /*SyncWord Buffer*/
  RxConfig.fsk.whiteSeed = 0x01FF; /*WhiteningSeed*/
#if (APP_LONG_PACKET==0)
  RxConfig.fsk.LengthMode  = RADIO_FSK_PACKET_VARIABLE_LENGTH; /* legacy: payload length field is 1 byte long*/
#else
  RxConfig.fsk.LengthMode  = RADIO_FSK_PACKET_2BYTES_LENGTH;  /* payload length field is 2 bytes long */
#endif /* APP_LONG_PACKET */
  RxConfig.fsk.CrcLength = RADIO_FSK_CRC_2_BYTES_IBM;       /* Size of the CRC block in the GFSK packet*/
  RxConfig.fsk.CrcPolynomial = 0x8005;
  RxConfig.fsk.CrcSeed = 0xFFFF;
  RxConfig.fsk.Whitening = RADIO_FSK_DC_FREEWHITENING;
  RxConfig.fsk.MaxPayloadLength = MAX_APP_BUFFER_SIZE;
  RxConfig.fsk.StopTimerOnPreambleDetect = 0;
  RxConfig.fsk.AddrComp = RADIO_FSK_ADDRESSCOMP_FILT_OFF;
  if (0UL != Radio.RadioSetRxGenericConfig(GENERIC_FSK, &RxConfig, RX_CONTINUOUS_ON, 0))
  {
    while (1);
  }
#else
#error "Please define a modem in the compiler subghz_phy_app.h."
#endif /* USE_MODEM_LORA | USE_MODEM_FSK */

#if (APP_LONG_PACKET==0)
  Radio.Rx(RX_TIMEOUT_VALUE);
#else
  (void) Radio.ReceiveLongPacket(0, RX_TIMEOUT_VALUE, RxLongPacketChunk);
#endif /* APP_LONG_PACKET */

#elif (TEST_MODE == RADIO_TX)
  tx_payload_generator();

#if (( USE_MODEM_LORA == 1 ) && ( USE_MODEM_FSK == 0 ))
  /*lora modulation*/
  Radio.SetTxConfig(MODEM_LORA, TX_OUTPUT_POWER, 0, LORA_BANDWIDTH,
                    LORA_SPREADING_FACTOR, LORA_CODINGRATE,
                    LORA_PREAMBLE_LENGTH, LORA_FIX_LENGTH_PAYLOAD_ON,
                    true, 0, 0, LORA_IQ_INVERSION_ON, TX_TIMEOUT_VALUE);

  Radio.SetMaxPayloadLength(MODEM_LORA, MAX_APP_BUFFER_SIZE);
#elif (( USE_MODEM_LORA == 0 ) && ( USE_MODEM_FSK == 1 ))
  /*fsk modulation*/
  TxConfig.fsk.ModulationShaping = RADIO_FSK_MOD_SHAPING_G_BT_05;
  TxConfig.fsk.FrequencyDeviation = FSK_FDEV;
  TxConfig.fsk.BitRate = FSK_DATARATE; /*BitRate*/
  TxConfig.fsk.PreambleLen = 4;   /*in Byte        */
  TxConfig.fsk.SyncWordLength = sizeof(syncword); /*in Byte        */
  TxConfig.fsk.SyncWord = syncword; /*SyncWord Buffer*/
  TxConfig.fsk.whiteSeed =  0x01FF; /*WhiteningSeed  */
#if (APP_LONG_PACKET==0)
  TxConfig.fsk.HeaderType  = RADIO_FSK_PACKET_VARIABLE_LENGTH; /*legacy: payload length field is 1 byte long*/
#else
  TxConfig.fsk.HeaderType  = RADIO_FSK_PACKET_2BYTES_LENGTH;  /* payload length field is 2 bytes long */
#endif /* APP_LONG_PACKET */
  TxConfig.fsk.CrcLength = RADIO_FSK_CRC_2_BYTES_IBM;       /* Size of the CRC block in the GFSK packet*/
  TxConfig.fsk.CrcPolynomial = 0x8005;
  TxConfig.fsk.CrcSeed = 0xFFFF;
  TxConfig.fsk.Whitening = RADIO_FSK_DC_FREEWHITENING;
  if (0UL != Radio.RadioSetTxGenericConfig(GENERIC_FSK, &TxConfig, TX_OUTPUT_POWER, TX_TIMEOUT_VALUE))
  {
    while (1);
  }
#else
#error "Please define a modem in the compiler subghz_phy_app.h."
#endif /* USE_MODEM_LORA | USE_MODEM_FSK */

#if (APP_LONG_PACKET==0)
  /* First TX is started by Per_Process after GPS payload is prepared */
#else
  /* First TX is started by Per_Process after GPS payload is prepared */
#endif /* APP_LONG_PACKET */
#else
#error should be either Tx or Rx
#endif /* TEST_MODE */

  /*register task to to be run in while(1) after Radio IT*/
  UTIL_SEQ_RegTask((1 << CFG_SEQ_Task_SubGHz_Phy_App_Process), UTIL_SEQ_RFU, Per_Process);
#if (TEST_MODE == RADIO_TX)
  UTIL_SEQ_SetTask((1 << CFG_SEQ_Task_SubGHz_Phy_App_Process), CFG_SEQ_Prio_0);
#endif
  /* USER CODE END SubghzApp_Init_2 */
}

/* USER CODE BEGIN EF */

/* USER CODE END EF */

/* Private functions ---------------------------------------------------------*/
static void OnTxDone(void)
{
  /* USER CODE BEGIN OnTxDone */
  RadioTxDone_flag = 1;
  UTIL_SEQ_SetTask((1 << CFG_SEQ_Task_SubGHz_Phy_App_Process), CFG_SEQ_Prio_0);
  /* USER CODE END OnTxDone */
}

static void OnRxDone(uint8_t *payload, uint16_t size, int16_t rssi, int8_t LoraSnr_FskCfo)
{
  /* USER CODE BEGIN OnRxDone */
  last_rx_rssi = rssi;
  last_rx_cfo = LoraSnr_FskCfo;

  /* Set Rxdone flag */
  RadioRxDone_flag = 1;
  /* Run Per process in background*/
  UTIL_SEQ_SetTask((1 << CFG_SEQ_Task_SubGHz_Phy_App_Process), CFG_SEQ_Prio_0);
#if (APP_LONG_PACKET==0)
  memcpy(data_buffer, payload, size);
  payloadLen = size;
#else
  /*from chunk*/
  payloadLen = data_offset;
  /*payload data are not relevant in long packet mode*/
#endif /* APP_LONG_PACKET */
  /* USER CODE END OnRxDone */
}

static void OnTxTimeout(void)
{
  /* USER CODE BEGIN OnTxTimeout */
  RadioTxTimeout_flag = 1;
  /* Run process in background*/
  UTIL_SEQ_SetTask((1 << CFG_SEQ_Task_SubGHz_Phy_App_Process), CFG_SEQ_Prio_0);
  /* USER CODE END OnTxTimeout */
}

static void OnRxTimeout(void)
{
  /* USER CODE BEGIN OnRxTimeout */
  RadioRxTimeout_flag = 1;
  /* Run Per process in background*/
  UTIL_SEQ_SetTask((1 << CFG_SEQ_Task_SubGHz_Phy_App_Process), CFG_SEQ_Prio_0);
  /* USER CODE END OnRxTimeout */
}

static void OnRxError(void)
{
  /* USER CODE BEGIN OnRxError */
  RadioError_flag = 1;
  /* Run Per process in background*/
  UTIL_SEQ_SetTask((1 << CFG_SEQ_Task_SubGHz_Phy_App_Process), CFG_SEQ_Prio_0);
  /* USER CODE END OnRxError */
}

/* USER CODE BEGIN PrFD */
#if (APP_LONG_PACKET!=0)
void RxLongPacketChunk(uint8_t *buffer, uint8_t chunk_size)
{
  uint8_t *rxdata = &data_buffer[data_offset];
  uint8_t *rxbuffer = buffer;

  if (data_offset + chunk_size > MAX_APP_BUFFER_SIZE)
  {
    __NOP();
    return;
  }
  for (int32_t i = 0; i < chunk_size; i++)
  {
    *rxdata++ = *rxbuffer++;
  }
  data_offset += chunk_size;
}

void TxLongPacketGetNextChunk(uint8_t **buffer, uint8_t chunk_size)
{
  *buffer = &data_buffer[data_offset];
  data_offset += chunk_size;
  /* APP_TPRINTF("Tx chunk: chunk_size=%d, data_offset=%d\r\n",chunk_size, data_offset); */
}
#endif /* APP_LONG_PACKET */
uint8_t buffer_error = 0;

static int32_t nmea_get_field(const char *nmea, uint8_t field_idx, char *out, uint16_t out_size)
{
  uint8_t idx = 0U;
  const char *p = nmea;
  const char *start;
  const char *end;
  uint16_t len;

  if ((nmea == NULL) || (out == NULL) || (out_size < 2U))
  {
    return -1;
  }

  start = p;
  while ((*p != '\0') && (idx < field_idx))
  {
    if (*p == ',')
    {
      idx++;
      start = p + 1;
    }
    p++;
  }

  if (idx != field_idx)
  {
    return -1;
  }

  end = start;
  while ((*end != '\0') && (*end != ',') && (*end != '*') && (*end != '\r') && (*end != '\n'))
  {
    end++;
  }

  len = (uint16_t)(end - start);
  if (len == 0U)
  {
    return -1;
  }

  if (len >= out_size)
  {
    len = (uint16_t)(out_size - 1U);
  }
  memcpy(out, start, len);
  out[len] = '\0';
  return 0;
}

static int32_t nmea_coord_to_udeg(const char *coord, char hemi, int32_t *udeg)
{
  const char *dot;
  uint8_t pre_dot_len;
  uint8_t deg_digits;
  char deg_buf[4];
  int32_t deg;
  double minutes;
  double decimal;
  int32_t sign = 1;

  if ((coord == NULL) || (udeg == NULL))
  {
    return -1;
  }

  dot = strchr(coord, '.');
  if (dot == NULL)
  {
    return -1;
  }

  pre_dot_len = (uint8_t)(dot - coord);
  if (pre_dot_len < 3U)
  {
    return -1;
  }

  deg_digits = (uint8_t)(pre_dot_len - 2U);
  if ((deg_digits < 2U) || (deg_digits > 3U))
  {
    return -1;
  }

  memcpy(deg_buf, coord, deg_digits);
  deg_buf[deg_digits] = '\0';
  deg = (int32_t)atoi(deg_buf);
  minutes = strtod(coord + deg_digits, NULL);
  decimal = (double)deg + (minutes / 60.0);

  if ((hemi == 'S') || (hemi == 'W'))
  {
    sign = -1;
  }
  else if ((hemi != 'N') && (hemi != 'E'))
  {
    return -1;
  }

  decimal *= (double)sign;
  if (decimal >= 0.0)
  {
    *udeg = (int32_t)((decimal * 1000000.0) + 0.5);
  }
  else
  {
    *udeg = (int32_t)((decimal * 1000000.0) - 0.5);
  }

  return 0;
}

static int32_t nmea_hex_nibble(char c)
{
  if ((c >= '0') && (c <= '9'))
  {
    return (int32_t)(c - '0');
  }
  if ((c >= 'A') && (c <= 'F'))
  {
    return (int32_t)(c - 'A' + 10);
  }
  if ((c >= 'a') && (c <= 'f'))
  {
    return (int32_t)(c - 'a' + 10);
  }
  return -1;
}

static int32_t nmea_checksum_valid(const char *nmea)
{
  const char *p;
  const char *star;
  uint8_t cs = 0U;
  int32_t hi;
  int32_t lo;
  uint8_t rx_cs;

  if (nmea == NULL)
  {
    return 0;
  }

  p = nmea;
  if (*p == '$')
  {
    p++;
  }

  star = strchr(p, '*');
  if ((star == NULL) || (star[1] == '\0') || (star[2] == '\0'))
  {
    return 0;
  }

  while (p < star)
  {
    cs ^= (uint8_t)(*p);
    p++;
  }

  hi = nmea_hex_nibble(star[1]);
  lo = nmea_hex_nibble(star[2]);
  if ((hi < 0) || (lo < 0))
  {
    return 0;
  }

  rx_cs = (uint8_t)((hi << 4) | lo);
  return (cs == rx_cs) ? 1 : 0;
}

static void nmea_get_field_opt(const char *nmea, uint8_t field_idx, char *out, uint16_t out_size)
{
  if ((out == NULL) || (out_size == 0U))
  {
    return;
  }

  if (nmea_get_field(nmea, field_idx, out, out_size) != 0)
  {
    out[0] = '\0';
  }
}

static int32_t nmea_get_sentence_type(const char *nmea, char type3[4])
{
  char id[8];
  const char *msg;
  uint16_t len;

  if ((nmea == NULL) || (type3 == NULL))
  {
    return -1;
  }

  if (nmea_get_field(nmea, 0U, id, sizeof(id)) != 0)
  {
    return -1;
  }

  msg = (id[0] == '$') ? (id + 1) : id;
  len = (uint16_t)strlen(msg);
  if (len < 3U)
  {
    return -1;
  }

  memcpy(type3, &msg[len - 3U], 3U);
  type3[3] = '\0';
  return 0;
}

static int32_t nmea_extract_lat_lon_udeg(const char *nmea, int32_t *lat_udeg, int32_t *lon_udeg);

static void nmea_print_parsed(const char *nmea)
{
  char type3[4];

  if (nmea_get_sentence_type(nmea, type3) != 0)
  {
    APP_TPRINTF("GPS PARSE: unknown sentence\r\n");
    return;
  }

  if (strcmp(type3, "RMC") == 0)
  {
    char time[16], status[4], lat[20], ns[4], lon[20], ew[4], spd[16], cog[16], date[16], mode[8];
    int32_t lat_udeg;
    int32_t lon_udeg;

    nmea_get_field_opt(nmea, 1U, time, sizeof(time));
    nmea_get_field_opt(nmea, 2U, status, sizeof(status));
    nmea_get_field_opt(nmea, 3U, lat, sizeof(lat));
    nmea_get_field_opt(nmea, 4U, ns, sizeof(ns));
    nmea_get_field_opt(nmea, 5U, lon, sizeof(lon));
    nmea_get_field_opt(nmea, 6U, ew, sizeof(ew));
    nmea_get_field_opt(nmea, 7U, spd, sizeof(spd));
    nmea_get_field_opt(nmea, 8U, cog, sizeof(cog));
    nmea_get_field_opt(nmea, 9U, date, sizeof(date));
    nmea_get_field_opt(nmea, 12U, mode, sizeof(mode));

    APP_TPRINTF("RMC: t=%s stat=%s spd_kn=%s cog=%s date=%s mode=%s\r\n", time, status, spd, cog, date, mode);
    if (nmea_extract_lat_lon_udeg(nmea, &lat_udeg, &lon_udeg) == 0)
    {
      APP_TPRINTF("RMC POS UDEG: lat=%d lon=%d\r\n", (int)lat_udeg, (int)lon_udeg);
    }
  }
  else if (strcmp(type3, "GGA") == 0)
  {
    char time[16], fixq[8], sats[8], hdop[16], alt[16], alt_u[4], geoid[16], geoid_u[4];
    int32_t lat_udeg;
    int32_t lon_udeg;

    nmea_get_field_opt(nmea, 1U, time, sizeof(time));
    nmea_get_field_opt(nmea, 6U, fixq, sizeof(fixq));
    nmea_get_field_opt(nmea, 7U, sats, sizeof(sats));
    nmea_get_field_opt(nmea, 8U, hdop, sizeof(hdop));
    nmea_get_field_opt(nmea, 9U, alt, sizeof(alt));
    nmea_get_field_opt(nmea, 10U, alt_u, sizeof(alt_u));
    nmea_get_field_opt(nmea, 11U, geoid, sizeof(geoid));
    nmea_get_field_opt(nmea, 12U, geoid_u, sizeof(geoid_u));

    APP_TPRINTF("GGA: t=%s fix=%s sats=%s hdop=%s alt=%s%s geoid=%s%s\r\n", time, fixq, sats, hdop, alt, alt_u, geoid, geoid_u);
    if (nmea_extract_lat_lon_udeg(nmea, &lat_udeg, &lon_udeg) == 0)
    {
      APP_TPRINTF("GGA POS UDEG: lat=%d lon=%d\r\n", (int)lat_udeg, (int)lon_udeg);
    }
  }
  else if (strcmp(type3, "GLL") == 0)
  {
    char time[16], status[4], mode[8];
    int32_t lat_udeg;
    int32_t lon_udeg;

    nmea_get_field_opt(nmea, 5U, time, sizeof(time));
    nmea_get_field_opt(nmea, 6U, status, sizeof(status));
    nmea_get_field_opt(nmea, 7U, mode, sizeof(mode));

    APP_TPRINTF("GLL: t=%s stat=%s mode=%s\r\n", time, status, mode);
    if (nmea_extract_lat_lon_udeg(nmea, &lat_udeg, &lon_udeg) == 0)
    {
      APP_TPRINTF("GLL POS UDEG: lat=%d lon=%d\r\n", (int)lat_udeg, (int)lon_udeg);
    }
  }
  else if (strcmp(type3, "VTG") == 0)
  {
    char cog_t[16], cog_m[16], spd_kn[16], spd_kmh[16], mode[8];

    nmea_get_field_opt(nmea, 1U, cog_t, sizeof(cog_t));
    nmea_get_field_opt(nmea, 3U, cog_m, sizeof(cog_m));
    nmea_get_field_opt(nmea, 5U, spd_kn, sizeof(spd_kn));
    nmea_get_field_opt(nmea, 7U, spd_kmh, sizeof(spd_kmh));
    nmea_get_field_opt(nmea, 9U, mode, sizeof(mode));

    APP_TPRINTF("VTG: cog_t=%s cog_m=%s spd_kn=%s spd_kmh=%s mode=%s\r\n", cog_t, cog_m, spd_kn, spd_kmh, mode);
  }
  else if (strcmp(type3, "ZDA") == 0)
  {
    char time[16], day[8], month[8], year[8], tzh[8], tzm[8];

    nmea_get_field_opt(nmea, 1U, time, sizeof(time));
    nmea_get_field_opt(nmea, 2U, day, sizeof(day));
    nmea_get_field_opt(nmea, 3U, month, sizeof(month));
    nmea_get_field_opt(nmea, 4U, year, sizeof(year));
    nmea_get_field_opt(nmea, 5U, tzh, sizeof(tzh));
    nmea_get_field_opt(nmea, 6U, tzm, sizeof(tzm));

    APP_TPRINTF("ZDA: t=%s date=%s/%s/%s tz=%s:%s\r\n", time, day, month, year, tzh, tzm);
  }
  else
  {
    APP_TPRINTF("GPS PARSE: type=%s (no detailed parser)\r\n", type3);
  }
}

static int32_t nmea_extract_lat_lon_udeg(const char *nmea, int32_t *lat_udeg, int32_t *lon_udeg)
{
  char id[8];
  char lat[20];
  char lon[20];
  char ns[3];
  char ew[3];
  uint8_t lat_idx;
  uint8_t ns_idx;
  uint8_t lon_idx;
  uint8_t ew_idx;
  const char *msg;

  if ((nmea == NULL) || (lat_udeg == NULL) || (lon_udeg == NULL))
  {
    return -1;
  }

  if (nmea_get_field(nmea, 0U, id, sizeof(id)) != 0)
  {
    return -1;
  }

  msg = (id[0] == '$') ? (id + 1) : id;

  if (strstr(msg, "GGA") != NULL)
  {
    lat_idx = 2U;
    ns_idx = 3U;
    lon_idx = 4U;
    ew_idx = 5U;
  }
  else if (strstr(msg, "RMC") != NULL)
  {
    lat_idx = 3U;
    ns_idx = 4U;
    lon_idx = 5U;
    ew_idx = 6U;
  }
  else if (strstr(msg, "GLL") != NULL)
  {
    lat_idx = 1U;
    ns_idx = 2U;
    lon_idx = 3U;
    ew_idx = 4U;
  }
  else
  {
    return -1;
  }

  if (nmea_get_field(nmea, lat_idx, lat, sizeof(lat)) != 0)
  {
    return -1;
  }
  if (nmea_get_field(nmea, ns_idx, ns, sizeof(ns)) != 0)
  {
    return -1;
  }
  if (nmea_get_field(nmea, lon_idx, lon, sizeof(lon)) != 0)
  {
    return -1;
  }
  if (nmea_get_field(nmea, ew_idx, ew, sizeof(ew)) != 0)
  {
    return -1;
  }

  if (nmea_coord_to_udeg(lat, ns[0], lat_udeg) != 0)
  {
    return -1;
  }
  if (nmea_coord_to_udeg(lon, ew[0], lon_udeg) != 0)
  {
    return -1;
  }

  return 0;
}

static void Per_Process(void)
{
  packetCnt++;
  data_offset = 0;
#if (TEST_MODE == RADIO_RX)
  if (RadioRxDone_flag == 1)
  {
    int16_t rssi = last_rx_rssi;
    int8_t cfo = last_rx_cfo;
   // float lat = 0;
    //float lon = 0;

    HAL_GPIO_WritePin(LED2_GPIO_Port, LED2_Pin, GPIO_PIN_SET); /* LED_GREEN */
    APP_TPRINTF("OnRxDone\r\n");
    APP_TPRINTF("RssiValue=%d dBm, cfo=%d kHz\r\n", rssi, cfo);
    APP_TPRINTF("payloadLen=%d bytes\r\n", payloadLen);
    if (payloadLen > 0U)
    {
      char nmea_text[160];
      uint16_t nmea_len;
      uint16_t show_len = payloadLen;
      if (show_len > 120U)
      {
        show_len = 120U;
      }
      data_buffer[show_len] = '\0';
      APP_TPRINTF("GPS RX: %s\r\n", (char *)data_buffer);

      nmea_len = payloadLen;
      if (nmea_len >= sizeof(nmea_text))
      {
        nmea_len = (uint16_t)(sizeof(nmea_text) - 1U);
      }
      memcpy(nmea_text, data_buffer, nmea_len);
      nmea_text[nmea_len] = '\0';

      if (nmea_checksum_valid(nmea_text) == 1)
      {
        nmea_print_parsed(nmea_text);
      }
      else
      {
        APP_TPRINTF("GPS POS: checksum invalid\r\n");
      }
    }
#if 0
    /* warning, delay between 2 consecutive Tx may be increased to allow DMA to empty printf queue*/
    APP_PPRINTF("data=\r\n");
    for (int32_t i = 0; i < payloadLen; i++)
    {
      APP_PRINTF("%02X", data_buffer[i]);
      if ((i % 16) == 15)
      {
        APP_PPRINTF("\r\n");
      } //payloadLen;
    }
    APP_PPRINTF("\n\r");
#endif /* 0 */
  }
  else
  {
    HAL_GPIO_WritePin(LED3_GPIO_Port, LED3_Pin, GPIO_PIN_SET); /* LED_RED */
  }

  if (RadioRxTimeout_flag == 1)
  {
    APP_TPRINTF("OnRxTimeout\r\n");
  }

  if (RadioError_flag == 1)
  {
    APP_TPRINTF("OnRxError\r\n");
  }

  /*check flag*/
  if ((RadioRxTimeout_flag == 1) || (RadioError_flag == 1))
  {
    count_RxKo++;
  }
  if (RadioRxDone_flag == 1)
  {
    count_RxOk++;
  }
  /* Reset timeout flag */
  RadioRxDone_flag = 0;
  RadioRxTimeout_flag = 0;
  RadioError_flag = 0;

  /* Compute PER */
  PER = (100 * (count_RxKo)) / (count_RxKo + count_RxOk);
  APP_TPRINTF("Rx %d>>> PER= %d %%\r\n", packetCnt, PER);
#if (APP_LONG_PACKET==0)
  Radio.Rx(RX_TIMEOUT_VALUE);
#else
  (void) Radio.ReceiveLongPacket(0, RX_TIMEOUT_VALUE, RxLongPacketChunk);
#endif /* APP_LONG_PACKET */
  HAL_Delay(10);
  HAL_GPIO_WritePin(LED2_GPIO_Port, LED2_Pin, GPIO_PIN_RESET); /* LED_GREEN */
  HAL_GPIO_WritePin(LED3_GPIO_Port, LED3_Pin, GPIO_PIN_RESET); /* LED_RED */
#elif (TEST_MODE == RADIO_TX)
  HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, GPIO_PIN_RESET); /* LED_BLUE */
  if (RadioTxDone_flag == 1)
  {
    APP_TPRINTF("OnTxDone payloadLen=%u\r\n", payloadLen);
  }

   // APP_TPRINTF("OnTxDone\r\n");


  if (RadioTxTimeout_flag == 1)
  {
    APP_TPRINTF("OnTxTimeout\r\n");
  }

  if (RadioError_flag == 1)
  {
    APP_TPRINTF("OnRxError\r\n");
  }
  /* This delay is only to give enough time to allow DMA to empty printf queue*/
  HAL_Delay(GPS_TX_SEND_DELAY_MS);
  /* Reset TX Done or timeout flags */
  RadioTxDone_flag = 0;
  RadioTxTimeout_flag = 0;
  RadioError_flag = 0;

  if (tx_payload_generator() == 0)
  {
#if (GPS_TX_TRACE == 1)
    {
      uint16_t show_len = payloadLen;
      char tx_text[GPS_TX_MAX_CHUNK + 1];
      if (show_len > GPS_TX_MAX_CHUNK)
      {
        show_len = GPS_TX_MAX_CHUNK;
      }
      memcpy(tx_text, data_buffer, show_len);
      tx_text[show_len] = '\0';
      APP_TPRINTF("GPS TX: %s\r\n", tx_text);
    }
#endif
#if (APP_LONG_PACKET==0)

    Radio.Send(data_buffer, payloadLen);

#else

    if (0UL != Radio.TransmitLongPacket(payloadLen, TX_TIMEOUT_VALUE, TxLongPacketGetNextChunk))
    {
        while (1);
    }



//#if (APP_LONG_PACKET==0)
//  Radio.Send(data_buffer, payloadLen);
//#else
//  if (0UL != Radio.TransmitLongPacket(payloadLen, TX_TIMEOUT_VALUE, TxLongPacketGetNextChunk))
//  {
//    while (1);
//  }
#endif /* APP_LONG_PACKET */
    APP_TPRINTF("Tx %d, %d bytes\r\n", packetCnt, payloadLen);
    HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, GPIO_PIN_SET); /* LED_BLUE */
  }
  else
  {
    HAL_Delay(20);
    UTIL_SEQ_SetTask((1 << CFG_SEQ_Task_SubGHz_Phy_App_Process), CFG_SEQ_Prio_0);
  }
#endif /* TEST_MODE */
}

#if (TEST_MODE == RADIO_TX)
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
  switch (GPIO_Pin)
  {
    case  BUT1_Pin:
      /* Increment by 16*/
      payloadLen += 16;
      if (payloadLen > payloadLenMax)
      {
        payloadLen = 16;
      }
      APP_TPRINTF("New Tx Payload Length= %d\r\n", payloadLen);
      break;
    case  BUT2_Pin:
      /* Increment by 1*/
      payloadLen += 1;
      if (payloadLen > payloadLenMax)
      {
        payloadLen = 1;
      }
      APP_TPRINTF("New Tx Payload Length= %d\r\n", payloadLen);

      break;
    case  BUT3_Pin:
      /* Toggle TxPayloadMode*/
      TxPayloadMode = (TxPayloadMode + 1) % 2;
      if (TxPayloadMode == 1)
      {
        APP_TPRINTF("Payload PRBS9 mode\r\n");
      }
      else
      {
        APP_TPRINTF("Payload Inc mode\r\n");
      }
      break;
    default:
      break;
  }
}

static int32_t tx_payload_generator(void)
{
  uint16_t n;
  char type3[4];

  __disable_irq();

  n = gps_rx_len_ready;
  if (n > 0U)
  {
    if ((nmea_get_sentence_type((const char *)gps_rx_buffer, type3) == 0) && (strcmp(type3, "ZDA") == 0))
    {
      gps_rx_len_ready = 0U;
      __enable_irq();
      return -1;
    }

    if (n > MAX_APP_BUFFER_SIZE)
    {
      n = MAX_APP_BUFFER_SIZE;
    }
    memcpy(data_buffer, gps_rx_buffer, n);
    gps_rx_len_ready = 0U;
    //gps_rx_len = 0U;
    payloadLen = n;
    __enable_irq();
    return 0;
  }
  __enable_irq();

#if (GPS_PAYLOAD_ONLY == 1)
  return -1;
#else

  if (TxPayloadMode == 1)
  {
    uint16_t prbs9_val = PRBS9_INIT;
    for (int32_t i = 0; i < payloadLen; i++)
    {
      data_buffer[i] = 0;
    }
    for (int32_t i = 0; i < payloadLen * 8; i++)
    {
      int32_t newbit = (((prbs9_val >> 8) ^ (prbs9_val >> 4)) & 1);
      prbs9_val = ((prbs9_val << 1) | newbit) & 0x01ff;
      data_buffer[i / 8] |= ((prbs9_val & 0x1) << (i % 8));
    }
  }
  else
  {
    for (int32_t i = 0; i < payloadLen; i++)
    {
      data_buffer[i] = i;
    }
  }

  return 0;
#endif
}

#endif
/* TEST_MODE */
/* USER CODE END PrFD */
