#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <time.h>
#include <pthread.h>

#define PORT 8888

void *connection_handler_escucha(void *);

int main (void){
	int sockfd, *new_sock;
	char texto[100];
	struct sockaddr_in server;
	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		perror ("socket");
		exit (1);
	}
	memset(&server, 0, sizeof(server));
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = inet_addr("172.16.0.112"); //Aca pongan la ip que tenga el server
	server.sin_port = htons(PORT);

	if (connect(sockfd, (struct sockaddr *)&server,sizeof(struct sockaddr)) == -1) {
		perror ("connect");
		exit (1);
	}
	pthread_t escucha;
	new_sock = malloc(1);
	*new_sock = sockfd;
	if( pthread_create( &escucha , NULL ,  connection_handler_escucha , (void*) new_sock) < 0)
	{
	    perror("could not create thread");
	    return 1;
	}
	while (1){
		printf ("Cliente: ");
		fgets (texto,100,stdin);
		fflush (stdin);
		
		//printf ("El texto ingresado es: %s\n",texto);
		//printf ("La longitud del texto es: %d\n", strlen(texto));
		if (send(sockfd, texto, 100, 0) == -1)
				perror ("send");
	}
	close (sockfd);
	return 0;
}

void *connection_handler_escucha(void *socket_desc)
{
    //Obtengo el descriptor del socket que pase por parametro
    int sock = *(int*)socket_desc;
    int read_size;
    char client_message[100];

    //Recibir mensaje del cliente
    while( (read_size = recv(sock , client_message , 100 , 0)) > 0 )
    {
        printf ("\n");
    	printf ("Server: %s\n",client_message);
    }
    if(read_size == 0)
    {
        puts("El Server se desconecto");
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

