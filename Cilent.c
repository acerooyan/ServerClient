#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <netdb.h>

// Yang Lu 58652582
// Hongji Yan 17575885

int open_clientfd(char *hostname, char *port) {
    int clientfd, rc;
    struct addrinfo hints, *listp, *p;

    /* Get a list of potential server addresses */
    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_socktype = SOCK_STREAM;  /* Open a connection */
    hints.ai_flags = AI_NUMERICSERV;  /* ... using a numeric port arg. */
    hints.ai_flags |= AI_ADDRCONFIG;  /* Recommended for connections */
    if ((rc = getaddrinfo(hostname, port, &hints, &listp)) != 0) {
        fprintf(stderr, "getaddrinfo failed (%s:%s): %s\n", hostname, port, gai_strerror(rc));
        return -2;
    }

    /* Walk the list for one that we can successfully connect to */
    for (p = listp; p; p = p->ai_next) {
        /* Create a socket descriptor */
        if ((clientfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) < 0)
            continue; /* Socket failed, try the next */

        /* Connect to the server */
        if (connect(clientfd, p->ai_addr, p->ai_addrlen) != -1)
            break; /* Success */
        if (close(clientfd) < 0) { /* Connect failed, try another */  //line:netp:openclientfd:closefd
            return -1;
        }
    }

    /* Clean up */
    freeaddrinfo(listp);
    if (!p) /* All connects failed */
        return -1;
    else    /* The last connect succeeded */
        return clientfd;
}//


int main(int argc, char **argv)
{
    int clientfd;
    char *host, *port, buf[1024];

    //host = argv[1];
    //port = argv[2];

    host = "localhost";
    port = "39000";

    clientfd = open_clientfd(host, port);

    printf("> ");
    memset(buf, 0, sizeof(buf));
    while (fgets(buf, 256, stdin) != NULL)
    {

        if (strncmp(buf, "quit", 4) == 0)
        {
            break;
        }

        else{
            write(clientfd, buf, strlen(buf));
            memset(buf, 0, sizeof(buf));
            read(clientfd, buf, 256);
            if (strcmp(buf, "NULL!!??") != 0)
            {
                fputs(buf, stdout);
                printf("\n");
            }
            printf("> ");
        }
    }
    close(clientfd);
    exit(0);
}


// openRead test.txt
// read 100