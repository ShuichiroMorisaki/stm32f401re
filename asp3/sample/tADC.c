/*
 * このファイルは tecsgen によりテンプレートとして自動生成されました
 * このファイルを編集して使用することが意図されていますが
 * tecsgen の再実行により上書きされてしまうため、通常
 *   gen/tADC_templ.c => src/tADC.c
 * のように名前, フォルダを変更してから修正します
 */
/* #[<PREAMBLE>]#
 * #[<...>]# から #[</...>]# で囲まれたコメントは編集しないでください
 * tecsmerge によるマージに使用されます
 *
 * #[</PREAMBLE>]# */

/* プロトタイプ宣言や変数の定義をここに書きます #_PAC_# */
#include "tADC_tecsgen.h"
#include "stm32f4xx_hal_adc.h"

#ifndef E_OK
#define	E_OK	0		/* success */
#define	E_ID	(-18)	/* illegal ID */
#endif

/* 受け口関数 #_TEPF_# */
/* #[<ENTRY_PORT>]# eADC
 * entry port: eADC
 * signature:  sADC
 * context:    task
 * #[</ENTRY_PORT>]# */

/* #[<ENTRY_FUNC>]# eADC_Initialize
 * name:         eADC_Initialize
 * global_name:  tADC_eADC_Initialize
 * oneway:       false
 * #[</ENTRY_FUNC>]# */
void
eADC_Initialize(CELLIDX idx, ADC_HandleTypeDef* hadc)
{
	CELLCB	*p_cellcb;
	if (VALID_IDX(idx)) {
		p_cellcb = GET_CELLCB(idx);
	}
	else {
		/* エラー処理コードをここに記述します */
	} /* end if VALID_IDX(idx) */

	/* ここに処理本体を記述します #_TEFB_# */
    hadc->Instance = ADC1;
    hadc->Init.ClockPrescaler = ATTR_ClockPrescaler;
    hadc->Init.Resolution = ATTR_Resolution;
    hadc->Init.ScanConvMode = ATTR_ScanConvMode;
    hadc->Init.ContinuousConvMode = ATTR_ContinuousConvMode;
    hadc->Init.DiscontinuousConvMode = ATTR_DiscontinuousConvMode;
    hadc->Init.ExternalTrigConvEdge = ATTR_ExternalTrigConvEdge;
    hadc->Init.ExternalTrigConv = ATTR_ExternalTrigConv;
    hadc->Init.DataAlign = ATTR_DataAlign;
    hadc->Init.NbrOfConversion = 1;
    hadc->Init.NbrOfDiscConversion = 0;
    hadc->Init.DMAContinuousRequests = ATTR_DMAContinuousRequests;
    hadc->Init.EOCSelection = ATTR_EOCSelection;
    HAL_ADC_Init(hadc);
}

/* #[<ENTRY_FUNC>]# eADC_StartDMA
 * name:         eADC_StartDMA
 * global_name:  tADC_eADC_StartDMA
 * oneway:       false
 * #[</ENTRY_FUNC>]# */
void
eADC_StartDMA(CELLIDX idx, ADC_HandleTypeDef* hadc, uint32_t* pData, uint32_t Length)
{
	CELLCB	*p_cellcb;
	if (VALID_IDX(idx)) {
		p_cellcb = GET_CELLCB(idx);
	}
	else {
		/* エラー処理コードをここに記述します */
	} /* end if VALID_IDX(idx) */

	/* ここに処理本体を記述します #_TEFB_# */
    HAL_ADC_Start_DMA(hadc, pData, Length);
}

/* #[<POSTAMBLE>]#
 *   これより下に非受け口関数を書きます
 * #[</POSTAMBLE>]#*/
