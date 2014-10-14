/****************************************************************************//**\file
 * Test sample program.!
 *
 * Real-time Embedded Systems ECEN5623, Fall 2013!
 * Group7:  Bhardwaj, Bhatt, Joshi, Lopez, Vyas
 ****************************************************************************/
 /* V4L2 video picture grabber
Copyright (C) 2009 Mauro Carvalho Chehab <mchehab@infradead.org>

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation version 2 of the License.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

*/
/*Work in Progress - May be modified at any time. Go through it, but do not implement any part of the code until I commit it into main code
 *
 * Messy code
 * --Mayank
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/mman.h>
#include <linux/videodev2.h>
#include <libv4l2.h>

#define CLEAR(x) memset(&(x), 0, sizeof(x))

#define NUM_IMAGES 20



struct buffer {
        void *start;
        size_t length;
};

static void xioctl(int fh, int request, void *arg)//
{
        int r;

        do {
                r = v4l2_ioctl(fh, request, arg);
        } while (r == -1 && ((errno == EINTR) || (errno == EAGAIN)));

        if (r == -1) {
                fprintf(stderr, "error %d, %s\n", errno, strerror(errno));
                exit(EXIT_FAILURE);
        }
}

int main(int argc, char **argv)
{

        int inTarget=0;
        unsigned int xCnt=0, xCntPrev=0, xCntPrevPrev=0, xCntSum, xEdgeL, xEdgeR, xCent, xCntMax=0;
        unsigned int xCentFinal, yCentFinal=0;
        unsigned char aveR, aveB, aveG;
        int xLessCnt = 0;
        unsigned int for_y=0;

        //Allocate some buffers -- test code
        unsigned char R[20][76800];
        unsigned char G[20][76800];
        unsigned char B[20][76800];
        unsigned char header[16];
        //unsigned char test_buff[230400];
        struct v4l2_format fmt;
        struct v4l2_buffer buf;
        struct v4l2_requestbuffers req;
        enum v4l2_buf_type type;
        fd_set fds;
        struct timeval tv;
        int r, fd = -1;
        unsigned int i, n_buffers;
        char *dev_name = "/dev/video0";
        char out_name[256];
        FILE *fout;
        struct buffer *buffers;
        int j,k;    //test
        int new_fout;   //test

        //vars for centroid markers
        unsigned char maxR = 255;
        unsigned char maxG = 255;
        unsigned char maxB = 255;
        unsigned char minR = 0;
        unsigned char minG = 0;
        unsigned char minB = 0;

        fd = v4l2_open(dev_name, O_RDWR | O_NONBLOCK, 0);
        if (fd < 0) {
                perror("Cannot open device");
                exit(EXIT_FAILURE);
        }

        CLEAR(fmt);
        fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        fmt.fmt.pix.width = 320;
        fmt.fmt.pix.height = 240;
        fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_RGB24;
        fmt.fmt.pix.field = V4L2_FIELD_INTERLACED;
        xioctl(fd, VIDIOC_S_FMT, &fmt);
        if (fmt.fmt.pix.pixelformat != V4L2_PIX_FMT_RGB24) {
                printf("Libv4l didn't accept RGB24 format. Can't proceed.\n");
                exit(EXIT_FAILURE);
        }
        if ((fmt.fmt.pix.width != 640) || (fmt.fmt.pix.height != 480))
                printf("Warning: driver is sending image at %dx%d\n",
                        fmt.fmt.pix.width, fmt.fmt.pix.height);

        CLEAR(req);
        req.count = 2;
        req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        req.memory = V4L2_MEMORY_MMAP;
        xioctl(fd, VIDIOC_REQBUFS, &req);

        buffers = calloc(req.count, sizeof(*buffers));      //returns a pointer to memory block allocated of buffers[2] -0 and 1 each of size 8 bytes
        printf("Size of buffers = %d\n",sizeof(*buffers));
        for (n_buffers = 0; n_buffers < req.count; ++n_buffers) {
                CLEAR(buf);

                buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
                buf.memory = V4L2_MEMORY_MMAP;
                buf.index = n_buffers;

                xioctl(fd, VIDIOC_QUERYBUF, &buf);

                buffers[n_buffers].length = buf.length;
                buffers[n_buffers].start = v4l2_mmap(NULL, buf.length,
                              PROT_READ | PROT_WRITE, MAP_SHARED,
                              fd, buf.m.offset);
                              printf("addr is %x length is %lu with offset being %x\n",buffers[n_buffers].start,buf.length,buf.m.offset);

                if (MAP_FAILED == buffers[n_buffers].start) {
                        perror("mmap");
                        exit(EXIT_FAILURE);
                }
        }

        printf("n_buffers = %d\n",n_buffers);

        for (i = 0; i < n_buffers; ++i) {
                CLEAR(buf);
                buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
                buf.memory = V4L2_MEMORY_MMAP;
                buf.index = i;
                xioctl(fd, VIDIOC_QBUF, &buf);
        }
        type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

        xioctl(fd, VIDIOC_STREAMON, &type);
        for (i = 0; i < NUM_IMAGES; i++) {
                do {
                        FD_ZERO(&fds);
                        FD_SET(fd, &fds);

                        /* Timeout. */
                        tv.tv_sec = 2;
                        tv.tv_usec = 0;

                        r = select(fd + 1, &fds, NULL, NULL, &tv);
                } while ((r == -1 && (errno = EINTR)));
                if (r == -1) {
                        perror("select");
                        return errno;
                }

                CLEAR(buf);
                buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
                buf.memory = V4L2_MEMORY_MMAP;
                xioctl(fd, VIDIOC_DQBUF, &buf);         //dequeue the filled buffer..returns the data back into the v4l2_buffer struct buf

                sprintf(out_name, "grabber%03d.ppm", i);
                fout = fopen(out_name, "w");
                if (!fout) {
                        perror("Cannot open image");
                        exit(EXIT_FAILURE);
                }
                fprintf(fout, "P6\n%d %d 255\n",
                        fmt.fmt.pix.width, fmt.fmt.pix.height);

                printf("bytes are %d\n",buf.bytesused);
                fwrite(buffers[buf.index].start, buf.bytesused, 1, fout);

                /*Test code begins here*/
                //memcpy(test_buff,buffers[buf.index].start,buf.bytesused);
                //FILE *test_fout = fopen("test.ppm", "w");
                //fprintf(test_fout, "P6\n%d %d 255\n",fmt.fmt.pix.width, fmt.fmt.pix.height);
                //fwrite(test_buff,buf.bytesused,1,test_fout);
                //fclose(test_fout);
                /*Test code ends here*/
                //printf("addr2 is %x and addr2+offset is %x\n",buffers[buf.index].start,buffers[buf.index].start+buf.m.offset);
                //printf("****addr = %x testaddr = %x  test3 = %x finaltest =%x \n",buffers[buf.index].start,(void *)(buffers[buf.index].start), (void *)&(buffers[buf.index].start), &(buffers[buf.index].start));
                for(j=0;j<76800;j++)
                {
                    R[i][j]= *(unsigned char *)(buffers[buf.index].start+(j*3));
                    //printf("mod%d is %x with data %d\n",j,buffers[buf.index].start+j*3, (void *)(buffers[buf.index].start+j*3));
//                  printf("\nR[%d][%d]=",i,3*j,R[i][(3*j)]);
                    G[i][j]= *(unsigned char *)(buffers[buf.index].start+(j*3)+1);
                    B[i][j]= *(unsigned char *)(buffers[buf.index].start+(j*3)+2);
                    //printf("\nR[%d][%d]=%d   G[%d][%d]=%d   B[%d][%d]=%d",i,j,R[i][j],i,j,G[i][j],i,j,B[i][j]);
                }
                printf("bytees used = %d\n",buf.bytesused);
                fclose(fout);

                xioctl(fd, VIDIOC_QBUF, &buf);
                //printf("checkpoint 0\n");
        }


        for(i=0;i<NUM_IMAGES;i++)
        {

            //Code for filtering
            for(j=0; j<76800;j++)
            {
                if((R[i][j]>(25+G[i][j])) && (R[i][j]>(25+B[i][j])))
                {
                    //printf("if last = %d\n",i);
                    R[i][j]=255;
                    G[i][j]=255;
                    B[i][j]=255;
                }
                else
                {
                    //printf("else last = %d\n",i);
                    R[i][j]=0;
                    G[i][j]=0;
                    B[i][j]=0;
                }
                //printf("i = %d\n",i);
            }
        }

        //Centroid test code**
        // Raster through image looking for target centroid

        for(k=0;k<NUM_IMAGES;k++)
        {
            for(i=0; i<240; i++)
            {
                inTarget=0;
                xCnt=0;

                for(j=0; j<320; j++)
                {
                    aveR = (R[k][(i*320)+j] + R[k][(i*320)+j+1] + R[k][(i*320)+j+2] +
                            R[k][(i*320)+j+3] + R[k][(i*320)+j+4] + R[k][(i*320)+j+5]) / 6;
                    aveG = (G[k][(i*320)+j] + G[k][(i*320)+j+1] + G[k][(i*320)+j+2] +
                            G[k][(i*320)+j+3] + G[k][(i*320)+j+4] + G[k][(i*320)+j+5]) / 6;
                    aveB = (B[k][(i*320)+j] + B[k][(i*320)+j+1] + B[k][(i*320)+j+2] +
                            B[k][(i*320)+j+3] + B[k][(i*320)+j+4] + B[k][(i*320)+j+5]) / 6;

                    // entered target
                    if(!inTarget && (aveR > 200) &&(aveG > 200) && (aveB>200) )
                    {
                        inTarget=1;
                        xCnt=0;
                        xEdgeL=(unsigned int)j;
                    }
                    else if(inTarget && (aveR < 200) && (aveG <200) && (aveB <200))
                    {
                        inTarget=0;
                        xEdgeR=(unsigned int)j;
                        xCent = xEdgeL + ((xEdgeR-xEdgeL)/2);
                        //printf("xCnt=%d, xEdgeR=%d, xEdgeL=%d, xCent=%d, yCent=%d\n", xCnt, xEdgeR, xEdgeL, xCent, i);

                        xCntSum = xCntPrevPrev+xCntPrev+xCnt;

                        if(xCntSum > xCntMax)
                        {
                            xCntMax = xCntSum;
                            xCentFinal = xCent;
                            yCentFinal = i;
                            //printf("in here ycent = %d\n",yCentFinal);
                            for_y++;

                        }


                        xCntPrevPrev=xCntPrev;
                        xCntPrev=xCnt;
                        xCnt=0;
                    }
                    else if(inTarget)
                    {
                        xCnt++;
                    }
                }
            }

            printf("xCentFinal = %d, yCentFinal = %d\n", xCentFinal, yCentFinal);
        }

        //yCentFinal /= for_y;

        //printf("xCentFinal = %d, yCentFinal = %d\n", xCentFinal, yCentFinal);



        //printf("Checkpoint 1\n");
        type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        xioctl(fd, VIDIOC_STREAMOFF, &type);
        for (i = 0; i < n_buffers; ++i)
                v4l2_munmap(buffers[i].start, buffers[i].length);
         printf("mod%d is %x with data %d\n",i,buffers[i].start, (void *)(buffers[i].start));
        v4l2_close(fd);

        //header[]="P6\n320 240 255\n";
        sprintf(header,"P6\n320 240 255\n");
        header[15]="\0";


        printf("Checkpoint 2\n");
        for(i=0;i<NUM_IMAGES;i++)
        {
            printf("in here %d\n",i);
                sprintf(out_name, "centroid%03d.ppm", i);

                new_fout = open(out_name, (O_RDWR|O_CREAT), 0644);

                //printf("mmap read buffer is %x\n",buffer_loc);
                if (!new_fout) {
                        perror("Cannot open image");
                        exit(EXIT_FAILURE);
                }
                //fprintf(new_fout, "P6\n%d %d 255\n",
                  //      fmt.fmt.pix.width, fmt.fmt.pix.height);
                  //header[]=
                  write(new_fout, (void *)header, 15);
                //fwrite(buffers[buf.index].start, buf.bytesused, 1, fout);
                for(j=0;j<76800;j++)
                {

                    if((i % 320) == xCentFinal)
                    {
                        write(new_fout, (void *)&minR, 1);
                        write(new_fout, (void *)&maxG, 1);
                        write(new_fout, (void *)&minB, 1);
                    }
                    else if((i/320) == yCentFinal)
                    {
                        write(new_fout, (void *)&minR, 1);
                        write(new_fout, (void *)&maxG, 1);
                        write(new_fout, (void *)&minB, 1);
                    }
                    else
                    {
                        write(new_fout, (void *)&R[i][j], 1);
                        write(new_fout, (void *)&G[i][j], 1);
                        write(new_fout, (void *)&B[i][j], 1);
                    }
                    //R[i][(3*j)]=buffers[buf.index].start+(j*3);
                    //G[i][(3*j)+1]=buffers[buf.index].start+(j*3)+1;
                    //B[i][(3*j)+2]=buffers[buf.index].start+(j*3)+2;
                    //printf("\nR[%d][%d]=%d   G[%d][%d]=%d   B[%d][%d]=%d",i,j,R[i][j],i,j,G[i][j],i,j,B[i][j]);
                }
                //printf("bytees used = %d\n",buf.bytesused);
                close(new_fout);
        }

        return 0;
}

