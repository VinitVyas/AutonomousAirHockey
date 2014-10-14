/****************************************************************************//**\file
 * Contains the actuator thread business logic implementation.
 *
 * Real-time Embedded Systems ECEN5623, Fall 2013
 * Group7:  Bhardwaj, Bhatt, Joshi, Lopez, Vyas
 ****************************************************************************/
#include "pong.h"

static volatile int     paddlePos=25;
static volatile int     predictedPos=15;
static volatile byte    newData=0;

int main(int argc, char *argv[])
{
    unsigned int dir=0;
    signed int count=0, lcount=0;
    signed int tem=0,i=0, tem1, pred,pos,span=0,accel=800;
    int DIR = 45, STEP=44, LIMIT=27,toggle=0;
    keepWorking=1;
    int moveOneGrid(int);

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

    gpioWritePin( DIR, HIGH );


    /************************************************************************************************************************/



    while ( keepWorking )
    {


        pred=predictedPos;
        pos=paddlePos;
        if(pred>17) pred=17;
        if(pred<2) pred=1;

        tem = pred-pos;
        tem1=abs(tem)+1;

        tem1--;
        //printf("Value of tem %d\n",tem1);


        if ( tem1>2 )
        {
            gpioWritePin( DIR, (pred> pos)? LOW:HIGH );

            if ( span++ > accel )
            {

                //printf("Write Start:");
                for(i=0;i<10000;i++)
                {

                    gpioWritePin(STEP, toggle ? HIGH : LOW );
                    toggle = !toggle;

                    usleep( 50 );
                }

                printf("Write STOP:");


                span = 0;
                if ( accel > 400 )
                    accel -= 0.2;
            }

        }
        else
        {
            gpioWritePin( DIR, HIGH );
            printf("Position reached\n");
        }

    }


    printf( FG13 "  *** Actuator exited...");
    return NULL;
}


/*int moveOneGrid(int STEP)
{
    int i, span=0, accel=900, toggle=0, tem1=415*609;
    for(i=0;i<tem1;i++)
    {
        if ( span++ > accel )
        {
            toggle = !toggle;
            usleep( 50 );
            gpioWritePin( STEP, toggle ? HIGH : LOW );
            span = 0;
            if ( accel > 400 )
                accel -= 0.2;
        }

    }//for loop
    return(0);
}
*/
