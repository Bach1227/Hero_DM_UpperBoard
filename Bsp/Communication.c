#include "Communication.h"

extern imu_t imu;
extern uint8_t AccelerationCompleted;

static int32_t floatToUint(float value, float min, float max, uint8_t bits)
{
    float span = max - min;
    float offset = min;
    if (bits == 32)
    {
        return (int) ((value-offset)*((float)(0xFFFFFFFF/span)));
    }
    else
    {
        return (int) ((value-offset)*((float)(((1<<bits) - 1)/span)));
    }
}

static float unitToFloat(int32_t value, float min, float max, uint8_t bits)
{
    float span = max - min;
    float offset = min;
    if (bits == 32)
    {
        return (value*(span/((float)0xFFFFFFFF))+offset);
    }
    else
    {
        return (value*(span/((float)(1<<bits) - 1))+offset);
    }
}

void Communication_SendPosInfo(void)
{
    //前四位pitch
    //后四位yaw
    float yaw_rad = imu.yaw / 180.0 * 3.1415926;
    float pitch_rad = imu.pitch / 180.0 * 3.1415926;
    int32_t yaw_uint = floatToUint(yaw_rad, -3.1415926, 3.1415926, 31);
    int32_t pitch_uint = floatToUint(pitch_rad, -3.1415926, 3.1415926, 31);
    uint8_t data[8];
    data[0] = yaw_uint >> 24;
    data[1] = (yaw_uint >> 16) & 0xFF;
    data[2] = (yaw_uint >> 8) & 0xFF;
    data[3] = yaw_uint & 0xFF;
    data[4] = pitch_uint >> 24;
    data[5] = (pitch_uint >> 16) & 0xFF;
    data[6] = (pitch_uint >> 8) & 0xFF;
    data[7] = pitch_uint & 0xFF;
    CAN_Transmit_STD(&hfdcan1, 0x101, data, 8);
}

void Communication_SendSpdInfo(void)
{
    uint32_t yaw_spd_uint = floatToUint(imu.gyro[2], -34.88, 34.88, 31);
    uint32_t pitch_spd_uint = floatToUint(imu.gyro[0], -34.88, 34.88, 31);
    uint8_t data[8];
    data[0] = yaw_spd_uint >> 24;
    data[1] = (yaw_spd_uint >> 16) & 0xFF;
    data[2] = (yaw_spd_uint >> 8) & 0xFF;
    data[3] = yaw_spd_uint & 0xFF;
    data[4] = pitch_spd_uint >> 24;
    data[5] = (pitch_spd_uint >> 16) & 0xFF;
    data[6] = (pitch_spd_uint >> 8) & 0xFF;
    data[7] = pitch_spd_uint & 0xFF;
    CAN_Transmit_STD(&hfdcan1, 0x102, data, 8);
}

void 
Communication_SendTargetInfo(uint8_t* data)	
{
    //把收到的消息转发给下板
    data[0] = ((data[0] & 0x01) << 1) | (data[1] & 0x01);
    
    if (AccelerationCompleted == 1)
    {
        data[1] = 0x01;
    }
    else
    {
        data[1] = 0x00;
    }
    CAN_Transmit_STD(&hfdcan1, 0xFF, data, 8);
    
}

void Communication_Parse(uint8_t* data)
{
    //前两位为速度最大值
    //后两位为映射值
    float max_speed = (float)((data[0] << 8) | data[1]);
    float temp = (data[2] << 8) | data[3];
    float speed = unitToFloat(temp, -max_speed, max_speed, 16);
}

void Commu_GyroDataRequeset_Task(void* arg)
{
    while (1)
    {
        IMU_RequestData(&hfdcan3, 0x02, 3);
        IMU_RequestData(&hfdcan3, 0x02, 4);
        // uint8_t data[8] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08};
        // HAL_UART_Transmit(&huart10, data, 8, 1);
        // printf("%f,%f,%f\n",imu.yaw,imu.pitch,imu.roll);
        vTaskDelay(0);
    }
}

void Commu_GyroRequest_Init(void)
{
    xTaskCreate(Commu_GyroDataRequeset_Task, "Commu_GyroDataRequeset_Task", 512, NULL, 1, NULL);
}
//缓冲区接收数据

void Commu_JudgeDataSend_Task(void* arg)
{
    while (1)
    {
        xTaskNotifyWait(0x00000001, 0x00000000, NULL, portMAX_DELAY);
        
    }
    
}