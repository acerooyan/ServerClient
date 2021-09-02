#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <netdb.h>
#include <limits.h>
#include <pthread.h>

// Yang Lu 58652582
// Hongji Yan 17575885

int a = 0;
struct cilent_info
{
    pthread_t thread_id;
    FILE *fname;
    char filename[80];
    int opened;
    int appended;
};
struct cilent_info cilent_info_Array[4] = {0};


char buf[1024];

struct stock_sturct
{
    char date[80];
    float close;
};
struct stock_sturct StockArray1[1000];
struct stock_sturct StockArray2[1000];

int open_listenfd(char *port)
{
    struct addrinfo hints, *listp, *p;
    int listenfd, rc, optval=1;

    /* Get a list of potential server addresses */
    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_socktype = SOCK_STREAM;             /* Accept connections */
    hints.ai_flags = AI_PASSIVE | AI_ADDRCONFIG; /* ... on any IP address */
    hints.ai_flags |= AI_NUMERICSERV;            /* ... using port number */
    if ((rc = getaddrinfo(NULL, port, &hints, &listp)) != 0) {
        fprintf(stderr, "getaddrinfo failed (port %s): %s\n", port, gai_strerror(rc));
        return -2;
    }

    /* Walk the list for one that we can bind to */
    for (p = listp; p; p = p->ai_next) {
        /* Create a socket descriptor */
        if ((listenfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) < 0)
            continue;  /* Socket failed, try the next */

        /* Eliminates "Address already in use" error from bind */
        setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR,    //line:netp:csapp:setsockopt
                   (const void *)&optval , sizeof(int));

        /* Bind the descriptor to the address */
        if (bind(listenfd, p->ai_addr, p->ai_addrlen) == 0)
            break; /* Success */
        if (close(listenfd) < 0) { /* Bind failed, try the next */

            return -1;
        }
    }


    /* Clean up */
    freeaddrinfo(listp);
    if (!p) /* No address worked */
        return -1;

    /* Make it a listening socket ready to accept connection requests */
    if (listen(listenfd, 1024) < 0) {
        close(listenfd);
        return -1;
    }
    return listenfd;
}



int free_space(){
    int j;

    for(j = 0; j < 4; j++){
        if (cilent_info_Array[j].thread_id == (pthread_t)0){
            return j;
        }
    }
    printf("no more than 4 cilents\n");
    return -1;

}


int find_sameid( pthread_t id_){

    int j;

    for(j = 0; j < 4; j++){
        if (cilent_info_Array[j].thread_id == id_){
            return j;
        }
    }

    return -1;
}

int find_file(const char filenames[]){

    int j;

    for(j = 0; j < 4; j++){
        if (strcmp(cilent_info_Array[j].filename, filenames) == 0){
            return j;
        }
    }

    return -1;
}



char* open_read(char filenames[])
{
    char * words;
    words= (char *)calloc(200, sizeof(char ));

    pthread_t pid;
    pid = pthread_self();
    int array_index = find_sameid(pid);
    if (cilent_info_Array[array_index].opened == 1 && cilent_info_Array[array_index].appended == 0)
    {
        words = "A file is already open for reading.";
        printf("A file is already open for reading.\n");
        return words;
    }

    else if (cilent_info_Array[array_index].opened == 1 && cilent_info_Array[array_index].appended == 1)
    {
        words = "A file is already open for appending.";
        printf("A file is already open for appending.\n");
        return words;
    }

    int array_index2 = find_file(filenames);
    if (array_index2 != -1 && cilent_info_Array[array_index2].appended == 1)
    {
        words = "This file is already open for appending.";
        printf("This file is already open for appending.\n");
        return words;
    }


    FILE *fp;
    fp = fopen(filenames , "r");
    if(fp == NULL) {
        words = "Error opening file";
        return words;
    }

    int index = free_space();
    if (index != -1)
    {
        cilent_info_Array[index].opened =1;
        cilent_info_Array[index].thread_id = pthread_self();
        strcpy(cilent_info_Array[index].filename,filenames);
        cilent_info_Array[index].fname = fp;
    }
    words = "file opened";
    return words;
}

char* file_close(char filenames[])
{
    char * words;
    words= (char *)calloc(200, sizeof(char ));
    pthread_t pid;
    pid = pthread_self();
    int i;
    i = find_sameid(pid);

    if (i != -1)
    {
        if ( strcmp(cilent_info_Array[i].filename, filenames) == 0){
            fclose(cilent_info_Array[i].fname);

            cilent_info_Array[i].opened = 0;
            strcpy(cilent_info_Array[i].filename, "NULL");
            cilent_info_Array[i].fname = NULL;
            cilent_info_Array[i].thread_id = (pthread_t)0;
            cilent_info_Array[i].appended = 0;
        }
        else{
            words = "file name not found!";
            return words;
        }
    }
    words = "NULL!!??";
    return words;
}


char* file_read(int bytes ){


    char * words;
    words= (char *)calloc(bytes, sizeof(char ));




    int i;
    i = find_sameid(pthread_self());
    if (i != -1)
    {
        if( fgets (words, bytes +1,cilent_info_Array[i].fname )!=NULL ) {

        }
        else
        {
            words = "NULL!!??";
        }

        return words;
    }
    words = "File not open.";
    printf("File not open.\n");
    return words;
}

char* open_append(char filenames[])
{
    char * words;
    words= (char *)calloc(100, sizeof(char ));

    pthread_t pid;
    pid = pthread_self();
    int array_index = find_sameid(pid);
    if (cilent_info_Array[array_index].opened == 1 && cilent_info_Array[array_index].appended == 0)
    {
        words = "A file is already open for reading.";
        printf("A file is already open for reading.\n");
        return words;
    }

    else if (cilent_info_Array[array_index].opened == 1 && cilent_info_Array[array_index].appended == 1)
    {
        words = "A file is already open for appending.";
        printf("A file is already open for appending.\n");
        return words;
    }

    int openornot = find_file(filenames);  //since another client is appending to this file
    if (openornot != -1)
    {
        words = "A file is open by another client.";
        printf("A file is open by another client.\n");
        return words;
    }


    FILE *fp;
    fp = fopen(filenames , "a");
    if(fp == NULL) {
        words = "Error opening file";
        return words;
    }

    int index = free_space();
    if (index != -1)
    {
        cilent_info_Array[index].opened = 1;
        cilent_info_Array[index].thread_id = pthread_self();
        strcpy(cilent_info_Array[index].filename,filenames);
        cilent_info_Array[index].fname = fp;
        cilent_info_Array[index].appended = 1;
    }
    words = "NULL!!??";
    return words;
}

char* file_append(char appendd[] ){
    char * words;
    words= (char *)calloc(200, sizeof(char ));
    int  k = strlen(appendd);
    if (sizeof(appendd) > 200){

        words = "max bytes read at once = 200";
        printf("max bytes read at once = 200\n");
        return words;
    }



    int i;
    i = find_sameid(pthread_self());
    if (i != -1)
    {
        fprintf (cilent_info_Array[i].fname, "%s",appendd);
        words = "NULL!!??";

        return words;
    }
    words = "File not open.";
    printf("File not open.\n");
    return words;
}

void echo(int connfd) {
    char *result;
    size_t n;

    memset(buf, '\0', sizeof(buf));
    while (1)
    {
        if (read(connfd, buf, 256) != 0)
        {

            char buf_[1024];
            strcpy(buf_, buf);

            char *gettokens[80] = {0};
            char* words = strtok(buf_, " ");
            int j = 0;// count words
            while (words != NULL)
            {
                gettokens[j] = words;
                words = strtok(NULL, " ");
                j++;
            }
            gettokens[j-1][strlen(gettokens[j-1])-1] = '\0';

            printf("%d\n",  pthread_self());
            printf("%s", buf);
            if (strcmp(gettokens[0] , "openRead") == 0 && j == 2)
            {
                result = open_read(gettokens[1]);

            }

            else if (strcmp(gettokens[0] , "close") == 0 && j == 2 )
            {
                result = file_close(gettokens[1]);
            }

            else if (strcmp(gettokens[0] , "read") == 0 && j == 2 )
            {
                result = file_read(atoi(gettokens[1]));
            }

            else if (strcmp(gettokens[0] , "openAppend") == 0 && j == 2 )
            {
                result = open_append(gettokens[1]);
            }

            else if (strcmp(gettokens[0] , "append") == 0 && j == 2 )
            {
                result = file_append(gettokens[1]);
            }

            else
            {
                result = "NULL!!??";
            }



            char s[1024] = {0};
            int k;
            for (k=0; k<strlen(result); k++)
            {
                s[k] = result[k];
            }


            send(connfd , s , strlen(s) , 0 );
            memset(buf, '\0', sizeof(buf));


        }
        else
        {
            printf("Client stop");
            break;
        }

    }
}


void *thread(void *vargp){



    int connfd = *((int *)vargp);
    pthread_detach(pthread_self());
    free(vargp);
    echo(connfd);
    close(connfd);
    return NULL;
}

int main(int argc, char **argv)
{


    int j;

    for(j = 0; j < 4; j++){
        cilent_info_Array[j].thread_id = (pthread_t)0;
        cilent_info_Array[j].opened = 0;
        strcpy(cilent_info_Array[j].filename, "NULL");
        cilent_info_Array[j].fname = NULL;
        cilent_info_Array[j].appended = 0;


    }


    int listenfd, *connfd;
    socklen_t clientlen;
    struct sockaddr_storage clientaddr; /* Enough room for any addr */
    char client_hostname[1024], client_port[1024];
    pthread_t tid;

    argv[1] = "39000";

    printf("server started\n");


    listenfd = open_listenfd(argv[1]);

    while (1) {
        clientlen = sizeof(struct sockaddr_storage); /* Important! */
        connfd = malloc(sizeof(int ));
        *connfd = accept(listenfd, (struct sockaddr *) &clientaddr, &clientlen);
        getnameinfo((struct sockaddr *) &clientaddr, clientlen, client_hostname, 1024, client_port, 1024, 0);
        pthread_create(&tid, NULL, thread, connfd);

    }
}