#include <stdio.h>
#include "libclogger/logger.h"
#include "libclogger/loggerconf.h"

void myfunc3(void) {
    LOG_ERROR("error myfunc3","");
}

static void myfunc2(void) {
    myfunc3();
}

void myfunc(int ncalls) {
    if (ncalls > 1)
        myfunc(ncalls - 1);
    else
        myfunc2();
}

int main(int argc, char *argv[]) {
    char filename[256];
    if (argc <= 1) {
        logger_initConsoleLogger(stderr);
        logger_setLevel(LogLevel_DEBUG);
    } else {
        strncpy(filename, argv[1], strlen(argv[1]));
        logger_configure(filename);
    }

    LOG_TRACE("trace","");
    LOG_DEBUG("degug","");
    LOG_DEBUG("format example: %d%c%s", 1, '2', "3");
    LOG_INFO("info","");
    LOG_INFO("file logging","");
    LOG_WARN("warn","");
    LOG_ERROR("error","");
    LOG_FATAL("fatal","");

    myfunc(10);

    return 0;
}
