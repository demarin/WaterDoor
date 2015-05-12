#include "linreg.h"


class VelociMeter {
public:
    bool runFlag;
    int valveClosed;
    int numberOfSensors;
    double criticalVelo;

    void (*openValve)();
    void (*closeValve)();
    void (*updateInactivityTimer)();

    double data[30][2]; // [time, sensorNumber] for future analysis functions
    int dataLength;

    double *coords;
    double waterCoord;
    double lastCoord;

    double currentVelocity;

    double processedPoints[4][2];
    int pPLength;

    LinearReg regression;

    VelociMeter(void (*openFunc)(),void (*closeFunc)(),void (*timerFunc)(),double criticalVelocity, double sensorsCoords[], int sensorsNum, double lastPointCoordinate, double waterCoordinate){
        openValve = openFunc;
        closeValve = closeFunc;
        updateInactivityTimer = timerFunc;

        criticalVelo = criticalVelocity;
        coords = sensorsCoords;
        numberOfSensors = sensorsNum;
        lastCoord = lastPointCoordinate;
        waterCoord = waterCoordinate;
        clearValues();

    }
    void clearValues(){
        runFlag = false;
        dataLength = 0;
        pPLength = 0;
        regression.clearValues();
        Uart.Printf("\r clearing values...");

    }
    void processPoint(double t,int sensorNum){
        updateInactivityTimer();
        data[dataLength][0] = t;
        data[dataLength][1] = sensorNum;
        if(dataLength<29){
            dataLength ++;
        }
        else{
            dataLength = 0; //It is temporary cap, 'cause we don't use data now
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
        regression.addPoint(t,lastCoord);
        currentVelocity = regression.countK();
        if((currentVelocity*1000.0)>criticalVelo){
            Uart.Printf("\rYou are pretty fast, it is enough to close valve.");
            closeValve();
        }
        Uart.Printf("\rVelocity was: %d meters per second", (int)(currentVelocity*1000.0));
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
        regression.addPoint(t,x);

    }


};

/*
char foo[32];
char * buf = foo;
*/
/*
char * doubleToStr(double doubleVar){
    snprintf(buf, sizeof(foo), "%g", doubleVar);
    return buf;
}
*/
