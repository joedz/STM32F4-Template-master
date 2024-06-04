#include "AW3644.h"
#include "stdio.h"

#define AW3644_SLAVE_ADDR 0x63// AW3644 的 7 位从机地址

void I2C2_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    I2C_InitTypeDef I2C_InitStructure;

    // 使能 I2C2 和 GPIOB 时钟
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C2, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);

    // 配置 I2C2 的 SCL (PB10) 和 SDA (PB11) 引脚
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_11;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    // 配置 I2C2 参数
    I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
    I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;
    I2C_InitStructure.I2C_OwnAddress1 = 0x00;
    I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;
    I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
    I2C_InitStructure.I2C_ClockSpeed = 50000; // 100kHz
    I2C_Init(I2C2, &I2C_InitStructure);

    // 使能 I2C2
    I2C_Cmd(I2C2, ENABLE);
}

void AW3644_Init(void)
{
    // 初始化 I2C2
    I2C2_Init();

    // 配置 AW3644
    AW3644_Write(AW3644_Enable, 0x01);                // 使能 AW3644
    AW3644_Write(AW3644_LED1_Torch_Brightness, 0x7F); // 设置 LED1 的 Torch 亮度为最大
    AW3644_Write(AW3644_LED2_Torch_Brightness, 0x7F); // 设置 LED2 的 Torch 亮度为最大
    AW3644_Write(AW3644_Boost_Configuration, 0x28);   // 配置 Boost 模式
    AW3644_Write(AW3644_Timing_Configuration, 0x33);  // 配置 Flash 和 Torch 时间
}

void AW3644_WaitEvent(I2C_TypeDef *I2Cx, uint32_t I2C_EVENT)
{
    uint32_t Timeout;
    Timeout = 10000;
    while (I2C_CheckEvent(I2Cx, I2C_EVENT) != SUCCESS)
    {
        Timeout--;
        if (Timeout == 0)
        {
            break;
        }
    }
}
/**
 * @brief 向 AW3644 寄存器写入数据
 * @param reg 寄存器地址
 * @param data 要写入的数据
 */
void AW3644_Write(uint8_t reg, uint8_t data)
{
    // 等待 I2C2 总线空闲
    AW3644_WaitEvent(I2C2, I2C_FLAG_BUSY);

    // 发送起始信号
    I2C_GenerateSTART(I2C2, ENABLE);

    // 等待 EV5 事件(发送起始信号完成)
    AW3644_WaitEvent(I2C2, I2C_EVENT_MASTER_MODE_SELECT);

    // 发送从机地址(写操作)
    I2C_Send7bitAddress(I2C2, AW3644_SLAVE_ADDR, I2C_Direction_Transmitter);

    // 等待 EV6 事件(从机地址已发送,ACK已收到)
    AW3644_WaitEvent(I2C2, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED);

    // 发送寄存器地址
    I2C_SendData(I2C2, reg);

    // 等待 EV8 事件(数据字节已发送)
    AW3644_WaitEvent(I2C2, I2C_EVENT_MASTER_BYTE_TRANSMITTED);

    // 发送数据
    I2C_SendData(I2C2, data);

    // 等待 EV8 事件(数据字节已发送)
    AW3644_WaitEvent(I2C2, I2C_EVENT_MASTER_BYTE_TRANSMITTED);

    // 发送停止信号
    I2C_GenerateSTOP(I2C2, ENABLE);
}

/**
 * @brief 从 AW3644 寄存器读取数据
 * @param reg 寄存器地址
 * @return 读取的数据
 */
uint8_t AW3644_Read(uint8_t reg)
{
    uint8_t data = 0;

    // 等待 I2C2 总线空闲
    AW3644_WaitEvent(I2C2, I2C_FLAG_BUSY);

    // 发送起始信号
    I2C_GenerateSTART(I2C2, ENABLE);

    // 等待 EV5 事件(发送起始信号完成)
    AW3644_WaitEvent(I2C2, I2C_EVENT_MASTER_MODE_SELECT);

    // 发送从机地址(写操作)
    I2C_Send7bitAddress(I2C2, AW3644_SLAVE_ADDR, I2C_Direction_Transmitter);

    // 等待 EV6 事件(从机地址已发送,ACK已收到)
    AW3644_WaitEvent(I2C2, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED);

    // 发送寄存器地址
    I2C_SendData(I2C2, reg);

    // 等待 EV8 事件(数据字节已发送)
    AW3644_WaitEvent(I2C2, I2C_EVENT_MASTER_BYTE_TRANSMITTED);

    // 发送重新启动信号
    I2C_GenerateSTART(I2C2, ENABLE);

    // 等待 EV5 事件(发送重新启动信号完成)
    AW3644_WaitEvent(I2C2, I2C_EVENT_MASTER_MODE_SELECT);

    // 发送从机地址(读操作)
    I2C_Send7bitAddress(I2C2, AW3644_SLAVE_ADDR, I2C_Direction_Receiver);

    // 等待 EV6 事件(从机地址已发送,ACK已收到)
    AW3644_WaitEvent(I2C2, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED);

    // 等待 EV7 事件(数据已接收)
    AW3644_WaitEvent(I2C2, I2C_EVENT_MASTER_BYTE_RECEIVED);

    // 读取数据
    data = I2C_ReceiveData(I2C2);

    // 发送停止信号
    I2C_GenerateSTOP(I2C2, ENABLE);

    return data;
}

uint8_t AW3644_GetChipID(void)
{
    return AW3644_Read(AW3644_Chip_ID);
}
