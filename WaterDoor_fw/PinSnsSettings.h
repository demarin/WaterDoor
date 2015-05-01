/*
 * SnsPins.h
 *
 *  Created on: 17 џэт. 2015 у.
 *      Author: Kreyl
 */

/* ================ Documentation =================
 * There are several (may be 1) groups of sensors (say, buttons and USB connection).
 * There is GPIO and Pin data for every sensor.
 *
 */

#ifndef PINSNSSETTINGS_H_
#define PINSNSSETTINGS_H_

#define SIMPLESENSORS_ENABLED   TRUE

#if SIMPLESENSORS_ENABLED
#include "ch.h"
#include "hal.h"
#ifdef STM32F2XX
#include "kl_lib_f2xx.h"
#elif defined STM32L1XX_MD || defined STM32L1XX_HD
#include "kl_lib_L15x.h"
#endif

#include "main.h" // App.thd here
#include "evt_mask.h"

#define SNS_POLL_PERIOD_MS  1

enum PinSnsState_t {pssLo, pssHi, pssRising, pssFalling};
typedef void (*ftVoidPSnsStLen)(PinSnsState_t *PState, uint32_t Len);

// Single pin setup data
struct PinSns_t {
    GPIO_TypeDef *PGpio;
    uint16_t Pin;
    PinPullUpDown_t Pud;
    ftVoidPSnsStLen Postprocessor;
    void Init() const { PinSetupIn(PGpio, Pin, Pud); }
    void Off()  const { PinSetupAnalog(PGpio, Pin);  }
    bool IsHi() const { return PinIsSet(PGpio, Pin); }
};

// ================================= Settings ==================================
// Motion sensors handler
void ProcessSensors(PinSnsState_t *PState, uint32_t Len);

const PinSns_t PinSns[] = {
        {GPIOB,  4, pudPullUp, ProcessSensors}, // SNS1
        {GPIOB,  5, pudPullUp, ProcessSensors}, // SNS2
        {GPIOB,  0, pudPullUp, ProcessSensors}, // SNS3
        {GPIOB,  1, pudPullUp, ProcessSensors}, // SNS4
        {GPIOB,  6, pudPullUp, ProcessSensors}, // SNS5
        {GPIOB,  7, pudPullUp, ProcessSensors}, // SNS6
        {GPIOB,  8, pudPullUp, ProcessSensors}, // SNS7
        {GPIOB,  9, pudPullUp, ProcessSensors}, // SNS8
        {GPIOB,  3, pudPullUp, ProcessSensors}, // SNS9
        {GPIOB, 10, pudPullUp, ProcessSensors}, // SNS10
};
#define PIN_SNS_CNT     countof(PinSns)

#endif

#endif /* PINSNSSETTINGS_H_ */
