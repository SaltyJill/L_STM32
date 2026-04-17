/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2026 STMicroelectronics.
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
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define Rx_Buffer_Size 15
uint8_t Rx_Buffer[Rx_Buffer_Size];
uint8_t Rx_Zigbee[Rx_Buffer_Size];
uint8_t Zigbee_CFG[2] = {0x23, 0xA0}; // 查询参数,返回A2+14byte
uint8_t Zigbee_SET[2] = {0x23, 0xFE}; // 设置参数
uint8_t Zigbee_RST[2] = {0x23, 0x23}; // 设备重启 CFG 必须高电平
uint8_t Zigbee_getFlag = RESET;       // CFG接收完成标志
// uint8_t Zigbee_setFlag = RESET; // CFG设置完成标志
uint8_t Uart2_RxFlag = RESET; // 串口2接收完成标志
/*
(byte-meaning)：
    2-模块地址,网络ID,中心节点,主从发送,波特率
    无校验,数据8N1,地址编码HEX,串口超时,无线频点,发射功率,源地址不输出
*/
uint8_t Master_CFG[14] = {0x00, 0x00, // address
                          0x10,       // NET_ID
                          0x01,       // NET_Type
                          0x01,       // Point_Type
                          0x02,       // TR_Mode
                          0x06,       // Baudrate 38400
                          0x01,       // 校验位
                          0x01,       // 8N1
                          0x02,       // address code:HEX
                          0xFF,
                          0x0A,
                          0x00,
                          0x01};
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
void Zigbee_CFGstart(void)
{
    cfg_start;
    HAL_Delay(3000);
}
void Zigbee_CFGExit(void)
{
    HAL_Delay(50);
    cfg_end;
    HAL_Delay(50);
    HAL_UART_Transmit(&huart3, Zigbee_RST, 2, 100);
}
void Zigbee_getCFG(uint8_t *CFG_Buffer)
{
    Zigbee_CFGstart();
    HAL_UART_Transmit(&huart3, Zigbee_CFG, 2, 100);
    HAL_UART_Receive_IT(&huart3, CFG_Buffer, 15);
}
void Zigbee_setCFG(uint8_t *CFG)
{
    Zigbee_CFGstart();
    HAL_UART_Transmit(&huart3, Zigbee_SET, 2, 100);
    HAL_UART_Transmit(&huart3, CFG, 14, 800);
    Zigbee_CFGExit();
    HAL_UART_Transmit(&huart2, (uint8_t *)"CFG SET\r\n", 9, 100);
}
void Clear_Buffer(uint8_t *Buffer, uint8_t Size)
{
    for (uint8_t i = 0; i < Size; i++)
    {
        Buffer[i] = 0;
    }
}
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
  MX_USART2_UART_Init();
  MX_USART3_UART_Init();
  /* USER CODE BEGIN 2 */
    HAL_UART_Transmit(&huart2, (uint8_t *)"At your service\n", 16, 200);
    HAL_UART_Receive_IT(&huart2, Rx_Buffer, 2); // 启动串口2接收中断，等待电脑发送命令
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
    while (1)
    {
			 if (Zigbee_getFlag == SET)
        {
						led_off;
            Zigbee_getFlag = RESET;
            Zigbee_CFGExit();
            HAL_UART_Transmit(&huart2, Rx_Zigbee, Rx_Buffer_Size, 800); // 将查询到的参数通过串口2发送回电脑
            Clear_Buffer(Rx_Zigbee, Rx_Buffer_Size);
        }
        if (Uart2_RxFlag == SET)
        {
						led_off;
            Uart2_RxFlag = RESET;
            if (Rx_Buffer[0] == '#')
            {
                if (Rx_Buffer[1] == '1')
                {
									  HAL_UART_Transmit(&huart2, (uint8_t *)"It's not work\n", 14, 200);
                    Zigbee_getCFG(Rx_Zigbee);
                }
                else if (Rx_Buffer[1] == '2')
                {
									HAL_UART_Transmit(&huart2, (uint8_t *)"It's Showtime...\n", 17, 200);
                    Zigbee_setCFG(Master_CFG);
                }
            }
            else
            {
                HAL_UART_Transmit(&huart2, (uint8_t *)"Invalid Command\n", 16, 200);
            }
            HAL_UART_Receive_IT(&huart2, Rx_Buffer, 2); // 继续接收下一条命令
        }
				else
				{
					led_on;
					HAL_Delay(100);
					led_off;
					HAL_Delay(100);
				}
    /* USER CODE END WHILE */

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

  /** Configure the main internal regulator output voltage
  */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if (huart->Instance == USART3)
    {
        Zigbee_getFlag = SET;
    }
    else if (huart->Instance == USART2)
    {
        Uart2_RxFlag = SET;
    }
}
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
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
