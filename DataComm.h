#ifndef __DATACOMM_HEADER
#define __DATACOMM_HEADER

#include "UART/UART_ProtocolDeal.h"
#include <string>
#include "mscomm1.h"

using namespace std;

#define TAG_INDEX 2
#define SENSOR_DATA_LEN 2

extern CMscomm1 m_MSCOMM1;
extern bool start_measure;
extern bool trans_sensorData;
extern bool power_off;
extern bool change_dlg;
extern bool save_result;
extern bool calibrating;
//发送的数据
extern unsigned char sendstr[20];
extern CByteArray byteArr;

//sensor data
//extern float sensorData[2];
struct SensorData
{
	short Dvalue;
	short Dvalue0 = 0;
	double dip;
	float Lvalue;
	float trackDis;
	float trackDisRef = 0;
	unsigned char battery;
	unsigned char version;
};
extern SensorData sensorData;
//sensor data len
const int SDLEN_horizontal = 2;
const int SDLEN_trackDis = 4;
const int SDLEN_battery = 1;
const int SDLEN_version = 1;
const int SD_START_INDEX = 5;


bool open_COM();
void set_callback();
void resolve_data(const uint8_t * const data);
void make_sendString(const uint8_t *const data, uint32_t len);
void change_to_hex(const uint8_t *const data, int len, CString& str);
void send_data(const CByteArray& data); 

void stop_measure();

//Layer control
void do_open_topLayer();
void do_open_belowLayer();
void do_open_allLayer();
void do_close_topLayer();
void do_close_belowLayer();
void do_close_allLayer();
 
void deal_sensorData(SensorData& data);


#endif