/**
  ******************************************************************************
  * File Name          : I2C.c
  * Description        : This file provides code for the configuration
  *                      of the I2C instances.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "i2c.h"

/* USER CODE BEGIN 0 */
#define SLAVE_ADD 0xA0
#define BUF_SIZE 150
/* USER CODE END 0 */

I2C_HandleTypeDef hi2c1;

/* I2C1 init function */
void MX_I2C1_Init(void)
{

  hi2c1.Instance = I2C1;
  hi2c1.Init.ClockSpeed = 100000;
  hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c1.Init.OwnAddress1 = 160;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_ENABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    Error_Handler();
  }

}

void HAL_I2C_MspInit(I2C_HandleTypeDef* i2cHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if(i2cHandle->Instance==I2C1)
  {
  /* USER CODE BEGIN I2C1_MspInit 0 */

  /* USER CODE END I2C1_MspInit 0 */
  
    __HAL_RCC_GPIOB_CLK_ENABLE();
    /**I2C1 GPIO Configuration    
    PB8     ------> I2C1_SCL
    PB9     ------> I2C1_SDA 
    */
    GPIO_InitStruct.Pin = GPIO_PIN_8|GPIO_PIN_9;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF4_I2C1;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    /* I2C1 clock enable */
    __HAL_RCC_I2C1_CLK_ENABLE();

    /* I2C1 interrupt Init */
    HAL_NVIC_SetPriority(I2C1_EV_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(I2C1_EV_IRQn);
    HAL_NVIC_SetPriority(I2C1_ER_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(I2C1_ER_IRQn);
  /* USER CODE BEGIN I2C1_MspInit 1 */

		
  /* USER CODE END I2C1_MspInit 1 */
  }
}

void HAL_I2C_MspDeInit(I2C_HandleTypeDef* i2cHandle)
{

  if(i2cHandle->Instance==I2C1)
  {
  /* USER CODE BEGIN I2C1_MspDeInit 0 */

  /* USER CODE END I2C1_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_I2C1_CLK_DISABLE();
  
    /**I2C1 GPIO Configuration    
    PB8     ------> I2C1_SCL
    PB9     ------> I2C1_SDA 
    */
    HAL_GPIO_DeInit(GPIOB, GPIO_PIN_8|GPIO_PIN_9);

    /* I2C1 interrupt Deinit */
    HAL_NVIC_DisableIRQ(I2C1_EV_IRQn);
    HAL_NVIC_DisableIRQ(I2C1_ER_IRQn);
  /* USER CODE BEGIN I2C1_MspDeInit 1 */

  /* USER CODE END I2C1_MspDeInit 1 */
  }
} 

/* USER CODE BEGIN 1 */


uint8_t master_send_buf[150]={0};
uint8_t master_rec_buf[150]={0};
uint8_t slave_rec_buf[150]={0};
uint8_t slave_send_buf[150]={0};
//void master_work(void)
//{
//	I2C_HandleTypeDef i2cmaster;
//	uint8_t i;
//	
//	for(i=0;i<BUF_SIZE;i++)
//	{
//		master_send_buf[i]=BUF_SIZE - i;		
//	}
//	while(HAL_I2C_Master_Transmit(&hi2c2, (uint16_t)SLAVE_ADD, (uint8_t*)master_send_buf, BUF_SIZE, 10000)!= HAL_OK)
//  {
//    /* Error_Handler() function is called when Timeout error occurs.
//       When Acknowledge failure occurs (Slave don't acknowledge it's address)
//       Master restarts communication */
//    if (HAL_I2C_GetError(&hi2c2) != HAL_I2C_ERROR_AF)
//    {
//      Error_Handler();
//    }
//  }
//	
//  while(HAL_I2C_Master_Receive(&hi2c2, (uint16_t)SLAVE_ADD, (uint8_t*)master_rec_buf, BUF_SIZE, 10000) != HAL_OK)
//  {
//    if (HAL_I2C_GetError(&hi2c2) != HAL_I2C_ERROR_AF)
//    {
//      Error_Handler();
//    }   
//  }	
//}



void slave_work(void)
{
	if(HAL_I2C_EnableListen_IT(&hi2c1) != HAL_OK)
	{
		/* Transfer error in reception process */
		Error_Handler();
	}
}


uint8_t uslaveRecIndex = 0;
uint8_t uslaveSendIndex = BUF_SIZE-1;
void HAL_I2C_SlaveRxCpltCallback(I2C_HandleTypeDef *I2cHandle)
{
	if(HAL_I2C_Slave_Seq_Receive_IT(I2cHandle, &slave_rec_buf[uslaveRecIndex], 1, I2C_FIRST_FRAME) != HAL_OK)
	{
		Error_Handler();
	}
	uslaveRecIndex++;
}

void HAL_I2C_SlaveTxCpltCallback(I2C_HandleTypeDef *I2cHandle)
{
	if(HAL_I2C_Slave_Seq_Transmit_IT(&hi2c1, &slave_rec_buf[uslaveSendIndex], 1, I2C_LAST_FRAME) != HAL_OK)
	{
		Error_Handler();
	}
	uslaveSendIndex--;
}

void HAL_I2C_AddrCallback(I2C_HandleTypeDef *hi2c, uint8_t TransferDirection, uint16_t AddrMatchCode)
{
  if(AddrMatchCode == SLAVE_ADD)
  {
    /* First of all, check the transfer direction to call the correct Slave Interface */
    if(TransferDirection == I2C_DIRECTION_TRANSMIT)
    {
			uslaveRecIndex = 0;
      if(HAL_I2C_Slave_Seq_Receive_IT(&hi2c1, &slave_rec_buf[uslaveRecIndex], 1, I2C_FIRST_FRAME) != HAL_OK)
      {
        Error_Handler();
      }
      uslaveRecIndex++;
    }
    else
    {
			uslaveSendIndex = BUF_SIZE-1;
      if(HAL_I2C_Slave_Seq_Transmit_IT(&hi2c1, &slave_rec_buf[uslaveSendIndex], 1, I2C_LAST_FRAME) != HAL_OK)
      {
        Error_Handler();
      }
			uslaveSendIndex--;
    }
  }
  else
  {
    /* Call Error Handler, Wrong Address Match Code */
//    Error_Handler();
  }
}

void HAL_I2C_ListenCpltCallback(I2C_HandleTypeDef *hi2c)
{
	if(HAL_I2C_EnableListen_IT(&hi2c1) != HAL_OK)
    {
      /* Transfer error in reception process */
      Error_Handler();
    }
}
/* USER CODE END 1 */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
