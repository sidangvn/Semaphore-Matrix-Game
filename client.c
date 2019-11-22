// Group members: Davin Hill, Alexander Silva, Si Dang

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<netdb.h>

#define PORTNUM  5182 
#define DEFAULT_PROTOCOL 0  

void socketCommunication(int sock)
{
	char buffer[1];
	char matrixVisited[16];
	int count;
	int i;
	int ready = 0;

	while(1)
	{
		bzero(matrixVisited, 16);
		read(sock, matrixVisited, sizeof(matrixVisited));
		
		if((strncmp("a", matrixVisited, 1) == 0) || (strncmp("-", matrixVisited, 1) == 0))
		{	
			for (i = 0; i < 16; i ++)
			{
				printf("%c ", matrixVisited[i]);
				if(i % 4 == 3)
				{
					printf("\n");
				} 
				else
				{
					printf(" ");
				}
			}
			printf("\n");

			if(strncmp("z", buffer, 1) == 0)
			{
				printf("Exit game");
				break;
			}
		}
		else
		{
			printf("%s: ", matrixVisited);
			printf("\n");
            bzero(matrixVisited, 16);
            read(sock, matrixVisited, sizeof(matrixVisited));
			printf("%s ", matrixVisited);

			bzero(buffer, 1);
			count = 0;

			while((buffer[count++] = getchar()) != '\n') ;
			char  ch[1] = {buffer[0]};
			write(sock, ch, sizeof(buffer));
			
			if(strncmp("n", buffer, 1) == 0)
			{
				printf("Game Exit\n");
				break;
			}
			else{
				printf("Wait for other player\n");
				ready = 1;
			}
		}

		if (ready == 0 || ready == 10)
		{
			bzero(buffer, 1);
			count = 0;
			while((buffer[count++] = getchar()) != '\n');
			char  ch[1] = {buffer[0]};
			write(sock, ch, sizeof(buffer));
		}
		else
		{
			ready = 0; 
		}
	}
}

void main()
{
	int  portCompare;
	int  socketid;      
	int  status;        
	char buffer[16]; 
	struct sockaddr_in serv_addr;
   	char socketTextCompare[] = "ready to play\n";
   	int result = 0;

	struct hostent *server;

	socketid = socket (AF_INET, SOCK_STREAM, DEFAULT_PROTOCOL);
	if (socketid < 0) 
		{
			printf( "error in creating client socket\n"); 
			exit (-1);
		}
		printf("created client socket successfully\n");
		server = gethostbyname("osnode12");

	if (server == NULL)
	{
		printf(" error trying to identify the machine where the server is running\n");
		exit(0);
	}

	portCompare = PORTNUM;

	bzero((char *) &serv_addr, sizeof(serv_addr));

	serv_addr.sin_family = AF_INET;
	bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length); 
	serv_addr.sin_port = htons(portCompare);

	status = connect(socketid, (struct sockaddr *) &serv_addr, sizeof(serv_addr));
	if (status < 0)
	{
		printf( " error in connecting client socket with server	\n");
		exit(-1);
	}

	printf("connected client socket to the server socket \n");

	printf("Type ready to play to start the game\n");
	bzero(buffer,16);
	fgets(buffer,16,stdin);
	status = write(socketid, buffer, 15);

	if (status < 0)
	{
		printf("error while sending client message to server\n");
	}
  	result = strcmp(buffer, socketTextCompare);
  	if(result != 0)
  	{
    	printf("You didn't type the right message, disconnecting\n");
    	close(socketid);
  	}

	if(status > 0)
	{
		printf("Confirmation needed from two players \n");
		socketCommunication(socketid);
		close(socketid);
	}
} 


