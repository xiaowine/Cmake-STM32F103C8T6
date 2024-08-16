#include "Drive_DHT11.h"
extern TIM_HandleTypeDef htim1;
static void DHT11_Mode_IPU(const DHT11_Data_TypeDef* DHT11_Data); //设置端口为输入模式
static void DHT11_Mode_Out_pp(const DHT11_Data_TypeDef* DHT11_Data); //设置端口为输出模式
static uint8_t DHT11_ReadByte(const DHT11_Data_TypeDef* DHT11_Data); //读取一个字节
static void delay_us(uint16_t us); //us级延时函数

#define DHT11_Data_IN HAL_GPIO_ReadPin(DHT11_Data->_GPIO_PORT,DHT11_Data->GPIO_Pin)

#define DHT11_GPIO_HIGH HAL_GPIO_WritePin(DHT11_Data->_GPIO_PORT,DHT11_Data->GPIO_Pin,GPIO_PIN_SET);
#define DHT11_GPIO_LOW HAL_GPIO_WritePin(DHT11_Data->_GPIO_PORT,DHT11_Data->GPIO_Pin,GPIO_PIN_RESET);

static void delay_us(const uint16_t us)
{
    uint16_t differ = 0xffff - us - 5;

    HAL_TIM_Base_Start(&htim1);
    __HAL_TIM_SetCounter(&htim1, differ);
    while (differ < 0xffff - 5)
    {
        differ = __HAL_TIM_GetCounter(&htim1);
    }
    HAL_TIM_Base_Stop(&htim1);
}

static void DHT11_Mode_IPU(const DHT11_Data_TypeDef* DHT11_Data)
{
    GPIO_InitTypeDef GPIO_InitStruct;
    GPIO_InitStruct.Pin = DHT11_Data->GPIO_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(DHT11_Data->_GPIO_PORT, &GPIO_InitStruct);
}

static void DHT11_Mode_Out_pp(const DHT11_Data_TypeDef* DHT11_Data)
{
    GPIO_InitTypeDef GPIO_InitStruct;

    GPIO_InitStruct.Pin = DHT11_Data->GPIO_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(DHT11_Data->_GPIO_PORT, &GPIO_InitStruct);
}

static uint8_t DHT11_ReadByte(const DHT11_Data_TypeDef* DHT11_Data)
{
    uint8_t temp = 0;

    for (uint8_t i = 0; i < 8; ++i)
    {
        while (DHT11_Data_IN == GPIO_PIN_RESET)
        {
        } //等待拉高信号,开始读取
        delay_us(40); //等待

        //        数据信号电平高低转换位0,1
        if (DHT11_Data_IN == GPIO_PIN_SET)
        {
            while (DHT11_Data_IN == GPIO_PIN_SET)
            {
            }
            temp |= (uint8_t)(0x01 << (7 - i));
        }
        else
        {
            temp &= (uint8_t)~(0x01 << (7 - i));
        }
    }
    return temp;
}

/**
 * 读取DHT11的数据
 * @param DHT11_Data DHT11的结构体指针
 * @return 0->成功读取;1->错误
 */
uint8_t DHT11_Read_TempHumi(DHT11_Data_TypeDef* DHT11_Data)
{
    //    通知DHT11开始读取数据
    DHT11_Mode_Out_pp(DHT11_Data);
    DHT11_GPIO_LOW
    HAL_Delay(18);
    //    delay_us(18);
    DHT11_GPIO_HIGH;
    delay_us(30);

    //    设置输入模式防止干扰
    DHT11_Mode_IPU(DHT11_Data);
    if (DHT11_Data_IN == GPIO_PIN_RESET)
    {
        //        等待DHT11通知可以开始读取
        while (DHT11_Data_IN == GPIO_PIN_RESET)
        {
        }
        while (DHT11_Data_IN == GPIO_PIN_SET)
        {
        }

        DHT11_Data->humi_high8bit = DHT11_ReadByte(DHT11_Data);
        DHT11_Data->humi_low8bit = DHT11_ReadByte(DHT11_Data);
        DHT11_Data->temp_high8bit = DHT11_ReadByte(DHT11_Data);
        DHT11_Data->humi_low8bit = DHT11_ReadByte(DHT11_Data);
        DHT11_Data->check_sum = DHT11_ReadByte(DHT11_Data);

        //        读取完成的扫尾工作
        DHT11_Mode_Out_pp(DHT11_Data);
        DHT11_GPIO_HIGH;

        //        计算校验和
        const uint8_t temp_check = DHT11_Data->humi_high8bit + DHT11_Data->humi_low8bit + DHT11_Data->temp_high8bit +
            DHT11_Data->temp_low8bit;
        if (temp_check == DHT11_Data->check_sum) return 0;
        return 1;
    }
    return 1;
}
