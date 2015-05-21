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
#include "SimpleSensors.h"
#include "waterdoor.h"
#include <stdio.h>

void setTimer();
void openV();
void closeV();
void ledTurnOff();
void ledTurnOn();

App_t App;
LedBlinker_t Led({GPIOA, 0});   // Just LED to blink
// PWM output for IR LED
PinOutputPWM_t<1, invInverted, omOpenDrain> IRLed(GPIOB, 15, TIM11, 1);
// 12V output
PinOutputPushPull_t Output12V(GPIOA, 6);
// Time for rising and falling
struct SnsData_t {
    systime_t TimeMoveIn, TimeMoveOut;
    bool HasChanged;
};

SnsData_t SnsData[PIN_SNS_CNT];


//Need to input real data here. Coordinates in meters.
double ordinarySensorsCoords[] = {0.0, 0.0, 0.4, 0.4, 0.8, 0.8};
double numberOfOrdinarySensors = countof(ordinarySensorsCoords);
double lastSensorsCoord = 1.2;
double waterCoord = 2.0;
double requiredVelo = 3.0; //meters per second
double requiredVeloForKids = 1.5; //meters per second

//Initialisation with parameters of our installation
VelociMeter velociMeter(openV,closeV,setTimer, ledTurnOn, ledTurnOff, requiredVelo, requiredVeloForKids, ordinarySensorsCoords, numberOfOrdinarySensors, lastSensorsCoord, waterCoord);
/*
static WORKING_AREA(waTimerThread, 256);
__attribute__((noreturn))
static void TimerThread(void *arg) {
    chRegSetThreadName("Timer");
    launchTimer();
}

chThdCreateStatic(waTimerThread, sizeof(waTimerThread), NORMALPRIO, (tfunc_t)TimerThread, NULL);
*/
systime_t timer = 0;
bool timerOn = false;

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

    PinSensors.Init();

    Led.Init();
    Led.StartSequence(lsqBlink);
    Output12V.Init();
    // Main cycle

    App.ITask();
}

__attribute__ ((__noreturn__))
void App_t::ITask() {
    Uart.Printf("\rHello!");
    while(true) {
        uint32_t EvtMsk = chEvtWaitAny(ALL_EVENTS);

/*        if(EvtMsk == EVTMSK_SNS) {
            for(uint8_t i=0; i<PIN_SNS_CNT; i++) {
                if(SnsData[i].HasChanged) {
                    SnsData[i].HasChanged = false;

                }
            }
            Uart.Printf("\r");
        }*/
        if(EvtMsk == EVTMSK_TIMER) {
            while(timerOn){
                Uart.Printf("\r Time is %u, sleep till %u, sleep for %u",chTimeNow(),timer,((int)timer-(int)chTimeNow()+50));
                chThdSleepMilliseconds(((int)timer-(int)chTimeNow()+50));
                if(timerOn && timer<=chTimeNow()){
                    Uart.Printf("\r Timer is %u, and time is %u, din-don", timer, chTimeNow());
                    timerOn = false;
                    velociMeter.inactivityReset();

                }
            }
        }

//        chThdSleepMilliseconds(207);
//        Output12V.SetHi();
//        chThdSleepMilliseconds(207);
//        Output12V.SetLo();
    } // while true
}

#if 1 // ==== Motion sensors ====
void ProcessSensors(PinSnsState_t *PState, uint32_t Len) {
    bool HasChanged = false;
    systime_t Now = chTimeNow();
    for(uint8_t i=0; i<PIN_SNS_CNT; i++) {
        if(PState[i] == pssFalling and SnsData[i].TimeMoveOut != Now) {

            HasChanged = true;
            SnsData[i].TimeMoveOut = Now;
            SnsData[i].HasChanged = true;
            Uart.Printf("\rSns%02u: In=%u; Out=%u", i, SnsData[i].TimeMoveIn, SnsData[i].TimeMoveOut);
            //App.SignalEvt(EVTMSK_SNS);
        }
        if(PState[i] == pssRising and SnsData[i].TimeMoveIn != Now) {

            HasChanged = true;
            SnsData[i].TimeMoveIn = Now;
            SnsData[i].HasChanged = true;
            //App.SignalEvt(EVTMSK_SNS);
            Uart.Printf("\rSns%02u: In=%u; Out=%u", i, SnsData[i].TimeMoveIn, SnsData[i].TimeMoveOut);
            velociMeter.processPoint((double)Now,i);
        }
    }

}
void ProcessLastSensor(PinSnsState_t *PState, uint32_t Len) {
    bool HasChanged = false;
    systime_t Now = chTimeNow();
    for(uint8_t i=0; i<PIN_SNS_CNT; i++) {
        if(PState[i] == pssFalling and SnsData[i].TimeMoveOut != Now) {

            HasChanged = true;
            SnsData[i].TimeMoveOut = Now;
            SnsData[i].HasChanged = true;
            Uart.Printf("\rSns%02u, last sensor: In=%u; Out=%u", i, SnsData[i].TimeMoveIn, SnsData[i].TimeMoveOut);
            //App.SignalEvt(EVTMSK_SNS);
        }
        if(PState[i] == pssRising and SnsData[i].TimeMoveIn != Now) {

            HasChanged = true;
            SnsData[i].TimeMoveIn = Now;
            SnsData[i].HasChanged = true;
            //App.SignalEvt(EVTMSK_SNS);
            Uart.Printf("\rSns%02u, last sensor: In=%u; Out=%u", i, SnsData[i].TimeMoveIn, SnsData[i].TimeMoveOut);
            velociMeter.processLastPoint((double)Now);
        }
    }

}
void ProcessExitSensor(PinSnsState_t *PState, uint32_t Len) {
    bool HasChanged = false;
    systime_t Now = chTimeNow();
    for(uint8_t i=0; i<PIN_SNS_CNT; i++) {
        if(PState[i] == pssFalling and SnsData[i].TimeMoveOut != Now) {

            HasChanged = true;
            SnsData[i].TimeMoveOut = Now;
            SnsData[i].HasChanged = true;
            Uart.Printf("\rSns%02u, Exit sensor: In=%u; Out=%u", i, SnsData[i].TimeMoveIn, SnsData[i].TimeMoveOut);
            //App.SignalEvt(EVTMSK_SNS);
        }
        if(PState[i] == pssRising and SnsData[i].TimeMoveIn != Now) {

            HasChanged = true;
            SnsData[i].TimeMoveIn = Now;
            SnsData[i].HasChanged = true;
            Uart.Printf("\rSns%02u, Exit sensor: In=%u; Out=%u", i, SnsData[i].TimeMoveIn, SnsData[i].TimeMoveOut);
            velociMeter.processExitPoint();

        }
    }

}

void setTimer(){

    timer = chTimeNow() + (systime_t)1000;
    Uart.Printf("\r Setting timer to %u...",timer);
    if(timerOn == false){
        timerOn = true;
        App.SignalEvt(EVTMSK_TIMER);
    }
    Uart.Printf("\r Setting timer finished");

}

//cannot send SetHi and SetLo directly as pointers
void closeV(){
    Uart.Printf("\rFinish! closig valve... time is: %u",chTimeNow());
    Output12V.SetHi();


}
void openV(){
    Output12V.SetLo();
    Uart.Printf("\r opening valve...");
}
void ledTurnOff(){
    Led.Off();
}
void ledTurnOn(){
    Led.On();
}

#endif
