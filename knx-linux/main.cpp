#include "../examples/knx-logik/Logikmodul.h"
#include "knx/bau57B0.h"
#include "knx_facade.h"
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

KnxFacade<LinuxPlatform, Bau57B0> knx;

int main(int argc, char **argv)
{
    knx.platform().cmdLineArgs(argc, argv);

    appSetup();
    
    while (1)
    {
        knx.loop();
        if(knx.configured())
            appLoop();
        delay(10);
    }
}