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
#include<stdlib.h>
#include<string.h>
#include<unistd.h>


static void usage() {
    extern char* __progname;
    fprintf(stderr,"usage: %s portnumber\n", __progname);
    exit(1);
}

/** Signal handler for child process. */
static void childHandler(int signum) {
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
    if (*argv[1] == '\0' || *ep != '\0') {
        /* parameter is empty or not a number. */
        fprintf(stderr, "%s - value out of range\n", argv[1]);
        usage();
    }
    port = p;
    /* Form the message that needs to be sent to the client */
    strlcpy(buffer,"Give me a number: \n", sizeof(buffer));
    memset(&sockname,0,sizeof(sockname));
    sockname.sin_family = AF_INET;
    /* Convert between host and network byte order. */
    sockname.sin_port = htons(port);
    sockname.sin_addr.s_addr = htonl(INADDR_ANY);
    sd=socket(AF_INET,SOCK_STREAM,0);
    if (sd == -1)
        err(1,"socket failed");

    if (listen(sd,3) == -1)
        err(1,"socket listen failed");

    /* Now ready to listen for connections on sd socket, each call to accept
     * will return a descriptor talking to a connected client
     */

    sa.sa_handler = childHandler;
    sigemptyset(&sa.sa_mask);
    /* Allow system calls to be restarted if interrupted by a SIGCHLD */
    sa.sa_flags = SA_RESTART;
    if (sigaction(SIGCHLD,&sa,NULL) == -1)
        err(1,"Sigaction failed.");

    /* Finally we are ready to accept connections and serve clients. */
    for(;;) {
        int clientsd;
        clientlen = sizeof(&client);
        clientsd = accept(sd, (struct sockaddr *)&client, &clientlen);
        if (clientsd == -1)
            err(1,"Socket accept failed.");
        pid = fork();

        if (pid == -1)
            err(1, "Fork failed.");

        if (pid == 0) {
            ssize_t msg_written, w;
            /* Write the message to the client making sure to handle a short write
             * or being interrupted by a signal
             */

            w =0;
            msg_written = 0;
            while(msg_written < strlen(buffer)) {
                w = write(clientsd, buffer + msg_written,strlen(buffer) - msg_written);
                if (w == -1) {
                    if (errno != EINTR)
                        err(1,"write to client failed.");
                } else {
                    msg_written += w;
                }
            }
            close(clientsd);
            exit(0);
        }
        close(clientsd);

    }

}
