#ifndef __ZIGBEE_H
#define __ZIGBEE_H

#include "stm32l1xx_hal.h"

#define MainPoint 1
#define Termminal 0
//参数可以是 MainPoint 或者 Termminal
void Zigbee_CFGinit(uint8_t CFGname);

#endif
