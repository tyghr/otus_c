#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <syslog.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/resource.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <confuse.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <stdbool.h>

#define SOCKET_NAME "/tmp/echo.socket"

static char *global_file_name = NULL;
bool daemonized = false;

void daemonize(const char *cmd);
int parse_conf(const char *config_file);
size_t get_file_size(void);
int server_start(void);
void local_log(int level, const char* fmt, ...);

int main(int argc, char *argv[]) {
    parse_conf("appd.conf");

    for (int i=1; i < argc; i++) {
        if (strcmp(argv[i],"-d") == 0 || strcmp(argv[i],"--daemon") == 0) {
            daemonized = true;
        }
    }
    if (daemonized)
        daemonize("test_uds_daemon");

    local_log(LOG_DEBUG, "global_file_name: %s", global_file_name);

    int err = server_start();
    local_log(err == 0 ? LOG_INFO : LOG_WARNING,
        "server exited with code %d", err);

	free(global_file_name);
    closelog();
}

void local_log(int level, const char* fmt, ...) {
    va_list argptr;
    va_start(argptr,fmt);
    if (daemonized) {
        vsyslog(level, fmt, argptr);
    } else {
        vfprintf(stdout, fmt, argptr);
        fprintf(stdout, "\n");
    }
    va_end(argptr);
}

void daemonize(const char *cmd) {
    int fd0, fd1, fd2;
    pid_t pid;
    struct rlimit rl;
    struct sigaction sa;

    // Инициализировать файл журнала.
    openlog(cmd, LOG_CONS, LOG_DAEMON);

    // Сбросить маску режима создания файла.
    umask(0);

    // Получить максимально возможный номер дескриптора файла.
    if (getrlimit(RLIMIT_NOFILE, &rl) < 0) {
        local_log(LOG_ERR,"невозможно получить максимальный номер дескриптора");
        return;
    }

    // Стать лидером нового сеанса, чтобы утратить управляющий терминал.
    if ((pid = fork()) < 0) {
        local_log(LOG_ERR,"ошибка вызова функции fork");
        return;
    }
    else if (pid != 0) { // родительский процесс
        exit(0);
    }
    setsid();

    // Обеспечить невозможность обретения управляющего терминала в будущем.
    sa.sa_handler = SIG_IGN;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    if (sigaction(SIGHUP, &sa, NULL) < 0)
        local_log(LOG_CRIT, "невозможно игнорировать сигнал SIGHUP");

    if ((pid = fork()) < 0)
        local_log(LOG_CRIT, "ошибка вызова функции fork");
    else if (pid != 0) // родительский процесс
        exit(0);

    // Назначить корневой каталог текущим рабочим каталогом,
    // чтобы впоследствии можно было отмонтировать файловую систему.
    if (chdir("/") < 0)
        local_log(LOG_CRIT, "невозможно сделать текущим рабочим каталогом /");

    // Закрыть все открытые файловые дескрипторы.
    if (rl.rlim_max == RLIM_INFINITY)
        rl.rlim_max = 1024;
    for (rlim_t i = 0; i < rl.rlim_max; i++)
        close(i);

    // Присоединить файловые дескрипторы 0, 1 и 2 к /dev/null.
    fd0 = open("/dev/null", O_RDWR);
    fd1 = dup(0);
    fd2 = dup(0);
    if (fd0 != 0 || fd1 != 1 || fd2 != 2)
        local_log(LOG_CRIT, "ошибочные файловые дескрипторы %d %d %d", fd0, fd1, fd2);
}

int parse_conf(const char *config_file) {
	cfg_opt_t opts[] = {
		CFG_SIMPLE_STR("file_name", &global_file_name),
		CFG_END()
	};
	cfg_t *cfg;

    // default
	global_file_name = strdup("default_file.tmp");

	cfg = cfg_init(opts, 0);
    cfg_parse(cfg, config_file);

	cfg_free(cfg);

	return 0;
}

size_t get_file_size(void) {
    struct stat st;
    int err = stat(global_file_name, &st);
    if (err != 0) {
        return 0;
    }
    size_t size = st.st_size;
    return size;
}

int server_start(void) {
    int sock, msgsock, rval;
    struct sockaddr_un server;
    char buf_read[1024];
    size_t sz;

    sock = socket(AF_UNIX, SOCK_STREAM, 0);
    if (sock < 0) {
        local_log(LOG_ERR, "opening stream socket");
        return 1;
    }
    server.sun_family = AF_UNIX;
    strcpy(server.sun_path, SOCKET_NAME);
    if (bind(sock, (struct sockaddr *) &server, sizeof(struct sockaddr_un))) {
        local_log(LOG_ERR, "binding stream socket");
        return 1;
    }
    local_log(LOG_INFO, "Socket has name %s", server.sun_path);

    int err;
    if ((err = listen(sock, 5)) != 0) {
        local_log(LOG_ERR, "listener start");
        return 1;
    }

    for (;;) {
        msgsock = accept(sock, 0, 0);
        if (msgsock == -1) {
            local_log(LOG_ERR, "accept");
            break;
        }

        local_log(LOG_DEBUG, "Accept connection");

        bzero(buf_read, sizeof(buf_read));
        if ((rval = read(msgsock, buf_read, 1024)) < 0) {
            local_log(LOG_ERR, "reading stream message");
        } else if (rval == 0) {
            local_log(LOG_INFO, "Ending connection");
        } else {
            char buf_send[1024];
            sz = get_file_size();
            sprintf(buf_send, "file_name: %s size: %ld", global_file_name, sz);
            send(msgsock, buf_send, strlen(buf_send), 0);
        }

        close(msgsock);
    }
    close(sock);
    unlink(SOCKET_NAME);

    return 0;
}
