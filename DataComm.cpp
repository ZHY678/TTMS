#include "stdafx.h"
#include "DataComm.h"
#include "Utils.h"

CMscomm1 m_MSCOMM1;
//float sensorData[2];
SensorData sensorData;
bool start_measure = false;
bool trans_sensorData = false;
bool power_off = false;
bool change_dlg = false;
bool save_result = false;
bool calibrating = false;

unsigned char sendstr[20] = { 0x00 };
CByteArray byteArr;

void(*pResoveFun)(const uint8_t * const) = resolve_data;
void(*pMakeStrFun)(const uint8_t *const, uint32_t) = make_sendString;

bool open_COM()
{
	//读入串口参数
	CString path = GetAppExecPath();
	CString file;
	file.Format("%s\\Config.ini", path);

	LPTSTR strValue = new char[20];
	GetPrivateProfileStringA("COM", "com_port", "", strValue, 20, file);
	int com_port = atoi(strValue);
	GetPrivateProfileStringA("COM", "com_setting", "", strValue, 20, file);
	CString com_setting(strValue);
	delete strValue;

	//串口设置
	if (m_MSCOMM1.get_PortOpen())
	{
		m_MSCOMM1.put_PortOpen(false);
	}
	m_MSCOMM1.put_CommPort(com_port);
	m_MSCOMM1.put_InputMode(1);
	m_MSCOMM1.put_InBufferSize(1024);
	m_MSCOMM1.put_OutBufferSize(1024);
	m_MSCOMM1.put_Settings(com_setting);
	m_MSCOMM1.put_RThreshold(1);
	m_MSCOMM1.put_InputLen(0);

	if (!m_MSCOMM1.get_PortOpen())
	{
		m_MSCOMM1.put_PortOpen(true); //打开串口
	}

	if (!m_MSCOMM1.get_PortOpen()) //如果打开失败，返回false
	{
		return false;
	}
	set_callback();

	return true;
}

void set_callback()
{
	//设置接收串口数据回调函数
	set_rx_pakage_indicate(pResoveFun);
	//设置发送串口数据回调函数
	set_uart_tx_indicate(pMakeStrFun);

}

void resolve_data(const uint8_t * const data)
{
	eDataType tag;
	uint8_t tag_data = data[TAG_INDEX];
	if (tag_data == 0x01)
	{
		tag = eDataType::START_MEASURE;
	}
	else if (tag_data == 0x04)
	{
		tag = eDataType::SENSOR_DATA;
	}
	else if (tag_data == 0x05)
	{
		tag = eDataType::POWER_OFF;
	}
	else if (tag_data ==0x06)
	{
		tag = eDataType::CALIB_RATE;
	}
	else if (tag_data ==0x07)
	{
		tag = eDataType::SAVE_RESULT;
	}
	else if (tag_data == 0x08)
	{
		tag = eDataType::CALIB_RATE;
	}
	switch (tag)
	{
	case START_MEASURE:
		start_measure = true;
		break;
	case SENSOR_DATA:
		trans_sensorData = true;
		sensorData.Dvalue = *(short*)&data[SD_START_INDEX];
		sensorData.trackDis = *(float*)&data[SD_START_INDEX + SDLEN_horizontal];
		sensorData.battery = *(unsigned char*)&data[SD_START_INDEX + SDLEN_horizontal + SDLEN_trackDis];
		sensorData.version = *(unsigned char*)&data[SD_START_INDEX + SDLEN_horizontal + SDLEN_trackDis + SDLEN_battery];

		break;

	case POWER_OFF:
		power_off = true;
		break;
	case CALIB_RATE:
		calibrating = true;
		break;
	case SAVE_RESULT:
		save_result = true;
		break;

	}
}

void make_sendString(const uint8_t *const data, uint32_t len)
{
	byteArr.SetSize(len);
	for (int i = 0; i < len; i++)
	{
		//sendstr[i] = data[i];
		byteArr.SetAt(i, data[i]);
	}
}

void change_to_hex(const uint8_t *const data, int len, CString& str)
{
	str.Empty();
	CString s;
	for (int i = 0; i < len; i++)
	{
		s.Format(_T("%02X"), data[i]);
		str += s;
	}

}

void send_data(const CByteArray& data)
{
	m_MSCOMM1.put_Output(COleVariant(data));
}

void stop_measure()
{
	BYTE txdata[14] = { 0x00 };
	make_protocol_pakage(eDataType::STOPT_MEASURE, txdata, 14);
	send_data(byteArr);
}

void do_open_topLayer()
{
	BYTE txdata[14] = { 0x01, 0x01 };  //如果数组太大会越界，会覆盖栈
	make_protocol_pakage(eDataType::LAYER_CONTRL, txdata, 14);
	send_data(byteArr);
}

void do_open_belowLayer()
{
	BYTE txdata[14] = { 0x02, 0x01 };  
	make_protocol_pakage(eDataType::LAYER_CONTRL, txdata, 14);
	send_data(byteArr);
}

void do_open_allLayer()
{
	BYTE txdata[14] = { 0x00, 0x01 };
	make_protocol_pakage(eDataType::LAYER_CONTRL, txdata, 14);
	send_data(byteArr);
}
void do_close_topLayer()
{
	BYTE txdata[14] = { 0x01, 0x00 };
	make_protocol_pakage(eDataType::LAYER_CONTRL, txdata, 14);
	send_data(byteArr);

}
void do_close_belowLayer()
{
	BYTE txdata[14] = { 0x02, 0x00 };   
	make_protocol_pakage(eDataType::LAYER_CONTRL, txdata, 14);
	send_data(byteArr);
}
void do_close_allLayer()
{
	BYTE txdata[14] = { 0x00, 0x00 };
	make_protocol_pakage(eDataType::LAYER_CONTRL, txdata, 14);
	send_data(byteArr);
}


void deal_sensorData(SensorData& data)
{
	data = sensorData;
}