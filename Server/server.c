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
    char *message;
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
    c = sizeof(struct sockaddr_in);
    //while( (new_socket = accept(socket_desc, (struct sockaddr *)&client, (socklen_t*)&c)) )
    //{
    	if ((new_socket = accept(socket_desc, (struct sockaddr *)&client, (socklen_t*)&c)) == -1) {
    		perror ("accept");
    		exit (-1);
    	}
    	pthread_t escucha;
        new_sock = malloc(1);
        *new_sock = new_socket;
        if( pthread_create( &escucha , NULL ,  connection_handler_escucha , (void*) new_sock) < 0)
        {
            perror("could not create thread");
            return 1;
        }
        while (1) {
        	printf ("Server: ");
        	fgets (texto,100,stdin);
        	if (send (new_socket,texto,100,0) == -1) {
        		perror ("send");
        		exit (-1);
        	}
        }
        pthread_join( escucha , NULL);
    //}
    return 0;
}

/*
 * Handler para cada nueva conexion
 * */
void *connection_handler_escucha(void *socket_desc)
{
    //Obtengo el descriptor del socket que pase por parametro
    int sock = *(int*)socket_desc;
    int read_size;
    char client_message[100];

    //Recibir mensaje del cliente
    while( (read_size = recv(sock , client_message , 100 , 0)) > 0 )
    {
        printf ("Cliente: %s\n",client_message);
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
    //Libero el puntero al descriptor del socket
    free(socket_desc);
    return 0;
}

