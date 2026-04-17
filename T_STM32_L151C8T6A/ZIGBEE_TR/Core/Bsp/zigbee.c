#include "zigbee.h"
#include "usart.h"

//--- 网络设置 ---
#define netID 0x50    // 网络ID区分组
#define netPoint 0x0E // 频点，区分频道。推荐4-2.425,9-2.450,E-2.475,F-2.480,避免WiFi干扰
// 终端节点地址构成，0x(NETID|netPoint)+0x(netADD)
#define pointADD 0x50 // 节点地址
//--- 引脚定义 ---
#define zb_RST_Pin GPIO_PIN_2
#define zb_RST_GPIO_Port GPIOB
#define CFG_Pin GPIO_PIN_8
#define CFG_GPIO_Port GPIOA
#define SLP_Pin GPIO_PIN_4
#define SLP_GPIO_Port GPIOB

#define Zigbee_RSTenter HAL_GPIO_WritePin(zb_RST_GPIO_Port, zb_RST_Pin, GPIO_PIN_RESET)
#define Zigbee_RSTexit HAL_GPIO_WritePin(zb_RST_GPIO_Port, zb_RST_Pin, GPIO_PIN_SET)
#define Zigbee_CFGenter HAL_GPIO_WritePin(CFG_GPIO_Port, CFG_Pin, GPIO_PIN_RESET)
#define Zigbee_CFGexit HAL_GPIO_WritePin(CFG_GPIO_Port, CFG_Pin, GPIO_PIN_SET)
#define Zigbee_SLPenter HAL_GPIO_WritePin(SLP_GPIO_Port, SLP_Pin, GPIO_PIN_RESET)
#define Zigbee_SLPexit HAL_GPIO_WritePin(SLP_GPIO_Port, SLP_Pin, GPIO_PIN_SET)
//--- ZIGBEE 命令 ---
//  uint8_t ZIGBEE_CFGget[2] = {0x23, 0xA0}; // 查询参数,返回A2+14byte
uint8_t ZIGBEE_CFGset[2] = {0x23, 0xFE}; // 设置参数
uint8_t ZIGBEE_CFGrst[2] = {0x23, 0x23}; // 设备重启 CFG 必须高电平
// ZIGBEE 参数
uint8_t MainPoint_CFG[14] = {
    0x00, 0x00, // 节点地址 中心节点固定0000,有效范围0001-FFFE
    netID,      // 网络ID   00-FF
    0x02,       // 网络类型 01-网状网,02-星型网,07-对等网
    0x01,       // 节点类型 01-中心节点,03-中继路由,04-终端节点
    0x02,       // 发送模式 01-广播,02-主从,03-点对点
    0x06,       // 波特率   01-1200,02-2400,03-4800,04-9600,05-19200,06-38400,07-57600,08-115200
    0x01,       // 奇偶校验 01-NONE，02-EVEN，03-ODD
    0x01,       // 数据格式 01-8位
    0x02,       // 地址编码 01-ACSII，02-HEX
    0xFF,       // 串口超时 05-FF
    netPoint,      // 无线频点 00-0F
    0x00,       // 发射功率 缺省
    0x01        // 源地址   01-不输出,02-ASCII 输出,03-HEX输出
};
uint8_t Termminal_CFG[14] = {
    netID | netPoint, pointADD, // 节点地址 中心节点固定0000,有效范围0001-FFFE
    netID,                      // 网络ID   00-FF
    0x02,                       // 网络类型 01-网状网,02-星型网,07-对等网
    0x04,                       // 节点类型 01-中心节点,03-中继路由,04-终端节点
    0x02,                       // 发送模式 01-广播,02-主从,03-点对点
    0x06,                       // 波特率   01-1200,02-2400,03-4800,04-9600,05-19200,06-38400,07-57600,08-115200
    0x01,                       // 奇偶校验 01-NONE，02-EVEN，03-ODD
    0x01,                       // 数据格式 01-8位
    0x02,                       // 地址编码 01-ACSII，02-HEX
    0xFF,                       // 串口超时 05-FF
    netPoint,                      // 无线频点 00-0F
    0x00,                       // 发射功率 缺省
    0x01                        // 源地址   01-不输出,02-ASCII 输出,03-HEX输出
};

void Zigbee_CFGinit(uint8_t CFGname)
{
    Zigbee_RSTexit;
    Zigbee_SLPexit;
    HAL_Delay(50);
    Zigbee_CFGenter;
    HAL_Delay(3000);
		//Zigbee_RSTexit;
		//HAL_Delay(50);
    HAL_UART_Transmit(&huart3, ZIGBEE_CFGset, 2, 100);
    if (CFGname)
    {
        HAL_UART_Transmit(&huart3, MainPoint_CFG, 14, 500);
    }
    else
    {
        HAL_UART_Transmit(&huart3, Termminal_CFG, 14, 500);
    }
    Zigbee_CFGexit;
    HAL_Delay(50);
    HAL_UART_Transmit(&huart3, ZIGBEE_CFGrst, 2, 100);
    //Zigbee_RSTexit;
    //HAL_Delay(50);
    //Zigbee_RSTexit;
}
