#pragma once

#ifndef PARSER__H__
#define PARSER__H__

#include <stdint.h>

// see parser.c
void printWelcomeMessage();
void printCommandList();
void printExitMessage();
void printStatusMessage(int statusCode);
int32_t getDegrees(char* cmd);
char* createCmdString(int cmd, int engineNum, char* value);
int readCommand(const int fd, char* cmd, char* buffer, int bufferSize);
void loopCmd(char* serialPort);

// see serialport.c
int initSerialPort(const char* serialport);
int closeSerialPort(int fd);
int writeSerialPort(int fd, const char* str);
int readuntilSerialPort(int fd, char* buffer, char until, int bufferMax, int timeout);
int flushSerialPort(int fd);

#endif