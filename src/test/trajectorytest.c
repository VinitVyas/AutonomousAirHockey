/****************************************************************************//**\file
 * Contains the trajectory thread business logic implementation.
 *
 * Real-time Embedded Systems ECEN5623, Fall 2013
 * Group7:  Bhardwaj, Bhatt, Joshi, Lopez, Vyas
 ****************************************************************************/
#include "pong.h"

#define G_HEIGHT 18
#define G_WIDTH  19

typedef struct trajectoryCalc
{
    float m1, c1, m2, c2;
    float intersection_x, intersection_y;
}TrajectoryCalc_t;

/*Diagram Showning puck movement and trajectory calculations
 *
 *       |=========FRAME_WIDTH/2=============|
 *
 *       0,19                                   Fixed Line
 *    - -  ----------------------------------- + fix1 (x,y) 0.0     is the double line here
 *     |   |               \                  ||
 *     |   |                + prev(x,y)       ||
 *   F |   |                 \                ||
 *   R |   |                  \               ||
 *   A |   |                   \              ||
 *   M |   |                    + next(x,y)   ||
 *   E |   |                     \            ||
 *     |   |                      \           ||
 *   H |   |                       \          ||
 *   E |   |                        \         ||
 *   I |   |                         \        ||
 *   G |   |                          \       ||
-*   H--   ----------------------------\-------+ fix2 (x,y) 0,18
 *   T                                  \      |
 *                                       \     |
 *                                        \    |
 *                                         \   |
 *                                          \  |
 *                                           \ |
 *                                            \|
 *                                             + intersection(x,y)
 */

//Routine to calculate the projections of paths and the intersection point of the coordinates

bool intersect(Coords_t p1, Coords_t p2, Coords_t q1,Coords_t q2, TrajectoryCalc_t* trajectoryCalc )
{
    int x1, y1, x2, y2;
    int x3, y3, x4, y4;
    float dx1, dy1, dx2, dy2;

    x1 = p1.x;
    y1 = p1.y;
    x2 = p2.x;
    y2 = p2.y;
    x3 = q1.x;
    y3 = q1.y;
    x4 = q2.x;
    y4 = q2.y;

    log("the coordinates recvd by intersect 1 %d %d %d %d ",x1,y1,x2,y2);
    log("the coordinates recvd by intersect 2 %d %d %d %d ",x3,y3,x4,y4);

    dx1 = x2 - x1;
    dy1 = y2 - y1;
    if (dx1 ==0)   //Check for divide by zero errors
    {
        trajectoryCalc->intersection_x=x1;
        dx2 = x4 - x3;
        dy2 = y4 - y3;
            if (dx2 ==0)
                return (false);
        trajectoryCalc->m2 = dy2 / dx2;
        trajectoryCalc->c2 = y1 - trajectoryCalc->m2 * x1;
        trajectoryCalc->intersection_y = trajectoryCalc->m2* (trajectoryCalc->intersection_x) + trajectoryCalc->c2;
        return (true);
    }
    trajectoryCalc->m1 = dy1 / dx1;
    log("the slope for line 1 is %f", trajectoryCalc->m1);
    // y = mx + c
    // intercept c = y - mx
    trajectoryCalc->c1 = y1 - trajectoryCalc->m1 * x1; // which is same as y2 - slope * x2
    dx2 = x4 - x3;
    dy2 = y4 - y3;
        if(dx2 == 0)
        {
            trajectoryCalc->intersection_x = x3;
            trajectoryCalc->intersection_y = trajectoryCalc->m1*(trajectoryCalc->intersection_x) + trajectoryCalc->c1;
                    return (true);
        }
    trajectoryCalc->m2 = dy2 / dx2;
    // comment :y = mx + c
    // comment :intercept c = y - mx
    trajectoryCalc->c2 = y1 - trajectoryCalc->m2 * x1; // which is same as y2 - slope * x2
    log("the slope for line 2 is %f", trajectoryCalc->m2);
     if( (trajectoryCalc->m1 - trajectoryCalc->m2) == 0)
        return(false);
    else
    {
        trajectoryCalc->intersection_x = ((trajectoryCalc->c2 - trajectoryCalc->c1) / (trajectoryCalc->m1 - trajectoryCalc->m2));
        trajectoryCalc->intersection_y = (trajectoryCalc->m1 * trajectoryCalc->intersection_x + trajectoryCalc->c1);
        return(true);
    }
}



/****************************************************************************//**
 * Trajectory thread main entry point.
 * @param arg Optional thread parameter, this is unused.
 * @return Always null
 ****************************************************************************/
static void *trajectoryThread( void *arg )
{
    volatile PongInstance_t* pongInstance = (PongInstance_t*) arg;

    //    TrajectoryCalc_t trajectoryCalc;
    Coords_t next;
    Coords_t prev;
    Coords_t a1;          /* Fixed Coordinates for the second line*/

    int result;
    int slope;

    log( "Trajectory Thread has been Started %d ", keepWorking );
    while ( keepWorking )
    {
        // 1. Get a new coordinate pair from centroid thread

        result = mq_read( pongInstance->mqXYCoords, &prev, sizeof(prev), 100000 );
            if ( result != OK )
            {
                if ( result == ERROR )
                    log( "mqXYCoords mq_read failed - %s", strerror(errno) );

                 continue;  //Dunnow why it dosent like this continue
                /* Compiler message:src/pong/trajectory.c:139:19: error: continue statement not within a loop
                 make: *** [src/pong/trajectory.o] Error 1   */

            } /*if*/
            log( "[%s] First set of coordinates X,Y=(%d, %d)   ", pongInstance->id?"SideB":"SideA", prev.x, prev.y );

            log( "Trajectory Thread has been Started " );

            // 1. Get a new coordinate pair from centroid thread
            result = mq_read( pongInstance->mqXYCoords, &next, sizeof(next), 100000 );
            if ( result != OK )
            {
                if ( result == ERROR )
                    log( "mqXYCoords mq_read failed - %s", strerror(errno) );

                continue;

            } // if
            log( "Got new coordinates next X,Y=(%d, %d) and prev (%d, %d)   ", next.x, next.y, prev.x, prev.y );

            // 2. Process the coordinates to generate trajectory
            //TODO: apply algorithm to generate coordinate

            if(next.y < prev.y)
            {                       //towards if y is decreasing
                if(next.x == prev.x) //straight
                    pongInstance->predictedPos = prev.x;
            }
            else if(next.y > prev.y)
            {                       //away if y is increasing
                // do nothing
            }
            else
                slope = (next.y - prev.y)/(next.x - prev.x)

    //            log( "The calculated posistion is %d ", pos );

    //        } /*if*/
            //Savin of the "next" coordinates to "prev"
//            pongInstance->predictedPos = prev.x;
            //usleep( 1000000 );

    }/*while*/

    log( FG13 "  *** Trajectory exited..." );
    return NULL ;
}

/****************************************************************************//**
 * Starts the trajectory thread.
 * @return true if the thread started successfully, false otherwise.
 ****************************************************************************/
bool startTrajectory(PongInstance_t* pongInstance)
{
    if( pthread_create( &pongInstance->trajectoryThreadId, NULL, trajectoryThread, pongInstance ) != 0 )
    {
        loge("Failed to create trajectory thread - %s", strerror(errno) );
        return false;
    }
    return true;
}
