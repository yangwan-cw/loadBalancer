#include "signal_handler.h"
#include <stdio.h>

extern volatile int running;

void signal_handler(int sig) {
    running = 0;
    printf("\n[INFO] Shutting down...\n");
}
