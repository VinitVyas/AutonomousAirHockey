#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "pong.h"
//0x090 0x0f  /* P8_07 66 OUTPUT DIR   */
//0x094 0x0f  /* P8_08 67 OUTPUT  STEP    */
//0x09c 0x0f  /* P8_09 69 OUTPUT  KICK  */
//0x098 0x2f  /* P8_10 68 INPUT  limit  */
//0x034 0x2f  /* P8_11 45 INPUT  photo  */

#define KICK_DELAY 1100

unsigned int DIR = 66;
unsigned int STEP = 69;
unsigned int KICK = 45;
unsigned int LIMIT = 23;
unsigned int PHOTO = 47;

unsigned int DIR2 = 67;
unsigned int STEP2 = 68;
unsigned int KICK2 = 44;
unsigned int LIMIT2 = 26;
unsigned int PHOTO2 = 46;

void kickerTask();
void kickerTask2();
void paddleTask();



int main( int argc, char *argv[] )
{
    printf( "\e[2J\e[H     \n" );
    printf( "Testing the GPIO Pins\n" );

    gpioInit();

    gpioSetPinDirection( DIR, OUTPUT );
    gpioSetPinDirection( STEP, OUTPUT );
    gpioSetPinDirection( KICK, OUTPUT );
    gpioSetPinDirection( LIMIT, INPUT );
    gpioSetPinDirection( PHOTO, INPUT );

    gpioSetPinDirection( DIR2, OUTPUT );
    gpioSetPinDirection( STEP2, OUTPUT );
    gpioSetPinDirection( KICK2, OUTPUT );
    gpioSetPinDirection( LIMIT2, INPUT );
    gpioSetPinDirection( PHOTO2, INPUT );

    gpioWritePin( KICK, LOW );
    gpioWritePin( KICK2, LOW );
    // gpioWritePin(KICK, HIGH);
    // usleep(50000);         // on for 200ms
    // gpioWritePin(KICK, LOW);

    /*
     unsigned int value = LOW;
     do {
     gpioReadPin(LIMIT, &n);
     cout << ".";
     usleep(1000);      // sleep for one millisecond
     } while (value==HIGH);
     cout << endl <<  "Button was just pressed!" << endl;

     cout << "Finished Testing  " << endl;
     */
     int count=0;
     unsigned int n, n2;
    while (count ++ < 5 )
    {

     // paddleTask();
      //  paddleTask2();
       kickerTask();
       kickerTask2();
        //   kickerTask( PHOTO2, KICK2);
//
//         gpioWritePin(KICK, LOW);
//         sleep(1);
//         gpioWritePin(KICK, HIGH);
//         sleep(1);

//        n = gpioReadPin( PHOTO  );
//        n2 = gpioReadPin( PHOTO2 );

//        if ( n == 0 )
//        {
//            //gpioWritePin(KICK, HIGH);
//            printf( "\e[1;40H A[  ] " );
//        }
//        else
//        {
//            //gpioWritePin(KICK, LOW);
//            printf( "\e[1;40H A[on] " );
//        }
//
//        if ( n2 == 0 )
//        {
//            //gpioWritePin(KICK, HIGH);
//            printf( "  B[  ]      \n" );
//        }
//        else
//        {
//            //gpioWritePin(KICK, LOW);
//            printf( "  B[on]     \n" );
//        }
//
//        n = gpioReadPin( LIMIT  );
//        n2 = gpioReadPin( LIMIT2 );
//
//        if ( n == 0 )
//        {
//            //gpioWritePin(KICK, HIGH);
//            printf( "\e[2;40H A[  ] " );
//        }
//        else
//        {
//            //gpioWritePin(KICK, LOW);
//            printf( "\e[2;40H A[on] " );
//        }
//
//        if ( n2 == 0 )
//        {
//            //gpioWritePin(KICK, HIGH);
//            printf( "  B[  ]      \n" );
//        }
//        else
//        {
//            //gpioWritePin(KICK, LOW);
//            printf( "  B[on]     \n" );
//        }

    }

    puts("IO Fixed.");
    return 0;

}

void paddleTask()
{
    static unsigned int k = 0, n = 0, step = 0, dir = 1;
    static bool toogle = false;
    static unsigned long span = 0;

    switch ( step )
    {
        case 0:
            //n = digitalRead(6);
            n = gpioReadPin( LIMIT2 );

            if ( n == 1 )
                k++;
            else
                k = 0;

            if ( k > 10 )
            {
                step++;
                k = 0;
            }

            if ( span++ > 1 )
            {
                toogle = !toogle;
                // digitalWrite( STEP, toogle ? HIGH : LOW );
                gpioWritePin( STEP2, toogle ? HIGH : LOW );
                span = 0;
            }

            break;
        case 1:
            //n = digitalRead(6);
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
            gpioWritePin( DIR2, ++dir % 2 == 0 ? HIGH : LOW );
        }
            break;
        case 2:
            n = gpioReadPin( LIMIT2);

            if ( n == 0 )
                k++;
            else
                k = 0;

            if ( k > 10 )
            {
                step = 0;
                k = 0;
            }

            if ( span++ > 1 )
            {
                toogle = !toogle;
                // digitalWrite( STEP, toogle ? HIGH : LOW );
                gpioWritePin( STEP2, toogle ? HIGH : LOW );
                span = 0;
            }

            break;
    }/*switch*/

}


void kickerTask()
{
    static unsigned int k = 0, n = 0, laser = 0;

    switch ( laser )
    {
        case 0:
            //n = digitalRead(4);
            n = gpioReadPin( PHOTO );
            if ( n == 0 )
                k++;
            else
                k = 0;

            if ( k > 6 )
            {
                //digitalWrite(13, HIGH) ;
                //digitalWrite(5,  HIGH ) ;
                gpioWritePin( KICK, HIGH );
                //printf( "\e[1;1H KICKING         " );
                laser++;
                k = 0;
            }
            break;
        case 1:
            k++;
            //printf( "\e[1;1H KICKING %d  ", k );
            if ( k > 3000 )
            {
                //digitalWrite(13, LOW) ;
                //digitalWrite(5,  LOW ) ;
                gpioWritePin( KICK, LOW );
                laser++;
                k = 0;
            }

            break;
        case 2:
            //n = digitalRead(4);
            //printf( "\e[1;1H                  " );
            n = gpioReadPin( PHOTO );
            if ( n == 1 )
                k++;
            else
                k = 0;

            if ( k > 50 )
            {
                //printf( "\e[1;1H                  " );

                laser = 0;
//                k = 0;
            }
            break;
    }/*switch*/

}

void kickerTask2()
{
    static unsigned int k = 0, n = 0, laser = 0;

    switch ( laser )
    {
        case 0:
            //n = digitalRead(4);
            n = gpioReadPin( PHOTO2 );
            if ( n == 0 )
                k++;
            else
                k = 0;

            if ( k > 5 )
            {
                //digitalWrite(13, HIGH) ;
                //digitalWrite(5,  HIGH ) ;
                gpioWritePin( KICK2, HIGH );
                //printf( "\e[1;1H KICKING         " );
                laser++;
                k = 0;
            }
            break;
        case 1:
            k++;
            //printf( "\e[1;1H KICKING %d  ", k );
            if ( k > KICK_DELAY )
            {
                //digitalWrite(13, LOW) ;
                //digitalWrite(5,  LOW ) ;
                gpioWritePin( KICK2, LOW );
                laser++;
                k = 0;
            }

            break;
        case 2:
            //n = digitalRead(4);
            //printf( "\e[1;1H                  " );
            n = gpioReadPin( PHOTO2 );
            if ( n == 1 )
                k++;
            else
                k = 0;

            if ( k > 50 )
            {
                //printf( "\e[1;1H                  " );

                laser = 0;
                k = 0;
            }
            break;
    }/*switch*/

}
