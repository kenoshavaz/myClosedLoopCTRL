// Proj : Closed Loop to control the DC Motor
// Doc  : myQE.c
// Name : Kenosha Vaz
// Date : 19 April, 2018

#include <stdio.h>
#include <stdint.h>
#include <ctype.h>

#include "common.h"

static TIM_HandleTypeDef tim1;

HAL_StatusTypeDef rc;

/* Initialise the GPIO pins */

ParserReturnVal_t CmdCL_Init(int action)
{
  GPIO_InitTypeDef GPIO_InitStruct;

  uint16_t rc,value;
  
  __HAL_RCC_GPIOA_CLK_ENABLE();
  GPIO_InitStruct.Pin = GPIO_PIN_1|GPIO_PIN_0;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  GPIO_InitStruct.Alternate = 0;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  rc = fetch_uint16_arg(&value);
  if(rc)
    {
      printf("\nMust supply a Period Value!\n\n");
      return CmdReturnBadParameter1;
    }  
   
  /* Initialize Timer 3 */

  GPIO_InitStruct.Pin = GPIO_PIN_8|GPIO_PIN_9;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Alternate = 6;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Initialise the Timer Delay*/
  
  __HAL_RCC_TIM1_CLK_ENABLE();

 /* Initialize PWM */

  tim1.Instance = TIM1;
  tim1.Init.Prescaler     = HAL_RCC_GetPCLK2Freq() / 1000000;
  tim1.Init.CounterMode   = TIM_COUNTERMODE_UP;
  tim1.Init.Period        = value;
  tim1.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  tim1.Init.RepetitionCounter = 0;

  HAL_TIM_Base_Init(&tim1);

  HAL_NVIC_SetPriority(TIM1_UP_TIM16_IRQn, 0, 1);
  HAL_NVIC_EnableIRQ(TIM1_UP_TIM16_IRQn);

  HAL_TIM_Base_Start(&tim1);


  TIM1->DIER |= 0x01;

  /* Configure Output */

  TIM_OC_InitTypeDef sConfig;

  sConfig.OCMode       = TIM_OCMODE_PWM1;
  sConfig.Pulse        = 0;
  sConfig.OCPolarity   = TIM_OCPOLARITY_HIGH;
  sConfig.OCNPolarity  = TIM_OCNPOLARITY_LOW;
  sConfig.OCFastMode   = TIM_OCFAST_DISABLE;
  sConfig.OCIdleState  = TIM_OCIDLESTATE_RESET;
  sConfig.OCNIdleState = TIM_OCNIDLESTATE_RESET;

  /* For Channel 1 */

  HAL_TIM_PWM_ConfigChannel(&tim1,&sConfig, TIM_CHANNEL_1);

  HAL_TIM_PWM_Start(&tim1,TIM_CHANNEL_1); /* Start PWM Output */

  /* For Channel 2 */

  HAL_TIM_PWM_ConfigChannel(&tim1,&sConfig, TIM_CHANNEL_2);

  HAL_TIM_PWM_Start(&tim1,TIM_CHANNEL_2); /* Start PWM Output */

  return CmdReturnOk;
}
ADD_CMD("clinit",CmdCL_Init," <LIM>          Initialise Closed Loop Control")

void Looper(){

  

}

ParserReturnVal_t CmdCL(int action){if(action!=CMD_INTERACTIVE) return CmdReturnOk;

  uint16_t rc, dir, spd;

  rc = fetch_uint16_arg(&dir);
  if(rc)
    {
      printf("\nMust supply a Channel Number!\n\n");
      return CmdReturnBadParameter1;
    }

  if(dir==1){
    HAL_GPIO_WritePin(GPIOA,GPIO_PIN_1,GPIO_PIN_SET);
    HAL_GPIO_WritePin(GPIOA,GPIO_PIN_0,GPIO_PIN_RESET);
    rc = fetch_uint16_arg(&spd);
    if(rc)
      {
	printf("\nMust supply an Output Value!\n\n");
	return CmdReturnBadParameter1;
      }    

    TIM1->CCR1=spd;
    
  }else if(dir==2){
    HAL_GPIO_WritePin(GPIOA,GPIO_PIN_0,GPIO_PIN_SET);
    HAL_GPIO_WritePin(GPIOA,GPIO_PIN_1,GPIO_PIN_RESET);
     rc = fetch_uint16_arg(&spd);
     if(rc)
       {
	 printf("\nMust supply an Output Value!\n\n");
	 return CmdReturnBadParameter1;
       }

    TIM1->CCR1=spd;
    
  }else if(dir==0){
    HAL_GPIO_WritePin(GPIOA,GPIO_PIN_0,GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOA,GPIO_PIN_1,GPIO_PIN_RESET);

    spd=0;
    TIM1->CCR1=0;
    
  }else{
    printf("\nMust be 1|2 for desired Channel!\n\n");
    return CmdReturnBadParameter1;
  }
  return CmdReturnOk;

}
ADD_CMD("cl",CmdCL," <DIR> <VAL>    Set Closed Loop Output")
