/**
******************************************************************************
* @file    Loader_Src.c
* @author  MCD Application Team
* @brief   This file defines the operations of the external loader for
*          MX25LM51245G OSPI memory of STM32U559J-DK.
*           
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

#include "stm32u599j_discovery_ospi.h"
#include "mx25lm51245g.h" 
#include "Loader_Src.h"
#include <string.h>

BSP_OSPI_NOR_Init_t Flash;

  
/* Private functions ---------------------------------------------------------*/
/** @defgroup STM32U599J_DK_OSPI_Exported_Functions Exported Functions
* @{
*/

void HAL_Delay(uint32_t Delay)
{
  int i=0;
  for (i=0; i<0x1000; i++);
}


 HAL_StatusTypeDef HAL_InitTick(uint32_t TickPriority)
{ 
  return HAL_OK;
}


/*******************************************************************************
* @brief  System initialization.
* @param  None
* @retval  1      : Operation succeeded
* @retval  0      : Operation failed
*******************************************************************************/
int Init_OSPI()
{
    

	 memset(&Flash, 0, sizeof(Flash));	
  SystemInit();  
  HAL_Init();  
  		
  Flash.InterfaceMode = BSP_OSPI_NOR_SPI_MODE; 
  Flash.TransferRate  = BSP_OSPI_NOR_STR_TRANSFER; 
	
/* Configure the system clock  */
  SystemClock_Config();
  
 /* Initialaize OSPI */   
    if( BSP_OSPI_NOR_Init(0, &Flash)!=0)
			return 0;

    if( BSP_OSPI_NOR_EnableMemoryMappedMode(0)!=0)
			return 0;
 
  
  return 1;
}

/*******************************************************************************
* @brief   masserase memory.
* @retval  1      : Operation succeeded
* @retval  0      : Operation failed
*******************************************************************************/

int MassErase (uint32_t Parallelism ){

   if(BSP_OSPI_NOR_DeInit(0)!=0)
		return 0;  
  if( BSP_OSPI_NOR_Init(0, &Flash)!=0)
			return 0;
  if(BSP_OSPI_NOR_Erase_Chip(0)!=0)
		return 0;
  while (BSP_OSPI_NOR_GetStatus(0)!=0);
 
	return 1;
}

/*******************************************************************************
* @brief   Program memory.
* @param   Address: page address
* @param   Size   : size of data
* @param   buffer : pointer to data buffer
* @retval  1      : Operation succeeded
* @retval  0      : Operation failed
*******************************************************************************/
int Write (uint32_t Address, uint32_t Size, uint8_t* buffer)
{  
  Address = Address & 0x0fffffff;

  if(BSP_OSPI_NOR_DeInit(0)!=0)
		return 0;    

  if( BSP_OSPI_NOR_Init(0, &Flash)!=0)
			return 0;
  if(BSP_OSPI_NOR_Write(0,buffer,Address, Size)!=0)
		return 0;

  return 1;
}

/*******************************************************************************
* @brief   Sector erase.
* @param   EraseStartAddress :  erase start address
* @param   EraseEndAddress   :  erase end address
* @retval  None
*******************************************************************************/
int SectorErase (uint32_t EraseStartAddress ,uint32_t EraseEndAddress)
{
  uint32_t BlockAddr;
  EraseStartAddress &= 0x0FFFFFFF;  
  EraseEndAddress   &= 0x0FFFFFFF;
  EraseStartAddress = EraseStartAddress -  EraseStartAddress % 0x10000;

  if(BSP_OSPI_NOR_DeInit(0)!=0)
		return 0;  
	
  if( BSP_OSPI_NOR_Init(0, &Flash)!=0)
			return 0;

  while (EraseEndAddress >EraseStartAddress)
  {
    BlockAddr = EraseStartAddress;   
    if(BSP_OSPI_NOR_Erase_Block(0,BlockAddr,  MX25LM51245G_ERASE_64K)!=0)
			return 0;
    while (BSP_OSPI_NOR_GetStatus(0)!=0);  
    EraseStartAddress+=0x10000;		
			
  } 
	if(BSP_OSPI_NOR_EnableMemoryMappedMode(0)!=0)
				return 0;

	
  return 1;	
}

/**
* @brief  System Clock Configuration
*         The system Clock is configured as follows :
*            System Clock source            = PLL (MSI)
*            SYSCLK(Hz)                     = 160000000
*            HCLK(Hz)                       = 160000000
*            AHB Prescaler                  = 1
*            APB1 Prescaler                 = 1
*            APB2 Prescaler                 = 1
*            APB3 Prescaler                 = 1
*            MSI Frequency(Hz)              = 4000000
*            PLL_MBOOST                     = 1
*            PLL_M                          = 1
*            PLL_N                          = 80
*            PLL_Q                          = 2
*            PLL_R                          = 2
*            PLL_P                          = 2
*            Flash Latency(WS)              = 4
* @param  None
* @retval None
*/
void SystemClock_Config(void)
{
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  
  /* Enable voltage range 1 for frequency above 100 Mhz */
  __HAL_RCC_PWR_CLK_ENABLE();
  HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1);
  __HAL_RCC_PWR_CLK_DISABLE();
  
  /* MSI Oscillator enabled at reset (4Mhz), activate PLL with MSI as source */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_MSI;
  RCC_OscInitStruct.MSIState = RCC_MSI_ON;
  RCC_OscInitStruct.MSIClockRange = RCC_MSIRANGE_4;
  RCC_OscInitStruct.MSICalibrationValue = RCC_MSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_MSI;
//  RCC_OscInitStruct.PLL.PLLMBOOST = RCC_PLLMBOOST_DIV1;
  RCC_OscInitStruct.PLL.PLLM = 1;
  RCC_OscInitStruct.PLL.PLLN = 80;
  RCC_OscInitStruct.PLL.PLLR = 2;
  RCC_OscInitStruct.PLL.PLLP = 2;
  RCC_OscInitStruct.PLL.PLLQ = 2;          
  RCC_OscInitStruct.PLL.PLLFRACN= 0;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    /* Initialization Error */
    while(1);
  }
  
  /* Select PLL as system clock source and configure bus clocks dividers */
  RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | \
    RCC_CLOCKTYPE_PCLK2  | RCC_CLOCKTYPE_PCLK3);
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB3CLKDivider = RCC_HCLK_DIV1;
  if(HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK)
  {
    /* Initialization Error */
    while(1);
  }
}



/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
