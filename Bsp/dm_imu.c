#include "dm_imu.h"
#include "stdio.h"

imu_t imu;
uint32_t count = 0;

uint8_t q_data[8] = {0};

static void IMU_SendQData(void);

/**
************************************************************************
* @brief:      	float_to_uint: ������ת��Ϊ�޷�����������
* @param[in]:   x_float:	��ת���ĸ�����
* @param[in]:   x_min:		��Χ��Сֵ
* @param[in]:   x_max:		��Χ���ֵ
* @param[in]:   bits: 		Ŀ���޷���������λ��
* @retval:     	�޷����������
* @details:    	�������ĸ����� x ��ָ����Χ [x_min, x_max] �ڽ�������ӳ�䣬ӳ����Ϊһ��ָ��λ�����޷�������
************************************************************************
**/
int float_to_uint(float x_float, float x_min, float x_max, int bits)
{
	/* Converts a float to an unsigned int, given range and number of bits */
	float span = x_max - x_min;
	float offset = x_min;
	return (int) ((x_float-offset)*((float)((1<<bits)-1))/span);
}
/**
************************************************************************
* @brief:      	uint_to_float: �޷�������ת��Ϊ����������
* @param[in]:   x_int: ��ת�����޷�������
* @param[in]:   x_min: ��Χ��Сֵ
* @param[in]:   x_max: ��Χ���ֵ
* @param[in]:   bits:  �޷���������λ��
* @retval:     	���������
* @details:    	���������޷������� x_int ��ָ����Χ [x_min, x_max] �ڽ�������ӳ�䣬ӳ����Ϊһ��������
************************************************************************
**/
float uint_to_float(int x_int, float x_min, float x_max, int bits)
{
	/* converts unsigned int to float, given range and number of bits */
	float span = x_max - x_min;
	float offset = x_min;
	return ((float)x_int)*span/((float)((1<<bits)-1)) + offset;
}

void IMU_RequestData(FDCAN_HandleTypeDef* hcan,uint16_t can_id,uint8_t reg)
{
	uint8_t cmd[4]={(uint8_t)can_id,(uint8_t)(can_id>>8),reg,0xCC};
	
	CAN_Transmit_STD(hcan, 0x6FF, cmd, 4);
}


void IMU_UpdateAccel(uint8_t* pData)
{
	uint16_t accel[3];
	
	accel[0]=pData[3]<<8|pData[2];
	accel[1]=pData[5]<<8|pData[4];
	accel[2]=pData[7]<<8|pData[6];
	
	imu.accel[0]=uint_to_float(accel[0],ACCEL_CAN_MIN,ACCEL_CAN_MAX,16);
	imu.accel[1]=uint_to_float(accel[1],ACCEL_CAN_MIN,ACCEL_CAN_MAX,16);
	imu.accel[2]=uint_to_float(accel[2],ACCEL_CAN_MIN,ACCEL_CAN_MAX,16);
	
}

void IMU_UpdateGyro(uint8_t* pData)
{
	uint16_t gyro[3];
	
	gyro[0]=pData[3]<<8|pData[2];
	gyro[1]=pData[5]<<8|pData[4];
	gyro[2]=pData[7]<<8|pData[6];
	
	imu.gyro[0]=uint_to_float(gyro[0],GYRO_CAN_MIN,GYRO_CAN_MAX,16);
	imu.gyro[1]=uint_to_float(gyro[1],GYRO_CAN_MIN,GYRO_CAN_MAX,16);
	imu.gyro[2]=uint_to_float(gyro[2],GYRO_CAN_MIN,GYRO_CAN_MAX,16);
}


void IMU_UpdateEuler(uint8_t* pData)
{
	int euler[3];
	
	euler[0]=pData[3]<<8|pData[2];
	euler[1]=pData[5]<<8|pData[4];
	euler[2]=pData[7]<<8|pData[6];
	
	imu.pitch=uint_to_float(euler[0],PITCH_CAN_MIN,PITCH_CAN_MAX,16);
	imu.yaw=uint_to_float(euler[1],YAW_CAN_MIN,YAW_CAN_MAX,16);
	imu.roll=uint_to_float(euler[2],ROLL_CAN_MIN,ROLL_CAN_MAX,16);
}


void IMU_UpdateQuaternion(uint8_t* pData)
{
	int w = pData[1]<<6| ((pData[2]&0xF8)>>2);
	int x = (pData[2]&0x03)<<12|(pData[3]<<4)|((pData[4]&0xF0)>>4);
	int y = (pData[4]&0x0F)<<10|(pData[5]<<2)|(pData[6]&0xC0)>>6;
	int z = (pData[6]&0x3F)<<8|pData[7];
	
	imu.q[0] = uint_to_float(w,Quaternion_MIN,Quaternion_MAX,14);
	imu.q[1] = uint_to_float(x,Quaternion_MIN,Quaternion_MAX,14);
	imu.q[2] = uint_to_float(y,Quaternion_MIN,Quaternion_MAX,14);
	imu.q[3] = uint_to_float(z,Quaternion_MIN,Quaternion_MAX,14);
}

void IMU_UpdateData(uint8_t* pData)
{

	switch(pData[0])
	{
		case 1:
			IMU_UpdateAccel(pData);
			break;
		case 2:
			IMU_UpdateGyro(pData);
			break;
		case 3:
			IMU_UpdateEuler(pData);
			// printf("%f,%f,%f\n",imu.yaw,imu.pitch,imu.roll);
			// uint8_t test = 0xFF;
			// HAL_UART_Transmit(&huart10,&test,1,0);
			break;
		case 4:
			IMU_UpdateQuaternion(pData);
			// memcpy(q_data,pData,8);
			// IMU_SendQData();
			break;
	}
}

//在云台自稳下，在底盘陀螺仪更新后更新云台应有的角度


float q_test[4];

void IMU_SendQData(void)
{
	int16_t x,y,z,w;
	uint16_t temp[4] = 0;
	uint8_t send[8] = {0};
	x = (q_data[2] & 0x03) << 12 | (q_data[3] << 4) | ((q_data[4] & 0xF0));
	y = (q_data[4] & 0x0F) << 10 | (q_data[5] << 2) | (q_data[6] & 0xC0);
	z = (q_data[6] & 0x3F) << 8 | q_data[7];
	w = (q_data[1]) << 6 | (q_data[2] & 0xFC);

	send[0] = x >> 8;
	send[1] = x & 0xFF;
	send[2] = y >> 8;
	send[3] = y & 0xFF;
	send[4] = z >> 8;
	send[5] = z & 0xFF;
	send[6] = w >> 8;
	send[7] = w & 0xFF;

	temp[0] = (send[0] << 8 | send[1]);
	temp[1] = (send[2] << 8 | send[3]);
	temp[2] = (send[4] << 8 | send[5]);
	temp[3] = (send[6] << 8 | send[7]);

	q_test[0] = uint_to_float(temp[0],Quaternion_MIN,Quaternion_MAX,14);
	q_test[1] = uint_to_float(temp[1],Quaternion_MIN,Quaternion_MAX,14);
	q_test[2] = uint_to_float(temp[2],Quaternion_MIN,Quaternion_MAX,14);
	q_test[3] = uint_to_float(temp[3],Quaternion_MIN,Quaternion_MAX,14);

	// if (temp[0] > 8191)
	// {
	// 	temp[0] -= 16384;
	// }
	// if (temp[1] > 8191)
	// {
	// 	temp[1] -= 16384;
	// }
	// if (temp[2] > 8191)
	// {
	// 	temp[2] -= 16384;
	// }
	// if (temp[3] > 8191)
	// {
	// 	temp[3] -= 16384;
	// }
	

	// q_test[0] = temp[0] / 16384.0f;
	// q_test[1] = temp[1] / 16384.0f;
	// q_test[2] = temp[2] / 16384.0f;
	// q_test[3] = temp[3] / 16384.0f;

	CAN_Transmit_STD(&hfdcan2, 0x100, send, 8);
}
