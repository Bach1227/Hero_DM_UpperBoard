#ifndef __INTERRUPTHANDLER_H
#define __INTERRUPTHANDLER_H

//BSP
#define STM32H7
#define FD_CAN
#include "tim.h"
#include "CAN_DmMotor.h"

#ifdef STM32F1
#include "stm32f1xx_hal.h"
#endif // DEBUG

#ifdef STM32F4
#include "stm32f4xx_hal.h"
#endif // DEBUG

#ifdef STM32H7
#include "stm32h7xx_hal.h"
#endif // DEBUG

#ifdef FD_CAN
#include "fdcan.h"
#define CAN_Handler1           hfdcan1
#endif // DEBUG

#include "dm_imu.h"
#include "Communication.h"
#include "FrictionController.h"

#include "FreeRTOS.h"
#include "cmsis_os2.h"
#include "task.h"
#include "event_groups.h"
#include "croutine.h"
#include "semphr.h"
#include "stream_buffer.h"

void InterruptInit(void);
void FrictionController_SendIsFrictionReady(void);


#endif // !__INTERRUPTHANDLER_H
