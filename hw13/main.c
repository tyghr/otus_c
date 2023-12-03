#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <termios.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdbool.h>

// #include <sys/types.h>
// #include <sys/socket.h>
// #include <errno.h>


#define HOST "telehack.com"
#define PORT "1337"

#define DO 0xfd
#define WONT 0xfc
#define WILL 0xfb
#define DONT 0xfe
#define CMD 0xff
#define CMD_ECHO 1
#define CMD_WINDOW_SIZE 31
 
void negotiate(int sock, unsigned char *buf, int len) {
    int i;
     
    if (buf[1] == DO && buf[2] == CMD_WINDOW_SIZE) {
        unsigned char tmp1[10] = {255, 251, 31};
        if (send(sock, tmp1, 3 , 0) < 0)
            exit(1);
         
        unsigned char tmp2[10] = {255, 250, 31, 0, 80, 0, 24, 255, 240};
        if (send(sock, tmp2, 9, 0) < 0)
            exit(1);
        return;
    }
     
    for (i = 0; i < len; i++) {
        if (buf[i] == DO)
            buf[i] = WONT;
        else if (buf[i] == WILL)
            buf[i] = DO;
    }
 
    if (send(sock, buf, len , 0) < 0)
        exit(1);
}
 
static struct termios tin;
 
static void terminal_set(void) {
    // save terminal configuration
    tcgetattr(STDIN_FILENO, &tin);
     
    static struct termios tlocal;
    memcpy(&tlocal, &tin, sizeof(tin));
    cfmakeraw(&tlocal);
    tcsetattr(STDIN_FILENO,TCSANOW,&tlocal);
}
 
static void terminal_reset(void) {
    // restore terminal upon exit
    tcsetattr(STDIN_FILENO,TCSANOW,&tin);
}
 
int read_all(int sock_fd, bool print_out);

#define BUFLEN 20
int main(int argc , char *argv[]) {
    if (argc != 2 && argc != 3) {
        printf("Usage: %s message [font]\n", argv[0]);
        return EXIT_FAILURE;
    }
    char *font;
    if (argc == 3) {
        font = argv[2];
    }

    //Create socket
    int sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (sock_fd == -1) {
        perror("Could not create socket. Error");
        return EXIT_FAILURE;
    }
 
    struct addrinfo hints, *res, *result;
    int errcode;
    char addrstr[100];
    void *ptr;
    memset (&hints, 0, sizeof (hints));
    hints.ai_family = PF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags |= AI_CANONNAME;
    errcode = getaddrinfo (HOST, PORT, &hints, &result);
    if (errcode != 0) {
        perror ("getaddrinfo");
        return EXIT_FAILURE;
    }
    res = result;
    if (!res) {
        perror ("getaddrinfo");
        return EXIT_FAILURE;
    }
    inet_ntop (res->ai_family, res->ai_addr->sa_data, addrstr, 100);
    if (res->ai_family != AF_INET) {
        perror ("getaddrinfo");
        return EXIT_FAILURE;
    }
    ptr = &((struct sockaddr_in *) res->ai_addr)->sin_addr;
    inet_ntop (res->ai_family, ptr, addrstr, 100);
    printf ("address: %s (%s)\n", addrstr, res->ai_canonname);

    if(connect(sock_fd, res->ai_addr, res->ai_addrlen) < 0){
        perror("connect");
        close(sock_fd);
        return EXIT_FAILURE;
    }
    freeaddrinfo(result);
    result = NULL;
    puts("Connected...\n");
 
    // set terminal
    terminal_set();
    atexit(terminal_reset);
     
    char msg[100];
    if (font != NULL) {
        sprintf(msg, "figlet /%s %s\r\n", font, argv[1]);
    } else {
        sprintf(msg, "figlet %s\r\n", argv[1]);
    }

    errcode = read_all(sock_fd, false);
    if (errcode != 0) {
        perror("read");
        close(sock_fd);
        return EXIT_FAILURE;
    }

    if (send(sock_fd, msg, sizeof(msg), 0) < 0)
        return EXIT_FAILURE;

    errcode = read_all(sock_fd, true);
    if (errcode != 0) {
        perror("read");
        close(sock_fd);
        return EXIT_FAILURE;
    }

    close(sock_fd);
    return 0;
}

int read_all(int sock_fd, bool print_out) {
    unsigned char buf[BUFLEN + 1];

    struct timeval ts;
    ts.tv_sec = 1; // 1 second
    ts.tv_usec = 0;

    int len;
    int timeout = 4;

    while (timeout > 0) {
        // select setup
        fd_set fds;
        FD_ZERO(&fds);
        // if (sock_fd != 0)
        FD_SET(sock_fd, &fds);
        // FD_SET(0, &fds);
 
        // wait for data
        int nready = select(sock_fd + 1, &fds, (fd_set *) 0, (fd_set *) 0, &ts);
        if (nready < 0) {
            perror("select. Error");
            return 1;
        }
        else if (nready == 0) {
            ts.tv_sec = 1; // 1 second
            ts.tv_usec = 0;

            --timeout;
        }
        else if (sock_fd != 0 && FD_ISSET(sock_fd, &fds)) {
            // start by reading a single byte
            int rv;
            if ((rv = recv(sock_fd , buf , 1 , 0)) < 0)
                return 1;
            else if (rv == 0) {
                printf("Connection closed by the remote end\n\r");
                return 0;
            }
 
            if (buf[0] == CMD) {
                // read 2 more bytes
                len = recv(sock_fd , buf + 1 , 2 , 0);
                if (len  < 0)
                    return 1;
                else if (len == 0) {
                    printf("Connection closed by the remote end\n\r");
                    return 0;
                }
                negotiate(sock_fd, buf, 3);
            }
            else {
                len = 1;
                buf[len] = '\0';
                if (print_out)
                    printf("%s", buf);
                fflush(0);
            }
        }
    }

    return 0;
}
