/*
 * File:   main.cpp
 * Author: Kreyl
 * Project: Salem Box
 *
 * Created on Mar 22, 2015, 01:23
 */

#include <buttons.h>
#include "main.h"
#include "Sequences.h"
#include "led.h"
#include "kl_lib_L15x.h"

App_t App;
LedBlinker_t Led({GPIOA, 0});   // Just LED to blink
// PWM output for IR LED
PinOutputPWM_t<1, invInverted, omOpenDrain> IRLed(GPIOB, 15, TIM11, 1);
PinOutputPushPull_t Output12V(GPIOA, 6);

// Universal VirtualTimer callback
void TmrGeneralCallback(void *p) {
    chSysLockFromIsr();
    App.SignalEvtI((eventmask_t)p);
    chSysUnlockFromIsr();
}

int main(void) {
    // ==== Init Vcore & clock system ====
    SetupVCore(vcore1V8);
    // 12 MHz*4 = 48; 48/3 = 16
    Clk.SetupPLLMulDiv(pllMul4, pllDiv3);
    Clk.SetupFlashLatency(16);  // Setup Flash Latency for clock in MHz
    Clk.SetupBusDividers(ahbDiv1, apbDiv1, apbDiv1);
    uint8_t ClkResult = Clk.SwitchToPLL();
    Clk.UpdateFreqValues();

    // ==== Init OS ====
    halInit();
    chSysInit();

    // ==== Init Hard & Soft ====
    Uart.Init(115200);
    Uart.Printf("\rWaterDoor AHB=%u", Clk.AHBFreqHz);
    if(ClkResult != OK) Uart.Printf("\rClk failure: %u", ClkResult);

    App.InitThread();
    // IR LED generator
    IRLed.Init();
    IRLed.SetFrequencyHz(56000);
    IRLed.Set(1);

//    PinSensors.Init();

    Led.Init();
    Led.StartSequence(lsqBlink);
    Output12V.Init();
    // Main cycle
    App.ITask();
}

__attribute__ ((__noreturn__))
void App_t::ITask() {
    while(true) {
//        uint32_t EvtMsk = chEvtWaitAny(ALL_EVENTS);

        chThdSleepMilliseconds(207);
//        Output12V.SetHi();
//        chThdSleepMilliseconds(207);
//        Output12V.SetLo();
#if 1 // ==== Motion sensors ====

#endif
    } // while true
}

#if 1 // ===================== Load/save settings ==============================
void App_t::LoadSettings() {
    if(EE_PTR->ID < ID_MIN or EE_PTR->ID > ID_MAX) Settings.ID = ID_DEFAULT;
    else Settings.ID = EE_PTR->ID;

    if(EE_PTR->DurationActive_s < DURATION_ACTIVE_MIN_S or
       EE_PTR->DurationActive_s > DURATION_ACTIVE_MAX_S
       ) {
        Settings.DurationActive_s = DURATION_ACTIVE_DEFAULT;
    }
    else Settings.DurationActive_s = EE_PTR->DurationActive_s;

    Settings.DeadtimeEnabled = EE_PTR->DeadtimeEnabled;

    SettingsHasChanged = false;
}

void App_t::SaveSettings() {
    chSysLock();
    if(chVTIsArmedI(&ITmrSaving)) chVTResetI(&ITmrSaving);  // Reset timer
    chVTSetEvtI(&ITmrSaving, S2ST(4), EVTMSK_SAVE);
    chSysUnlock();
}

void App_t::ISaveSettingsReally() {
    Flash_t::UnlockEE();
    chSysLock();
    uint8_t r = OK;
    uint32_t *Src = (uint32_t*)&Settings;
    uint32_t *Dst = (uint32_t*)EE_PTR;
    for(uint32_t i=0; i<SETTINGS_SZ32; i++) {
        r = Flash_t::WaitForLastOperation();
        if(r != OK) break;
        *Dst++ = *Src++;
    }
    Flash_t::LockEE();
    chSysUnlock();
    if(r == OK) {
        Uart.Printf("\rSettings saved");
        SettingsHasChanged = false;
    }
    else {
        Uart.Printf("\rSettings saving failure");
    }
}
#endif
