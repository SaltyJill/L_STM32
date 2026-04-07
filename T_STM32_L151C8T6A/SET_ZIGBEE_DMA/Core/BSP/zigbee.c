#include "zigbee.h"
#include "usart.h"
#include "stm32l1xx_hal.h"
#include <stdint.h>

#define CFG_Pin GPIO_PIN_8
#define CFG_GPIO_Port GPIOA
#define SLP_Pin GPIO_PIN_4
#define SLP_GPIO_Port GPIOB
#define zigbee_CFGenter HAL_GPIO_WritePin(CFG_GPIO_Port, CFG_Pin, GPIO_PIN_RESET)
#define zigbee_CFGexit HAL_GPIO_WritePin(CFG_GPIO_Port, CFG_Pin, GPIO_PIN_SET)
#define zigbee_SLPenter HAL_GPIO_WritePin(SLP_GPIO_Port, SLP_Pin, GPIO_PIN_RESET)
#define zigbee_SLPexit HAL_GPIO_WritePin(SLP_GPIO_Port, SLP_Pin, GPIO_PIN_SET)
#define netID 0xA0
#define netPT 0x08
#define pointADD 0x39

uint8_t ZIGBEE_CFGget[2] = {0x23, 0xA0}; // 获取设置
uint8_t ZIGBEE_CFGset[2] = {0x23, 0xFE}; // 设置参数
uint8_t TERMMINAL_CFG[14] = {
    netID | netPT, pointADD, // 节点地址 中心节点固定0000,有效范围0001-FFFE
    netID,                   // 网络ID   00-FF
    0x02,                    // 网络类型 01-网状网,02-星型网,07-对等网
    0x04,                    // 节点类型 01-中心节点,03-中继路由,04-终端节点
    0x02,                    // 发送模式 01-广播,02-主从,03-点对点
    0x06,                    // 波特率   01-1200,02-2400,03-4800,04-9600,05-19200,06-38400,07-57600,08-115200
    0x01,                    // 奇偶校验 01-NONE，02-EVEN，03-ODD
    0x01,                    // 数据格式 01-8位
    0x02,                    // 地址编码 01-ACSII，02-HEX
    0xFF,                    // 串口超时 05-FF
    netPT,                   // 无线频点 00-0F
    0x00,                    // 发射功率 缺省
    0x01                     // 源地址   01-不输出,02-ASCII 输出,03-HEX输出
};

void Delay1s_Flash(uint32_t time)
{
    led_off;
    for (uint32_t i = 0; i < time; i++)
    {
        led_on;
        HAL_Delay(500);
        led_off;
        HAL_Delay(500);
    }
}

void ST_Zigbee_CFGset(void)
{
    zigbee_CFGenter;
    Delay1s_Flash(3);
    HAL_UART_Transmit(&huart3, ZIGBEE_CFGset, 2, 100);
    HAL_UART_Transmit_DMA(&huart3, TERMMINAL_CFG, 14);
    zigbee_CFGexit;
}

void ST_Zigbee_CFGget(uint8_t *buf)
{
    zigbee_CFGenter;
    Delay1s_Flash(3);
    HAL_UART_Transmit(&huart3, ZIGBEE_CFGget, 2, 100);
    HAL_UART_Receive_DMA(&huart3, buf, 15);
    zigbee_CFGexit;
}
