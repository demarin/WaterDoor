#include "linreg.h"


class VelociMeter {
public:
    bool runFlag;
    bool adultFlag;
    bool lastSnsFlag;
    int valveClosed;
    int numberOfSensors;
    double criticalVelo;
    double childCriticalVelo;
    double firstSensorTime;

    void (*openValve)();
    void (*closeValve)();
    void (*updateInactivityTimer)();
    void (*goLight)();
    void (*stopLight)();

    double data[30][2]; // [time, sensorNumber] for future analysis functions
    int dataLength;

    double *coords;
    double waterCoord;
    double lastCoord;

    double currentVelocity;

    double processedPoints[4][2];
    int pPLength;

    LinearReg regression;

    VelociMeter(void (*openFunc)(),void (*closeFunc)(),void (*timerFunc)(), void (*ledOn)(), void (*ledOff)(), double criticalVelocity,double childCriticalVelocity, double sensorsCoords[], int sensorsNum, double lastPointCoordinate, double waterCoordinate){
        openValve = openFunc;
        closeValve = closeFunc;
        updateInactivityTimer = timerFunc;
        goLight = ledOn;
        stopLight = ledOff;


        criticalVelo = criticalVelocity;
        childCriticalVelo = childCriticalVelocity;
        coords = sensorsCoords;
        numberOfSensors = sensorsNum;
        lastCoord = lastPointCoordinate;
        waterCoord = waterCoordinate;
        clearValues();

    }
    void clearValues(){
        firstSensorTime = 0;
        adultFlag = false;
        runFlag = false;
        lastSnsFlag = false;

        dataLength = 0;
        pPLength = 0;
        regression.clearValues();
        goLight();
        Uart.Printf("\r clearing values...");

    }
    void processPoint(double t,int sensorNum){
        updateInactivityTimer();
        stopLight();
        data[dataLength][0] = t;
        data[dataLength][1] = sensorNum;
        if(sensorNum%2==1){
            adultFlag = true;
        }
        if(dataLength<29){
            dataLength ++;
        }
        else{
            dataLength = 0; //It is temporary cap, 'cause we don't use data now. Ќа самом деле это офигеть как правильно, ибо позвол€ет внедрить циклическую перезапись.
        }
        if(pPLength > 0){
            if(processedPoints[pPLength-1][1] < coords[sensorNum]){
                countFunc(t,coords[sensorNum]);
            }
        }
        else{
            countFunc(t,coords[sensorNum]);
        }
    }
    void processLastPoint(double t){
        if(lastSnsFlag == false){
            lastSnsFlag = true;
            if(firstSensorTime == 0){
                firstSensorTime = t;
                regression.addPoint(0,lastCoord);
            }
            else{
                regression.addPoint((t-firstSensorTime),lastCoord);

            }

            currentVelocity = regression.countK();
            if((currentVelocity*1000.0)>(adultFlag==true?criticalVelo:childCriticalVelo)){
                Uart.Printf("\rYou are pretty fast, it is enough to close valve.");
                closeValve();
            }
            Uart.Printf("\rVelocity was: %d meters per second", (int)(currentVelocity*1000.0));
        }
        stopLight();
        updateInactivityTimer();

    }
    void processExitPoint(){
        clearValues();
        openValve();
    }
    void inactivityReset(){
        Uart.Printf("\r 1 sec of inactivity, reset");
        clearValues();
        openValve();

    }
    void countFunc(double t, double x){
        Uart.Printf("\r New point: t = %d  x = %d cm", (int)(t*100.0), (int)(x*100.0));
        processedPoints[pPLength][0] = t;
        processedPoints[pPLength][1] = x;
        pPLength ++;
        if(firstSensorTime == 0){
            firstSensorTime = t;
            regression.addPoint(0,x);
        }
        else{
            regression.addPoint((t-firstSensorTime),x);

        }
    }


};
