#pragma once

#ifndef PARSER__H__
#define PARSER__H__

#include <stdint.h>

// see parser.c
typedef uint8_t byte;

union comval {
	int intval;
	byte byteval[4];
	float floatval;
};

typedef struct {
	char** mass;
	int size;
} tokenList;

typedef struct { 
    char* mass; 
    int size;
} byteArray;

void printWelcomeMessage();
void loop(char* serialPort);

// see serialport.c
int serialportInit(const char* serialport);
int serialportClose(int fd);
int serialportWrite(int fd, const char* str, int len);
int serialportReadUntil(int fd, char* buf, char until, int amount, int buf_max, int timeout);
int serialportFlush(int fd);

#endif