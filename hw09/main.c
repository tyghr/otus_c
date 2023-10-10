#include <stdio.h>
#include "libclogger/logger.h"
#include "libclogger/loggerconf.h"

//  статическая библиотека
//  интерфейс инициализации (для указания логфайла)
//  интерфейс журналирования
//      выводящую в лог-файл заданные сообщения с заданным уровнем важности (один из debug, info, warning, error) и с местом в коде, в котором была вызвана функция журналирования
// !     для уровня error необходимо также выводить текущий стек вызовов.
//          https://man7.org/linux/man-pages/man3/backtrace.3.html
//      Создано тестовое консольное приложение, демонстрирующее работу библиотеки.
//      Бонусные баллы за пригодность для использования при аварийных ситуациях (как-то malloc, вернувший NULL).
//      Бонусные баллы за потокобезопасность.


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
