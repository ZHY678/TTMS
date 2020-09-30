/******************** (C) COPYRIGHT 2017 ����̩�� ******************************
* File Name          : UART_ProtocolDeal.h
* Author             : ����� 
* Description        : 

* Change Logs:
* Date           Author        Version         Notes
 19/6/2017       wunihaoo       V1.0
 
 
********************************************************************************/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __UART_ProtocolDeal_H
#define __UART_ProtocolDeal_H

#include <stdint.h>


#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/


/* Public define ------------------------------------------------------------*/

/* Public typedef -----------------------------------------------------------*/
typedef enum _DATA_TYPE_
{
	START_MEASURE          = 0X01,
	STOPT_MEASURE          = 0X02,
	LAYER_CONTRL           = 0X03,
	SENSOR_DATA            = 0X04,
	POWER_OFF              = 0X05,
	CHANGE_DLG             = 0X06,
	SAVE_RESULT            = 0X07, 
	CALIB_RATE             = 0X08
}eDataType;


/* Exported macro ------------------------------------------------------------*/

/* Exported variables ------------------------------------------------------- */


/* Exported functions ------------------------------------------------------- */

/*******************************************************************************
* Function Name  : uart_rx_fill
* Description    : ���մ�������
* Input          : - data: ���ݻ���
*                  - data_len: ���ݳ��� 
* Output         : None
* Return         : None
*******************************************************************************/
void uart_rx_fill(const uint8_t *const data, uint32_t data_len);

/*******************************************************************************
* Function Name  : set_rx_pakage_indicate
* Description    : ����Э�����ݰ����ջص��������������û�������ʵ��
* Input          : - rx_ind���ص�����
                   - data: ���ݻ��棬��һ�ֽ�ΪЭ��ͷ
* Output         : None
* Return         : None
*******************************************************************************/
void set_rx_pakage_indicate(void (*rx_ind)(const uint8_t * const data));

/*******************************************************************************
* Function Name  : make_protocol_pakage
* Description    : ���ݰ�Э�����
* Input          : - tag: ���ݱ�ʶ
*                  - data: ���ݻ���
                   - len: ���ݳ���
* Output         : None
* Return         : None
*******************************************************************************/
void make_protocol_pakage(eDataType tag, const uint8_t *const data, uint32_t len);

/*******************************************************************************
* Function Name  : set_uart_tx_indicate
* Description    : ���ô��ڷ��ͻص��������������û�������ʵ��
* Input          : - tx_ind���ص�����
                   - data: ���ݻ���
                   - len:  ���ݳ���
* Output         : None
* Return         : None
*******************************************************************************/
void set_uart_tx_indicate(void (*tx_ind)(const uint8_t *const data, uint32_t len));


#ifdef __cplusplus
}
#endif

#endif /* __CAN_APP_H */

/******************* (C) COPYRIGHT 2017 *****END OF FILE***********************/
