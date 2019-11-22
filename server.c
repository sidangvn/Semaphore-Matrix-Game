// Group members: Davin Hill, Alexander Silva, Si Dang

#include<string.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<netdb.h>
#include<stdbool.h>
#include<sys/ipc.h>
#include<sys/shm.h>
#include<sys/sem.h>
#include<unistd.h>
#include <arpa/inet.h>
#include <time.h>

#define PORTNUM  5182 
#define DEFAULT_PROTOCOL 0  
#define SHMKEY ((key_t) 7890)
#define SEMKEY ((key_t) 400L)
#define NSEMS 1

int semid;
int status;
int semnum;
int semval;
int i;
int j;
int status;
int pid;
int position = 0;
int mainScore = 0;
char players[5][15];

union
{
	int semaphoreValues;
} semctl_arg;

typedef struct
{
	int valueMatrices;
	int resetCount;
	int finalScore;
	int quitChecker;
	int values[4][4];
	char visited[16];
}	shared_mem;
	shared_mem *data;

int sockfd, newsockfd, portno, clilen;
struct sockaddr_in serv_addr, cli_addr;
static struct sembuf Wait = {0, -1, 0};
static struct sembuf Signal = {0, 1, 0};

void setMatrixValues()
{
	int counter = 0;
	while(counter < 16)
    {
        data -> visited[counter] = 'a' + counter;
        counter++;
    }
	for(i = 0; i < 4; i++)
	{
		for(j = 0; j < 4; j++)
		{
			data -> values[i][j] = rand() % 10 + 1;
		}
	}
}

void newGame()
{
	printf("New Game Started \n\n");
	setMatrixValues();
	data -> valueMatrices = 0;
	data -> finalScore = 0;
	mainScore = 0;
}

void scoreSystem(int currentScore)
{
	if(currentScore > data -> finalScore)
	{
		data -> finalScore = currentScore;
	}
}

int playTurnChecking()
{
	int total = 0;
	for(i = 0; i < 4; i++)
	{
		for(j = 0; j < 4; j++)
		{
			total += data -> values[i][j];
		}
	}	
	return total;
}

void scoreDisplay()
{
	for (i = 0; i < 4; i++) 
	{
		for (j = 0; j < 4; j++) 
		{
			printf("%d ", data -> values[i][j]);
		}
		printf("\n");
	}
	printf("\n");
}

void processClient (int sock) 
{
	int status;
	char buffer[16];
 	int result;
	char visitedMatrix[16];
 	char str[INET_ADDRSTRLEN];
	char socketTextCompare[] = "ready to play\n";
	bzero(buffer, 16);
	int response = 0;
	int i;
	status = read(sock,buffer,15);
 
	if (status < 0) 
	{
		perror("ERROR reading from socket");
		exit(1);
	}

	result = strcmp(buffer, socketTextCompare);

	if(result == 0)
	{
    	inet_ntop( AF_INET, &cli_addr, str, INET_ADDRSTRLEN);
    	printf("Computer: %s is ready to play\n", str);
		status = semop(semid, &Wait, 1);
		data -> valueMatrices = data -> valueMatrices + 1;
		status = semop(semid, &Signal, 1);
	}
	else
	{
		inet_ntop(AF_INET, &cli_addr, str, INET_ADDRSTRLEN);
		printf("Computer: %s failed to type \"ready to play\", they are disconnecting\n", str);
	}
	
	while(data -> valueMatrices < 2)
	{
		sleep(1);	 
	}
	
	if(data -> valueMatrices == 4)
	{
		scoreDisplay();
	}
}

void resultDisplay(int winScore,int sock)
{
	char str[INET_ADDRSTRLEN];
  
	if(winScore == data -> finalScore)
	{
    	inet_ntop( AF_INET, &cli_addr, str, INET_ADDRSTRLEN);
		printf("Computer: %s WINS with score: %d\n", str, winScore);
     	write(sock, "You Won \n", 16);
	}
 	else
	{
     	inet_ntop( AF_INET, &cli_addr, str, INET_ADDRSTRLEN);
		printf("Computer: %s LOSES with score: %d\n", str, winScore);
		write(sock, "You Lost \n", 16);
	}
}

void arrayValuesDisplay(char ch)
{
	switch(ch)
	{			
		case 'a': 
			printf("Score: %d\n", data -> values[0][0]);
			mainScore += data -> values[0][0];
			data -> values[0][0] = 0;
			data -> visited[0] = '-';
			break;
		case 'b': 
			printf("Score: %d\n", data -> values[0][1]);
			mainScore += data -> values[0][1];
			data -> values[0][1] = 0;
			data -> visited[1] = '-';
			break;			
		case 'c': 
			printf("Score: %d\n", data -> values[0][2]);
			mainScore += data -> values[0][2];
			data -> values[0][2] = 0;
			data -> visited[2] = '-';
			break;			
		case 'd': 
			printf("Score: %d\n", data -> values[0][3]);
			mainScore += data -> values[0][3];
			data -> values[0][3] = 0;
			data -> visited[3] = '-';
			break;			
		case 'e':
			printf("Score: %d\n", data -> values[1][0]);
			mainScore += data -> values[1][0];
			data -> values[1][0] = 0;
			data -> visited[4] = '-';
			break;			
		case 'f': 
			printf("Score: %d\n", data -> values[1][1]);
			mainScore += data -> values[1][1];
			data -> values[1][1] = 0;
			data -> visited[5] = '-';
			break;			
		case 'g': 
			printf("Score: %d\n", data -> values[1][2]);
			mainScore += data -> values[1][2];
			data -> values[1][2] = 0;
			data -> visited[6] = '-';
			break;
		case 'h': 
			printf("Score: %d\n", data -> values[1][3]);
			mainScore += data -> values[1][3];
			data -> values[1][3] = 0;
			data -> visited[7] = '-';
			break;			
		case 'i': 
			printf("Score: %d\n", data -> values[2][0]);
			mainScore += data -> values[2][0];
			data -> values[2][0] = 0;
			data -> visited[8] = '-';
			break;			
		case 'j':
			printf("Score: %d\n", data -> values[2][1]);
			mainScore += data -> values[2][1];
			data -> values[2][1] = 0;
			data -> visited[9] = '-';
			break;
		case 'k': 
			printf("Score: %d\n", data -> values[2][2]);
			mainScore += data -> values[2][2];
			data -> values[2][2] = 0;
			data -> visited[10] = '-';
			break;
		case 'l': 
			printf("Score: %d\n", data -> values[2][3]);
			mainScore += data -> values[2][3];
			data -> values[2][3] = 0;
			data -> visited[11] = '-';
			break;
		case 'm': 
			printf("Score: %d\n", data -> values[3][0]);
			mainScore += data -> values[3][0];
			data -> values[3][0] = 0;
			data -> visited[12] = '-';
			break;
		case 'n': 
			printf("Score: %d\n", data -> values[3][1]);
			mainScore += data -> values[3][1];
			data -> values[3][1] = 0;
			data -> visited[13] = '-';
			break;
		case 'o': 
			printf("Score: %d\n", data -> values[3][2]);
			mainScore += data -> values[3][2];
			data -> values[3][2] = 0;
			data -> visited[14] = '-';
			break;
		case 'p': 
			printf("Score: %d\n", data -> values[3][3]);
			mainScore += data -> values[3][3];
			data -> values[3][3] = 0;
			data -> visited[15] = '-';
			break;	
	}
	scoreSystem(mainScore);
}

void socketCommunication(int sock)
{
	int newGamePlay = 0;
	char buffer[1];
	int n;
	int countLeft; 
  	char str[INET_ADDRSTRLEN];
	
	while(1)
	{
    	countLeft = playTurnChecking();
    
		if(countLeft == 0)
		{
			resultDisplay(mainScore, sock);
			write(sock, "Play again?(y/n)", 16);

			bzero(buffer, 1);
			read(sock, buffer, 1);
			arrayValuesDisplay(buffer[0]);

			if(strncmp(buffer, "y", 1) == 0)
			{		
				status = semop(semid, &Wait, 1);
				data -> resetCount = data -> resetCount + 1;
				status = semop(semid, &Signal, 1);
	
				while(data -> resetCount % 2  == 1)
				{
					sleep(1);
				}			
				newGamePlay = 1;		
				newGame();				
			}
			else
			{
				printf("Quit game\n");
				break;
			}
		}		
		if(countLeft > 0 || newGamePlay > 0)
		{
			write(sock, data -> visited, 16);
			newGamePlay = 0;
		}

		bzero(buffer, 1);
		read(sock, buffer, 1);
    	inet_ntop( AF_INET, &cli_addr, str, INET_ADDRSTRLEN);
    	printf("Computer: %s selected %c ", str, buffer[0]);
		arrayValuesDisplay(buffer[0]);
	}
}

int main( int argc, char *argv[] ) 
{
	char buffer[256];
	int status;
	int pid;
	char *shmadd;
	int shmid;
	int newPid;
	int semaphoreStatus;
	key_t key = 123;
	shmadd = (char *) 0;
   
	if ((shmid = shmget (SHMKEY, sizeof(int), IPC_CREAT | 0666)) < 0)
	{
		perror ("shmget");
		exit (1);
	}
	
	if ((data = (shared_mem *) shmat (shmid, shmadd, 0)) == (shared_mem *) -1)
	{
		perror ("shmat");
		exit (0);
	}

	data -> finalScore = 0;
	data -> resetCount = 0;
	setMatrixValues();
	
	semid = semget (SEMKEY, NSEMS, IPC_CREAT | 0660);
	if (semid < 0)
	{
		printf( "error in creating semaphore");
		exit (1);
	}

	semnum = 0;
	semctl_arg.semaphoreValues = 1;
	semaphoreStatus = semctl(semid, semnum, SETVAL, semctl_arg);
	if(semaphoreStatus != 0)
	{
		perror("Semaphore Creation wasn't successful");
	}
	
	semval = semctl(semid, semnum, GETVAL, semctl_arg);
	data -> valueMatrices = 0;
	fflush (stdout);
	printf("Setup Sever... \n");
	sockfd = socket(AF_INET, SOCK_STREAM,DEFAULT_PROTOCOL );

	if(sockfd < 0) 
	{
		perror("ERROR opening socket");
		exit(1);
	}

	bzero((char *) &serv_addr, sizeof(serv_addr));
	portno = PORTNUM;
	
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons(portno);
	status =  bind(sockfd, (struct sockaddr *) &serv_addr, sizeof (serv_addr));
	
	if (status < 0) 
	{
		perror("ERROR on binding");
		exit(1);
	}
	listen(sockfd, 5);
	clilen = sizeof(cli_addr);
	
	data -> quitChecker = 0;
	while (data -> quitChecker < 2) 
	{
		newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
   		char str[INET_ADDRSTRLEN];
    	inet_ntop( AF_INET, &cli_addr, str, INET_ADDRSTRLEN );
    	strcpy(players[position], str);
    	printf("Connected from %s\n", players[position]);
    	position++;
		
		if (newsockfd < 0) 
		{
			perror("ERROR on accept");
			exit(1);
		}
		
		pid = fork();
		
		if (pid < 0) 
		{
			perror("ERROR on fork");
			exit(1);
		}
		   
		if (pid == 0) 
		{	
			close(sockfd);
			processClient(newsockfd);
			socketCommunication(newsockfd);
			close(newsockfd);
			break;
		}
		else 
		{
			data -> quitChecker = data -> quitChecker + 1;
			close(newsockfd);
		}			
	}

	if(pid != 0)
	{
		newPid = wait(&semaphoreStatus);
		newPid = wait(&semaphoreStatus);
			
		if ((shmctl (shmid, IPC_RMID, (struct shmid_ds *) 0)) == -1)
		{
			perror ("shmctl");
			exit (-1);
		}
				
		semctl_arg.semaphoreValues = 0;
		semnum = 0;
		semaphoreStatus = semctl(semid, semnum, IPC_RMID, semctl_arg);
		
		if (status < 0)
		{
			printf( "error in releasing semaphore \n");
			exit(1);
		}
		else
		{
			printf( "Semaphore has been released \n");
		}
		exit (0);
	}	
}
