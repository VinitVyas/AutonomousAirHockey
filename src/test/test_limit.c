#include"pong.h"

/*********************************************************

LIMIT SWITCHER, WHEN DEPRESSED, RETURNS LOW  (OR ZERO)
                WHEN PRESSED,   RETURNS HIGH (OR ONE)

***********************************************************/

int main(int argc, char *argv[])
{
    static unsigned int k = 0, n = 0, step = 0, dir=0;
    static int toggle = 0;
    double span = 0, accel=800;
    int DIR = 66, STEP=69, LIMIT=23;
    if (gpioInit()!=0)
    {
        perror("GPIO Initialization Error");
        exit(1);
    }


    if(gpioSetPinDirection(DIR,OUTPUT)!=0)
    {
        perror("Error setting the PIN Direction");
        exit(1);
    }


    if(gpioSetPinDirection(STEP,OUTPUT)!=0)
    {
        perror("Error setting the PIN Direction");
        exit(1);
    }

    if(gpioSetPinDirection(LIMIT,INPUT)!=0)
    {
        perror("Error setting the PIN Direction");
        exit(1);
    }

    gpioWritePin( DIR, ++dir % 2 == 0 ? HIGH : LOW );
    /*while(temp<50)
    {

        gpioWritePin( STEP, LOW );
        usleep(50);
        gpioWritePin( STEP, HIGH );
        usleep(50);
        span = 0;
        temp++;
    }*/

    while(1)
    {
        switch ( step )
        {
            case 0:
                //n = digitalRead(6);
                n = (gpioReadPin( LIMIT ));

                if ( n == 1 )
                    k++;
                else
                    k = 0;

                if ( k > 5 )
                {
                    step++;
                    k = 0;
                    accel=800;
                }

                if ( span++ > accel )
                {
                    toggle = !toggle;
                    // digitalWrite( STEP, toogle ? HIGH : LOW );

                    gpioWritePin( STEP, toggle ? HIGH : LOW );
                    /*usleep(40);
                    gpioWritePin( STEP, HIGH );
                    usleep(40);*/
                    if(accel>300) accel-=0.15;
                    span=0;
                }

                break;
            case 1:
    //            n = digitalRead(6);
    //            gpioReadPin( LIMIT, &n );
    //            if ( n == 1 )
    //                k++;
    //            else
    //                k = 0;
    //
    //            if ( k > 50 )
            {
                step = 2;
                k = 0;
                //digitalWrite( DIR, ++dir % 2 ==0 ? HIGH : LOW );
                gpioWritePin( DIR, ++dir % 2 == 0 ? HIGH : LOW );
                accel=800;
                span=0;
            }
                break;


            case 2:

                if ( span++ > accel )
                {
                    n = (gpioReadPin( LIMIT ));

                    if ( n == 0 )
                        k++;
                    else
                        k = 0;

                    if ( k > 5 )
                    {
                        step = 0;
                        //accel = 800;
                        k = 0;
                    }

                    toggle = !toggle;
                    // digitalWrite( STEP, toogle ? HIGH : LOW );
                    usleep(10);
                    gpioWritePin( STEP, toggle ? HIGH : LOW );
                    /*usleep(40);
                     gpioWritePin( STEP, HIGH );
                     usleep(40);*/
                    span = 0;
                    if(accel>300) accel-=0.15;
                }
                break;
        }/*switch*/
    }
    cleanup();
    return 0;
}
