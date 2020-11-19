#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include <arpa/inet.h>
#include <unistd.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<netinet/in.h>
#include<errno.h>

struct rootrecord{
	char dextension[20];
	int tldport;
	char ipadd[16];
};

#define ip "127.0.0.1"
#define portno 7777
#define clientportno 6666
#define timeout 30


void main()
{
		printf("######### Root_DNS ########\n");


	struct sockaddr_in rootserver,rootclient;
	int rootfd,r,i,j;
	char buffer[512],ch;
	char copiebuffer[512];
	char copiebuffer2[512];
	int len_buffer = 512;
	int len_rootserver = sizeof(rootserver);
	char extension[10];
	char extensioncopy[10];
	int temp,flag =0;
	int resultport[1];
	int sent;
	char resultip[16];
	struct rootrecord record;

	//Creation de la socket
	if( (rootfd = socket(AF_INET,SOCK_DGRAM,0) ) == -1)
	{
		perror("socket");
		exit(-1);
	}

	//remplir la strucutre sockadd
	rootclient.sin_family = AF_INET;
	rootclient.sin_port = htons(clientportno);
	rootclient.sin_addr.s_addr = inet_addr(ip);
	memset( &(rootclient.sin_zero),'\0',8 );

	rootserver.sin_family = AF_INET;
	rootserver.sin_port = htons(portno);
	rootserver.sin_addr.s_addr = inet_addr(ip);
	memset( &(rootserver.sin_zero),'\0',8 );

	//lie la structure créee
	if( (bind(rootfd, (struct sockaddr *)&rootserver, len_rootserver) ) == -1 )
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
    setsockopt(rootfd, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof tv);

	//reception de la requete de Local_DNS
	memset (buffer, 0, sizeof (buffer));

	r = recvfrom(rootfd,buffer,700,0,(struct sockaddr *)&rootclient, &len_rootserver );
		for(int i = 0; i<512; i++){copiebuffer[i] = buffer[i];copiebuffer2[i] = buffer[i];}

	buffer[r]='\0';
	printf("Hostname from localserver: %s\n",buffer); // tout le header est dans le buffer
	printf("%d bytes received\n",r);

	//recuperer l'extension
	char *strtoken = strtok(copiebuffer, ".");
	strtoken = strtok ( NULL, "." );
	strtoken = strtok ( NULL, "." );

	printf("Extension : %s \n", strtoken);

	for(i=r-1,j=0;i>=0;i--)
	{
		if(buffer[i]== '.')
			break;
		else
		{
			extensioncopy[j]= buffer[i];
			j++;
		}
	}
	for(i=0;i<j;i++)
	{
		extension[i]=extensioncopy[j-1-i];
	}
	extension[j]='\0';


	//Recherche de l'extention dans le fichier rootdnsfile.txt
	FILE *fp;
	fp = fopen("Fichiers_txt/rootdnsfile.txt","r");
	if( (ch = getc(fp)) == EOF )
	{
		printf("File is Empty Add the tld portnumbers into it\n");
	}
	else
	{
		rewind(fp);

		while( !(feof(fp)) )
		{
			fscanf(fp,"%s %s %d",record.dextension,record.ipadd,&record.tldport);
			temp = strcmp(record.dextension, strtoken);
			if(temp ==0 )
			{
				strcpy(resultip, record.ipadd);
				resultport[0] = record.tldport;
				flag = 1;
			}
		}

		//Cas d'erreur: extension non trouvée -> erreur
		if(flag == 0)
		{
			printf("No TLD dns server found for the input...!\n");
			resultport[0]=4567;
			sent = sendto(rootfd,resultport,sizeof(int),0, (struct sockaddr *)&rootclient, sizeof(rootclient));
			printf("%d bytes sent\n", sent);
			char resultip1[]="1.0.0.0";
			sent = sendto(rootfd,resultip1,sizeof(resultip1),0, (struct sockaddr *)&rootclient, sizeof(rootclient));
			printf("%d bytes sent\n", sent);
			fclose(fp);
			exit(-1);
		}
	}
	fclose(fp);

	//envois du port
	sent = sendto(rootfd,resultport,sizeof(int),10, (struct sockaddr *)&rootclient, sizeof(rootclient));
	printf("%d bytes sent\n", sent);

	//envois de l'IP
	sent = sendto(rootfd,resultip,sizeof(resultip),10, (struct sockaddr *)&rootclient, sizeof(rootclient));
	printf("%d bytes sent\n", sent);

	//envois de la reponse  complete sous la forme demandé dans le sujet
	char partie2[100];
	snprintf(partie2,100,"%s %d %s %s %s" , "|", flag, "|", strtoken, resultip);
	char *rep1;
	rep1 = strcat(copiebuffer2, partie2);
	sent = sendto(rootfd,copiebuffer2,sizeof(copiebuffer2),10, (struct sockaddr *)&rootclient, sizeof(rootclient));
	printf("Reponse 1 : \n");
	printf("%s \n", rep1);
	printf("%d bytes sent\n", sent);
					printf("------------------------------------\n");



}	close(rootfd);

						}}
