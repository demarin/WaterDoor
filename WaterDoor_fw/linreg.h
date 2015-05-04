#include <math.h>                           /* math functions                */


class LinearReg {
public:
    double   b;                                 /* y-intercept of best fit line  */
    double   k;                                 /* slope of best fit line        */
    double   n;                           /* number of data points         */
    double   r;                                 /* correlation coefficient       */
    double   sumx;                        /* sum of x                      */
    double   sumx2;                       /* sum of x**2                   */
    double   sumxy;                       /* sum of x * y                  */
    double   sumy;                        /* sum of y                      */
    double   sumy2;                       /* sum of y**2                   */

    LinearReg(){
        clearValues();
    }
    void clearValues(){
        n = 0.0;
        sumx = 0.0;
        sumx2 = 0.0;
        sumxy = 0.0;
        sumy = 0.0;
        sumy2 = 0.0;
    }
    void addPoint(double t, double x){
        n += 1.0;                             /* increment num of data points  */
        sumx  += t;                           /* compute sum of x              */
        sumx2 += t * t;                       /* compute sum of x^2           */
        sumxy += t * x;                       /* compute sum of x * y          */
        sumy  += x;                           /* compute sum of y              */
        sumy2 += x * x;                       /* compute sum of y^2           */

    }
    double countB(){
        b = (sumy * sumx2  -  sumx * sumxy) /    /* compute y-intercept           */
             (n * sumx2  -  sqr(sumx));
        return b;
    }
    double countK(){
        k = (n * sumxy  -  sumx * sumy) /        /* compute slope                 */
               (n * sumx2 - sqr(sumx));
        return k;
    }
    double countR(){
        r = (sumxy - sumx * sumy / n) /          /* compute correlation coeff     */
              sqrt((sumx2 - sqr(sumx)/n) *
              (sumy2 - sqr(sumy)/n));
        return r;
    }
    double sqr (double x)
    {
       return x * x;                            /* compute square of argument    */
    }
};

