#include "InterruptHandler.h"
// #include "Debug.h"

FDCAN_RxHeaderTypeDef rx_header_hcan1;
uint8_t rx_data_hcan1[8];
FDCAN_RxHeaderTypeDef rx_header_hcan2;
uint8_t rx_data_hcan2[8];
FDCAN_RxHeaderTypeDef rx_header_hcan3;
uint8_t rx_data_hcan3[8];

uint8_t sbus_received_count = 0;
uint8_t rx_data_usart1[20];
uint8_t CAN_Rx_IT_State = 0;
HAL_StatusTypeDef CAN_State;

uint32_t test = 0;
uint32_t SecCount = 0;

uint32_t can_recv_count = 0;
uint32_t transcount = 0;

void InterruptInit(void)
{
    HAL_TIM_Base_Start_IT(&htim2);
    HAL_TIM_Base_Start_IT(&htim3);
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef* huart)
{
}

void HAL_FDCAN_RxFifo0Callback(FDCAN_HandleTypeDef *hcan, uint32_t RxFifo0ITs)
{
    if(hcan == &hfdcan1)
    {
        HAL_FDCAN_GetRxMessage(hcan, FDCAN_RX_FIFO0, &rx_header_hcan1, rx_data_hcan1);
        if(RxFifo0ITs & FDCAN_IT_RX_FIFO0_NEW_MESSAGE)
        {
            switch (rx_header_hcan1.Identifier)
            {
                case 0xFE:
                    FrictionController_Parse(rx_data_hcan1);
                    test++;
                    break;
                
                default:
                    break;
            }
            // CAN_Transmit_STD(hcan, 0x001, rx_data_hcan1, rx_header_hcan1.DataLength);
        }
    }
    if(hcan == &hfdcan2)
    {
        if(RxFifo0ITs & FDCAN_IT_RX_FIFO0_NEW_MESSAGE)
        {
            HAL_FDCAN_GetRxMessage(hcan, FDCAN_RX_FIFO0, &rx_header_hcan2, rx_data_hcan2);
            switch (rx_header_hcan2.Identifier)
            {
                case 0x001:
                    can_recv_count++;
                    break;	

                case 0x010:
                    Communication_SendTargetInfo(rx_data_hcan2);
                    PC_data_record(rx_data_hcan2);
                    transcount++;
                    break;

                case 0x40:
                    Friction_Online(1);
                    break;

                case 0x41:
                    Friction_Online(2);
                    break;

                case 0x42:
                    Friction_Online(3);
                    break;
                    
                case 0x43:
                    Friction_Online(4);
                    break;
            }
        }
    }

    if(hcan == &hfdcan3)
    {
        if(RxFifo0ITs & FDCAN_IT_RX_FIFO0_NEW_MESSAGE)
        {
            HAL_FDCAN_GetRxMessage(hcan, FDCAN_RX_FIFO0, &rx_header_hcan3, rx_data_hcan3);
            switch (rx_header_hcan3.Identifier)
            {
                case 0x003:
                {
                    IMU_UpdateData(rx_data_hcan3);
                    test++;
                    // printf("%")
                    break;
                }
            }
        }
    }
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef* htim)
{
    if (htim == &htim2)
    {
        test++;
        // FrictionController_Init();
        Communication_SendPosInfo();
        // Communication_SendSpdInfo();
        FrictionController_TransmitData();
        FrictionController_SetSpeed();
        // FrictionController_SendIsFrictionReady();
        SendAngle();
        // uint8_t test[8] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08};
        // CAN_Transmit_STD(&hfdcan1, 0x001, test, 8);
        // printf("%d\n",HAL_FDCAN_GetTxFifoFreeLevel(&hfdcan1));
    }

    if (htim == &htim3)
    {
        SecCount++;
        // printf("SecCount: %d\n", test);
        test = 0;
    }

    
    
    
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    if (GPIO_Pin == GPIO_PIN_15)
    {
        FrictionController_EnableReverse();
    }
}