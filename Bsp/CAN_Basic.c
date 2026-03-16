#include "CAN_Basic.h"

/*
CAN_FilterTypeDef FilterArray[14];

void CAN_FilterStructureInit(CAN_FilterTypeDef* FilterSturcture)
{
    FilterSturcture->FilterActivation = CAN_FILTER_ENABLE;
    FilterSturcture->FilterBank = 
}

void CAN_FilterInit(uint8_t FilterBank_x, CAN_FilterTypeDef* FilterStucture)
{
    switch (FilterBank_x)
    {
    case 0:

        break;
    
    default:

        break;
    }
}
*/

                                                                      

void CAN_Init(void)
{
    #ifdef STM32F1
    HAL_CAN_Start(&hcan);
    #endif

    #ifdef STM32F4
    HAL_CAN_Start(&hcan1);
    HAL_CAN_Start(&hcan2);
    #endif
}

#ifdef STM32F1
void CAN_Transmit_STD(uint32_t ID, uint8_t* data, uint8_t len)
{
    //uint8_t test[3] = {0xA1, 0xBB, 0xCC};
    CAN_TxHeaderTypeDef TxMSG;
    TxMSG.StdId = ID;
    TxMSG.ExtId = 0;
    TxMSG.IDE = CAN_ID_STD;
    TxMSG.RTR = CAN_RTR_DATA;
    TxMSG.DLC = 3;

    uint32_t TxMailbox;

    HAL_CAN_AddTxMessage(&TxMSG, data, &TxMailbox);
}

void CAN_Transmit_EXT(uint32_t ID, uint8_t* data, uint8_t Len)
{
    CAN_TxHeaderTypeDef TxMSG;

    TxMSG.StdId = 0x000;
    TxMSG.ExtId = ID;
    TxMSG.IDE = CAN_ID_EXT;
    TxMSG.RTR = CAN_RTR_DATA;
    TxMSG.DLC = Len;

    uint32_t TxMailbox;
    HAL_CAN_AddTxMessage(&hcan, &TxMSG, data, &TxMailbox);
}

uint8_t RxFifoX_IsEmpty(uint32_t RxFifo)
{
    if (HAL_CAN_GetRxFifoFillLevel(&hcan, RxFifo) == 0)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}
#endif // STM32F1

#ifdef STM32F4
void CAN_Transmit_STD(CAN_HandleTypeDef* hcan_num, uint32_t ID, uint8_t* data, uint8_t len)
{
    //uint8_t test[3] = {0xA1, 0xBB, 0xCC};
    CAN_TxHeaderTypeDef TxMSG;
    TxMSG.StdId = ID;
    TxMSG.ExtId = 0;
    TxMSG.IDE = CAN_ID_STD;
    TxMSG.RTR = CAN_RTR_DATA;
    TxMSG.DLC = len;

    uint32_t TxMailbox;

    HAL_CAN_AddTxMessage(hcan_num, &TxMSG, data, &TxMailbox);
}

void CAN_Transmit_EXT(CAN_HandleTypeDef* hcan_num, uint32_t ID, uint8_t* data, uint8_t Len)
{
    CAN_TxHeaderTypeDef TxMSG;

    TxMSG.StdId = 0x000;
    TxMSG.ExtId = ID;
    TxMSG.IDE = CAN_ID_EXT;
    TxMSG.RTR = CAN_RTR_DATA;
    TxMSG.DLC = Len;

    uint32_t TxMailbox;
    HAL_CAN_AddTxMessage(hcan_num, &TxMSG, data, &TxMailbox);
}

uint8_t RxFifoX_IsEmpty(CAN_HandleTypeDef* hcan_num, uint32_t RxFifo)
{
    if (HAL_CAN_GetRxFifoFillLevel(hcan_num, RxFifo) == 0)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

void CAN_FilterInit(void)
{
    CAN_FilterTypeDef can_filter_st;
    can_filter_st.FilterActivation = ENABLE;
    can_filter_st.FilterMode = CAN_FILTERMODE_IDMASK;
    can_filter_st.FilterScale = CAN_FILTERSCALE_32BIT;
    can_filter_st.FilterIdHigh = 0x0000;
    can_filter_st.FilterIdLow = 0x0000;
    can_filter_st.FilterMaskIdHigh = 0x0000;
    can_filter_st.FilterMaskIdLow = 0x0000;
    can_filter_st.FilterBank = 0;
    can_filter_st.FilterFIFOAssignment = CAN_RX_FIFO0;
    HAL_CAN_ConfigFilter(&hcan1, &can_filter_st);
    // HAL_CAN_Start(&hcan1);
    // HAL_CAN_ActivateNotification(&hcan1, CAN_IT_RX_FIFO0_MSG_PENDING);


    can_filter_st.SlaveStartFilterBank = 14;
    can_filter_st.FilterBank = 14;
    HAL_CAN_ConfigFilter(&hcan2, &can_filter_st);
    // HAL_CAN_Start(&hcan2);
    // HAL_CAN_ActivateNotification(&hcan2, CAN_IT_RX_FIFO0_MSG_PENDING);
}

#endif // STM32F4

#ifdef STM32H7

void CAN_FilterInit(void)
{
	FDCAN_FilterTypeDef fdcan_filter;
	
	fdcan_filter.IdType = FDCAN_STANDARD_ID;                       //��׼ID
	fdcan_filter.FilterIndex = 0;                                  //�˲�������                   
	fdcan_filter.FilterType = FDCAN_FILTER_RANGE;                  //�˲�������
	fdcan_filter.FilterConfig = FDCAN_FILTER_TO_RXFIFO0;           //������0������FIFO0  
	fdcan_filter.FilterID1 = 0x0000;                               //32λID-
	fdcan_filter.FilterID2 = 0x0000;                               //���FDCAN����Ϊ��ͳģʽ�Ļ���������32λ����
	if(HAL_FDCAN_ConfigFilter(&hfdcan1, &fdcan_filter)!=HAL_OK) 		
	{
		Error_Handler();
	}

	if(HAL_FDCAN_ConfigFilter(&hfdcan2, &fdcan_filter)!=HAL_OK) 		
	{
		Error_Handler();
	}

	// HAL_FDCAN_ConfigFifoWatermark(&hfdcan1, FDCAN_CFG_RX_FIFO0, 1);
	// HAL_FDCAN_ConfigFifoWatermark(&hfdcan2, FDCAN_CFG_RX_FIFO0, 1);
	// HAL_FDCAN_ConfigFifoWatermark(&hfdcan3, FDCAN_CFG_RX_FIFO0, 1);
}

HAL_StatusTypeDef CAN_Transmit_STD(FDCAN_HandleTypeDef *hfdcan, uint16_t id, uint8_t *data, uint32_t len)
{	
    FDCAN_TxHeaderTypeDef TxMSG;
    TxMSG.Identifier = id;
    TxMSG.IdType = FDCAN_STANDARD_ID;
    TxMSG.TxFrameType = FDCAN_DATA_FRAME;
    TxMSG.DataLength = len;
    TxMSG.ErrorStateIndicator = FDCAN_ESI_ACTIVE;
    TxMSG.BitRateSwitch = FDCAN_BRS_OFF;
    TxMSG.FDFormat = FDCAN_CLASSIC_CAN;
    TxMSG.TxEventFifoControl = FDCAN_NO_TX_EVENTS;
    TxMSG.MessageMarker = 0;

    // uint32_t TxMailbox;

    return HAL_FDCAN_AddMessageToTxFifoQ(hfdcan, &TxMSG, data);
}

#endif // STM32H7

uint8_t CAN_GetMailBoxFree(FDCAN_HandleTypeDef *hcan)
{
    return HAL_FDCAN_GetTxFifoFreeLevel(hcan);
}

