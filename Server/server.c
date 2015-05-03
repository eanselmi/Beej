#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>

void *connection_handler_escucha(void *);

int main(int argc , char *argv[])
{
    int socket_desc , new_socket , c , *new_sock;
    struct sockaddr_in server , client;
    char texto[100];
    socket_desc = socket(AF_INET , SOCK_STREAM , 0);
    if (socket_desc == -1)
    {
        perror("socket");
    }
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons( 8888 );
    if( bind(socket_desc,(struct sockaddr *)&server , sizeof(server)) < 0)
    {
        perror("bind");
        exit (-1);
    }
    if (listen(socket_desc , 3) == -1 ) {
    	perror ("listen");
    	exit (-1);
    }
    printf ("Server online\n");
    c = sizeof(struct sockaddr_in);
    if ((new_socket = accept(socket_desc, (struct sockaddr *)&client, (socklen_t*)&c)) == -1) {
    	perror ("accept");
    	exit (-1);
    }
    printf ("Cliente conectado, ya pueden iniciar el chat\n");
    pthread_t escucha;
    new_sock = malloc(1);
    *new_sock = new_socket;
    if( pthread_create( &escucha , NULL ,  connection_handler_escucha , (void*) new_sock) < 0)
    {
       perror("could not create thread");
       return 1;
    }
    while (1) {
       	fgets (texto,100,stdin);
       	if (send (new_socket,texto,100,0) == -1) {
    		perror ("send");
       		exit (-1);
       	}
    }
    pthread_join( escucha , NULL);
    return 0;
}
void *connection_handler_escucha(void *socket_desc)
{
    int sock = *(int*)socket_desc;
    int read_size;
    char client_message[100];

    while( (read_size = recv(sock , client_message , 100 , 0)) > 0 )
    {
    	printf ("\n");
    	printf ("%s\n",client_message);
    }
    if(read_size == 0)
    {
        puts("El cliente se desconecto");
        fflush(stdout);
    }
    else if(read_size == -1)
    {
        perror("recv failed");
    }
    free(socket_desc);
    return 0;
}

