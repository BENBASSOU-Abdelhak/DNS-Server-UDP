#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include <arpa/inet.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<netinet/in.h>
#include<errno.h>

struct alphabetarecord{
	char name[200];
	char value[16];
};

#define ip "127.0.0.1"
#define authip "127.0.0.41"
#define authportno 21000
#define localportno 6666
#define timeout 50

void main()
{

	printf("######### AUTHORATIVE_DNS/COM ########\n");

	int authfd;
	int sent,r;
	char buffer[512],ch;
	int len_buffer = 512;
	int temp,flag=0;
	struct alphabetarecord record;
	char ipbuffer[16];

	//Creation de la socket
	if( (authfd = socket(PF_INET,SOCK_DGRAM,0) ) == -1)
	{
		perror("socket");
		exit(-1);
	}

	//remplir la strucutre sockadd
	struct sockaddr_in authserver,locserver;
	authserver.sin_family = AF_INET;
	authserver.sin_port = htons(authportno);
	authserver.sin_addr.s_addr = inet_addr(authip);
	memset( &(authserver.sin_zero),'\0',8 );
	int len_authserver = sizeof(authserver);

	locserver.sin_family = AF_INET;
	locserver.sin_port = htons(localportno);
	locserver.sin_addr.s_addr = inet_addr(ip);
	memset( &(locserver.sin_zero),'\0',8 );
	int len_locserver = sizeof(locserver);

	//lie la structure créee
	if( (bind(authfd, (struct sockaddr *)&authserver, len_authserver) ) == -1 )
	{
		perror("bind");
		exit(-1);
	}


		FILE *domain;
		domain = fopen("Fichiers_txt/domain.txt","r");

		if( (ch= fgetc(domain)) == EOF){
		printf("Nothing Present in Cache\n");}
			else{
			rewind(domain);
			while(!feof(domain)){

	//Initialisation du timeout
	struct timeval tv;
    tv.tv_sec = timeout;
    tv.tv_usec = 0;
    setsockopt(authfd, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof tv);

	//reception de la requete de Local_DNS
	memset (buffer, 0, sizeof (buffer));
	r = recvfrom(authfd,buffer,len_buffer,0, (struct sockaddr *)&locserver,&len_locserver );
	buffer[r]='\0';

	//affichage des infos
	printf("%s \n",buffer);
	printf("%d bytes received\n", r);

//buffer -> strtoken

char *domainname = strtok(buffer, "|");
domainname = strtok ( NULL, "|" );
domainname = strtok ( NULL, "|" );

printf("domain name: %s \n", domainname);

	//Recherche du nom de domaine dans le fichier authdns.txt
	FILE *fp;
	fp = fopen("Fichiers_txt/authdns.txt","r");
	if((ch = getc(fp)) == EOF)
	{
		printf("No Entry in the Authorative ALPHABETA server\n");
	}
	else
	{
		rewind(fp);
		while(!feof(fp))
		{

			fscanf(fp,"%s %s",record.name,record.value);
			temp = strcmp(record.name,domainname);
			if(temp == 0)
			{
				strcpy(ipbuffer,record.value);
				puts(record.value);
				flag =1;
				// printf("%d flagged value\n", flag);
			}
		}

	//Cas d'erreur: nom de domaine non trouvé -> erreur
	if(flag == 0)
	{
		printf("No Entry Found for the input\n");
		char ipbuffer1[]="1.0.0.0";
		r = sendto (authfd,ipbuffer1,strlen(ipbuffer1),0,(struct sockaddr *)&locserver, len_locserver );
		printf("%d bytes sent\n",r);
		fclose(fp);
		exit(-1);
	}

	//envois de l'IP du nom de domaine
	int ll = strlen(ipbuffer);
	r = sendto (authfd,ipbuffer,ll,0,(struct sockaddr *)&locserver, len_locserver );
	printf("%d bytes sent\n",r);
					printf("------------------------------------\n");

	}
	fclose(fp);

			}
}
}
