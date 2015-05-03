#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>

void *connection_handler(void *);

int main(int argc , char *argv[])
{
    int socket_desc , new_socket , c , *new_sock;
    struct sockaddr_in server , client;
    char *message;

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
    puts("Bind Exitoso");

    if (listen(socket_desc , 3) == -1 ) {
    	perror ("listen");
    	exit (-1);
    }

    puts("Esperando conexiones...");
    c = sizeof(struct sockaddr_in);
    while( (new_socket = accept(socket_desc, (struct sockaddr *)&client, (socklen_t*)&c)) )
    {
        puts("Connection aceptada");

        message = "Hola cliente nuevo! te asigno un handler especial para vos!\n";
        if (send(new_socket, message, strlen(message), 0) == -1) {
        	perror("send:");
        	exit(1);
        }
        pthread_t sniffer_thread;
        new_sock = malloc(1);
        *new_sock = new_socket;

        if( pthread_create( &sniffer_thread , NULL ,  connection_handler , (void*) new_sock) < 0)
        {
            perror("could not create thread");
            return 1;
        }
        pthread_join( sniffer_thread , NULL);
        puts("Handler asignado");
    }

    if (new_socket<0)
    {
        perror("accept");
        exit (-1);
    }
    return 0;
}

/*
 * Handler para cada nueva conexion
 * */
void *connection_handler(void *socket_desc)
{
    //Obtengo el descriptor del socket que pase por parametro
    int sock = *(int*)socket_desc;
    int read_size;
    char *message , client_message[2000];

    //Envio mensaje al cliente
    message = "Bienvenido! yo sere tu handler!\n";
    if (send(sock, message, strlen(message), 0) == -1) {
    	perror ("send");
    	exit (-1);
    }
    message = "Escribe algo por pantalla, yo lo leere y te lo volvere a enviar \n";
    if (send(sock, message, strlen(message), 0) == -1) {
        	perror ("send");
        	exit (-1);
    }
    //Recibir mensaje del cliente
    while( (read_size = recv(sock , client_message , 2000 , 0)) > 0 )
    {
        //Le envio al cliente el mismo mensaje que me envio a mi
        if (send(sock, client_message, strlen(client_message), 0) == -1) {
    	    	perror ("send");
    	    	exit (-1);
    	}
    }

    if(read_size == 0)
    {
        puts("Client disconnected");
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
