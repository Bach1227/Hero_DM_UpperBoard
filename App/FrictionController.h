#ifndef FRICTIONCONTROLLER_H
#define FRICTIONCONTROLLER_H

#include "stm32h7xx_hal.h"
#include "CAN_Basic.h"
#include "CAN_DmMotor.h"
#include "FrictionController.h"
#include "stdio.h"
#include "math.h"
#include "dm_imu.h"

#include "FreeRTOS.h"
#include "cmsis_os2.h"
#include "task.h"
#include "event_groups.h"
#include "croutine.h"
#include "semphr.h"
#include "stream_buffer.h"

typedef struct
{
    float Start_Speed;
    float Target_Speed;
    float LastTarget_Speed;
    float Current_Speed;

    uint32_t Accelation_Time;
    uint32_t Current_Time;

    float Speed_Threshold;
}SlinePlanner_t;

void FrictionController_Init(void);
void FrictionController_SetSpeed(void);
void FrictionController_EnableReverse(void);
void FrictionController_Parse(uint8_t *data);
void FrictionController_Enable(void);
void FrictionController_Disable(void);
void FrictionController_SendIsFrictionReady(void);
void PC_data_record(uint8_t* data);
void Friction_Online(uint8_t id);
void FrictionController_TransmitData(void);

void SendAngle(void);

#endif

