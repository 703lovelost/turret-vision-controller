#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "turrel-interface.h"

#define PROCESSED_COMMAND 100
#define UNSPECIFIED_COMMAND 101
#define UNKNOWN_COMMAND 102

#define LEFT_ROTATION_LIMIT (-360)
#define RIGHT_ROTATION_LIMIT 360

#define ROTATE_CODE 1
#define ACCEL_CODE 2
#define TURNRATE_CODE 3

#define ENGINE_BODY 1
#define ENGINE_CAMERA 2

#define bufferMax 256

union int32 {
    int32_t i;
    char s[sizeof(int32_t)];
};

union float32 {
    float f;
    char s[sizeof(float)];
};

char cmdExit[] = "exit";
char cmdHelp[] = "help";

char cmdRotate[] = "rt";
char cmdAccelerate[] = "acl";
char cmdTurnRate[] = "tr";

char bodyParam[] = "body";
char cameraParam[] = "camera";

void printWelcomeMessage() {
    printf("Controller is executed.\n");
    printf("Please enter the parameters.\n");
    printf("Use 'help' command to watch all commands available.\n");
}

void printCommandList() {
    printf("List of commands:\n");
    printf("\n");

    printf("rt *rotating-element* *rotation-degree*\n");
    printf("\t*rotating-element*:\n");
    printf("\t\tbody\n");
    printf("\t\tcamera\n");
    printf("\t*rotation-degree*:\n");
    printf("\t\tvalue from -360 to 360 (every other input will be turned into the closest limit value)\n");

    printf("\n");

    printf("exit\n");
}

void printExitMessage() {
    printf("Controller is off.");
}

void printStatusMessage(int statusCode) {
    switch (statusCode) {
    case UNSPECIFIED_COMMAND:
        printf("Unspecified command.\n");
        break;
    case UNKNOWN_COMMAND:
        printf("Unknown command.\n");
        break;
    case PROCESSED_COMMAND:
        printf("Command is sent.\n");
        break;
    }
}

int32_t getDegrees(char* cmd) {
    char* degreesStr = strtok(cmd, " ");
    int32_t degreesInt = atoi(degreesStr);

    if (degreesInt < LEFT_ROTATION_LIMIT) {
        degreesInt = LEFT_ROTATION_LIMIT;
    }

    if (degreesInt > RIGHT_ROTATION_LIMIT) {
        degreesInt = RIGHT_ROTATION_LIMIT;
    }

    return degreesInt;
}

char* createCmdString(int cmd, int engineNum, char* value) {
    char string[6];
    int i;

    string[0] = (char)cmd;
    string[1] = (char)engineNum;
    for (i = 2; i < 6; i++) {
        string[i] = value[i - 2];
    }

    return string;
}

int readCommand(const int fd, char* cmd, char* buffer, int bufferSize) {
    char* cmdParsed;
    char eolchar = '\n';
    int timeout = 5000;

    if (strcmp(cmdRotate, cmd) == 0) {
        union int32 degrees;
        cmd = strtok(cmd, " ");

        if (strcmp(bodyParam, cmd) == 0) {
            degrees.i = getDegrees(cmd);
            cmdParsed = createCmdString(ROTATE_CODE, ENGINE_BODY, degrees.s);
            writeSerialPort(fd, cmdParsed);
            readuntilSerialPort(fd, buffer, eolchar, bufferSize, timeout);
            printf("%s\n", buffer);
        }
        else if (strcmp(cameraParam, cmd) == 0) {
            degrees.i = getDegrees(cmd);
            cmdParsed = createCmdString(ROTATE_CODE, ENGINE_CAMERA, degrees.s);
            writeSerialPort(fd, cmdParsed);
            readuntilSerialPort(fd, buffer, eolchar, bufferSize, timeout);
            printf("%s\n", buffer);
        }
        else {
            return 0;
        }
    }
    else {
        return 2;
    }
}

void loopCmd(char* serialPort) {

    char cmd[bufferMax];
    char buffer[bufferMax];

    int cmdStatus;
    char* tokenCmd;
    int fd = -1;
    int rdCode;

    fd = initSerialPort(serialPort);

    if (fd == -1) {
        printf("Error: unknown serial port");
    }
    else {
        while (1) {
            scanf("%s", cmd);
            tokenCmd = strtok(cmd, " ");

            if (strcmp(cmdExit, tokenCmd) == 0) {
                printExitMessage();
                break;
            }
            else if (strcmp(cmdHelp, tokenCmd) == 0) {
                printCommandList();
            }
            else {
                while (tokenCmd != NULL) {
                    rdCode = readCommand(fd, cmd, buffer, bufferMax);
                    switch (rdCode) {
                    case 0:
                        printStatusMessage(UNSPECIFIED_COMMAND);
                        break;
                    case 1:
                        printStatusMessage(PROCESSED_COMMAND);
                        break;
                    case 2:
                        printStatusMessage(UNKNOWN_COMMAND);
                        break;
                    }
                    break;
                }
            }
        }
        closeSerialPort(fd);
    }
}