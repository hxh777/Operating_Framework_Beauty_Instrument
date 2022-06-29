/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
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
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "stdio.h"
#include "string.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
	int fputc(int ch,FILE *f){
	uint8_t temp[1] = {ch};
	HAL_UART_Transmit(&huart1,temp,1,10);//huanrt  根据配置修改
	return ch;
	}
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
	uint16_t flag=255;
	uint8_t Rx_flag  =0x5A;
	uint8_t buf[255];
	uint8_t stat=2;
	#define USART1_LEN 72 //缓冲区大小  一次只能连按 USART1_LEN 次，连USART1_LEN+1 会只执行一次
	
	typedef struct 
	{
		uint8_t buff[USART1_LEN];//缓冲区
		uint8_t usart1_rx_len;//保存的数据长度	
		uint8_t w;//写
		uint8_t r;//读
		uint8_t usart1_flag;//数据接收完成标志
	}USART1_RX;

	USART1_RX USART1_rx={{0},0,0,0,0};//串口接收数据缓冲区初始化
	
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{	 
	if(huart->Instance == USART1)
	{
        //写入数据到缓冲区
        USART1_rx.buff[USART1_rx.w]=Rx_flag;
        USART1_rx.w=(USART1_rx.w+1)%USART1_LEN;
		USART1_rx.buff[USART1_rx.w] = 0;
		flag = 1;
		HAL_UART_Receive_IT(&huart1,&Rx_flag,1);  
		
	}
}


void step_motor_direction_angle_delay(uint8_t direction,uint16_t angle,uint16_t delay);
uint8_t Usart1_Annular_txdata(void);
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
  MX_TIM5_Init();
  MX_USART1_UART_Init();
  /* USER CODE BEGIN 2 */
  
  //HAL_TIM_Base_Start_IT(&htim5);

  
  //step_motor_direction_angle_delay(0,90,2);
  step_motor_direction_angle_delay(0,90,1);  //0是顺时针
  step_motor_direction_angle_delay(1,90,1);
  HAL_UART_Receive_IT(&huart1,&Rx_flag,1);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {		
	  if (flag==1)
	  {	  	
		  while(stat)
		  {
		  
			  if(stat == 0x01)
			  {
				step_motor_direction_angle_delay(0,10,1);
			  }
			  else if(stat ==0x5a)
			  {
				step_motor_direction_angle_delay(1,10,1);
			  }
			  else
			  {
				
			  }
			  //读取串口环形缓冲区
			 stat = Usart1_Annular_txdata();
		  }
		flag = 255;
		stat = 2;
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
}

/* USER CODE BEGIN 4 */
/***************************************************************************/
//0 顺时针
void step_motor_direction_angle_delay(uint8_t direction,uint16_t angle,uint16_t delay)
{
	int num=0;
	num = (angle/360.0)*512.0;
	uint16_t  t =delay;
	if(direction == 0)
	{ 	
		
		while(num--)
		{
		HAL_Delay(delay);
		
		HAL_GPIO_WritePin(GPIOB,GPIO_PIN_7|GPIO_PIN_8|GPIO_PIN_9,GPIO_PIN_RESET);
		HAL_GPIO_WritePin(GPIOB,GPIO_PIN_6,GPIO_PIN_SET);
		
		HAL_Delay(delay);
		
		HAL_GPIO_WritePin(GPIOB,GPIO_PIN_6|GPIO_PIN_8|GPIO_PIN_9,GPIO_PIN_RESET);
		HAL_GPIO_WritePin(GPIOB,GPIO_PIN_7,GPIO_PIN_SET);
		
		HAL_Delay(delay);
		
		HAL_GPIO_WritePin(GPIOB,GPIO_PIN_6|GPIO_PIN_7|GPIO_PIN_9,GPIO_PIN_RESET);
		HAL_GPIO_WritePin(GPIOB,GPIO_PIN_8,GPIO_PIN_SET);
		
		HAL_Delay(delay);
		
		HAL_GPIO_WritePin(GPIOB,GPIO_PIN_7|GPIO_PIN_8|GPIO_PIN_9,GPIO_PIN_RESET);
		HAL_GPIO_WritePin(GPIOB,GPIO_PIN_9,GPIO_PIN_SET);
		
		}
		
	}
	else if(direction ==1)
	{	
			while(num--)
		{
		
		HAL_Delay(delay);
		
		HAL_GPIO_WritePin(GPIOB,GPIO_PIN_7|GPIO_PIN_8|GPIO_PIN_9,GPIO_PIN_RESET);
		HAL_GPIO_WritePin(GPIOB,GPIO_PIN_9,GPIO_PIN_SET);
		
		HAL_Delay(delay);
		
		HAL_GPIO_WritePin(GPIOB,GPIO_PIN_6|GPIO_PIN_7|GPIO_PIN_9,GPIO_PIN_RESET);
		HAL_GPIO_WritePin(GPIOB,GPIO_PIN_8,GPIO_PIN_SET);
		
		HAL_Delay(delay);
		
		HAL_GPIO_WritePin(GPIOB,GPIO_PIN_6|GPIO_PIN_8|GPIO_PIN_9,GPIO_PIN_RESET);
		HAL_GPIO_WritePin(GPIOB,GPIO_PIN_7,GPIO_PIN_SET);
		
		HAL_Delay(delay);
		
		HAL_GPIO_WritePin(GPIOB,GPIO_PIN_7|GPIO_PIN_8|GPIO_PIN_9,GPIO_PIN_RESET);
		HAL_GPIO_WritePin(GPIOB,GPIO_PIN_6,GPIO_PIN_SET);
			
		}
	
	}
	else
	{ 
	
		printf("error has occured");
	
	}
	
	

}




/**********************从缓冲区读取数据******************
**
**形参:u8 *tx_data -- 读取数据保存地址
**
*********************************************************/
uint8_t Usart1_Annular_txdata()
{   
    uint8_t ch=0;
    //缓冲区为空 或者 USART1_rx.usart1_flag 数据接收完成标志(为了兼容字符串接收处理)
    //if(USART1_rx.usart1_rx_len==0 || USART1_rx.usart1_flag==0)return 0;
    
    
    ch=USART1_rx.buff[USART1_rx.r];//读取缓冲区数据
    USART1_rx.r= (USART1_rx.r+1)%USART1_LEN;  
	
	if(ch==0)
	{	
		USART1_rx.r = (USART1_rx.r-1)%USART1_LEN;
		if(USART1_rx.r == 0xFF)
		{ 
		USART1_rx.r = USART1_LEN-1; 	
		}
		return ch;
	}
    
    //USART1_rx.usart1_flag=0;//清除标志位
    
    return ch;//返回读取到的字符个数
}


/***************************************************************************/
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
