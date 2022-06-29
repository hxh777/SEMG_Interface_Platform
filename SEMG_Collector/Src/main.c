/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2022 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "adc.h"
#include "dma.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "stdio.h"
#include "string.h"
#define buf_size 6
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
typedef enum {
	false = 0,
	true
}bool;

int fputc(int ch,FILE *f){
 uint8_t temp[1] = {ch};
 HAL_UART_Transmit(&huart1,temp,1,10);//huanrt  根据配置修改
 return ch;
}


int flag = 0;
volatile uint32_t adc_value[buf_size] = {0};
bool TIM_1ms    = false;
bool TIM_5ms    = false;
bool TIM_10ms   = false;
bool TIM_100ms  = false;
bool TIM_1000ms = false;

float send_data[buf_size] = {0};

typedef union {
	uint8_t i[4];
	float fl;
}my_fl;
my_fl var; 
float tmp;
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define long_time_threshold 1500
#define noise_threshold 50

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

//0x20空闲 0x24按下 0x28抬起 0x32按下前消抖 0x36抬起前消抖
void key_do()
{
	static uint8_t s = 0x20;
	static uint16_t cnt = 0;
	static uint16_t tmp = 0; //按下时长 
	uint8_t key_v ;
	key_v = HAL_GPIO_ReadPin(GPIOC,GPIO_PIN_12);
	//0 按下 1 抬起
	switch(s){
		
		case 0x20 :       //不确定按键按下还是抖动
			if(0 == key_v)
			{
				s = 0x32;
				cnt = 0;
			}
			break;
		case 0x24 :		 //确定按下，开始计算按压时间
			
			if(key_v == 0) 
				cnt++;
			else if(key_v == 1) //抬起？抖动
			{
				tmp += cnt;       //计算抖动前值和抖动至抬起的总时间
				cnt = 0;
			    s = 0x36;	}
			break;
		case 0x28 :              //确定按键已经抬起了  判断长按还是短按
			if(tmp > long_time_threshold)
				flag = 2;
			else if(tmp > 0)
				flag = 1;
			tmp = 0;
			s = 0x20;
			break;
		case 0x32 :                 //50ms抖动判定
			cnt++;
			if(cnt > noise_threshold)
			{
				cnt = 0;
				if(key_v == 1)
				{
					s = 0x20;
				}
				else if(key_v == 0 )
					s = 0x24;
			}
			break;
			
		case 0x36 :    //50ms上升抖动判定
			cnt++;
			if(cnt > noise_threshold) 
			{
				cnt = 0;
				if(key_v == 1)
				{
					s = 0x28;
				}
				else if(key_v == 0 )
					s = 0x24;
			}
			break;
		default: break;
	}

}

void my_ADCsample(){

	HAL_ADC_Start_DMA(&hadc1,(uint32_t *)adc_value,buf_size);
}
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	if (GPIO_Pin == GPIO_PIN_11)
	{

		printf("anxia\r\n");
		my_ADCsample();
	}

}
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc){
	
//	static uint8_t i = 0;
//	send_data[i%2]   =  adc_value*3.3/4095;
//	i++;
//	if( i == 2  )
//	{	
//		 
//		HAL_UART_Transmit(&huart1,(uint8_t *)send_data,sizeof(send_data),99);
//		i = 0;
//	}
////	send_data   =  adc_value*0.00080586080;
////	HAL_UART_Transmit(&huart1,(uint8_t *)&send_data,sizeof(send_data),1);
	
}



/**
  * @brief  定时器记满回调函数  (7199 99 ->10ms,7199 499 ->50ms)
  * @param  tim句柄 &htimx
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	if(htim->Instance == TIM3) //0.1ms 进入 
	{	
		static uint32_t cnt = 0;
		cnt++;
		cnt % 1 == 0 ? TIM_1ms = true : false;
		
		cnt % 10 == 0 ? TIM_10ms = true : false;               //注意TIM_10ms 此处为 1ms 标志位
		cnt % 25 == 0 ? TIM_5ms = true : false;                //注意TIM_10ms 此处为2.5ms 标志位
		cnt % 100 == 0 ? TIM_100ms = true : false;
		cnt % 1000 == 0 ? (TIM_1000ms = true,cnt = 0) : false;
		
		
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
	var.fl = 1.123;
	static bool key_flag = false; 
	static float cnt = 0;
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
  MX_DMA_Init();
  MX_TIM3_Init();
  MX_USART1_UART_Init();
  MX_ADC1_Init();
  MX_USART3_UART_Init();
  /* USER CODE BEGIN 2 */
  //printf("hxh! hello.\r\n");
  HAL_TIM_Base_Start_IT(&htim3);                // 使能定时器3中断 0.1ms  
  my_ADCsample();                               // 3通道adc DMA连续扫描采集
  
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {	
	  if(TIM_1ms)
	  {     //1000float /s  115200bps
		TIM_1ms = false;
	  }
	  
	  if(TIM_5ms)
	 {     								//2.5ms    6float      115200 bps/s
										//1000ms   2400float   9600B < 11520
										//3通道 每通道 (9600 / 3 )字节
		if(key_flag)
		{	
		  
			for(int i =0;i<buf_size;i++)
			{
				send_data[i] = adc_value[i] * 0.00080586080;
				
			} 
			
			HAL_UART_Transmit(&huart1,(uint8_t *)send_data,sizeof(send_data),10);
			//HAL_UART_Transmit(&huart3,(uint8_t *)send_data,sizeof(send_data),10);
		 }

		TIM_5ms = false;
	  }
	  
	  
	  if(TIM_10ms)
	 { 
		 key_do();
		TIM_10ms = false;
	  }
	  
	  if(TIM_100ms)
	  {  
		TIM_100ms = false;
		
	  }
	  if(TIM_1000ms)
	  {  
		TIM_1000ms = false;
	  }
	  
	  
	  if(flag == 2)
	  {
		 flag = 0;
		printf("long1.5 event \r\n");
	  }
	  if(flag == 1)
	  {
		 flag = 0;
		//printf("short event \r\n");
		  key_flag = (key_flag == false) ?  true : false;
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
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_ADC;
  PeriphClkInit.AdcClockSelection = RCC_ADCPCLK2_DIV6;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
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

  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
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
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
