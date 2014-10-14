#include"pong.h"

/*********************************************************

 LIMIT SWITCHER, WHEN DEPRESSED, RETURNS LOW  (OR ZERO)
 WHEN PRESSED,   RETURNS HIGH (OR ONE)

 ***********************************************************/

/*int main( int argc, char *argv[] )
{
    int i;
    int pos;

    configurePongInstances();

    keepWorking = true;
    startActuator( &pongInstances[0] );

    union sigval sval;

    for (i = 0; i < 100; i++ )
    {
        usleep( 100000 );

        pos = ceil( 20 + 10 * sin(i*0.1));
        loge("*** sending pos=%d ****", pos);

        // 3. Send predicted puck position value to actuator thread
        sval.sival_int = 0x01000000 | pos;     // The upper MSB byte=01h, tells the actuator thread what type of value
                                               // is sent in the signal, 01h is reserved for "current paddle position"
        log( "pos is %d", pos );

        if ( pthread_sigqueue( pongInstances[0].actuatorThreadId, pongInstances[0].actuatorSignal, sval ) == 0 )
        {
            log( "sent signal out current pos=%d      ", pos );
            //This should happen!
        }
        else
        {
            loge( "pthread_sigqueue failed /*, strerror(errno)*//*" );
        }

    }

    keepWorking = false;
    usleep( 1000000 );
    return 0;
}
*/
