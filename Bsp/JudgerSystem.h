#ifndef JUDGERSYSTEM_H
#define JUDGERSYSTEM_H

#include "stm32h7xx_hal.h"
#include "usart.h"

#include "stdio.h"
#include "string.h"

#include "FreeRTOS.h"
#include "cmsis_os2.h"
#include "task.h"
#include "event_groups.h"
#include "croutine.h"
#include "semphr.h"
#include "stream_buffer.h"

#include "CAN_Basic.h"

#define VTM_RC_CH_MIN 364
#define VTM_RC_CH_MID 1024
#define VTM_RC_CH_MAX 1684

// 键盘按键位映射宏定义 (依据偏移136处定义) [cite: 358]
#define VTM_KEY_W      (1 << 0)
#define VTM_KEY_S      (1 << 1)
#define VTM_KEY_A      (1 << 2)
#define VTM_KEY_D      (1 << 3)
#define VTM_KEY_SHIFT  (1 << 4)
#define VTM_KEY_CTRL   (1 << 5)
#define VTM_KEY_Q      (1 << 6)
#define VTM_KEY_E      (1 << 7)
#define VTM_KEY_R      (1 << 8)
#define VTM_KEY_F      (1 << 9)
#define VTM_KEY_G      (1 << 10)
#define VTM_KEY_Z      (1 << 11)
#define VTM_KEY_X      (1 << 12)
#define VTM_KEY_C      (1 << 13)
#define VTM_KEY_V      (1 << 14)
#define VTM_KEY_B      (1 << 15)

#define RC_CH_MID_FLOAT 1024.0f // 摇杆中值 [cite: 164]
#define RC_CH_RANGE     660.0f  // 摇杆单侧最大行程 (1684 - 1024) [cite: 164]

// --- 速度映射极值 ---
#define MAX_SPEED_XY    100.0f  // X轴与Y轴最大速度
#define MAX_SPEED_W     5.0f    // W轴(旋转)最大速度

// --- 摇杆基础参数 ---
#define RC_CH_MID_FLOAT 1024.0f // 摇杆中值 [cite: 164]
#define RC_CH_RANGE     660.0f  // 摇杆单侧最大行程 (1684 - 1024) [cite: 164]

// --- 映射与灵敏度参数 ---
#define MAX_OUTPUT_SPEED   100.0f // 最终输出的最大速度限制
#define MOUSE_SENSITIVITY  0.5f   // 鼠标灵敏度系数 (根据实际 DPI 和手感微调)
#define JOYSTICK_WEIGHT    1.0f   // 摇杆权重系数

// --- 数据打包精度缩放系数 ---
// 如果您只需要纯整数，可将以下宏全改为 1.0f
#define PACK_SCALE_XY   100.0f  // 放大100倍: 100.0 -> 10000 (安全存入int16_t)
#define PACK_SCALE_W    1000.0f // 放大1000倍: 5.0 -> 5000 (安全存入int16_t)

// 限幅宏，防止数值异常越界
#define CLAMP(val, min, max) ((val) > (max) ? (max) : ((val) < (min) ? (min) : (val)))

// 图传模块遥控数据解析结构体
typedef struct {
    // 摇杆通道 (11 bit) [cite: 356]
    uint16_t ch0;         // 右摇杆水平
    uint16_t ch1;         // 右摇杆竖直
    uint16_t ch2;         // 左摇杆竖直
    uint16_t ch3;         // 左摇杆水平

    // 开关与按键 (小位宽) [cite: 356]
    uint8_t mode_switch;  // 挡位切换开关 (2 bit): 0(C), 1(N), 2(S)
    uint8_t pause_btn;    // 暂停按键 (1 bit): 0未按, 1按下
    uint8_t custom_l;     // 自定义按键左 (1 bit): 0未按, 1按下
    uint8_t custom_r;     // 自定义按键右 (1 bit): 0未按, 1按下

    // 拨轮与扳机 [cite: 358]
    uint16_t dial;        // 拨轮 (11 bit)
    uint8_t trigger;      // 扳机键 (1 bit): 0未按, 1按下

    // 鼠标数据 [cite: 358]
    int16_t mouse_x;      // 鼠标 X 轴移动速度 (有符号16位)
    int16_t mouse_y;      // 鼠标 Y 轴移动速度 (有符号16位)
    int16_t mouse_z;      // 鼠标 Z 轴滚轮速度 (有符号16位)
    uint8_t mouse_l;      // 鼠标左键 (2 bit)
    uint8_t mouse_r;      // 鼠标右键 (2 bit)
    uint8_t mouse_m;      // 鼠标中键 (2 bit)

    // 键盘数据 [cite: 358]
    uint16_t keyboard;    // 键盘按键状态集 (16 bit)
} VTM_RC_Data_t;

typedef struct 
{ 
  uint8_t robot_id; 
  uint8_t robot_level; 
  uint16_t current_HP;  
  uint16_t maximum_HP; 
  uint16_t shooter_barrel_cooling_value; 
  uint16_t shooter_barrel_heat_limit; 
  uint16_t chassis_power_limit;  
  uint8_t power_management_gimbal_output : 1; 
  uint8_t power_management_chassis_output : 1;  
  uint8_t power_management_shooter_output : 1; 
}robot_status_t; 

typedef struct 
{ 
  uint16_t reserved; 
  uint16_t buffer_energy; 
  uint16_t shooter_17mm_1_barrel_heat; 
  uint16_t shooter_42mm_barrel_heat; 
}power_heat_data_t; 

typedef struct 
{ 
  uint8_t bullet_type;  
  uint8_t shooter_number; 
  uint8_t launching_frequency;  
  uint32_t initial_speed;  
}shoot_data_t; 

typedef struct
{
  int16_t rc_l_x;
  int16_t rc_l_y;
  int16_t rc_l_z;

  int16_t rc_r_x;
  int16_t rc_r_y;
  int16_t rc_r_z;

  
  int16_t mouse_spd_x;
  int16_t mouse_spd_y;
  int16_t mouse_spd_z;
}control_data_t;



#define JudgeSystem_DataOffset 7

void JudgeSystem_Init(void);
void JudgerSystem_Receive_Callback(void);
void JudgeSystem_VtParseNotify(void);

void JudgeSystem_ChassisControlData_Pact(control_data_t* control_data, uint8_t* pacted_data);
void JudgeSystem_GimbalControlData_Pact(control_data_t* control_data, uint8_t* pacted_data);

#endif