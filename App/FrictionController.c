#include "FrictionController.h"

#define Accelation_Period 2000 // 1s
#define FrictionTargetSpeedDef 10

float TargetSpeed = 0;
uint8_t Friction_Enable = 0;
float Friction_SetSpeed = 0;
float FrictionTargetSpeed = 0;
float yaw_gimbal = 0;

uint8_t IsFriciton1Online = 0;
uint8_t IsFriciton2Online = 0;
uint8_t IsFriciton3Online = 0;
uint8_t IsFriciton4Online = 0;

uint8_t AccelerationCompleted = 0;
uint8_t FrictionMesNeedSend = 0;
uint8_t FrictionState = 0;

SlinePlanner_t FrictionSline;

TaskHandle_t InitTaskHandle;

extern imu_t imu;
imu_t test_imu;
float q[4] = {0};
uint16_t uintq[4] = {0};
uint8_t send[8] = {0};

uint8_t PC_data[8] = {0};

float roll_rad;
float pitch_rad;
float yaw_rad;

uint8_t AnamyColor = 0;

void MoveControl_SlinePlannerInit(SlinePlanner_t* param, uint32_t Period, float Threshold);
void FrictionMotorInit_Task(void* arg);
void quaternion_to_euler_wxyz(const float q[4], imu_t* imu);

void PC_data_record(uint8_t* data)
{
    for(uint8_t i = 0; i < 8; i++)
    {
        PC_data[i] = data[i];
    }
}

static float MoveControl_SLine_BaseFunction(float delta)
{
        // float delta = (float)time / Accelation_Period;

    if (delta >= 1)
    {
        return 1;
    }

    // float x3 = delta * delta * delta;
    // float x4 = x3 * delta;
    // float x5 = x4 * delta;
    
    //三次型
    float x2 = delta * delta;
    float x3 = x2 * delta;
    return 3 * x2 - 2 * x3;

    // return 10 * x3 - 15 * x4 + 6 * x5;
}

void MoveControl_SlinePlannerInit(SlinePlanner_t* param, uint32_t Period, float Threshold)
{
    param->Accelation_Time = Period;
    param->Speed_Threshold = Threshold;
    param->Current_Time = 0;
    param->Current_Speed = 0;
}

void MoveControl_SpeedPlanning(SlinePlanner_t* param)
{
    // 1. 判断目标速度是否发生显著变化
    if (param->Target_Speed - param->LastTarget_Speed > param->Speed_Threshold || 
        param->Target_Speed - param->LastTarget_Speed < -param->Speed_Threshold)
    {
        // 目标改变，重置时间，并把【当前瞬间的速度】锁定为新 S 曲线的【起点】
        param->Current_Time = 0;
        param->Start_Speed = param->Current_Speed;
    }
    else
    {
        // 目标未变，时间累加。加入防溢出保护，防止到达目标后时间变量无限累加
        if (param->Current_Time < param->Accelation_Time) 
        {
            param->Current_Time++;
        }
    }

    // 2. 计算当前时间的归一化比例 (0.0 ~ 1.0)
    // 增加防除零保护，提高嵌入式代码的健壮性
    float time_ratio = 1.0f;
    if (param->Accelation_Time > 0) 
    {
        time_ratio = (float)param->Current_Time / param->Accelation_Time;
    }

    // 3. 计算 S 型系数 (0.0 ~ 1.0)
    float s_curve_ratio = MoveControl_SLine_BaseFunction(time_ratio);

    // 4. 核心计算：起点 + (总差值 * 比例)
    // 【关键修复】：这里必须用 Target_Speed - Start_Speed ！！！
    param->Current_Speed = param->Start_Speed + 
                           (param->Target_Speed - param->Start_Speed) * s_curve_ratio;

    // 5. 更新历史目标值，用于下一次判断
    param->LastTarget_Speed = param->Target_Speed;
}

void Friction_Online(uint8_t id)
{
    switch (id)
    {
        case 0x01:
            IsFriciton1Online = 1;
            break;
        case 0x02:
            IsFriciton2Online = 1;
            break;
        case 0x03:
            IsFriciton3Online = 1;
            break;
        case 0x04:
            IsFriciton4Online = 1;
            break;
        default:
            break;
    }
}

void FrictionController_Init(void)
{
    xTaskCreate(FrictionMotorInit_Task, "FrictionMotorInit_Task", 128, NULL, osPriorityAboveNormal5, &InitTaskHandle);
}

//最好的方式是开一个定时器
void FrictionController_SetSpeed(void)
{   
    if (Friction_Enable == 1)
    {
        FrictionSline.Target_Speed = 18;
        MoveControl_SpeedPlanning(&FrictionSline);
    }
    else
    {
        FrictionSline.Target_Speed = 0;
        MoveControl_SpeedPlanning(&FrictionSline);
    }

    switch (FrictionState)
    {
    case 0: //速度不到位
        if (FrictionSline.Current_Speed - FrictionTargetSpeedDef > -0.5)
        {
            // FrictionState = 1;
            AccelerationCompleted = 1;
        }
        else
        {
            AccelerationCompleted = 0;
        }
        break;

    case 1: //速度到位，需要发送指令
        FrictionMesNeedSend = 1;
        break;

    case 2: //速度到位，且指令已发送
        break;

    case 3: //速度不到位，发送失能指令
    
    default:
        break;
    }

    if (IsFriciton1Online && IsFriciton2Online && IsFriciton3Online && IsFriciton4Online)
    {
        DM_Motor_MIT_3519_SetSpeed(&hfdcan2, 0x20, FrictionSline.Current_Speed);
        DM_Motor_MIT_3519_SetSpeed(&hfdcan2, 0x21, FrictionSline.Current_Speed);
        DM_Motor_MIT_3519_SetSpeed(&hfdcan2, 0x22, -FrictionSline.Current_Speed);
        DM_Motor_MIT_3519_SetSpeed(&hfdcan2, 0x23, -FrictionSline.Current_Speed);
    }

    // printf("Speed: %f\n", speed);
    
}

void Friction_MsgSended(void)
{
    if (FrictionState == 1)
    {
        FrictionState = 2;
    }
    else if (FrictionState == 3)
    {
        FrictionState = 0;
    }
    
}

void FrictionController_Enable(void)
{
    Friction_Enable = 1;
}

void FrictionController_Disable(void)
{
    Friction_Enable = 0;
}

void FrictionController_EnableReverse(void)
{
    Friction_Enable = !Friction_Enable;
}

int32_t floatToUnit(float value, float min, float max, uint8_t bits)
{
    float span = max - min;
    float offset = min;
    return (int) ((value-offset)*((float)((1<<bits)-1))/span);
}

float unitToFloat(int32_t value, float min, float max, uint8_t bits)
{
    float span = max - min;
    float offset = min;
    return (value*(span/((float)((1<<bits)-1)))+offset);
}

void normalize_quaternion(float* q) 
{
    float norm = sqrtf(q[0] * q[0] + q[1] * q[1] + q[2] * q[2] + q[3] * q[3]);
    if (norm != 0)
    {
        q[0] = q[0] / norm;
        q[1] = q[1] / norm;
        q[2] = q[2] / norm;
        q[3] = q[3] / norm;
    }
}   

float ToRad(float angle)
{
    return angle * 3.1415926 / 180.0;
}

float ToAngle(float rad)
{
    return rad * 180.0 / 3.1415926;
}

void euler_to_quaternion_wxyz(float roll, float pitch, float yaw, float* q) {
    float cr = cosf(roll * 0.5f);
    float sr = sinf(roll * 0.5f);
    float cp = cosf(pitch * 0.5f);
    float sp = sinf(pitch * 0.5f);
    float cy = cosf(yaw * 0.5f);
    float sy = sinf(yaw * 0.5f);

    q[0] = cr * cp * cy + sr * sp * sy;  // w
    q[1] = sr * cp * cy - cr * sp * sy;  // x
    q[2] = cr * sp * cy + sr * cp * sy;  // y
    q[3] = cr * cp * sy - sr * sp * cy;  // z

    normalize_quaternion(q);
}

void quaternion_to_euler_wxyz(const float q[4], imu_t* imu) {
    float w = q[0], x = q[1], y = q[2], z = q[3];
    float sinr_cosp = 2.0f * (w * x + y * z);
    float cosr_cosp = 1.0f - 2.0f * (x*x + y*y);
    imu->roll = ToAngle(atan2f(sinr_cosp, cosr_cosp));
    

    float sinp = 2.0f * (w * y - z * x);
    if (sinp >= 1.0f) imu->pitch = 3.14159265358979323846f / 2.0f;
    else if (sinp <= -1.0f) imu->pitch = -3.14159265358979323846f / 2.0f;
    else imu->pitch = ToAngle(asinf(sinp));

    float siny_cosp = 2.0f * (w * z + x * y);
    float cosy_cosp = 1.0f - 2.0f * (y*y + z*z);
    imu->yaw = ToAngle(atan2f(siny_cosp, cosy_cosp));
}

void FrictionController_TransmitData(void)
{
    uint8_t temp[8] = {0};
    
    for (uint8_t i = 0; i < 8; i++)
    {
        temp[i] = PC_data[i];
    }
    
    temp[0] = ((temp[0] & 0x01) << 1) | (temp[1] & 0x01);
    
    if (AccelerationCompleted == 1)
    {
        temp[1] = 0x01;
    }
    else
    {
        temp[1] = 0x00;
    }
    CAN_Transmit_STD(&hfdcan1, 0xFF, temp, 8);
}

void FrictionController_Parse(uint8_t *data)
{
    // uint8_t temp[8] = {0xFF, 0xFF,0xFF, 0xFF,0xFF, 0xFF,0xFF, 0xFF};
    uint8_t msg[8];
    yaw_gimbal = unitToFloat(data[0]<<8 | data[1], -180, 180, 16);
    // printf("yaw_gimbal: %d,%d\n", data[0],data[1]);
    // printf("yaw_gimbal: %f\n", yaw_gimbal);

    if (data[3] == 1)
    {
        AnamyColor = 1;
    }
    else if (data[3] == 2)
    {
        AnamyColor =2;
    }
    else
    {
        AnamyColor = 0;
    }

    // roll_rad = ToRad(imu.roll);
    // pitch_rad = ToRad(imu.pitch);
    // yaw_rad = ToRad(yaw_gimbal);
    // euler_to_quaternion_wxyz(roll_rad, pitch_rad, yaw_rad, q);
    // quaternion_to_euler_wxyz(q, &test_imu);
    // uintq[0] = floatToUnit(q[0], -1, 1, 16);
    // uintq[1] = floatToUnit(q[1], -1, 1, 16);
    // uintq[2] = floatToUnit(q[2], -1, 1, 16);
    // uintq[3] = floatToUnit(q[3], -1, 1, 16);
    // send[0] = uintq[0] >> 8;
    // send[1] = uintq[0] & 0xff;
    // send[2] = uintq[1] >> 8;
    // send[3] = uintq[1] & 0xff;
    // send[4] = uintq[2] >> 8;
    // send[5] = uintq[2] & 0xff;
    // send[6] = uintq[3] >> 8;
    // send[7] = uintq[3] & 0xff;
    // CAN_Transmit_STD(&hfdcan2, canid, send, 8);
    // CAN_Transmit_STD(&hfdcan2, 0x105, msg, 8);
    if (data[2] == 1)
    {
        FrictionController_Enable();
    }
    else
    {
        FrictionController_Disable();
    }
    
    // 顺序：wxyz
    // FrictionTargetSpeed = unitToFloat(data[2]<<8 | data[3], 0, 100, 16);
    // FrictionController_SetSpeed();
}

void FrictionController_SendIsFrictionReady(void)
{
    if (FrictionMesNeedSend == 1)
    {
        if (FrictionState == 1)
        {
            uint8_t temp[2] = {0x01, 0xFF};
             CAN_Transmit_STD(&hfdcan2, 0x110, temp, 2);
            FrictionState = 2;
        }
        else if(FrictionState == 3)
        {
            uint8_t temp[2] = {0xFF, 0x01};
            CAN_Transmit_STD(&hfdcan2, 0x110, temp, 2);
            FrictionState = 0;
        }
        FrictionMesNeedSend = 0;
    }
}

void SendAngle(void)
{
    // uint8_t temp[8] = {0xFF, 0xFF,0xFF, 0xFF,0xFF, 0xFF,0xFF, 0xFF};

    uint16_t canid = 0x123;

    if (AnamyColor == 1)
    {
        canid = 0x100;
    }
    else if (AnamyColor == 2)
    {
        canid = 0x101;
    }
    else
    {
        canid = 0x123;
    }
    
    

    roll_rad = ToRad(imu.roll);
    pitch_rad = ToRad(imu.pitch);
    yaw_rad = ToRad(yaw_gimbal);
    euler_to_quaternion_wxyz(roll_rad, pitch_rad, yaw_rad, q);
    quaternion_to_euler_wxyz(q, &test_imu);
    uintq[0] = floatToUnit(q[0], -1, 1, 16);
    uintq[1] = floatToUnit(q[1], -1, 1, 16);
    uintq[2] = floatToUnit(q[2], -1, 1, 16);
    uintq[3] = floatToUnit(q[3], -1, 1, 16);
    send[0] = uintq[0] >> 8;
    send[1] = uintq[0] & 0xff;
    send[2] = uintq[1] >> 8;
    send[3] = uintq[1] & 0xff;
    send[4] = uintq[2] >> 8;
    send[5] = uintq[2] & 0xff;
    send[6] = uintq[3] >> 8;
    send[7] = uintq[3] & 0xff;
    CAN_Transmit_STD(&hfdcan2, canid, send, 8);
    // CAN_Transmit_STD(&hfdcan2, 0x120, temp, 8);
}

void FrictionMotorInit_Task(void* arg)
{
    MoveControl_SlinePlannerInit(&FrictionSline, 500, 0.5);
    while (IsFriciton1Online != 1 || IsFriciton2Online != 1 || IsFriciton3Online != 1 || IsFriciton4Online != 1)
    {
        if (IsFriciton1Online != 1)
        {
            DM_Motor_Enable(&hfdcan2, 0x20);
        }
        if (IsFriciton2Online != 1)
        {
            DM_Motor_Enable(&hfdcan2, 0x21);
        }
        if (IsFriciton3Online != 1)
        {
            DM_Motor_Enable(&hfdcan2, 0x22);
        }
        if (IsFriciton4Online != 1)
        {
            DM_Motor_Enable(&hfdcan2, 0x23);
        }
        // HAL_Delay(100);
        // Osdelay(100);
        osDelay(100);
        
        DM_Motor_Enable(&hfdcan2, 0x20);
        DM_Motor_Enable(&hfdcan2, 0x21);
        DM_Motor_Enable(&hfdcan2, 0x22);
        DM_Motor_Enable(&hfdcan2, 0x23);
        // }
    }
    vTaskDelete(NULL);
}

