#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
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
	printf ("Cliente conectado, ya puede empezar a chatear\n");
	pthread_t escucha;
	new_sock = malloc(1);
	*new_sock = sockfd;
	if( pthread_create( &escucha , NULL ,  connection_handler_escucha , (void*) new_sock) < 0)
	{
	    perror("could not create thread");
	    return 1;
	}
	while (1){
		fgets (texto,100,stdin);
		fflush (stdin);
		if (send(sockfd, texto, 100, 0) == -1)
				perror ("send");
	}
	close (sockfd);
	return 0;
}

void *connection_handler_escucha(void *socket_desc)
{
    int sock = *(int*)socket_desc;
    int read_size;
    char client_message[100];

    while( (read_size = recv(sock , client_message , 100 , 0)) > 0 )
    {
        printf ("%s",client_message);
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
    free(socket_desc);
    return 0;
}

