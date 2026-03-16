#ifndef __DataStructure_H
#define __DataStructure_H

#include "stm32h7xx_hal.h"  // STM32硬件抽象层

/* 16位整型环形缓冲区结构体 */
typedef struct
{
    int16_t buf[1000];  // 缓冲区数组
    int16_t Size;       // 缓冲区总大小
    uint16_t Head;      // 头部指针(最新数据位置)
    uint16_t Tail;      // 尾部指针(最旧数据位置)
}RoundArray_int16;

/* 16位整型环形缓冲区插入函数 */
uint16_t RoundArrayInsert_int16(RoundArray_int16* RoundArray, int16_t Data);

/* 带滑动窗口平均的浮点型环形缓冲区结构体 */
typedef struct {
    float *buffer;       // 数据存储区指针
    uint16_t size;       // 缓冲区总容量
    uint16_t head;       // 头指针(最新元素位置)
    uint16_t tail;       // 尾指针(最旧元素位置) 
    uint16_t count;      // 当前元素数量
    uint16_t window_size; // 滑动窗口大小(计算平均值用)
} CircularBuffer;

/* 带滑动窗口平均的16位整型环形缓冲区结构体 */
typedef struct {
    int16_t *buffer;     // 数据存储区指针
    uint16_t size;       // 缓冲区总容量
    uint16_t head;       // 头指针(最新元素位置)
    uint16_t tail;       // 尾指针(最旧元素位置)
    uint16_t count;      // 当前元素数量
    uint16_t window_size; // 滑动窗口大小(计算平均值用)
} CircularBuffer_int16;

/* 浮点型缓冲区操作函数 */
void CircularBuffer_float_Init(CircularBuffer *cbuf, float *buffer, uint16_t size, uint16_t window_size);
void CircularBuffer_float_Push(CircularBuffer *cbuf, float value);
float CircularBuffer_float_GetAverage(CircularBuffer *cbuf);
uint8_t CircularBuffer_float_IsEmpty(CircularBuffer *cbuf);
uint8_t CircularBuffer_float_IsFull(CircularBuffer *cbuf);
void CircularBuffer_float_Clear(CircularBuffer *cbuf);

/* 16位整型缓冲区操作函数 */
void CircularBuffer_int16_Init(CircularBuffer_int16 *cbuf, int16_t *buffer, uint16_t size, uint16_t window_size);
void CircularBuffer_int16_Push(CircularBuffer_int16 *cbuf, int16_t value);
int16_t CircularBuffer_int16_GetAverage(CircularBuffer_int16 *cbuf);
uint8_t CircularBuffer_int16_IsEmpty(CircularBuffer_int16 *cbuf);
uint8_t CircularBuffer_int16_IsFull(CircularBuffer_int16 *cbuf);
void CircularBuffer_int16_Clear(CircularBuffer_int16 *cbuf);

#endif // __DataStructure_H
