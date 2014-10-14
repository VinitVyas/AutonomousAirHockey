/****************************************************************************//**\file
 * Test sample program.
 *
 * Real-time Embedded Systems ECEN5623, Fall 2013
 * Group7:  Bhardwaj, Bhatt, Joshi, Lopez, Vyas
 ****************************************************************************/
#include "pong.h"



/****************************************************************************//**
 * Displays program's usage information and exits.
 * @param progName Usually argv[0], the name of the executable.
 ****************************************************************************/
static void usage( const char *progName )
{
    fprintf( stderr, "Usage: %s [-b] [-1|2|3] [-f logfilter]\n\n", progName );
    fprintf( stderr, " -b        Start pong using gpio hardware in the B-side of the table.\n"
                     "           If this option is not present, pong defaults to the A-Side of the table.\n\n" );
    fprintf( stderr, " -f        Specifies a log filter. For example the expression \"y=|time\".\n"
                     "           makes the log show entries containing the patterns 'y=' or 'time'\n\n" );
    fprintf( stderr, " -1|-2|-3  Set the logging level: 1=warn, 2=err 3=none. The default is all. \n\n" );
    fprintf( stderr, " -p        Purge MQ and exits. Useful after a crash where the MQ are left unlinked.\n\n" );

    fprintf( stderr, "\n" );
    exit( -1 );
}

static int k =0;
extern volatile bool keepWorking;

/****************************************************************************//**
 * Performs unit test for feature trajectory
 ****************************************************************************/
int main( int argc, char **argv )
{

    puts( FG15 "Test trajectory calculations" );
    puts( "Group7: Real-Time Embedded Systems, ECEN5623 Fall 2013\n" NOC );

    // 1. Parse command-line options
    int   opt;                          // Used to iterate through letter options on the command line
    bool  usingSideA = true;            // Flag used to determine which side (A or B) we are running pong (adjusted by command line options below)
    bool  mqPurgueAndQuit = false;      // Flag set true is we are just cleaning the MQs and exiting (adjusted by command line options below)
    int result;
    Coords_t coords;

    while ( (opt = getopt( argc, argv, "b123f:p" )) != -1 ) // Tip, a ':' following an option means the option has an argument 'optarg'
    {                                                       // The global variable 'optarg' contains the string value passed as option argument, also you can use atoi(optarg) to convert that value to int
        switch ( opt )
        {
            // Purge MQ and exits
            case 'p':
                mqPurgueAndQuit = true;
                break;

            // Specifies side B of the table
            case 'b':
                usingSideA = false;
                break;

            // Enable custom log filtering
            case 'f':
                logSetFilter( optarg );
                break;

            // Set logging level:
            case '1':
            case '2':
            case '3':
                logSetLevel( opt-0x30 ); //0=info, 1=warn, 2=err 3=none
                break;

            // If we find an option we don't support, display usage and quit
            default:
                usage( argv[0] );

        }/*switch*/

    }/*while*/


    // 2. Configure the gobal configuration structure 'pongInstance'
    //    to be used in either side A or B
    configurePongInstance( usingSideA );


    // 3. Purging the mq? do it and quit...
    if ( mqPurgueAndQuit )
    {
        stopPong(); //clean up the queues
        puts("Purgue done.");
        exit(0);
    }


    // 4. Setup application environment, variable initial state
    if ( !setupPong() )
    {
        loge("Failed to setup program environment.");
        exit(-1);
    }

    //Setting up queues before starting the isolated thread
    // 3. Create queues for centroid coordinates
    keepWorking = true;
    pongInstance.videocapThreadId        = 0;
    pongInstance.redFilterThreadId       = 0;
    pongInstance.centroidThreadId        = 0;
    pongInstance.trajectoryThreadId      = 0;
    pongInstance.greenFilterThreadId     = 0;
    pongInstance.paddleTrackerThreadId   = 0;
    pongInstance.actuatorThreadId        = 0;
    /* struct mq_attr attr;
        attr.mq_maxmsg = VFRAME_MQ_SIZE;
        attr.mq_msgsize = sizeof(int);
        attr.mq_flags = 0;
        attr.mq_maxmsg = XYCOORDS_MQ_SIZE;
        attr.mq_msgsize = sizeof(Coords_t);*/

    /*    if ( (pongInstance.mqXYCoords = mq_open( pongInstance.mqNameXYCoords, O_RDWR | O_CREAT, 0777, &attr )) == (mqd_t)-1 )
        {
            perror( "mqXYCoords - mq_open failed" );
            return false;
        }
        else
            log("Opening mqXYCoords - Successfull!");*/




    // 5. Start a isolated thread to do unit testing
    if ( !startTrajectory() )
    {
        loge("Failed to start Trajectory thread.");
        stopPong(); //stop any partially started threads
        exit(-1);
    }

    // TODO: In here implement logic to feed the input queue
    //       for the pong thread being tested, and read the
    //       respective output queue to see is it is generating
    //       the correct output

    while(keepWorking)
            {
            coords.x = abs( (int)(39 * cos(k) ) );  //generate dummy x,y values
                       coords.y = abs( (int)(39 * sin(k) ) );

                       result = mq_write( pongInstance.mqXYCoords, &coords, sizeof(coords), 100000 );

                       if ( result==ERROR )
                           loge("mqXYCoords mq_write failed - %s", strerror(errno) );
                       else
                           log("mqXYCoords sent x,y=(%d, %d)         ", coords.x, coords.y );
                       usleep(2000000);
                       k++;
            }


    log("Test started. (Type anything and press enter to exit)\n");

    scanf ("%i", &opt); //dummy read


    // 6. Clean up and exit
    log( "Terminating test...");
    stopPong();

    log("bye!");
    return 0;
}
