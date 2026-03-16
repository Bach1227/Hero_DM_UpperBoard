#include "DataStructure.h"
#include <string.h>
#include <stdbool.h>

/**
 * @brief 向16位整型环形缓冲区插入数据
 * @param RoundArray 环形缓冲区指针
 * @param Data 要插入的16位整型数据
 * @return 当前缓冲区中的数据数量
 * 
 * 功能说明:
 * 1. 当尾部指针大于头部指针时，直接在尾部插入数据
 * 2. 否则需要同时移动头部和尾部指针(缓冲区已满情况)
 * 3. 对指针位置进行取模运算实现环形缓冲
 * 4. 返回当前缓冲区中的数据数量
 */
uint16_t RoundArrayInsert_int16(RoundArray_int16* RoundArray, int16_t Data)
{
    if (RoundArray->Tail > RoundArray->Head)
    {
        RoundArray->buf[RoundArray->Tail] = Data;
        RoundArray->Tail++;
    }
    else
    {
        RoundArray->buf[RoundArray->Tail] = Data;
        RoundArray->Head++;
        RoundArray->Tail++;
    }
    RoundArray->Head %= RoundArray->Size;
    RoundArray->Tail %= RoundArray->Size;
    if (RoundArray->Tail > RoundArray->Head)
    {
        return RoundArray->Tail - RoundArray->Head;
    }
    else
    {
        return RoundArray->Size;
    }
}

/**
 * @brief 初始化浮点型环形缓冲区
 * @param cbuf 环形缓冲区指针
 * @param buffer 数据存储区指针
 * @param size 缓冲区总大小
 * @param window_size 滑动窗口大小
 * 
 * 功能说明:
 * 1. 初始化缓冲区各参数
 * 2. 确保窗口大小不超过缓冲区大小
 * 3. 清空缓冲区数据
 */
void CircularBuffer_float_Init(CircularBuffer *cbuf, float *buffer, uint16_t size, uint16_t window_size) {
    cbuf->buffer = buffer;
    cbuf->size = size;
    cbuf->window_size = (window_size > size) ? size : window_size;
    cbuf->head = 0;
    cbuf->tail = 0;
    cbuf->count = 0;
    memset(buffer, 0, size * sizeof(float));
}

/**
 * @brief 向环形缓冲区添加新数据
 * @param cbuf 环形缓冲区指针
 * @param value 要插入的浮点数据
 * 
 * 功能说明:
 * 1. 在头部位置插入新数据
 * 2. 环形移动头部指针
 * 3. 更新数据计数器
 * 4. 缓冲区满时移动尾部指针
 */
void CircularBuffer_float_Push(CircularBuffer *cbuf, float value) {
    cbuf->buffer[cbuf->head] = value;
    cbuf->head = (cbuf->head + 1) % cbuf->size;
    
    if (cbuf->count < cbuf->size) {
        cbuf->count++;
    } else {
        cbuf->tail = (cbuf->tail + 1) % cbuf->size;
    }
}

/**
 * @brief 计算滑动窗口平均值
 * @param cbuf 环形缓冲区指针
 * @return 滑动窗口内的平均值
 * 
 * 功能说明:
 * 1. 空缓冲区返回0
 * 2. 计算实际窗口大小(取窗口大小和当前数据量的较小值)
 * 3. 从新到旧计算窗口内数据的和
 * 4. 返回平均值
 */
float CircularBuffer_float_GetAverage(CircularBuffer *cbuf) {
    if (cbuf->count == 0) return 0.0f;
    
    uint16_t window = (cbuf->count < cbuf->window_size) ? cbuf->count : cbuf->window_size;
    float sum = 0.0f;
    
    for (uint16_t i = 0; i < window; i++) {
        uint16_t idx = (cbuf->head - 1 - i + cbuf->size) % cbuf->size;
        sum += cbuf->buffer[idx];
    }
    
    return sum / window;
}

/**
 * @brief 检查缓冲区是否为空
 * @param cbuf 环形缓冲区指针
 * @return 1为空，0为非空 (uint8_t类型以兼容STM32)
 */
uint8_t CircularBuffer_float_IsEmpty(CircularBuffer *cbuf) {
    return cbuf->count == 0;
}

/**
 * @brief 检查缓冲区是否已满
 * @param cbuf 环形缓冲区指针
 * @return 1为满，0为非满 (uint8_t类型以兼容STM32)
 */
uint8_t CircularBuffer_float_IsFull(CircularBuffer *cbuf) {
    return cbuf->count == cbuf->size;
}

/**
 * @brief 清空环形缓冲区
 * @param cbuf 环形缓冲区指针
 * 
 * 功能说明:
 * 1. 重置头尾指针和计数器
 * 2. 清空缓冲区数据
 */
void CircularBuffer_float_Clear(CircularBuffer *cbuf) {
    cbuf->head = 0;
    cbuf->tail = 0;
    cbuf->count = 0;
    memset(cbuf->buffer, 0, cbuf->size * sizeof(float));
}

/**
 * @brief 初始化int16_t环形缓冲区
 * @param cbuf 环形缓冲区指针
 * @param buffer 数据存储区指针
 * @param size 缓冲区总大小
 * @param window_size 滑动窗口大小
 * 
 * 功能说明:
 * 1. 初始化缓冲区各参数
 * 2. 确保窗口大小不超过缓冲区大小
 * 3. 清空缓冲区数据
 */
void CircularBuffer_int16_Init(CircularBuffer_int16 *cbuf, int16_t *buffer, uint16_t size, uint16_t window_size) {
    cbuf->buffer = buffer;
    cbuf->size = size;
    cbuf->window_size = (window_size > size) ? size : window_size;
    cbuf->head = 0;
    cbuf->tail = 0;
    cbuf->count = 0;
    memset(buffer, 0, size * sizeof(int16_t));
}

/**
 * @brief 向int16_t缓冲区添加新数据
 * @param cbuf 环形缓冲区指针
 * @param value 要插入的16位整型数据
 * 
 * 功能说明:
 * 1. 在头部位置插入新数据
 * 2. 环形移动头部指针
 * 3. 更新数据计数器
 * 4. 缓冲区满时移动尾部指针
 */
void CircularBuffer_int16_Push(CircularBuffer_int16 *cbuf, int16_t value) {

    if (cbuf != NULL && cbuf->buffer != NULL)
    {
        cbuf->buffer[cbuf->head] = value;
        cbuf->head = (cbuf->head + 1) % cbuf->size;
        
        if (cbuf->count < cbuf->size) {
            cbuf->count++;
        } else {
            cbuf->tail = (cbuf->tail + 1) % cbuf->size;
        }
    }
}

/**
 * @brief 计算int16_t滑动窗口平均值
 * @param cbuf 环形缓冲区指针
 * @return 滑动窗口内的平均值(整数)
 * 
 * 功能说明:
 * 1. 空缓冲区返回0
 * 2. 计算实际窗口大小(取窗口大小和当前数据量的较小值)
 * 3. 使用32位整数计算和防止溢出
 * 4. 返回整数除法结果
 */
int16_t CircularBuffer_int16_GetAverage(CircularBuffer_int16 *cbuf) {
    if (cbuf != NULL && cbuf->buffer != NULL)
    {
        if (cbuf->count == 0) return 0;
        
        uint16_t window = (cbuf->count < cbuf->window_size) ? cbuf->count : cbuf->window_size;
        int32_t sum = 0; // 使用32位防止溢出
        
        for (uint16_t i = 0; i < window; i++) {
            uint16_t idx = (cbuf->head - 1 - i + cbuf->size) % cbuf->size;
            sum += cbuf->buffer[idx];
        }
        
        return (int16_t)(sum / window); // 整数除法
    }
}

/**
 * @brief 检查int16_t缓冲区是否为空
 * @param cbuf 环形缓冲区指针
 * @return 1为空，0为非空 (uint8_t类型以兼容STM32)
 */
uint8_t CircularBuffer_int16_IsEmpty(CircularBuffer_int16 *cbuf) {
    return cbuf->count == 0;
}

/**
 * @brief 检查int16_t缓冲区是否已满
 * @param cbuf 环形缓冲区指针
 * @return 1为满，0为非满 (uint8_t类型以兼容STM32)
 */
uint8_t CircularBuffer_int16_IsFull(CircularBuffer_int16 *cbuf) {
    return cbuf->count == cbuf->size;
}

/**
 * @brief 清空int16_t环形缓冲区
 * @param cbuf 环形缓冲区指针
 * 
 * 功能说明:
 * 1. 重置头尾指针和计数器
 * 2. 清空缓冲区数据
 */
void CircularBuffer_int16_Clear(CircularBuffer_int16 *cbuf) {
    cbuf->head = 0;
    cbuf->tail = 0;
    cbuf->count = 0;
    memset(cbuf->buffer, 0, cbuf->size * sizeof(int16_t));
}

// void RingBuffer_insert_8