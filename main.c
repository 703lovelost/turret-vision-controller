#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <string.h>

#include "turrel-interface.h"

int main(int argc, char** argv) {
    if (argc > 2) {
        printf("'control-turret *port*' was just enough.\n");
        printf("anyway\n");
        printf("\n");
    }

    char* serialPort = argv[1];

    printWelcomeMessage();

    loop(serialPort);

    return 0;
}