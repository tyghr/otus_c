#define _GNU_SOURCE

#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <netdb.h>
#include <string.h>
#include <sys/sendfile.h>
#include <sys/epoll.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h> 
#include <sys/mman.h>
#include <sys/wait.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define BUFSIZE 1024
#define MAXEPOLLSIZE 1000
#define BACKLOG 200 // how many pending connections queue will hold
#define MAXERRS 16

extern char **environ; /* the environment */

char *fileserver_path = ".";

// error - wrapper for perror used for bad syscalls
void error(char *msg) {
    perror(msg);
    exit(1);
}

// cerror - returns an error message to the client
void cerror(FILE *stream, char *cause, char *http_errno, char *shortmsg, char *longmsg) {
    fprintf(stream, "HTTP/1.1 %s %s\n", http_errno, shortmsg);
    fprintf(stream, "Content-type: text/html\n");
    fprintf(stream, "\n");
    fprintf(stream, "<html><title>Tiny Error</title>");
    fprintf(stream, "<body bgcolor=""ffffff"">\n");
    fprintf(stream, "%s: %s\n", http_errno, shortmsg);
    fprintf(stream, "<p>%s: %s\n", longmsg, cause);
    fprintf(stream, "<hr><em>The Tiny Web server</em>\n");
}

int set_non_blocking(int sockfd) {
	int flags, s;
	flags = fcntl(sockfd, F_GETFL, 0);
	if(flags == -1) {
		perror("fcntl");
		return -1;
	}
	flags |= O_NONBLOCK;
	s = fcntl(sockfd, F_SETFL, flags);
	if(s == -1) {
		perror("fcntl");
		return -1;
	}
	return 0;
}

int handle_request(int childfd) {
    /* variables for connection I/O */
    FILE *stream;          /* stream version of childfd */
    char buf[BUFSIZE];     /* message buffer */
    char method[BUFSIZE];  /* request method */
    char uri[BUFSIZE];     /* request uri */
    char version[BUFSIZE]; /* request method */
    char filename[BUFSIZE];/* path derived from uri */
    char filetype[BUFSIZE];/* path derived from uri */
    char *p;               /* temporary pointer */
    struct stat sbuf;      /* file status */
    int fd;                /* static content filedes */

    /* open the child socket descriptor as a stream */
    if ((stream = fdopen(childfd, "r+")) == NULL)
        error("ERROR on fdopen");

    /* get the HTTP request line */
    fgets(buf, BUFSIZE, stream);
    printf("%s", buf);
    sscanf(buf, "%s %s %s\n", method, uri, version);

    /* epoll tiny only supports the GET method */
    if (strcasecmp(method, "GET")) {
        cerror(stream, method, "501", "Not Implemented", 
            "Epoll Tiny does not implement this method");
        fclose(stream);
        return -1;
    }

    /* read (and ignore) the HTTP headers */
    fgets(buf, BUFSIZE, stream);
    printf("%s", buf);
    while(strcmp(buf, "\r\n")) {
        fgets(buf, BUFSIZE, stream);
        printf("%s", buf);
    }

    /* parse the uri [crufty] */
    /* serve only static content */
    strcpy(filename, ".");
    strcat(filename, uri);
    if (uri[strlen(uri)-1] == '/') 
        strcat(filename, "index.html");

    /* make sure the file exists */
    if (stat(filename, &sbuf) < 0) {
        cerror(stream, filename, "404", "Not found", 
            "Epoll Tiny couldn't find this file");
        fclose(stream);
        return -1;
    }

    /* serve static content */
    if (strstr(filename, ".html"))
        strcpy(filetype, "text/html");
    else if (strstr(filename, ".gif"))
        strcpy(filetype, "image/gif");
    else if (strstr(filename, ".jpg"))
        strcpy(filetype, "image/jpg");
    else 
        strcpy(filetype, "application/octet-stream"); // text/plain

    /* print response header */
    fprintf(stream, "HTTP/1.1 200 OK\n");
    fprintf(stream, "Server: Epoll Tiny Web Server\n");
    fprintf(stream, "Content-length: %d\n", (int)sbuf.st_size);
    fprintf(stream, "Content-type: %s\n", filetype);
    fprintf(stream, "\r\n"); 
    fflush(stream);

    char *full_file_path = strdup(fileserver_path);
    strcat(full_file_path,filename);

    /* Use mmap to return arbitrary-sized response body */
    fd = open(full_file_path, O_RDONLY);
    p = mmap(0, sbuf.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
    fwrite(p, 1, sbuf.st_size, stream);
    munmap(p, sbuf.st_size);

    /* clean up */
    fclose(stream);
}

int main(int argc, char *argv[]) {
	int status;
	int sockfd, new_fd, kdpfd, nfds, n, curfds;
	struct addrinfo hints;
	struct addrinfo *servinfo; 				// will point to the results 
	struct addrinfo *p;
	struct sockaddr_storage client_addr;
	struct epoll_event ev;
	struct epoll_event *events;
	socklen_t addr_size;

    if (argc != 2 && argc != 3) {
        fprintf(stderr, "usage: %s addr [path]\n", argv[0]);
        return EXIT_FAILURE;
	}
    if (argc == 3) {
        fileserver_path = argv[2];
    }

	memset(&hints, 0, sizeof hints);	// make sure the struct is empty
	hints.ai_family = AF_UNSPEC; 			// don't care IPv4 or IPv6
	hints.ai_socktype = SOCK_STREAM; 	// TCP stream sockets
	hints.ai_flags = AI_PASSIVE;			// fill in my IP for me

	if((status = getaddrinfo(NULL, argv[1], &hints, &servinfo)) != 0 ) {
		fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(status));
		return 2;
	}
	// servinfo now points to a linked list of 1 or more struct addrinfos
	for (p = servinfo; p != NULL; p = p->ai_next ) {

		// make a socket:
		if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
			perror("server: socket");
			continue;
		}

		// make the sock non blocking
		set_non_blocking(sockfd);

		// bind it to the port
		if ((bind(sockfd, p->ai_addr, p->ai_addrlen)) == -1) {
			close(sockfd);
			perror("server: bind");
			continue;
		}

		break;
	}

	if(p == NULL) {
		fprintf(stderr, "server: failed to bind\n");
		return 2;
	}

	freeaddrinfo(servinfo); // free the linked-list

	// listen for incoming connection
	if (listen(sockfd, BACKLOG) == -1) {
		perror("listen");
		exit(1);
	}

	printf("server: waiting for connections...\n");

	kdpfd = epoll_create(MAXEPOLLSIZE);
	ev.events = EPOLLIN|EPOLLET;
	ev.data.fd = sockfd;
	if(epoll_ctl(kdpfd, EPOLL_CTL_ADD, sockfd, &ev) < 0) {
		fprintf(stderr, "epoll set insert error.");
		return -1;
	} else {
		printf("success insert listening socket into epoll.\n");
	}
	events = calloc (MAXEPOLLSIZE, sizeof ev);
	curfds = 1;
	while(1) 
	{ //loop for accept incoming connection

		nfds = epoll_wait(kdpfd, events, curfds, -1);
		if(nfds == -1) {
			perror("epoll_wait");
			break;
		}		
		for (n = 0; n < nfds; ++n) {
			if(events[n].data.fd == sockfd){
				addr_size = sizeof client_addr;
				new_fd = accept(events[n].data.fd, (struct sockaddr *)&client_addr, &addr_size);
				if (new_fd == -1) {
					if((errno == EAGAIN) || (errno == EWOULDBLOCK)) {
						break;
					}
					else
					{
						perror("accept");
						break;
					}
				}
				printf("server: connection established...\n");
				set_non_blocking(new_fd);
				ev.events = EPOLLIN|EPOLLET;
				ev.data.fd = new_fd;
				if(epoll_ctl(kdpfd,EPOLL_CTL_ADD, new_fd, &ev)<0) {
					printf("Failed to insert socket into epoll.\n");
				}
				curfds++;
			} else {
                // handle
				if(handle_request(events[n].data.fd) != 0) {
					perror("handle_request");
					break;
				}

				epoll_ctl(kdpfd, EPOLL_CTL_DEL, events[n].data.fd, &ev);
				curfds--;
				close(events[n].data.fd);
			}
		}
	}
	free(events);
	close(sockfd);			
	return 0;
}
