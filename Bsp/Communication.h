#ifndef _COMMUNICATION_H_
#define _COMMUNICATION_H_

#include "stm32h7xx_hal.h"
#include "fdcan.h"
#include "usart.h"
#include "dm_imu.h"
#include "CAN_Basic.h"
#include "FrictionController.h"

#include "DataStructure.h"

#include "FreeRTOS.h"
#include "cmsis_os2.h"
#include "task.h"
#include "event_groups.h"
#include "croutine.h"
#include "semphr.h"
#include "stream_buffer.h"

void Communication_SendPosInfo(void);
void Communication_SendSpdInfo(void);
void Commu_GyroRequest_Init(void);
void Communication_SendTargetInfo(uint8_t* data);

typedef struct
{
    uint8_t* buffer;
    uint8_t* buffer_head;
    uint8_t* buffer_tail;
    uint16_t buffer_size;
}ringbuffer_t;

void RingBuffer_Read(void);

#endif
