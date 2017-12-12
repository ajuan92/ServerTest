#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/un.h>
#include <stdio.h>
#include <netinet/in.h>
#include <signal.h>
#include <stdint.h>
#include <pthread.h>

#define MAX_CLIENT 5 /*AQUI LE INDICAS EL LIMITE DE CLIENTES
			SI SE CONECTA MAS DE ESTE NUMERO
			YA NO CREA MAS THREADS*/


void error(char *msg);
void ExitApplication(int32_t Signal);
void *vfnThread(void* sockfd);

volatile uint8_t ExitAppFlag = 0;

int main(int argc, char *argv[]){

int sockfd, newsockfd, portno;
socklen_t clilen;
struct sockaddr_in serv_addr, cli_addr;
pthread_t thread[MAX_CLIENT];
int ThreadIndex = 0;


    (void)signal(SIGINT,ExitApplication);
    printf("Application started and signal callback registered\n\r");
    printf("Press CTRL+C to finish\n\r");

    if (argc < 2){
      fprintf(stderr,"ERROR, no port provided\n");
      exit(1);
    }	

    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    if (sockfd < 0){
      error("ERROR opening socket");
    }

    bzero((char *) &serv_addr, sizeof(serv_addr));

    portno = atoi(argv[1]);

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);

    if (bind(sockfd, (struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0){
       error("ERROR on binding");
    }

    listen(sockfd,MAX_CLIENT);
    clilen = sizeof(cli_addr);
    
   while(1){

        newsockfd = accept(sockfd,(struct sockaddr *) &cli_addr, &clilen);
        if(newsockfd < 0){
           error("ERROR on accept");
        }
	printf("%d\n",ThreadIndex);	

	if(MAX_CLIENT > thread[ThreadIndex]){
	   pthread_create(
	      &thread[ThreadIndex],
	      NULL,
	      vfnThread,
	      &newsockfd);
	   ThreadIndex++;
	}else{
	  printf("Client limit reached");
	  close(newsockfd);
	}
       
        if(ExitAppFlag == 1){
	    printf("Ending Application...\n\r");
	   
	    for(ThreadIndex = 0;ThreadIndex < MAX_CLIENT;ThreadIndex++){
	        pthread_cancel(thread[ThreadIndex]);	   
 	    }
	    pthread_exit(vfnThread); 
	    close(sockfd);
            break;
        }
    }
return 0;
}

void *vfnThread(void*sockfd)
{
    char buffer[256];
    int RWStatus;
    int newsockfd = *(int*)sockfd;

    while(1){
        bzero(buffer,256);
        RWStatus = read(*(int *)newsockfd,buffer,255);
        if(RWStatus < 0){
           error("ERROR reading from socket");
        }else{
        printf("Here is the message: %s\n",buffer);
        RWStatus = write(*(int *)newsockfd,"I got your message",18);
        }
	if(RWStatus < 0){
          error("ERROR writing to socket");
         }
    }
}

void ExitApplication(int32_t Signal){

    ExitAppFlag = 1;

}


void error(char *msg){
    perror(msg);
    exit(1);
}
