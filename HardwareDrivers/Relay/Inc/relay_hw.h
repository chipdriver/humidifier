#ifndef __RELAY_HW_H
#define __RELAY_HW_H

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"

#define MIST_RELAY_PORT       GPIOA
#define MIST_RELAY_PIN        GPIO_PIN_1

#define FAN_RELAY_PORT        GPIOB
#define FAN_RELAY_PIN         GPIO_PIN_0

#define RELAY_ACTIVE_LEVEL    GPIO_PIN_SET
#define RELAY_INACTIVE_LEVEL  GPIO_PIN_RESET

void Relay_HW_Init(void);

void MistRelay_On(void);
void MistRelay_Off(void);

void FanRelay_On(void);
void FanRelay_Off(void);

void Relay_HW_AllOff(void);
void Relay_HW_TestControlSide(void);

#ifdef __cplusplus
}
#endif

#endif /* __RELAY_HW_H */
 