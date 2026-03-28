#include "JudgerSystem.h"  

void JudgeSystem_BasicDataParse(uint8_t* data, uint16_t length);
TimerHandle_t ResetTimerHandle;

//crc8 generator polynomial:G(x)=x8+x5+x4+1  
const unsigned char CRC8_INIT = 0xff;  
const unsigned char CRC8_TAB[256] =  
{  
0x00, 0x5e, 0xbc, 0xe2, 0x61, 0x3f, 0xdd, 0x83, 0xc2, 0x9c, 0x7e, 0x20, 0xa3, 0xfd, 0x1f, 0x41,  
0x9d, 0xc3, 0x21, 0x7f, 0xfc, 0xa2, 0x40, 0x1e, 0x5f, 0x01, 0xe3, 0xbd, 0x3e, 0x60, 0x82, 0xdc, 0x23, 
0x7d, 0x9f, 0xc1, 0x42, 0x1c, 0xfe, 0xa0, 0xe1, 0xbf, 0x5d, 0x03, 0x80, 0xde, 0x3c, 0x62, 0xbe, 0xe0, 
0x02, 0x5c, 0xdf, 0x81, 0x63, 0x3d, 0x7c, 0x22, 0xc0, 0x9e, 0x1d, 0x43, 0xa1, 0xff, 0x46, 0x18, 0xfa, 
0xa4, 0x27, 0x79, 0x9b, 0xc5, 0x84, 0xda, 0x38, 0x66, 0xe5, 0xbb, 0x59, 0x07, 0xdb, 0x85, 0x67, 0x39, 
0xba, 0xe4, 0x06, 0x58, 0x19, 0x47, 0xa5, 0xfb, 0x78, 0x26, 0xc4, 0x9a, 0x65, 0x3b, 0xd9, 0x87, 0x04, 
0x5a, 0xb8, 0xe6, 0xa7, 0xf9, 0x1b, 0x45, 0xc6, 0x98, 0x7a, 0x24, 0xf8, 0xa6, 0x44, 0x1a, 0x99, 0xc7, 
0x25, 0x7b, 0x3a, 0x64, 0x86, 0xd8, 0x5b, 0x05, 0xe7, 0xb9,  
0x8c, 0xd2, 0x30, 0x6e, 0xed, 0xb3, 0x51, 0x0f, 0x4e, 0x10, 0xf2, 0xac, 0x2f, 0x71, 0x93, 0xcd, 0x11, 
0x4f, 0xad, 0xf3, 0x70, 0x2e, 0xcc, 0x92, 0xd3, 0x8d, 0x6f, 0x31, 0xb2, 0xec, 0x0e, 0x50, 0xaf, 0xf1, 
0x13, 0x4d, 0xce, 0x90, 0x72, 0x2c, 0x6d, 0x33, 0xd1, 0x8f, 0x0c, 0x52, 0xb0, 0xee, 0x32, 0x6c, 0x8e, 
0xd0, 0x53, 0x0d, 0xef, 0xb1, 0xf0, 0xae, 0x4c, 0x12, 0x91, 0xcf, 0x2d, 0x73, 0xca, 0x94, 0x76, 0x28, 
0xab, 0xf5, 0x17, 0x49, 0x08, 0x56, 0xb4, 0xea, 0x69, 0x37, 0xd5, 0x8b, 0x57, 0x09, 0xeb, 0xb5, 0x36, 
0x68, 0x8a, 0xd4, 0x95, 0xcb, 0x29, 0x77, 0xf4, 0xaa, 0x48, 0x16, 0xe9, 0xb7, 0x55, 0x0b, 0x88, 0xd6, 
0x34, 0x6a, 0x2b, 0x75, 0x97, 0xc9, 0x4a, 0x14, 0xf6, 0xa8,  
0x74, 0x2a, 0xc8, 0x96, 0x15, 0x4b, 0xa9, 0xf7, 0xb6, 0xe8, 0x0a, 0x54, 0xd7, 0x89, 0x6b, 0x35,  
}; 
unsigned char Get_CRC8_Check_Sum(unsigned char *pchMessage,unsigned int 
dwLength,unsigned char ucCRC8)  
{  
unsigned char ucIndex;  
while (dwLength--)  
{  
    ucIndex = ucCRC8^(*pchMessage++);  
    ucCRC8 = CRC8_TAB[ucIndex];  
}  
return(ucCRC8);  
} 
/*  
** Descriptions: CRC8 Verify function  
** Input: Data to Verify,Stream length = Data + checksum  
** Output: True or False (CRC Verify Result)  
*/  
unsigned int Verify_CRC8_Check_Sum(unsigned char *pchMessage, unsigned int dwLength)  
{  
    unsigned char ucExpected = 0; 
    if ((pchMessage == 0) || (dwLength <= 2)) return 0;  
    ucExpected = Get_CRC8_Check_Sum (pchMessage, dwLength-1, CRC8_INIT);  
    return ( ucExpected == pchMessage[dwLength-1] );  
}  
/*  
** Descriptions: append CRC8 to the end of data  
** Input: Data to CRC and append,Stream length = Data + checksum  
** Output: True or False (CRC Verify Result)  
*/  
void Append_CRC8_Check_Sum(unsigned char *pchMessage, unsigned int dwLength)  
{  
    unsigned char ucCRC = 0;  
    if ((pchMessage == 0) || (dwLength <= 2)) return;  
    ucCRC = Get_CRC8_Check_Sum ( (unsigned char *)pchMessage, dwLength-1, CRC8_INIT);  
    pchMessage[dwLength-1] = ucCRC;  
} 
 
uint16_t CRC_INIT = 0xffff;  
const uint16_t wCRC_Table[256] =  
{  
0x0000, 0x1189, 0x2312, 0x329b, 0x4624, 0x57ad, 0x6536, 0x74bf,  
0x8c48, 0x9dc1, 0xaf5a, 0xbed3, 0xca6c, 0xdbe5, 0xe97e, 0xf8f7,  
0x1081, 0x0108, 0x3393, 0x221a, 0x56a5, 0x472c, 0x75b7, 0x643e,  
0x9cc9, 0x8d40, 0xbfdb, 0xae52, 0xdaed, 0xcb64, 0xf9ff, 0xe876,  
0x2102, 0x308b, 0x0210, 0x1399, 0x6726, 0x76af, 0x4434, 0x55bd,  
0xad4a, 0xbcc3, 0x8e58, 0x9fd1, 0xeb6e, 0xfae7, 0xc87c, 0xd9f5,  
0x3183, 0x200a, 0x1291, 0x0318, 0x77a7, 0x662e, 0x54b5, 0x453c,  
0xbdcb, 0xac42, 0x9ed9, 0x8f50, 0xfbef, 0xea66, 0xd8fd, 0xc974,  
0x4204, 0x538d, 0x6116, 0x709f, 0x0420, 0x15a9, 0x2732, 0x36bb,  
0xce4c, 0xdfc5, 0xed5e, 0xfcd7, 0x8868, 0x99e1, 0xab7a, 0xbaf3,  
0x5285, 0x430c, 0x7197, 0x601e, 0x14a1, 0x0528, 0x37b3, 0x263a, 
0xdecd, 0xcf44, 0xfddf, 0xec56, 0x98e9, 0x8960, 0xbbfb, 0xaa72,  
0x6306, 0x728f, 0x4014, 0x519d, 0x2522, 0x34ab, 0x0630, 0x17b9,  
0xef4e, 0xfec7, 0xcc5c, 0xddd5, 0xa96a, 0xb8e3, 0x8a78, 0x9bf1,  
0x7387, 0x620e, 0x5095, 0x411c, 0x35a3, 0x242a, 0x16b1, 0x0738,  
0xffcf, 0xee46, 0xdcdd, 0xcd54, 0xb9eb, 0xa862, 0x9af9, 0x8b70,  
0x8408, 0x9581, 0xa71a, 0xb693, 0xc22c, 0xd3a5, 0xe13e, 0xf0b7,  
0x0840, 0x19c9, 0x2b52, 0x3adb, 0x4e64, 0x5fed, 0x6d76, 0x7cff,  
0x9489, 0x8500, 0xb79b, 0xa612, 0xd2ad, 0xc324, 0xf1bf, 0xe036,  
0x18c1, 0x0948, 0x3bd3, 0x2a5a, 0x5ee5, 0x4f6c, 0x7df7, 0x6c7e,  
0xa50a, 0xb483, 0x8618, 0x9791, 0xe32e, 0xf2a7, 0xc03c, 0xd1b5,  
0x2942, 0x38cb, 0x0a50, 0x1bd9, 0x6f66, 0x7eef, 0x4c74, 0x5dfd,  
0xb58b, 0xa402, 0x9699, 0x8710, 0xf3af, 0xe226, 0xd0bd, 0xc134,  
0x39c3, 0x284a, 0x1ad1, 0x0b58, 0x7fe7, 0x6e6e, 0x5cf5, 0x4d7c,  
0xc60c, 0xd785, 0xe51e, 0xf497, 0x8028, 0x91a1, 0xa33a, 0xb2b3,  
0x4a44, 0x5bcd, 0x6956, 0x78df, 0x0c60, 0x1de9, 0x2f72, 0x3efb,  
0xd68d, 0xc704, 0xf59f, 0xe416, 0x90a9, 0x8120, 0xb3bb, 0xa232,  
0x5ac5, 0x4b4c, 0x79d7, 0x685e, 0x1ce1, 0x0d68, 0x3ff3, 0x2e7a,  
0xe70e, 0xf687, 0xc41c, 0xd595, 0xa12a, 0xb0a3, 0x8238, 0x93b1,  
0x6b46, 0x7acf, 0x4854, 0x59dd, 0x2d62, 0x3ceb, 0x0e70, 0x1ff9,  
0xf78f, 0xe606, 0xd49d, 0xc514, 0xb1ab, 0xa022, 0x92b9, 0x8330,  
0x7bc7, 0x6a4e, 0x58d5, 0x495c, 0x3de3, 0x2c6a, 0x1ef1, 0x0f78 
};
/*  
** Descriptions: CRC16 checksum function  
** Input: Data to check,Stream length, initialized checksum  
** Output: CRC checksum  
*/  
uint16_t Get_CRC16_Check_Sum(uint8_t *pchMessage,uint32_t dwLength,uint16_t wCRC)  
{  
uint8_t chData;  
if (pchMessage == NULL)  
{  
return 0xFFFF;  
}  
while(dwLength--)  
{  
chData = *pchMessage++; 
(wCRC) = ((uint16_t)(wCRC) >> 8) ^ wCRC_Table[((uint16_t)(wCRC) ^ (uint16_t)(chData)) & 0x00ff];  
}  
return wCRC;  
} 

/*  
** Descriptions: CRC16 Verify function  
** Input: Data to Verify,Stream length = Data + checksum  
** Output: True or False (CRC Verify Result)  
*/  
uint32_t Verify_CRC16_Check_Sum(uint8_t *pchMessage, uint32_t dwLength)  
{  
uint16_t wExpected = 0;  
if ((pchMessage == NULL) || (dwLength <= 2))  
{  
return 0;  
}  
wExpected = Get_CRC16_Check_Sum ( pchMessage, dwLength - 2, CRC_INIT);  
return ((wExpected & 0xff) == pchMessage[dwLength - 2] && ((wExpected >> 8) & 0xff) == 
pchMessage[dwLength - 1]);  
} 
 
/*  
** Descriptions: append CRC16 to the end of data  
** Input: Data to CRC and append,Stream length = Data + checksum  
** Output: True or False (CRC Verify Result)  
*/  
void Append_CRC16_Check_Sum(uint8_t * pchMessage,uint32_t dwLength)  
{  
uint16_t wCRC = 0;  
if ((pchMessage == NULL) || (dwLength <= 2))  
{  
return; 
}  
wCRC = Get_CRC16_Check_Sum (pchMessage, dwLength-2, CRC_INIT );  
pchMessage[dwLength-2] = (uint8_t)(wCRC & 0x00ff);  
pchMessage[dwLength-1] = (uint8_t)((wCRC >> 8)& 0x00ff); 
}

//
uint8_t FrameHeadBuffer[5] = {0};
uint8_t FrameDataBuffer[128] = {0};

void JudgerSystem_Receive(void)
{
    HAL_UART_Receive_IT(&huart1, FrameHeadBuffer, 5);
}

// void JudgeSystem_Init(void)
// {
//     HAL_UART_Receive_IT(&huart1, FrameDataBuffer, 5);
// }

void JudgerSystem_Receive_Callback(void)
{ 
    static uint8_t state = 0;
    switch (state)
    {
    case 0:
        //帧头部分
        uint16_t dataLength = FrameDataBuffer[2] << 8 | FrameDataBuffer[1];
        // HAL_UART_Transmit(&huart10, FrameHeadBuffer, 5, 0);
        // HAL_UART_Receive_IT(&huart1, FrameHeadBuffer, 5);
        state = 1;
        HAL_UART_Receive_IT(&huart1, FrameDataBuffer + 5, dataLength + 4);
        break;
        
    case 1:
        //包里的具体内容
        state = 0;
        JudgeSystem_BasicDataParse(FrameDataBuffer, dataLength + 4);
        HAL_UART_Receive_IT(&huart1, FrameDataBuffer, 5);
        break;

    default:
        break;
    }
}

shoot_data_t shoot_data;
robot_status_t robot_status;
power_heat_data_t hero_power_data;

void JudgeSystem_ShootDataParse(uint8_t* data)
{
    shoot_data.bullet_type = data[0];
    shoot_data.shooter_number = data[1];
    shoot_data.launching_frequency = data[2];
    shoot_data.launching_frequency = data[6] << 24  || data[5] << 16 || data[4] << 8 || data[3];
}

void JudgeSystem_RobotStatusParse(uint8_t* data)
{
    robot_status.robot_id = data[0];
    robot_status.robot_level = data[1];
    robot_status.current_HP = data[3] << 8 | data[2];
    robot_status.maximum_HP = data[5] << 8 | data[4];
    robot_status.shooter_barrel_cooling_value = data[7] << 8 | data[6];
    robot_status.shooter_barrel_heat_limit = data[9] << 8 | data[8];
    robot_status.chassis_power_limit = data[11] << 8 | data[10];
}

void JudgeSystem_PowerHeatDataParse(uint8_t* data)
{

}

void JudgeSystem_BasicDataParse(uint8_t* data, uint16_t length)
{
    uint16_t cmdid = FrameDataBuffer[6] << 8 | FrameDataBuffer[5];
    switch (cmdid)
    {
    case 0x201: //机器人性能数据
        JudgeSystem_RobotStatusParse(FrameDataBuffer + JudgeSystem_DataOffset);
        /* code */
        break;
    
    case 0x202: //机器人底盘缓冲能量，射击热量
        break;

    case 0x207: //发送完成
        break;
    
    default:
        break;
    }
}

control_data_t control_data;
VTM_RC_Data_t vt_data;
uint32_t error_count = 0;



uint8_t Judge_Buffer[256] = {0};
uint8_t JudgeSystem_VtBuffer[256] = {0};
uint16_t Expect_CRC_value = 0;
uint16_t Frame_CRC_value = 0;



void JudgeSystem_VtDataUnify(VTM_RC_Data_t* data, uint8_t* packed_buf)
{
    float vx = 0.0f; // X轴速度 [-100, 100]
    float vy = 0.0f; // Y轴速度 [-100, 100]
    float vw = 0.0f; // W轴旋转 [-5, 5]

    // 1. 提取键盘 WASD 状态
    uint8_t key_w = data->keyboard & VTM_KEY_W;
    uint8_t key_s = data->keyboard & VTM_KEY_S;
    uint8_t key_a = data->keyboard & VTM_KEY_A;
    uint8_t key_d = data->keyboard & VTM_KEY_D;

    // 2. X 轴映射 (键盘 W/S 优先于 左摇杆竖直 Ch2)
    if (key_w || key_s) {
        if (key_w) vx += MAX_SPEED_XY;
        if (key_s) vx -= MAX_SPEED_XY;
    } else {
        vx = ((data->ch2 - RC_CH_MID_FLOAT) / RC_CH_RANGE) * MAX_SPEED_XY;
    }
    vx = CLAMP(vx, -MAX_SPEED_XY, MAX_SPEED_XY);

    // 3. Y 轴映射 (键盘 A/D 优先于 左摇杆水平 Ch3)
    if (key_a || key_d) {
        if (key_a) vy -= MAX_SPEED_XY; // A键向左
        if (key_d) vy += MAX_SPEED_XY; // D键向右
    } else {
        vy = ((data->ch3 - RC_CH_MID_FLOAT) / RC_CH_RANGE) * MAX_SPEED_XY;
    }
    vy = CLAMP(vy, -MAX_SPEED_XY, MAX_SPEED_XY);

    // 4. W 轴旋转映射 (右摇杆水平 Ch0 控制旋转)
    // vw = ((data->ch0 - RC_CH_MID_FLOAT) / RC_CH_RANGE) * MAX_SPEED_W;
    // vw = CLAMP(vw, -MAX_SPEED_W, MAX_SPEED_W);

    switch (data->mode_switch)
    {
    case 0x00:
        packed_buf[4] = 0xFF;
        break;

    case 0x01:
        packed_buf[4] = 0x00;
        break;

    case 0x02:
        packed_buf[4] = 0x1;
        break;
    
    default:
        break;
    }

    // 5. 浮点转定点 (乘以放大系数，保留小数精度)
    int16_t out_x = (int16_t)(vx * PACK_SCALE_XY);
    int16_t out_y = (int16_t)(vy * PACK_SCALE_XY);
    int16_t out_w = (int16_t)(vw * PACK_SCALE_W);
    
    // 获取原生的16位键盘按键数据
    uint16_t out_key = data->keyboard;

    // 6. 小端序装载至 8 字节数组
    // Byte 1, 2: X轴速度
    packed_buf[0] = out_x & 0xFF;
    packed_buf[1] = (out_x >> 8) & 0xFF;
    
    // Byte 3, 4: Y轴速度
    packed_buf[2] = out_y & 0xFF;
    packed_buf[3] = (out_y >> 8) & 0xFF;
    
    // Byte 5, 6: W轴(旋转)速度
    // packed_buf[4] = out_w & 0xFF;
    // packed_buf[5] = (out_w >> 8) & 0xFF;
    
    // Byte 7, 8: 键盘按键数据
    packed_buf[6] = out_key & 0xFF;
    packed_buf[7] = (out_key >> 8) & 0xFF;
}

void JudgeSystem_Map_And_Pack_Fusion(const VTM_RC_Data_t *rc_data, uint8_t out_buf[8]) {
    // 1. 提取右摇杆数据并映射到 [-100, 100]
    // ch0: 右摇杆水平 (对应 X 轴/Yaw) [cite: 164]
    // ch1: 右摇杆竖直 (对应 Y 轴/Pitch) [cite: 164]
    float joy_x = ((rc_data->ch0 - RC_CH_MID_FLOAT) / RC_CH_RANGE) * MAX_OUTPUT_SPEED;
    float joy_y = ((rc_data->ch1 - RC_CH_MID_FLOAT) / RC_CH_RANGE) * MAX_OUTPUT_SPEED;

    // 2. 提取鼠标移动速度并乘上灵敏度
    // 鼠标原生数据范围很大(-32768 ~ 32767)，通常乘以一个较小的灵敏度系数来控制 [cite: 165]
    float mouse_x_mapped = rc_data->mouse_x * MOUSE_SENSITIVITY;
    float mouse_y_mapped = rc_data->mouse_y * MOUSE_SENSITIVITY;

    // 3. 数据融合：摇杆 + 鼠标
    // 这样既可以用摇杆匀速转动，也可以用鼠标甩枪
    float final_x = (joy_x * JOYSTICK_WEIGHT) + mouse_x_mapped;
    float final_y = (joy_y * JOYSTICK_WEIGHT) + mouse_y_mapped;

    // 4. 最终限幅，确保输出严格在 [-100, 100] 区间内
    final_x = CLAMP(final_x, -MAX_OUTPUT_SPEED, MAX_OUTPUT_SPEED);
    final_y = CLAMP(final_y, -MAX_OUTPUT_SPEED, MAX_OUTPUT_SPEED);

    int16_t out_x = (int16_t)(final_x * 100);
    int16_t out_y = (int16_t)(final_y * 100);

    // 6. 提取鼠标按键状态 [cite: 167]
    // bit0: 左键, bit1: 右键, bit2: 中键
    uint8_t mouse_status = 0;
    if (rc_data->mouse_l == 1 || rc_data->trigger == 1) mouse_status |= (1 << 0);
    if (rc_data->mouse_r == 1) mouse_status |= (1 << 1);
    if (rc_data->mouse_m == 1) mouse_status |= (1 << 2);

    // 7. 小端序装载至 8 字节数组
    // Byte 1, 2: 融合后的 X 轴速度 (out_buf[0], out_buf[1])
    out_buf[0] = out_x & 0xFF;
    out_buf[1] = (out_x >> 8) & 0xFF;
    
    // Byte 3, 4: 融合后的 Y 轴速度 (out_buf[2], out_buf[3])
    out_buf[2] = out_y & 0xFF;
    out_buf[3] = (out_y >> 8) & 0xFF;
    
    // Byte 5: 鼠标状态按键位 (out_buf[4])
    out_buf[4] = mouse_status;
    
    // Byte 6, 7, 8: 留空填零
    out_buf[5] = 0x00;
    out_buf[6] = 0x00;
    out_buf[7] = 0x00;
}

uint8_t chassis_cmd[8];
uint8_t gimbal_cmd[8];

void Parse_VTM_RC_Data(const uint8_t *rx_buf, VTM_RC_Data_t *rc_data) 
{
    if (rx_buf == NULL || rc_data == NULL) return -1;

    // 1. 验证帧头 1 和 2 (Offset 0: 0xA9, Offset 8: 0x53) [cite: 356]
    if (rx_buf[0] != 0xA9 || rx_buf[1] != 0x53) {
        return -1; // 帧头错误，丢弃该帧
    }

    // 2. 摇杆通道解析 [cite: 356]
    // 通道 0: 起始位 16, 长度 11 -> rx_buf[2], rx_buf[3]
    rc_data->ch0 = (rx_buf[2] | (rx_buf[3] << 8)) & 0x07FF;
    // 通道 1: 起始位 27, 长度 11 -> rx_buf[3], rx_buf[4]
    rc_data->ch1 = ((rx_buf[3] >> 3) | (rx_buf[4] << 5)) & 0x07FF;
    // 通道 2: 起始位 38, 长度 11 -> rx_buf[4], rx_buf[5], rx_buf[6]
    rc_data->ch2 = ((rx_buf[4] >> 6) | (rx_buf[5] << 2) | (rx_buf[6] << 10)) & 0x07FF;
    // 通道 3: 起始位 49, 长度 11 -> rx_buf[6], rx_buf[7]
    rc_data->ch3 = ((rx_buf[6] >> 1) | (rx_buf[7] << 7)) & 0x07FF;

    // 3. 遥控器按键解析 [cite: 356]
    // 挡位切换开关: 起始位 60, 长度 2 -> rx_buf[7]
    rc_data->mode_switch = (rx_buf[7] >> 4) & 0x03;
    // 暂停按键: 起始位 62, 长度 1 -> rx_buf[7]
    rc_data->pause_btn   = (rx_buf[7] >> 6) & 0x01;
    // 自定义按键(左): 起始位 63, 长度 1 -> rx_buf[7]
    rc_data->custom_l    = (rx_buf[7] >> 7) & 0x01;
    // 自定义按键(右): 起始位 64, 长度 1 -> rx_buf[8]
    rc_data->custom_r    = rx_buf[8] & 0x01;

    // 4. 拨轮与扳机 [cite: 358]
    // 拨轮: 起始位 65, 长度 11 -> rx_buf[8], rx_buf[9]
    rc_data->dial    = ((rx_buf[8] >> 1) | (rx_buf[9] << 7)) & 0x07FF;
    // 扳机键: 起始位 76, 长度 1 -> rx_buf[9]
    rc_data->trigger = (rx_buf[9] >> 4) & 0x01;

    // 5. 鼠标运动数据解析 (有符号16位，直接拼接转换) [cite: 358]
    // X轴: 起始位 80 (Byte 10)
    rc_data->mouse_x = (int16_t)(rx_buf[10] | (rx_buf[11] << 8));
    // Y轴: 起始位 96 (Byte 12)
    rc_data->mouse_y = (int16_t)(rx_buf[12] | (rx_buf[13] << 8));
    // Z轴: 起始位 112 (Byte 14)
    rc_data->mouse_z = (int16_t)(rx_buf[14] | (rx_buf[15] << 8));

    // 6. 鼠标按键解析 [cite: 358]
    // 左键: 起始位 128 (Byte 16), 长度 2
    rc_data->mouse_l = rx_buf[16] & 0x03;
    // 右键: 起始位 130, 长度 2
    rc_data->mouse_r = (rx_buf[16] >> 2) & 0x03;
    // 中键: 起始位 132, 长度 2
    rc_data->mouse_m = (rx_buf[16] >> 4) & 0x03;

    // 7. 键盘按键解析 [cite: 358]
    // 键盘: 起始位 136 (Byte 17, 18), 长度 16
    rc_data->keyboard = (uint16_t)(rx_buf[17] | (rx_buf[18] << 8));

    
    JudgeSystem_VtDataUnify(&vt_data, chassis_cmd);
    JudgeSystem_Map_And_Pack_Fusion(&vt_data, gimbal_cmd);

    CAN_Transmit_STD(&hfdcan1, 0x07, chassis_cmd, 8);
    CAN_Transmit_STD(&hfdcan1, 0x08, gimbal_cmd, 8);

    if ((rc_data->keyboard & 0x0130) == 0x0130)
    {
        // uint32_t useless;
        // xTimerStartFromISR(ResetTimerHandle, &useless);
    }
    
    if((rc_data->keyboard & 0x230) == 0x230)
    {
        NVIC_SystemReset();
        // uint32_t useless;
        // xTimerStartFromISR(ResetTimerHandle, &useless);
    }

}

uint8_t JudgeSystem_VtDataParse(uint8_t* data, uint16_t length)
{
    Expect_CRC_value = Get_CRC16_Check_Sum(data, length-2, CRC_INIT);
    Frame_CRC_value = data[length-1] << 8 | data[length-2];

    if (data[0] != 0xA9 || Expect_CRC_value != Frame_CRC_value)
    {
        error_count++;
    }
    else
    {
        control_data.mouse_spd_x = data[10];
        control_data.mouse_spd_y = data[11];
        control_data.mouse_spd_z = data[12];

        control_data.rc_r_x = (data[3] & 0x07) << 8 | data[2];
        control_data.rc_r_y = (data[4]) << 5 | (data[3] & 0x0F8) >> 3;

        control_data.rc_l_y = ((data[4] >> 6) | (data[5] << 2) | (data[6] << 10)) & 0x07FF;
        control_data.rc_l_x = ((data[6] >> 1) | (data[7] << 7)) & 0x07FF;
        // control_data.rc_r_y = (data[4]) << 5 | (data[3] & 0x0F8) >> 3;
        
        // JudgeSystem_ChassisControlData_Pact
        // JudgeSystem_GimbalControlData_Pact
        // uint8_t test_data[8] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08};
        // CAN_Transmit_STD(&hfdcan1, 0x07, test_data, 8);
        // CAN_Transmit_STD(&hfdcan1, 0x08, test_data, 8);
    }

    return 1;
}

void JudgeSystem_ChassisControlData_Pact(control_data_t* control_data, uint8_t* pacted_data)
{
    
}

void JudgeSystem_GimbalControlData_Pact(control_data_t* control_data, uint8_t* pacted_data)
{

}

//在串口空闲中断时调用这个
void JudgeSystem_IdleReceive(void)
{
    static uint16_t last_rec_index = 0; //上次接收到的位置
    uint16_t now_rec_index = 0;

    now_rec_index = 256 - __HAL_DMA_GET_COUNTER(huart1.hdmarx); //当前接收到的位置

    uint16_t length = 0;
    if (now_rec_index > last_rec_index)
    {
        length = now_rec_index - last_rec_index;
        Parse_VTM_RC_Data(Judge_Buffer + last_rec_index, &vt_data);
    }
    else if (now_rec_index < last_rec_index)
    {
        length = 256 - last_rec_index + now_rec_index;
        uint8_t temp[128];
        memcpy(temp, Judge_Buffer + last_rec_index, 256 - last_rec_index);
        memcpy(temp + 256 - last_rec_index, Judge_Buffer, now_rec_index);
        Parse_VTM_RC_Data(temp, &vt_data);
    }
    else //正好256的整数倍
    {
        //跳过
    }
    
    last_rec_index = now_rec_index;

    //TODO:错误处理机制
}

void JudgeSystem_ReceiveStart(void)
{
    __HAL_UART_ENABLE_IT(&huart1, UART_IT_IDLE);
    HAL_UART_Receive_DMA(&huart1, Judge_Buffer, 256);
}

void JudgeSystem_DataParseTask(void* arg)
{
    osDelay(5000);
    JudgeSystem_ReceiveStart();

    while (1)
    {
        xTaskNotifyWait(0xFFFF, 0xFFFF, NULL, portMAX_DELAY);
        JudgeSystem_IdleReceive();
        printf("%d\n", vt_data.ch0);
        // uint8_t test_data[8] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08};
        // HAL_UART_Transmit(&huart10, test_data, 8, 1000);
    }
    
}

TaskHandle_t JudgeSystem_DataParseTaskHandle;


void JudgeSystem_ResetTimerCallback(void)
{
    NVIC_SystemReset();
}

uint32_t iscreated = 0;
void JudgeSystem_Init(void)
{
    ResetTimerHandle = xTimerCreate("reset_timer", 2000, pdFALSE, NULL, JudgeSystem_ResetTimerCallback);
    iscreated = xTaskCreate(JudgeSystem_DataParseTask, "JudgeSystem_DataParseTask", 512, NULL, 1, &JudgeSystem_DataParseTaskHandle);
    // JudgeSystem_ReceiveStart();
}

void JudgeSystem_VtParseNotify(void)
{
    uint32_t ulNotificationValue = 0;
    vTaskNotifyGiveFromISR(JudgeSystem_DataParseTaskHandle, &ulNotificationValue);
}