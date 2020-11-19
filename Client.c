#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include <arpa/inet.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<errno.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include <time.h>
#include <string.h>
#include <unistd.h>

#define ip "127.0.0.1"
#define portno 6666
#define cliport 12222


void main()
{

	printf("######### CLIENT ########\n");
	int sockfd;


	if( (sockfd = socket(AF_INET,SOCK_DGRAM,0)) == -1)
	{
		perror("socket");
		return;
	}

	struct sockaddr_in cli_server;
	cli_server.sin_family = AF_INET;
	cli_server.sin_port= htons(cliport);
	cli_server.sin_addr.s_addr = inet_addr(ip);
	int len_ser = sizeof(cli_server);
	memset( &(cli_server.sin_zero),'\0',8 );

	if((bind(sockfd,(struct sockaddr *)&cli_server,sizeof(cli_server) ) ) == -1)
	{
		perror("bind");
		exit(-1);
	}

	struct sockaddr_in loc_server;
	loc_server.sin_family = AF_INET;
	loc_server.sin_port= htons(portno);
	loc_server.sin_addr.s_addr = inet_addr(ip);
	 len_ser = sizeof(loc_server);
	memset( &(loc_server.sin_zero),'\0',8 );

	char ch,msg[512];
  char * line = NULL;
  size_t len = 0;
  ssize_t read;

/////////////

		FILE *fp;

			fp = fopen("Fichiers_txt/domain.txt","r");

			if( (ch= fgetc(fp)) == EOF){
				printf("EMPTY FILE\n");
			}
			else
			{
					rewind(fp);
					while(!feof(fp))
				{

				fscanf(fp,"%s",msg);
				if((read = getline(&line, &len, fp)) == -1){
					printf("Terminé \n");
					exit(0);
				}

				printf("taille est %ld \n", sizeof(msg));
				printf("%s \n", msg);
				int sent = sendto(sockfd,msg,20,0,(struct sockaddr *)&loc_server,sizeof(struct sockaddr));
				printf("%d bytes of data sent\n",sent );
				int lb = sizeof(struct sockaddr);

				struct timeval tv;
				tv.tv_sec = 60;
				tv.tv_usec = 0;
				setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof tv);
				int r = recvfrom(sockfd,msg,20,0,(struct sockaddr *)&loc_server,&lb);

				msg[r]='\0';

				char errip[]="1.0.0.0";
				if((strcmp(errip,msg)) == 0)
				{
					printf("No valid Server Found\n");
					exit(-1);
				}
				printf("%s IP ADDRESS\n", msg);
				printf("%d bytes received \n",r );

				int buf[1];
				setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof tv);
				int r1 = recvfrom(sockfd,buf,sizeof(int),0,(struct sockaddr *)&loc_server,&lb);
				printf("Number of query messages from local to all the servers is %d\n",buf[0]);
				printf("%d bytes received\n",r1);

				//vider le tableau
				memset (msg, 0, sizeof (msg));
				printf("------------------------------------ \n");

				sleep(5);

			}}
			fclose(fp);
      if (line)
          free(line);
      exit(EXIT_SUCCESS);
}
