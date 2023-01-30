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
#include <t_syslog.h>
#include <t_stdlib.h>
#include "kernel_cfg.h"
#include "tSample2.h"

/*
 *  Add by Morisaki
 */
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_def.h"
//#include "stm32f4xe.h"
#include "stm32f4xx_hal_uart.h"
#include "stm32f4xx_hal_gpio.h"
#include "stm32f4xx_hal_rcc.h"
#include "stm32f4xx_hal_adc.h"
//#include "adc.h"
//#include "joytest.h"
//#include "../arch/arm_m_gcc/stm32f4xx_stm32cube/Inc/Former/device.h"
//p#include "adctest.h"

UART_HandleTypeDef huart2;
ADC_HandleTypeDef adc1;

ADC_ChannelConfTypeDef adc1ChannelConf;

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


/*
 *  メインタスク
 */
void
eMainTask_main(void)
{
	//syslog(LOG_NOTICE, "Sample program start.");

    MX_GPIO_Init();
    MX_USART2_UART_Init();
    char buf[] = "usart1 test2\r\n\n";
    consume_time(1000LU);
    int i=0;
    for(i;i<4;i++){
        //HAL_UART_Transmit(&huart2, (uint8_t *) buf , sizeof(buf) , 10);
        cUart_HAL_UART_Transmit((uint8_t *) buf , sizeof(buf) , 10);
        consume_time(1000LU);
    }
    //syslog(LOG_NOTICE, "Sample program ends.");


	SVC_PERROR(exitKernel());
	assert(0);
}

void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */

  if (cUart_Initialize() != HAL_OK)
  {
    Error_Handler();
  }
  /*
  
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  */
  
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

}


void MX_GPIO_Init(void)
{

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOA_CLK_ENABLE();

}
/*

void MX_ADC2_ADC_Init(void){
  //adc1.Instance= ADC1;
  adc1.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV2;
  adc1.Init.Resolution = ADC_RESOLUTION_12B;
  adc1.Init.ScanConvMode = ADC_SCANMODE_DISABLE;
  adc1.Init.ContinuousConvMode = ADC_CONTINUOUS_ENABLE;
  adc1.Init.DiscontinuousConvMode = ADC_DISCONTINUOUS_DISABLE;
  adc1.Init.NbrOfDiscConversion = 0;
  adc1.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
  adc1.Init.ExternalTrigConv = ADC_EXTERNALTRIGCONV_T1_CC1;
  adc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  adc1.Init.NbrOfConversion = 1;
  adc1.Init.DMAContinuousRequests = ADC_DMACONTINUOUS_ENABLE;
  adc1.Init.EOCSelection = ADC_EOC_SEQ_DISABLE;
  HAL_ADC_Init(&adc1);

  adc1ChannelConf.Channel = ADCx_CHANNEL;
  adc1ChannelConf.Rank = 1;
  adc1ChannelConf.SamplingTime = ADC_SAMPLETIME_480CYCLES;

  GPIO_Init_t GPIO_InitStruct;
  GPIO_InitStruct.mode = GPIO_MODE_ANALOG;
  GPIO_InitStruct.pull = GPIO_NOPULL;
  gpio_setup(ADCx_CHANNEL_GPIO_PORT, &GPIO_InitStruct, ADCx_CHANNEL_PINNO);

}
*/