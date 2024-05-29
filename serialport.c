#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <termios.h>
#include <unistd.h>
#include <stdint.h>

#include "turrel-interface.h"

int serialportInit(const char* serialport)
{
    struct termios tty;
    int fd;
    
    fd = open(serialport, O_RDWR | O_NONBLOCK );
    
    if (fd == -1)  {
        perror("serialport_init: Unable to open port ");
        return -1;
    }

    if (tcgetattr(fd, &tty) < 0) {
        perror("serialport_init: Couldn't get term attributes");
        return -1;
    }

    cfsetispeed(&tty, B9600);
    cfsetospeed(&tty, B9600);

    tty.c_cflag &= ~PARENB;
    tty.c_cflag &= ~CSTOPB;
    tty.c_cflag &= ~CSIZE;
    tty.c_cflag |= CS8;
    tty.c_cflag &= ~CRTSCTS; //--std=gnu99

    tty.c_cflag |= CREAD | CLOCAL;
    tty.c_iflag &= ~(IXON | IXOFF | IXANY);

    tty.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
    tty.c_oflag &= ~OPOST;

    tty.c_cc[VMIN]  = 0;
    tty.c_cc[VTIME] = 0;
    
    tcsetattr(fd, TCSANOW, &tty);
    if( tcsetattr(fd, TCSAFLUSH, &tty) < 0) {
        perror("init_serialport: Couldn't set term attributes");
        return -1;
    }

    return fd;
}

int serialportClose(int fd)
{
    return close(fd);
}

int serialportWrite(int fd, const char* str, int len)
{
    int n = write(fd, str, len);
    //printf("Bytes written: %d\n", n);
    if( n!=len ) {
        perror("serialportWrite: couldn't write whole string\n");
        return -1;
    }

    return n;
}

int serialportReadUntil(int fd, char* buf, char until, int amount, int buf_max, int timeout)
{
    char b[1];
    int i=0;
    int count = 0;
    do { 
        int n = read(fd, b, 1);
        //printf("Bytes read: %d\n", n);
        if( n==-1) return -1;
        if( n==0 ) {
            usleep( 1 * 1000 );
            timeout--;
            if( timeout==0 ) return -2;
            continue;
        }

        if (n == 1) {
            count++;
        }

        if (count == amount) {
            return 0;
        }

        buf[i] = b[0]; 
        i++;
    } while( b[0] != until && i < buf_max && timeout>0 );

    buf[i] = 0;
    return 0;
}

int serialportFlush(int fd)
{
    sleep(2);
    return tcflush(fd, TCIOFLUSH);
}