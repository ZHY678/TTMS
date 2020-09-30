/******************** (C) COPYRIGHT 2017 安科泰丰 ******************************
* File Name          : UART_ProtocolDeal.h
* Author             : 吴邦明 
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
* Description    : 接收串口数据
* Input          : - data: 数据缓存
*                  - data_len: 数据长度 
* Output         : None
* Return         : None
*******************************************************************************/
void uart_rx_fill(const uint8_t *const data, uint32_t data_len);

/*******************************************************************************
* Function Name  : set_rx_pakage_indicate
* Description    : 设置协议数据包接收回调函数，必须在用户代码中实现
* Input          : - rx_ind：回调函数
                   - data: 数据缓存，第一字节为协议头
* Output         : None
* Return         : None
*******************************************************************************/
void set_rx_pakage_indicate(void (*rx_ind)(const uint8_t * const data));

/*******************************************************************************
* Function Name  : make_protocol_pakage
* Description    : 数据按协议填充
* Input          : - tag: 数据标识
*                  - data: 数据缓存
                   - len: 数据长度
* Output         : None
* Return         : None
*******************************************************************************/
void make_protocol_pakage(eDataType tag, const uint8_t *const data, uint32_t len);

/*******************************************************************************
* Function Name  : set_uart_tx_indicate
* Description    : 设置串口发送回调函数，必须在用户代码中实现
* Input          : - tx_ind：回调函数
                   - data: 数据缓存
                   - len:  数据长度
* Output         : None
* Return         : None
*******************************************************************************/
void set_uart_tx_indicate(void (*tx_ind)(const uint8_t *const data, uint32_t len));


#ifdef __cplusplus
}
#endif

#endif /* __CAN_APP_H */

/******************* (C) COPYRIGHT 2017 *****END OF FILE***********************/
