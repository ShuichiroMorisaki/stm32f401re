/*
 *  TOPPERS/ASP Kernel
 *      Toyohashi Open Platform for Embedded Real-Time Systems/
 *      Advanced Standard Profile Kernel
 * 
 *  Copyright (C) 2015-2016 by Ushio Laboratory
 *              Graduate School of Engineering Science, Osaka Univ., JAPAN
 *  Copyright (C) 2015-2019 by Embedded and Real-Time Systems Laboratory
 *              Graduate School of Information Science, Nagoya Univ., JAPAN
 * 
 *  上記著作権者は，以下の(1)〜(4)の条件を満たす場合に限り，本ソフトウェ
 *  ア（本ソフトウェアを改変したものを含む．以下同じ）を使用・複製・改
 *  変・再配布（以下，利用と呼ぶ）することを無償で許諾する．
 *  (1) 本ソフトウェアをソースコードの形で利用する場合には，上記の著作
 *      権表示，この利用条件および下記の無保証規定が，そのままの形でソー
 *      スコード中に含まれていること．
 *  (2) 本ソフトウェアを，ライブラリ形式など，他のソフトウェア開発に使
 *      用できる形で再配布する場合には，再配布に伴うドキュメント（利用
 *      者マニュアルなど）に，上記の著作権表示，この利用条件および下記
 *      の無保証規定を掲載すること．
 *  (3) 本ソフトウェアを，機器に組み込むなど，他のソフトウェア開発に使
 *      用できない形で再配布する場合には，次のいずれかの条件を満たすこ
 *      と．
 *    (a) 再配布に伴うドキュメント（利用者マニュアルなど）に，上記の著
 *        作権表示，この利用条件および下記の無保証規定を掲載すること．
 *    (b) 再配布の形態を，別に定める方法によって，TOPPERSプロジェクトに
 *        報告すること．
 *  (4) 本ソフトウェアの利用により直接的または間接的に生じるいかなる損
 *      害からも，上記著作権者およびTOPPERSプロジェクトを免責すること．
 *      また，本ソフトウェアのユーザまたはエンドユーザからのいかなる理
 *      由に基づく請求からも，上記著作権者およびTOPPERSプロジェクトを
 *      免責すること．
 * 
 *  本ソフトウェアは，無保証で提供されているものである．上記著作権者お
 *  よびTOPPERSプロジェクトは，本ソフトウェアに関して，特定の使用目的
 *  に対する適合性も含めて，いかなる保証も行わない．また，本ソフトウェ
 *  アの利用により直接的または間接的に生じたいかなる損害に関しても，そ
 *  の責任を負わない．
 * 
 *  $Id: tSample2.c 1237 2019-07-10 01:00:30Z ertl-hiro $
 */

/* 
 *  サンプルプログラム(2)の本体
 *
 *  ASPカーネルの基本的な動作を確認するためのサンプルプログラム（TECS版）．
 *
 *  プログラムの概要:
 *
 *  ユーザインタフェースを受け持つメインタスク（MainTask）と，3つの並
 *  行実行されるタスク（Task1〜Task3），例外処理タスク（ExceptionTask）
 *  の5つのタスクを用いる．これらの他に，システムログタスクが動作する．
 *  また，周期ハンドラ，アラームハンドラ，割込みサービスルーチン，CPU
 *  例外ハンドラをそれぞれ1つ用いる．
 *
 *  並行実行されるタスクは，task_loop回のループを実行する度に，タスク
 *  が実行中であることをあらわすメッセージを表示する．ループを実行する
 *  のは，プログラムの動作を確認しやすくするためである．また，低速なシ
 *  リアルポートを用いてメッセージを出力する場合に，すべてのメッセージ
 *  が出力できるように，メッセージの量を制限するという理由もある．
 *
 *  周期ハンドラ，アラームハンドラ，割込みサービスルーチンは，3つの優
 *  先度（HIGH_PRIORITY，MID_PRIORITY，LOW_PRIORITY）のレディキューを
 *  回転させる．周期ハンドラは，プログラムの起動直後は停止状態になって
 *  いる．
 *
 *  CPU例外ハンドラは，CPU例外からの復帰が可能な場合には，例外処理タス
 *  クを起動する．例外処理タスクは，CPU例外を起こしたタスクに対して，
 *  終了要求を行う．
 *
 *  メインタスクは，シリアルポートからの文字入力を行い（文字入力を待っ
 *  ている間は，並行実行されるタスクが実行されている），入力された文字
 *  に対応した処理を実行する．入力された文字と処理の関係は次の通り．
 *  Control-Cまたは'Q'が入力されると，プログラムを終了する．
 *
 *  '1' : 対象タスクをTASK1に切り換える（初期設定）．
 *  '2' : 対象タスクをTASK2に切り換える．
 *  '3' : 対象タスクをTASK3に切り換える．
 *  'a' : 対象タスクをcTask_activateにより起動する．
 *  'A' : 対象タスクに対する起動要求をcTask_cancelActivateによりキャンセルする．
 *  'e' : 対象タスクにexitTaskを呼び出させ，終了させる．
 *  't' : 対象タスクをcTask_terminateにより強制終了する．
 *  '>' : 対象タスクの優先度をHIGH_PRIORITYにする．
 *  '=' : 対象タスクの優先度をMID_PRIORITYにする．
 *  '<' : 対象タスクの優先度をLOW_PRIORITYにする．
 *  'G' : 対象タスクの優先度をcTask_getPriorityで読み出す．
 *  's' : 対象タスクにsleepを呼び出させ，起床待ちにさせる．
 *  'S' : 対象タスクにsleepTimeout10秒)を呼び出させ，起床待ちにさせる．
 *  'w' : 対象タスクをcTask_wakeupにより起床する．
 *  'W' : 対象タスクに対する起床要求をcTask_cancelWakeupによりキャンセルする．
 *  'l' : 対象タスクをcTask_releaseWaitにより強制的に待ち解除にする．
 *  'u' : 対象タスクをcTask_suspendにより強制待ち状態にする．
 *  'm' : 対象タスクの強制待ち状態をcTask_resumeにより解除する．
 *  'd' : 対象タスクにdelay(10秒)を呼び出させ，時間経過待ちにさせる．
 *  'x' : 対象タスクにraiseTerminateにより終了要求する．
 *  'y' : 対象タスクにdisableTerminateを呼び出させ，タスク終了を禁止する．
 *  'Y' : 対象タスクにenableTerminateを呼び出させ，タスク終了を許可する．
 *  'r' : 3つの優先度（HIGH_PRIORITY，MID_PRIORITY，LOW_PRIORITY）のレ
 *        ディキューを回転させる．
 *  'c' : 周期ハンドラを動作開始させる．
 *  'C' : 周期ハンドラを動作停止させる．
 *  'b' : アラームハンドラを5秒後に起動するよう動作開始させる．
 *  'B' : アラームハンドラを動作停止させる．
 *  'z' : 対象タスクにCPU例外を発生させる（ターゲットによっては復帰可能）．
 *  'Z' : 対象タスクにCPUロック状態でCPU例外を発生させる（復帰不可能）．
 *  'V' : 短いループを挟んで，fetchHighResolutionTimerで高分解能タイマを
 *        2回読む．
 *  'v' : 発行したシステムコールを表示する（デフォルト）．
 *  'q' : 発行したシステムコールを表示しない．
 */

#include "tSample2_tecsgen.h"
#include <kernel.h>
//#include <t_syslog.h>
#include <t_stdlib.h>
#include "kernel_cfg.h"
#include "tSample2.h"
#include <stdio.h>


/*
 *  Add by Morisaki
 */
#include "stm32f4xx_hal_adc.h"
#include "stm32f4xx_hal_usart.h"
#include "stm32f4xx_hal_adc.h"
#include "stm32f4xx_hal_gpio.h"
#include "stm32f4xx_hal_cortex.h"

UART_HandleTypeDef huart2;
ADC_HandleTypeDef hadc1;
DMA_HandleTypeDef hdma_adc1;

#define TOPPERS_OMIT_SYSLOG


Inline void
svc_perror(const char *file, int_t line, const char *expr, ER ercd)
{
	if (ercd < 0) {
		t_perror(LOG_ERROR, file, line, expr, ercd);
	}
}

#define	SVC_PERROR(expr)	svc_perror(__FILE__, __LINE__, #expr, (expr))

/*
 *  プロセッサ時間の消費
 *
 *  ループによりプロセッサ時間を消費する．最適化ができないように，ルー
 *  プ内でvolatile変数を読み込む．
 */
static volatile long_t	volatile_var;

static void
consume_time(ulong_t ctime)
{
	ulong_t		i;

	for (i = 0; i < ctime; i++) {
		(void) volatile_var;
	}
}

#define JS_OFF      0x00
#define JS_ON       0x01
#define JS_UP       0x02
#define JS_DOWN     0x03
#define JS_LEFT     0x04
#define JS_RIGHT    0x05
#define NUM_JSPOSITION (JS_RIGHT+1)

volatile uint16_t adc_data;
static uint32_t pos_tbl[NUM_JSPOSITION][2] = {
  {  500, JS_DOWN  },
  { 1000, JS_RIGHT },
  { 1500, JS_ON    },
  { 2500, JS_UP    },
  { 3800, JS_LEFT  },
  { 4096, JS_OFF   }
};
static char *pos_str[NUM_JSPOSITION] = {
  "    OFF     ",
  "    ON      ",
  "    UP      ",
  "    DOWN    ",
  "    LEFT    ",
  "    RIGHT   "
};
/* USER CODE END PV */



/*
 *  メインタスク
 */
void
eMainTask_main(void)
{
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_ADC1_Init();
  MX_USART2_UART_Init();
  /* USER CODE BEGIN 2 */
  int i=0;
  int j=0;
  int event = JS_OFF;
  int old_event = -1;

  printf("test start\r\n\n\n");

  consume_time(1000LU);
  if(HAL_ADC_Start_DMA(&hadc1,(uint16_t*) &adc_data, 1)==HAL_OK){
      //HAL_UART_Transmit(&huart2, (uint8_t *) buf2 , sizeof(buf) , 10);
  }
  while(i<20){
    //printf("%d\r\n",adc_data);
    for(j = 0 ; j < NUM_JSPOSITION ; j++){
      if(adc_data < pos_tbl[j][0]){
        event = pos_tbl[j][1];
        break;
      }
    }
    if(event != old_event){
      //printf("%d:[%s] val(%d)\r\n", i, pos_str[event], adc_data);
      old_event = event;
    }
    consume_time(1000LU);
    i++;
  }
  HAL_ADC_Stop_DMA(&hadc1);
  //printf("after%d\r\n",hadc1.State);
}


/**
  * @brief ADC1 Initialization Function
  * @param None
  * @retval None
  */
void MX_ADC1_Init(void)
{

  /* USER CODE BEGIN ADC1_Init 0 */

  /* USER CODE END ADC1_Init 0 */

  ADC_ChannelConfTypeDef sConfig = {0};

  /* USER CODE BEGIN ADC1_Init 1 */

  /* USER CODE END ADC1_Init 1 */
  /** Configure the global features of the ADC (Clock, Resolution, Data Alignment and number of conversion) 
  */
  hadc1.Instance = ADC1;
  hadc1.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV4;
  hadc1.Init.Resolution = ADC_RESOLUTION_12B;
  hadc1.Init.ScanConvMode = DISABLE;
  hadc1.Init.ContinuousConvMode = ENABLE;
  hadc1.Init.DiscontinuousConvMode = DISABLE;
  hadc1.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
  hadc1.Init.ExternalTrigConv = ADC_EXTERNALTRIGCONV_T1_CC1; //ADC_SOFTWARE_START;
  hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc1.Init.NbrOfConversion = 1;
  hadc1.Init.NbrOfDiscConversion = 0;
  hadc1.Init.DMAContinuousRequests = ENABLE;
  hadc1.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
  if (HAL_ADC_Init(&hadc1) != HAL_OK)
  {
    //Error_Handler();
  }
  /** Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time. 
  */
  sConfig.Channel = ADC_CHANNEL_8;
  sConfig.Rank = 1;
  sConfig.SamplingTime = ADC_SAMPLETIME_480CYCLES; //ADC_SAMPLETIME_3CYCLES;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    //Error_Handler();
  }
  /* USER CODE BEGIN ADC1_Init 2 */

  /* USER CODE END ADC1_Init 2 */

}

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    //Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

}

/** 
  * Enable DMA controller clock
  */
void MX_DMA_Init(void) 
{

  /* DMA controller clock enable */
  __HAL_RCC_DMA2_CLK_ENABLE();

  /* DMA interrupt init */
  /* DMA2_Stream0_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA2_Stream0_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA2_Stream0_IRQn);

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : B1_Pin */
  GPIO_InitStruct.Pin = B1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(B1_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : LD2_Pin */
  GPIO_InitStruct.Pin = LD2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(LD2_GPIO_Port, &GPIO_InitStruct);
}

/* USER CODE BEGIN 4 
 USER CODE END 4 */
