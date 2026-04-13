/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
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
#include "main.h"
#include "app_subghz_phy.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "usart_if.h"
#include <stdio.h>
#include <string.h>

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
  #define UART1_TEST_PROFILE_APP         0U
  #define UART1_TEST_PROFILE_LOOPBACK    1U
  #define UART1_TEST_PROFILE_GPS_SIMPLE  2U

  #define UART1_TEST_PROFILE UART1_TEST_PROFILE_APP

#if (UART1_TEST_PROFILE == UART1_TEST_PROFILE_LOOPBACK)
  static uint32_t uart1_test_pass = 0U;
  static uint32_t uart1_test_fail = 0U;
#endif
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
#if (UART1_TEST_PROFILE == UART1_TEST_PROFILE_LOOPBACK)
static uint8_t uart1_txrx_polling(const uint8_t *tx, uint8_t *rx, uint16_t len);
static void uart1_blocking_test_process(void);
#endif
#if (UART1_TEST_PROFILE == UART1_TEST_PROFILE_GPS_SIMPLE)
static void uart1_gps_simple_test_process(void);
#endif

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

#if (UART1_TEST_PROFILE == UART1_TEST_PROFILE_LOOPBACK)
static void uart1_blocking_test_process(void)
{
  static const uint8_t tx_payload[] = "Hello";
  static uint32_t loop_divider = 0U;
  uint8_t rx_payload[sizeof(tx_payload) - 1U];
  uint8_t io_ok;
  uint8_t pass;
  char msg[160];
  int len;

  loop_divider++;
  if (loop_divider >= 250000U)
  {
    loop_divider = 0U;

    memset(rx_payload, 0, sizeof(rx_payload));
    io_ok = uart1_txrx_polling(tx_payload, rx_payload, sizeof(tx_payload) - 1U);

    pass = (io_ok != 0U) && (memcmp(tx_payload, rx_payload, sizeof(rx_payload)) == 0);
    if (pass != 0U)
    {
      uart1_test_pass++;
    }
    else
    {
      uart1_test_fail++;
    }

    len = snprintf(msg, sizeof(msg),
                   "[LB1] tx='Hello' rx='%.*s' io=%u pass=%lu fail=%lu\r\n",
                   (int)sizeof(rx_payload),
                   (char *)rx_payload,
                   (unsigned int)io_ok,
                   (unsigned long)uart1_test_pass,
                   (unsigned long)uart1_test_fail);
    if (len > 0)
    {
      (void)HAL_UART_Transmit(&huart2, (uint8_t *)msg, (uint16_t)len, 50U);
    }
  }
}

static uint8_t uart1_txrx_polling(const uint8_t *tx, uint8_t *rx, uint16_t len)
{
  uint16_t i;
  uint32_t spin;

  while (__HAL_UART_GET_FLAG(&huart1, UART_FLAG_RXNE) != RESET)
  {
    (void)huart1.Instance->RDR;
  }

  for (i = 0U; i < len; i++)
  {
    spin = 0U;
    while (__HAL_UART_GET_FLAG(&huart1, UART_FLAG_TXE) == RESET)
    {
      spin++;
      if (spin > 200000U)
      {
        return 0U;
      }
    }
    huart1.Instance->TDR = tx[i];

    spin = 0U;
    while (__HAL_UART_GET_FLAG(&huart1, UART_FLAG_RXNE) == RESET)
    {
      spin++;
      if (spin > 200000U)
      {
        return 0U;
      }
    }
    rx[i] = (uint8_t)(huart1.Instance->RDR & 0xFFU);
  }

  return 1U;
}
#endif

#if (UART1_TEST_PROFILE == UART1_TEST_PROFILE_GPS_SIMPLE)
static void uart1_gps_simple_test_process(void)
{
  static uint8_t line_buf[128];
  static uint16_t line_len = 0U;
  uint8_t ch;
  HAL_StatusTypeDef st;
  char msg[180];
  int n;

  st = HAL_UART_Receive(&huart1, &ch, 1U, 100U);
  if (st == HAL_TIMEOUT)
  {
    return;
  }

  if (st != HAL_OK)
  {
    n = snprintf(msg, sizeof(msg), "[GPS1] RX error=%d\r\n", (int)st);
    if (n > 0)
    {
      (void)HAL_UART_Transmit(&huart2, (uint8_t *)msg, (uint16_t)n, 50U);
    }
    return;
  }

  if (ch == '\r')
  {
    return;
  }

  if (ch == '\n')
  {
    if (line_len > 0U)
    {
      line_buf[line_len] = '\0';
      n = snprintf(msg, sizeof(msg), "[GPS1] %s\r\n", (char *)line_buf);
      if (n > 0)
      {
        (void)HAL_UART_Transmit(&huart2, (uint8_t *)msg, (uint16_t)n, 50U);
      }
      line_len = 0U;
    }
    return;
  }

  if (line_len < (sizeof(line_buf) - 1U))
  {
    line_buf[line_len++] = ch;
  }
  else
  {
    line_buf[line_len] = '\0';
    n = snprintf(msg, sizeof(msg), "[GPS1] OVF %s\r\n", (char *)line_buf);
    if (n > 0)
    {
      (void)HAL_UART_Transmit(&huart2, (uint8_t *)msg, (uint16_t)n, 50U);
    }
    line_len = 0U;
  }
}
#endif

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_USART1_UART_Init();
#if (UART1_TEST_PROFILE != UART1_TEST_PROFILE_APP)
  MX_USART2_UART_Init();
#else
  MX_SubGHz_Phy_Init();
#endif

  /* USER CODE BEGIN 2 */

#if (UART1_TEST_PROFILE == UART1_TEST_PROFILE_LOOPBACK)
  {
    const char start_msg[] = "[LB1] Blocking UART1 test ON: short PB6->PB7\r\n";
    (void)HAL_UART_Transmit(&huart2, (uint8_t *)start_msg, (uint16_t)(sizeof(start_msg) - 1U), 100U);
  }
#elif (UART1_TEST_PROFILE == UART1_TEST_PROFILE_GPS_SIMPLE)
  {
    const char start_msg[] = "[GPS1] Simple UART1 GPS test ON (38400). Connect GPS TX->PB7 and GND.\r\n";
    (void)HAL_UART_Transmit(&huart2, (uint8_t *)start_msg, (uint16_t)(sizeof(start_msg) - 1U), 100U);
  }
#else
  extern volatile uint8_t gps_byte;
  HAL_UART_Receive_IT(&huart1, (uint8_t*)&gps_byte, 1);
#endif
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
#if (UART1_TEST_PROFILE == UART1_TEST_PROFILE_LOOPBACK)
    uart1_blocking_test_process();
#elif (UART1_TEST_PROFILE == UART1_TEST_PROFILE_GPS_SIMPLE)
    uart1_gps_simple_test_process();
#endif

    /* USER CODE END WHILE */
#if (UART1_TEST_PROFILE == UART1_TEST_PROFILE_APP)
    MX_SubGHz_Phy_Process();
#endif

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure LSE Drive Capability
  */
  HAL_PWR_EnableBkUpAccess();
  __HAL_RCC_LSEDRIVE_CONFIG(RCC_LSEDRIVE_LOW);

  /** Configure the main internal regulator output voltage
  */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_LSE|RCC_OSCILLATORTYPE_MSI;
  RCC_OscInitStruct.LSEState = RCC_LSE_ON;
  RCC_OscInitStruct.MSIState = RCC_MSI_ON;
  RCC_OscInitStruct.MSICalibrationValue = RCC_MSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.MSIClockRange = RCC_MSIRANGE_11;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure the SYSCLKSource, HCLK, PCLK1 and PCLK2 clocks dividers
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK3|RCC_CLOCKTYPE_HCLK
                              |RCC_CLOCKTYPE_SYSCLK|RCC_CLOCKTYPE_PCLK1
                              |RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_MSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.AHBCLK3Divider = RCC_SYSCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}
#ifdef USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  while (1)
  {
  }
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
