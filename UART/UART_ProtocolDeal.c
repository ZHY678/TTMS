/******************** (C) COPYRIGHT 2017 安科泰丰 ******************************
* File Name          : UART_ProtocolDeal.c
* Author             : 吴邦明 
* Description        : 

* Change Logs:
* Date           Author        Version         Notes
 19/6/2017       wunihaoo       V1.0
 
 
********************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "UART_ProtocolDeal.h"


/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
#define PRO_HEAD_LEN   5u
#define PRO_DATA_LEN   20u


/* Private typedef -----------------------------------------------------------*/
typedef enum _STATUS_
{
	IDLE_STATUS    = 0X01,
	HEAD_STATUS    = 0X02,
	FILL_STATUS    = 0X03,
	DEAL_STATUS    = 0X04
}eStatus;

/* Private variables ---------------------------------------------------------*/


/* Private function prototypes -----------------------------------------------*/
static void (* RX_Pakage_deal)(const uint8_t * const data);
static void (* TX_Pakage_deal)(const uint8_t *const data, uint32_t len);

/* External function prototypes ----------------------------------------------*/


/* Private functions ---------------------------------------------------------*/

/*------------------------------------------------------------------------------------------------
	                            interface
------------------------------------------------------------------------------------------------*/

/*******************************************************************************
* Function Name  : CRCCal
* Description    : 计算校验
* Input          : - data: 数据缓存
*                  - len: 数据长度 
* Output         : None
* Return         : 校验值
* Note           : 
*******************************************************************************/
static uint8_t CRCCal(const uint8_t *const data, uint32_t len)
{
	uint32_t index_i;
	uint8_t  crc = 0;

	for(index_i=0; index_i < len; index_i++)
	{
		crc += data[index_i];
	}	
	
	return crc;
}
/*******************************************************************************
* Function Name  : DelData
* Description    : 删除数据，并修改状态机状态和数据包下标
* Input          : - rx_buf: 一包数据，校验不正确
*                  - Status: 状态机状态 
                   - index：数据包写下标
* Output         : None
* Return         : None
* Note           : 
*******************************************************************************/
static void DelData(uint8_t *const buf, eStatus * const Status, uint32_t *const index)
{
	uint32_t buf_index;
	uint8_t  data_tmp;
	eStatus  DelDataStatus = IDLE_STATUS;
	
	*Status = IDLE_STATUS;  //默认设置为空状态
	
	for (buf_index = 1; buf_index < PRO_DATA_LEN; buf_index++)
	{
		data_tmp = buf[buf_index];
		
		switch(DelDataStatus)
		{
			case IDLE_STATUS:
				if (data_tmp == 0x55)
				{
					*Status = HEAD_STATUS;
					
					DelDataStatus = HEAD_STATUS;
				}
			break;
			
			case HEAD_STATUS:
				if (data_tmp == 0xAA)
				{
					*index = 0;
					buf[(*index)++] = 0x55;
					buf[(*index)++] = 0xAA;
					
					*Status = FILL_STATUS;
					
					DelDataStatus = FILL_STATUS;
				}
				else
				{
					*Status = IDLE_STATUS;
					
					DelDataStatus = IDLE_STATUS;
				}
			break;
			
			case FILL_STATUS:
				buf[(*index)++] = data_tmp;
			
				if (*index == (PRO_DATA_LEN - 1))
				  *Status = DEAL_STATUS;			
			break;

			case DEAL_STATUS:  //因为第一个字节肯定会被丢弃，所以不可能执行到这步
				
			break;

      default:
      break;			
			
		}
	}
}

/*******************************************************************************
* Function Name  : uart_rx_fill
* Description    : 接收串口数据
* Input          : - data: 数据缓存
*                  - data_len: 数据长度 
* Output         : None
* Return         : None
*******************************************************************************/
void uart_rx_fill(const uint8_t *const data, uint32_t data_len)
{
	static eStatus  Status = IDLE_STATUS;
	static uint8_t  rx_buf[PRO_DATA_LEN];
	static uint32_t write_index = 0;
	static uint8_t  data_tmp, rx_i;	
	
  if (data)	
	{
		for (rx_i=0; rx_i < data_len; rx_i++)
		{
			data_tmp = data[rx_i];
			
			switch (Status)
			{
				case IDLE_STATUS:
					if (data_tmp == 0x55)
						Status = HEAD_STATUS;
					
				break;

				case HEAD_STATUS:
					if (data_tmp == 0xAA)
					{
						write_index = 0;
						rx_buf[write_index++] = 0x55;
						rx_buf[write_index++] = 0xAA;
						
						Status = FILL_STATUS;
					}
					else
						Status = IDLE_STATUS;
					
				break;

				case FILL_STATUS:
					rx_buf[write_index++] = data_tmp;
				
				  if (write_index == (PRO_DATA_LEN - 1))
						Status = DEAL_STATUS;
				break;

				case DEAL_STATUS:
					rx_buf[write_index] = data_tmp; //crc位				  

          if (CRCCal(rx_buf, PRO_DATA_LEN - 1) == data_tmp)  //校验正确调用用户处理函数进行处理
					{
						if(RX_Pakage_deal != 0)
							RX_Pakage_deal(rx_buf);
						
						Status = IDLE_STATUS;
					}
          else  //校验不正确丢弃前面数据
					{
            DelData(rx_buf, &Status, &write_index);
					}						

          										
				break;

				default :
					Status = IDLE_STATUS;
				break;
				
			}	//switch (Status)		
		} //for (rx_i=0; rx_i < data_len; rx_i++)
	} //if (data)
	
}

/*******************************************************************************
* Function Name  : make_protocol_pakage
* Description    : 数据按协议填充
* Input          : - tag: 数据标识
*                  - data: 数据缓存
                   - len: 数据长度
* Output         : None
* Return         : None
*******************************************************************************/
void make_protocol_pakage(eDataType tag, const uint8_t *const data, uint32_t len)	
{ 
	static uint8_t send_buf[PRO_DATA_LEN];
  uint32_t index_i;

	if(data)
	{		
		send_buf[0] = 0x55;/*填充协议头*/
		send_buf[1] = 0xAA;
		send_buf[2] = tag;
		send_buf[3] = 0x00; //点对点，不需要地址
		send_buf[4] = 0x00;

		for(index_i=0; index_i<len; index_i++) //将数据填充
			send_buf[PRO_HEAD_LEN + index_i] = data[index_i];

		send_buf[PRO_DATA_LEN - 1] = CRCCal(send_buf, PRO_DATA_LEN - 1); //填充校验值

		if(TX_Pakage_deal != 0) //通过串口发送出去
		  TX_Pakage_deal(send_buf, PRO_DATA_LEN);

  }
}

/*******************************************************************************
* Function Name  : set_rx_pakage_indicate
* Description    : 设置协议数据包接收回调函数，必须在用户代码中实现
* Input          : - rx_ind：回调函数
                   - data: 数据缓存，第一字节为协议头
* Output         : None
* Return         : None
*******************************************************************************/
void set_rx_pakage_indicate(void (*rx_ind)(const uint8_t * const data))
{
	RX_Pakage_deal = rx_ind;
}

/*******************************************************************************
* Function Name  : set_uart_tx_indicate
* Description    : 设置串口发送回调函数，必须在用户代码中实现
* Input          : - tx_ind：回调函数
                   - data: 数据缓存
                   - len:  数据长度
* Output         : None
* Return         : None
*******************************************************************************/
void set_uart_tx_indicate(void (*tx_ind)(const uint8_t *const data, uint32_t len))
{
	TX_Pakage_deal = tx_ind;
}


/******************* (C) COPYRIGHT 2017 *****END OF FILE************************/
