// Proj : Quadrature Encoder to read steps in DC motor
// Doc  : myQE.c
// Name : Kenosha Vaz
// Date : 10 April, 2018

#include <stdio.h>
#include <stdint.h>
#include <ctype.h>

#include "common.h"

static TIM_HandleTypeDef tim3;

HAL_StatusTypeDef rc;

static float code;

/* Initialise the GPIO pins */

ParserReturnVal_t CmdQuadInit(int action)
{
  GPIO_InitTypeDef GPIO_InitStruct;
  
  __HAL_RCC_GPIOA_CLK_ENABLE();
  GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  GPIO_InitStruct.Alternate = 0;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Initialise the Timer Delay*/

  GPIO_InitStruct.Pin = GPIO_PIN_6|GPIO_PIN_7;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Alternate = 2;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /* Initialize Timer 3 */

  __HAL_RCC_TIM3_CLK_ENABLE();
  tim3.Instance = TIM3;
  tim3.Init.Prescaler = 0;
  tim3.Init.CounterMode = TIM_COUNTERMODE_UP;
  tim3.Init.Period = 0xffff;
  tim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  tim3.Init.RepetitionCounter = 0;
  
  rc = HAL_TIM_Base_Init(&tim3);

  if(rc != HAL_OK) {
    printf("Failed to initialize Timer 3 Base, ""rc=%u\n",rc);
    return CmdReturnBadParameter1;
  }

  TIM3->DIER |= 0x01;

  /* Initialise Encoder Data Structures */

  TIM_Encoder_InitTypeDef encoderConfig;
  
  encoderConfig.EncoderMode = TIM_ENCODERMODE_TI12;
  encoderConfig.IC1Polarity = 0;
  encoderConfig.IC1Selection = TIM_ICSELECTION_DIRECTTI;
  encoderConfig.IC1Prescaler = 0;
  encoderConfig.IC1Filter = 3;
  encoderConfig.IC2Polarity = 0;
  encoderConfig.IC2Selection = TIM_ICSELECTION_DIRECTTI;
  encoderConfig.IC2Prescaler = 0;
  encoderConfig.IC2Filter = 3;

  /* Call HAL Encoder Init */

  rc = HAL_TIM_Encoder_Init(&tim3,&encoderConfig);
  if(rc != HAL_OK) {
    printf("Failed to initialize Timer 3 Encoder, ""rc=%u\n",rc);
    return CmdReturnBadParameter1;
  }

   /* Call the Encoder Start */

  rc  = HAL_TIM_Encoder_Start(&tim3, TIM_CHANNEL_1);
  if(rc != HAL_OK) {
    printf("Failed to start Timer 3 Encoder, " "rc=%u\n",rc);
    return CmdReturnBadParameter1;
  }
  rc = HAL_TIM_Encoder_Start(&tim3, TIM_CHANNEL_2);
  if(rc != HAL_OK) {
    printf("Failed to start Timer 3 Encoder, ""rc=%u\n",rc);
    return CmdReturnBadParameter1;
  }  

  return CmdReturnOk;

}
ADD_CMD("qeinit",CmdQuadInit,"                Initialise Quadrature Encoder")

void Encoder(void){

  
  return;
}

ParserReturnVal_t CmdQE(int action)
{
  if(action!=CMD_INTERACTIVE) return CmdReturnOk;

  uint32_t rc, val;

  /* Call the Encoder Start */

  rc=fetch_uint32_arg(&val);

  if(rc){
    printf("Must supply Direction Value (1|2)\n\n");
    return CmdReturnBadParameter1;
  }

  code=TIM3->CNT;
  code=(code/117.88)*30;

  if(val==1){

    printf("\nValue = \t%lu\n\n",TIM3->CNT);
    printf("Code Number =\t%5.0f\n",code);
    
  }else if(val==2){

    printf("\nValue = \t%lu\n\n",TIM3->CNT);
    printf("Code Number =\t%5.0f\n",code);

  }else{
    printf("Must be 1 or 2 to read specific Motors!\n\n");
    return CmdReturnBadParameter1;
  }

  if(code==27){
    printf("\tCode Number Accepted! Lock Open.\n\n");
  }else{
    printf("\tCode Number Unaccepted! Lock Closed.\n\n");
  }
  
  return CmdReturnOk;
}
ADD_CMD("qe",CmdQE," <1|2>          Read Counter for Quad Encoder")
