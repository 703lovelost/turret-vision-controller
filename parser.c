#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>

#include "turrel-interface.h"

#define CMD_ROTATE 1
#define CMD_ACCELCHANGE 2
#define CMD_LIMITCHANGE 3

#define ENGINE_BODY 1
#define ENGINE_CAM 2

#define NO_TOKENS 1
#define UNEXPECTED_ARGUMENT 2
#define WRONG_VALUE 3
#define WRONG_COMMAND 4
#define WRONG_ARG_AMOUNT 5

#define HELP 11

#define BUFFER_SIZE 256
#define LEFT_ROTATION_LIMIT (-360)
#define RIGHT_ROTATION_LIMIT 360

char cmdExit[] = "exit";
char cmdHelp[] = "help";

char cmdRotate[] = "rt";
char cmdAccelerate[] = "accel";
char cmdLimitTurnRate[] = "limit";

char paramBody[] = "body";
char paramCam[] = "camera";

byteArray out;
int exitStatus = 0;

int allowInt(char* str) {
	for (int i = 0; i < strlen(str); i++) {
		if ((byte)str[i] < 48 || (byte)str[i] > 57) {
			return 0;
		}
	}

	return 1;
}

int allowFloat(char* str) {
	int cnt = 0;

	for (int i = 0; i < strlen(str); i++) {
		if (((byte)str[i] < 48 || (byte)str[i] > 57) && (byte)str[i] != 46) {
			return 0;
		}

		if ((byte)str[i] == 46) {
			if (cnt) {
				return 0;
			}

			cnt = 1;
		}
	}

	return 1;
}


tokenList splitInput(char* str, char sep) {
	tokenList returnize;
	char** out = NULL;
	int size, outi;
	size = 0;
	outi = 0;

	for (int i = 0; ; i++) {
		if (str[i] == sep || str[i] == '\n' || str[i] == '\0') {
			if (size > 0) {
				out[outi++][size] = '\0';
				size = 0;
			}

			if (str[i] == '\n' || str[i] == '\0') {
				break;
			}
		}
		else {
			if (size == 0) {
				out = (char**)realloc(out, sizeof(char*) * (outi + 1));
				out[outi] = (char*)malloc(sizeof(char) * BUFFER_SIZE);
			}
			out[outi][size++] = str[i];
		}
	}

	returnize.mass = out;
	returnize.size = outi;
	return returnize;
}

void printErrorMessage(int errortype, char* arg1, char* arg2, int arg3, int arg4) {
	switch (errortype) {
		case NO_TOKENS:
			printf("No command provided.\n");
			break;
		case UNEXPECTED_ARGUMENT:
			printf("Unexpected %s argument for command %s\n", arg2, arg1);
			break;
		case WRONG_VALUE:
			printf("Wrong value format for command %s: %s\n", arg1, arg2);
			break;
		case WRONG_COMMAND:
			printf("Wrong command: %s\n", arg1);
			break;
		case WRONG_ARG_AMOUNT:
			printf("Expected %d arguments for command %s, received: %d\n", arg3, arg1, arg4);
			break;
	}
}

void printWelcomeMessage() {
    printf("Controller is executed.\n");
    printf("Please enter the parameters.\n");
    printf("Use 'help' command to watch all commands available.\n");
}

void printExitMessage() {
    printf("Controller is off.\n");
}

void printHelpMessage(int msgtype) {
	switch (msgtype) {
		case HELP:
			printf("List of commands:\n");
			printf("\n");

			printf("rt *rotating-element* *rotation-degree*\n");
			printf("\t*rotating-element*:\n");
			printf("\t\tbody\n");
			printf("\t\tcamera\n");
			printf("\t*rotation-degree*:\n");
			printf("\t\tvalue from -360 to 360 (every other input will be turned into the closest cmdLimitTurnRate value)\n");

			printf("\n");

			printf("accel *element* *acceleration-speed*\n");
			printf("\t*element*:\n");
			printf("\t\tbody\n");
			printf("\t\tcamera\n");
			printf("\t*acceleration-speed*:\n");
			printf("\t\tfloat value (no limits set)\n");

			printf("\n");

			printf("limit *element* *speed-limitation*\n");
			printf("\t*element*:\n");
			printf("\t\tbody\n");
			printf("\t\tcamera\n");
			printf("\t*speed-limitation*:\n");
			printf("\t\tfloat value (no limits set)\n");

			printf("\n");

			printf("exit\n");
			break;
	}
}

void initExit() {
	exitStatus = 1;
}

int checkDegrees(int degreesInt) {
	if (degreesInt < LEFT_ROTATION_LIMIT) {
        degreesInt = LEFT_ROTATION_LIMIT;
    }

    if (degreesInt > RIGHT_ROTATION_LIMIT) {
        degreesInt = RIGHT_ROTATION_LIMIT;
    }

	return degreesInt;
}

byteArray* parser(tokenList tokens) {
	int intValue;
	float floatValue;

	if (tokens.size == 0) {
		printErrorMessage(NO_TOKENS, NULL, NULL, 0, 0); 
		return NULL; 
	}

	if (strcmp(tokens.mass[0], cmdHelp) == 0) { 
		printHelpMessage(HELP); 
		return NULL; 
	}

	if (strcmp(tokens.mass[0], cmdExit) == 0) { 
		initExit(); 
		return NULL; 
	}

	out.mass = (byte*)malloc(1);
	out.mass[0] = 0;

	if (strcmp(tokens.mass[0], cmdRotate) == 0) {
		if (tokens.size != 3) { 
			printErrorMessage(WRONG_ARG_AMOUNT, tokens.mass[0], NULL, 2, tokens.size - 1); 
			return NULL; 
		}

		out.mass = (byte*)realloc(out.mass, 6);
		out.mass[0] = CMD_ROTATE;
		out.mass[1] = 0;

		if (strcmp(tokens.mass[1], paramBody) == 0) { 
			out.mass[1] = ENGINE_BODY; 
		}

		if (strcmp(tokens.mass[1], paramCam) == 0) { 
			out.mass[1] = ENGINE_CAM; 
		}

		if (out.mass[1] == 0) { 
			printErrorMessage(UNEXPECTED_ARGUMENT, tokens.mass[0], tokens.mass[1], 0, 0); 
			return NULL; 
		}

		if (!allowInt(tokens.mass[2])) { 
			printErrorMessage(WRONG_VALUE, tokens.mass[0], tokens.mass[2], 0, 0); 
			return NULL; 
		}

		intValue = atoi(tokens.mass[2]);
		intValue = checkDegrees(intValue);

		for (int i = 0; i < 4; i++) {
			out.mass[i + 2] = ((byte*)(&intValue))[i];
		}

		out.size = 6;
		return &out;
	}

	if (strcmp(tokens.mass[0], cmdAccelerate) == 0) {
		if (tokens.size != 3) { 
			printErrorMessage(WRONG_ARG_AMOUNT, tokens.mass[0], NULL, 2, tokens.size - 1); 
			return NULL; 
		}

		out.mass[0] = CMD_ACCELCHANGE;
		out.mass[1] = 0;

		if (strcmp(tokens.mass[1], paramBody) == 0) { 
			out.mass[1] = ENGINE_BODY; 
		}

		if (strcmp(tokens.mass[1], paramCam) == 0) { 
			out.mass[1] = ENGINE_CAM; 
		}

		if (out.mass[1] == 0) { 
			printErrorMessage(UNEXPECTED_ARGUMENT, tokens.mass[0], tokens.mass[1], 0, 0); 
			return NULL; 
		}

		if (!allowFloat(tokens.mass[2])) { 
			printErrorMessage(WRONG_VALUE, tokens.mass[0], tokens.mass[2], 0, 0); 
			return NULL; 
		}

		floatValue = atof(tokens.mass[2]);
		intValue = atoi(tokens.mass[2]);

		for (int i = 0; i < 4; i++) {
			out.mass[i + 2] = ((byte*)(&floatValue))[i];
		}

		out.size = 6;
		return &out;
	}

	if (strcmp(tokens.mass[0], cmdLimitTurnRate) == 0) {
		if (tokens.size != 3) { 
			printErrorMessage(WRONG_ARG_AMOUNT, tokens.mass[0], NULL, 2, tokens.size - 1); 
			return NULL; 
		}

		out.mass[0] = CMD_LIMITCHANGE;
		out.mass[1] = 0;

		if (strcmp(tokens.mass[1], paramBody) == 0) { 
			out.mass[1] = ENGINE_BODY; 
		}

		if (strcmp(tokens.mass[1], paramCam) == 0) { 
			out.mass[1] = ENGINE_CAM; 
		}

		if (out.mass[1] == 0) { 
			printErrorMessage(UNEXPECTED_ARGUMENT, tokens.mass[0], tokens.mass[1], 0, 0); 
			return NULL; 
		}

		if (!allowFloat(tokens.mass[2])) { 
			printErrorMessage(WRONG_VALUE, tokens.mass[0], tokens.mass[2], 0, 0); 
			return NULL; 
		}

		floatValue = atof(tokens.mass[2]);
		intValue = atoi(tokens.mass[2]);

		for (int i = 0; i < 4; i++) {
			out.mass[i + 2] = ((byte*)(&floatValue))[i];
		}

		out.size = 6;
		return &out;
	}

	printErrorMessage(WRONG_COMMAND, tokens.mass[0], NULL, 0, 0);
	return NULL;
}

void loop(char* serialPort) {
	char str[BUFFER_SIZE];
	char output[BUFFER_SIZE];
	tokenList tokens;
	byteArray* command;
	char eolchar = '\n';
	int bytesWritten;
	int readStatus = -3;

	int fd = serialportInit(serialPort);

	while (1) {
		printf(">");
		memset(output, '0', BUFFER_SIZE);
		fgets(str, BUFFER_SIZE, stdin);
		command = parser(splitInput(str, ' '));

		if (command != NULL) {
			for (int i = 0; i < command->size; i++) {
				printf("%u ", command->mass[i]);
			}
			printf("\n");

			bytesWritten = serialportWrite(fd, command->mass, command->size);
	
			while (readStatus != 0) {
				readStatus = serialportReadUntil(fd, output, eolchar, 1, BUFFER_SIZE, 5000);
			}

			free(out.mass);
			out.size = 0;
			serialportFlush(fd);
		}

		if (exitStatus) {
			serialportClose(fd);
			printExitMessage();
			break;
		}
	}
}