#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <inttypes.h>
#include <string.h>

/****************************************
        Author: Tim Wood
        Author: Phil Lopreiato
        with a little help from
        http://beej.us/guide/bgnet/
****************************************/

#define MAX_PAYLOAD_SIZE 1024

int main(int argc, char ** argv)
{
        char* server_port = "1234";
        char* server_ip = "127.0.0.1";
        char *message = malloc(MAX_PAYLOAD_SIZE);
        int sockfd, rc;
        int i;
        struct addrinfo hints, *server;
        int o;

        /* Command line args:
                -p port
                -h host name or IP
                -m message to send
        */
        while ((o = getopt (argc, argv, "p:h:m:")) != -1) {
                switch(o){
                case 'p':
                        server_port = optarg;
                        break;
                case 'h':
                        server_ip = optarg;
                        break;
                case 'm':
                        message = optarg;
                        break;
                case '?':
                        if(optopt == 'p' || optopt == 'h' ) {
                                fprintf (stderr, "Option %c requires an argument.\n", optopt);
                        }
                        else {
                                fprintf (stderr, "Unknown argument: %c.\n", optopt);
                        }
                        break;
                }
        }

        printf("server_ip: %s   port: %s\n", server_ip, server_port);

        /* Initialize the message with some data
         * and ensure the string is terminated with \0
         */
        for(i=strlen(message); i<MAX_PAYLOAD_SIZE; i++){
                message[i] = 'A';
        }
        message[MAX_PAYLOAD_SIZE - 1] = '\0';

        /* The hints struct is used to specify what kind of server info we are looking for */
        memset(&hints, 0, sizeof hints);
        hints.ai_family = AF_INET;
        hints.ai_socktype = SOCK_STREAM; /* or SOCK_DGRAM */

        /* getaddrinfo() gives us back a server address we can connect to.
           It actually gives us a linked list of addresses, but we'll just use the first.
         */
        if (rc = getaddrinfo(server_ip, server_port, &hints, &server) != 0) {
                perror(gai_strerror(rc));
                exit(-1);
        }

        /* Now we can create the socket and connect */
        sockfd = socket(server->ai_family, server->ai_socktype, server->ai_protocol);
        if (sockfd == -1) {
                perror("ERROR opening socket");
                exit(-1;
        }
        rc = connect(sockfd, server->ai_addr, server->ai_addrlen);
        if (rc == -1) {
                perror("ERROR on connect");
                goto out;
                /* Watch out for velociraptors...
                 * http://xkcd.com/292/
                 */
        }

        /* Loop infinitely and send messages as fast as possible */
        while(1){
                /* Send the message, plus the \0 string ending. Use 0 flags. */
                rc = send(sockfd, message, MAX_PAYLOAD_SIZE, 0);
                if(rc < 0) {
                        perror("ERROR on send");
                        exit(-1);
                }
        }

        out:
        freeaddrinfo(server);
        close(sockfd);
        free(message);

        printf("Done.\n");
        return 0;
}
