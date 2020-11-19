#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include <arpa/inet.h>
#include <unistd.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<netinet/in.h>
#include<errno.h>

struct tldrecord{
	char domain[100];
	char ipadd[16];
	int authport;
};

#define ip "127.0.0.1"
#define iproot "127.0.0.23"
#define tldport 10000
#define localportno 6666
#define timeout 40

void main()
{

	printf("######### TLD_DNS/COM ########\n");


	int tldfd;
	int i,ctr=0,k;
	struct sockaddr_in tldserver,tldclient;
	int len_tldserver = sizeof(tldserver);
	int r,sent;
	int len_tldclient = sizeof(tldclient);
	int dotposition;
	char buffer[512];
	char copiebuffer[512];
	int len_buffer = 512;
	struct tldrecord record;
	char ch;
	int authdnsportnumber[1];
	int temp,flag=0;
	char authdnsportip[16];

	//Creation de la socket
	if( (tldfd = socket(AF_INET,SOCK_DGRAM,0) ) == -1)
	{
		perror("socket");
		exit(-1);
	}

	//remplir la strucutre sockadd
	tldserver.sin_family = AF_INET;
	tldserver.sin_port = htons(tldport);
	tldserver.sin_addr.s_addr = inet_addr(iproot);
	memset( &(tldserver.sin_zero),'\0',8 );

	tldclient.sin_family = AF_INET;
	tldclient.sin_port = htons(localportno);
	tldclient.sin_addr.s_addr = inet_addr(ip);
	memset( &(tldclient.sin_zero),'\0',8 );

	//lie la structure créee
	if( (bind(tldfd, (struct sockaddr *)&tldserver, len_tldserver) ) == -1 )
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
    setsockopt(tldfd, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof tv);

	//reception de la requete de Local_DNS
	memset (buffer, 0, sizeof (buffer));
	r = recvfrom(tldfd,buffer,len_buffer,0, (struct sockaddr *)&tldclient, &len_tldclient );
	buffer[r]='\0';

	//copie de la requete
	printf("%s\n",buffer );
	for(int i = 0; i<512; i++){copiebuffer[i] = buffer[i];}

	//extraire domaine.extension
	char *domainname = strtok(copiebuffer, ".");
	domainname = strtok ( NULL, "\n" );


	//Recherche du <sous-domaine>.<domaine> dans le fichier tldcom.txt
	FILE *fp ;
	fp = fopen("Fichiers_txt/tldcom.txt","r");
	if( (ch = getc(fp) ) == EOF )
	{
		printf("No Entries in .Com \n");
	}
	else
	{
		rewind(fp);
		while(!feof(fp))
		{
			fscanf(fp,"%s %s %d",record.domain,record.ipadd,&record.authport);

			temp = strcmp(record.domain,domainname);
			if(temp == 0)
			{
				flag = 1;
				authdnsportnumber[0]=record.authport;
				strcpy(authdnsportip,record.ipadd);
				printf("%d authport \n",authdnsportnumber[0] );
				printf("%s authportip \n",authdnsportip);
			}
		}
		//<sous-domaine>.<domaine> non trouvé dans le fichier tldcom.txt -> erreur
		if(flag == 0)
		{
			printf("Authorative entry not found for the domain\n");
			authdnsportnumber[0]=4567;
			sent = sendto(tldfd,authdnsportnumber,sizeof(int),0,(struct sockaddr *)&tldclient,len_tldclient );
		printf("%d bytes sent\n",sent);
		char authdnsportip1[]="1.0.0.0";
		sent = sendto(tldfd,authdnsportip1,sizeof(authdnsportip1),0,(struct sockaddr *)&tldclient,len_tldclient );
		printf("%d bytes sent\n",sent);
		fclose(fp);
			fclose(fp);
			exit(-1);
		}

		//envois du port
		sent = sendto(tldfd,authdnsportnumber,sizeof(int),0,(struct sockaddr *)&tldclient,len_tldclient );
		printf("%d bytes sent\n",sent);

		//envois de l'IP
		sent = sendto(tldfd,authdnsportip,sizeof(authdnsportip),0,(struct sockaddr *)&tldclient,len_tldclient );
		printf("%d bytes sent\n",sent);

		//envois de la reponse  complete sous la forme demandé dans le sujet
		char partie2[100];
		snprintf(partie2,100," %s %s %ls " , domainname, authdnsportip, authdnsportnumber  );
		char *rep2;

		printf("%s \n", copiebuffer);
		printf("%s \n", copiebuffer);

		rep2 = strcat(copiebuffer, partie2);
		sent = sendto(tldfd,copiebuffer,sizeof(buffer),10, (struct sockaddr *)&tldclient,len_tldclient);
		printf("Reponse 2 :\n");
		printf("%s \n", rep2);
						printf("------------------------------------\n");


		fclose(fp);
	}	}
	close(tldfd);


			}

}
