/**
 *  A simple socket server using forks
 */

#include<sys/types.h>
#include<sys/socket.h>
#include<sys/wait.h>
#include<netinet/in.h>  //Internet address lib

#include<err.h>
#include<errno.h>
#include<limits.h>
#include<signal.h>
#include<stdio.h>
#include<string.h>
#include<unistd.h>


static void usage() {
    extern char* __progname;
    fprintf(stderr,"usage: %s portnumber\n", __progname);
    exit(1);
}

/** Signal handler for child process. */
static void chldHandler(int signum) {
    waitpid(WAIT_ANY,NULL,WNOHANG);
}

int main(int argc, char *argcv[]) {
    struct sockaddr_in sockname, client;
    char buffer[100], *ep;
    struct sigaction sa;
    int clientlen, sd;
    u_short port;
    pid_t pid;
    u_long p;

    //arguments check
    if (argc != 2) //print usage
        usage();
    errno = 0;

    p = strtoul(argv[1],&ep,10);

}
