/****************************************************************************//**\file
 * Contains the test file for video capture test.
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

bool setup()
{

    // Initialize video
    if ( !setupVideo(pongInstance.videoDeviceName, VFRAME_WIDTH, VFRAME_HEIGHT))
    {
        perror( "Failed to setup video" );
        return false;
    }

    // Initialize MQs
    if ( !setupQueues() )
    {
        perror( "Failed to setup queues" );
        return false;
    }

    // No thread should be working right now
    keepWorking = false;

    log("setup completed.");
    return true;

}

bool start()
{

    // If pong already started, leave...
    if ( keepWorking )
    {
        logw("Called start twice. Stop pong first.");
        return false;
    }

    keepWorking = true;

    // Reset all thread identifiers
    pongInstance.videocapThreadId        = 0;

    // Launch all treads
    if ( !startVideocap() )     return false;

    log("start completed.");
    return true;

}

bool stop()
{

    // 1. Tell all thread to exit
    keepWorking = false;

    // 2. Wait for threads to end
    if ( pongInstance.videocapThreadId!= 0 )      pthread_join( pongInstance.videocapThreadId, NULL );
    //if ( pongInstance.redFilterThreadId!= 0 )     pthread_join( pongInstance.redFilterThreadId, NULL );

    // 3. Reset thread IDs
    pongInstance.videocapThreadId        = 0;

    // Release queues, so the don't live on after the application ends
    mq_unlink( pongInstance.mqNameVideoFull );
    mq_unlink( pongInstance.mqNameVideoEmpty );
    mq_unlink( pongInstance.mqNameGreenFull );
    mq_unlink( pongInstance.mqNameGreenEmpty );
    mq_unlink( pongInstance.mqNameRedFull );
    mq_unlink( pongInstance.mqNameRedEmpty );
    mq_unlink( pongInstance.mqNameXYCoords );

    if(closeVideo() == true)
        logw("Video device unopened or closed");

    log("stop completed.");

    return true;
}

/*stuff from main*/

int main( int argc, char **argv )
{
    // 1. Parse command-line options
    int   opt;                          // Used to iterate through letter options on the command line
    bool  usingSideA = true;            // Flag used to determine which side (A or B) we are running pong (adjusted by command line options below)
    bool  mqPurgueAndQuit = false;      // Flag set true is we are just cleaning the MQs and exitingq (adjusted by command line options below)

    puts( FG15 "Pong version 1.0" );
    puts( "Group7: Real-Time Embedded Systems, ECEN5623 Fall 2013\n" NOC );

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

    //Configure the global configuration structure 'pongInstance' to be used in either side A or B
    configurePongInstance( usingSideA );

    // 3. Purging the mq? do it and quit...
    if ( mqPurgueAndQuit )
    {
        stop(); //clean up the queues
        puts("Purge done.");
        exit(0);
    }

    //Setup application environment, variable initial state
    if ( !setup() )
    {
        loge("Failed to setup program environment.");
        exit(-1);
    }

    //Start all threads an let them do the work while we just hang in here until the user enters any input and hit enter
    if ( !start() )
    {
        loge("Failed to start program threads.");
        stop(); //stop any partially started threads
        exit(-1);
    }

    log("Pong is running (Type anything and press enter to exit)\n");
    scanf ("%i", &opt); //dummy read

    //Clean up and exit
    log("Terminating program...");
    stop();

    return 0;
}
