#include <math.h>

class LstSquQuadRegr{
public:
     /* instance variables */

    double pointArray[10][2];
    int numOfEntries;



    double Sx;
    double Sy;
    double Sx2;
    double Sx3;
    double Sx4;
    double Sxy;
    double Sx2y;
    double y_tot;
    double ss_tot;

    /*constructor */
    LstSquQuadRegr()
    {
        clearValues();

    }
    void clearValues(){

        numOfEntries = 0;
        Sx = 0;
        Sy = 0;
        Sx2 = 0;
        Sx3 = 0;
        Sx4 = 0;
        Sxy = 0;
        Sx2y = 0;
        y_tot = 0;


    }
    /*instance methods */
    /// <summary>
    /// add point pairs
    /// </summary>
    /// <param name="x">x value</param>
    /// <param name="y">y value</param>
    void AddPoints(double x, double y)
    {
        numOfEntries +=1;
        double ppair[2];
        ppair[0] = x;
        ppair[1] = y;
        pointArray[numOfEntries-1][0] = x;
        pointArray[numOfEntries-1][1] = y;

        Sx += ppair[0];

        Sy += ppair[1];

        Sx2 += sqr(ppair[0]); // sum of x^2


        Sx3 += pow(ppair[0], 3); // sum of x^3


        Sx4 += pow(ppair[0], 4); // sum of x^4


        Sxy += ppair[0] * ppair[1]; // sum of x*y


        Sx2y += sqr(ppair[0]) * ppair[1]; // sum of x^2*y


        y_tot += ppair[1];

        ss_tot += sqr(ppair[1] - getYMean());

    }

    /// <summary>
    /// returns the a term of the equation ax^2 + bx + c
    /// </summary>
    /// <returns>a term</returns>
    double aTerm()
    {
        if (numOfEntries < 3)
        {
                return 0;
        }
        //notation sjk to mean the sum of x_i^j*y_i^k.
        double s40 = Sx4(); //sum of x^4
        double s30 = Sx3(); //sum of x^3
        double s20 = Sx2(); //sum of x^2
        double s10 = Sx();  //sum of x
        double s00 = numOfEntries;
        //sum of x^0 * y^0  ie 1 * number of entries

        double s21 = Sx2y(); //sum of x^2*y
        double s11 = Sxy();  //sum of x*y
        double s01 = Sy();   //sum of y

        //a = Da/D
        return (s21*(s20 * s00 - s10 * s10) -
                s11*(s30 * s00 - s10 * s20) +
                s01*(s30 * s10 - s20 * s20))
                /
                (s40*(s20 * s00 - s10 * s10) -
                 s30*(s30 * s00 - s10 * s20) +
                 s20*(s30 * s10 - s20 * s20));
    }

    /// <summary>
    /// returns the b term of the equation ax^2 + bx + c
    /// </summary>
    /// <returns>b term</returns>
    double bTerm()
    {
        if (numOfEntries < 3)
        {
            return 0;
        }
        //notation sjk to mean the sum of x_i^j*y_i^k.
        double s40 = Sx4(); //sum of x^4
        double s30 = Sx3(); //sum of x^3
        double s20 = Sx2(); //sum of x^2
        double s10 = Sx();  //sum of x
        double s00 = numOfEntries;
        //sum of x^0 * y^0  ie 1 * number of entries

        double s21 = Sx2y(); //sum of x^2*y
        double s11 = Sxy();  //sum of x*y
        double s01 = Sy();   //sum of y

        //b = Db/D
        return (s40*(s11 * s00 - s01 * s10) -
                s30*(s21 * s00 - s01 * s20) +
                s20*(s21 * s10 - s11 * s20))
                /
                (s40 * (s20 * s00 - s10 * s10) -
                 s30 * (s30 * s00 - s10 * s20) +
                 s20 * (s30 * s10 - s20 * s20));
    }

    /// <summary>
    /// returns the c term of the equation ax^2 + bx + c
    /// </summary>
    /// <returns>c term</returns>
    double cTerm()
    {
        if (numOfEntries < 3)
        {
            return 0;
        }
        //notation sjk to mean the sum of x_i^j*y_i^k.
        double s40 = Sx4(); //sum of x^4
        double s30 = Sx3(); //sum of x^3
        double s20 = Sx2(); //sum of x^2
        double s10 = Sx();  //sum of x
        double s00 = numOfEntries;
        //sum of x^0 * y^0  ie 1 * number of entries

        double s21 = Sx2y(); //sum of x^2*y
        double s11 = Sxy();  //sum of x*y
        double s01 = Sy();   //sum of y

        //c = Dc/D
        return (s40*(s20 * s01 - s10 * s11) -
                s30*(s30 * s01 - s10 * s21) +
                s20*(s30 * s11 - s20 * s21))
                /
                (s40 * (s20 * s00 - s10 * s10) -
                 s30 * (s30 * s00 - s10 * s20) +
                 s20 * (s30 * s10 - s20 * s20));
    }

    double rSquare() // get r-squared
    {
        if (numOfEntries < 3)
        {
            return 0;
        }
        // 1 - (residual sum of squares / total sum of squares)
        return 1 - getSSErr() / ss_tot();
    }


    /*helper methods*/


    double getYMean() // mean value of y
    {

        return y_tot/numOfEntries;
    }





    double getSSErr(){
        //the sum of the squares of te difference between
        //the measured y values and the values of y predicted by the equation
        double ss_err = 0;
        for(int i = 0; i < numOfEntries; i++){
            ss_err += sqr(pointArray[i][1] - getPredictedY(pointArray[i][0]));
        }
        return ss_err;
    }
    double getPredictedY(double x)
    {
        //returns value of y predicted by the equation for a given value of x
        return aTerm() * sqr(x) + bTerm() * x + cTerm();
    }
    double sqr (double x)
    {
       return x * x;                            /* compute square of argument    */
    }
    double getPredictedYSlope(double x){
        return (bTerm() + 2.0*aTerm()*x);
    }
};
