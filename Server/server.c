#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>

void *connection_handler_escucha(void *); // Esta funcion escucha continuamente si recibo nuevos mensajes

int main(int argc , char *argv[])
{
    int socket_desc , new_socket , c , *new_sock;
    int yes=1;
    struct sockaddr_in server , client;
    char texto[100]; //No se pueden mandar mensajes de mas de 99 caracteres
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons( 8888 );
    if ((socket_desc = socket(AF_INET , SOCK_STREAM , 0)) == -1) {
        perror("socket");
        exit (-1);
    }
    if((setsockopt(socket_desc, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)))==-1) //Agregue esto que me olvide, es para salvar errores de bind
    	{
    		  perror("setsockopt:");
    		  exit(-1);
        }
    if( bind(socket_desc,(struct sockaddr *)&server , sizeof(server)) < 0)
    {
        perror("bind");
        exit (-1);
    }
    if (listen(socket_desc , 1) == -1 ) {
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
    pthread_t escucha; //Hilo que va a manejar los mensajes recibidos
    new_sock = malloc(1);
    *new_sock = new_socket; //Puntero necesario para pasar como parametro a pthread_create
    if( pthread_create( &escucha , NULL ,  connection_handler_escucha , (void*) new_sock) < 0)
    {
       perror("could not create thread");
       return 1;
    }
    while (1) { //La idea de esto es que es un server, y no debe parar nunca, para salir del server, Control + C
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
    int sock = *(int*)socket_desc; //acomodo mejor el descriptor del socket para que sea mas comodo usarlo
    int read_size;
    char client_message[100];

    while( (read_size = recv(sock , client_message , 100 , 0)) > 0 )
    {
    	printf ("%s",client_message);
    }
    if(read_size == 0)
    {
        puts("El cliente se desconecto");
        fflush(stdout); //Limpio el buffer de la pantalla
    }
    else if(read_size == -1)
    {
        perror("recv failed");
    }
    free(socket_desc);
    return 0;
}

