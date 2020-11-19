#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<errno.h>
#include<arpa/inet.h>
#include <time.h>
#include <string.h>

#define ip "127.0.0.1"
#define portno 6666
#define rootportno 7777
#define timeout 20

int qmsg[1];

struct dnsrecord
{
	char name[100];
	char value[16];
};


char ipaddress[16];
int i,flag=0;
int r,sent;
void main()
{
	printf("######### LOCAL_DNS ########\n");

	qmsg[0]=0;
//==============================================
	int newfd;
	struct sockaddr_in server,client;
	int ser_len = sizeof(server);
	char buffer[512];
	char copiebuffer2[512];
	char aenvoye[512];
	int len_buffer = 512;
	struct dnsrecord cache;
	char ch;
	int temp;
char a1[100];

//==============================================
	struct sockaddr_in rootserver,rootclient;
	int rootfd;
	int extension[1];
	int rootserver_length = sizeof(rootserver);

//==============================================================
int tldfd;
struct sockaddr_in tldserver;
int tldserver_length = sizeof(tldserver);
int authportnumber[1];

//==========================================================
	//remplir la strucutre sockadd
	server.sin_family = AF_INET;
	server.sin_port = htons(portno);
	server.sin_addr.s_addr = inet_addr(ip);
	memset( &(server.sin_zero),'\0',8);

	struct sockaddr_in clientserver;
	clientserver.sin_family = AF_INET;
	clientserver.sin_port = htons(12222);
	clientserver.sin_addr.s_addr = inet_addr(ip);
	memset( &(clientserver.sin_zero),'\0',8 );

		FILE *domain;
		domain = fopen("Fichiers_txt/domain.txt","r");

		if( (ch= fgetc(domain)) == EOF){
		printf("Nothing Present in Cache\n");}
			else{
			rewind(domain);
			while(!feof(domain)){

	//Creation de la socket pour recevoir le message du Client
	if( (newfd = socket(AF_INET,SOCK_DGRAM,0) ) == -1 )
	{
		perror("socket");
		exit(-1);
	}

	//liaison de la socket
	if( (bind(newfd,(struct sockaddr *)&server,sizeof(server) ) ) ==-1 )
	{
		perror("bind");
		exit(-1);
	}

	//initialisation du buffer qui va recevoir le nom de domaine du client
	for(i=0;i<512;i++)
	{
		buffer[i]='\0';
	}
	//Initialisation du timeout
	struct timeval tv;
    tv.tv_sec = timeout;
    tv.tv_usec = 0;
    setsockopt(newfd, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof tv);

	//Reception du nom de domaine du Client
	r = recvfrom(newfd,buffer,len_buffer,0,(struct sockaddr *)&client,&ser_len  );
	//buffer[r]='\0';

	//Copie du Buffer Recu pour une quelconque utilisation
  	for(int i = 0; i<512; i++){copiebuffer2[i] = buffer[i];}
	printf("Buffer Message is : %s",buffer);
	printf("\n%d Bytes received \n",r);

	//Création de la requete, sous la forme <identifiant de transaction>|<horodatage>|<nom>, comme demandé
	char rq1[700];
	int id = rand()%10;
	int horodatage = rand()%1000;
	snprintf(rq1,700,"%d %s %d %s %s" ,id, "|", horodatage, "|", buffer);
	printf("Requete 1 : %s \n", rq1);

	// extraire l'extension du nom de domaine: com/in
  		char *com1 = strtok(buffer, ".");
        com1 = strtok ( NULL, "." );
        com1 = strtok ( NULL, "." );


	/*Vérification dans le cache d'abord, avant de l'envoyer aux autres serveurs.
	Si le domaine y est, on revoit l'IP directement au Client, sinon, on procède
	à la résolution du nom de domaine
	/!\ Pensez à vider le cache manuellemebnt
	*/

	FILE *fp;
	fp = fopen("Fichiers_txt/localcache.txt","r");
	if( (ch= fgetc(fp)) == EOF){
		printf("Nothing Present in Cache\n");
	}

	{
		rewind(fp);
		while(!feof(fp))
			{
				//cache.name = nom de domaine dans le cache
				//cache.value = IP du nom de domaine dans le cache

				fscanf(fp,"%s %s",cache.name,cache.value);
				temp = strcmp(cache.name,copiebuffer2);

				//Si le nom de domaine n'existe pas dans le cache
				if(temp == 0)
				{
					strcpy(ipaddress,cache.value);
					flag = 1;
				}
			}

			//le nom de domaine existe déjà dans le cache ! Renvois direct au Client
			if(flag == 1)
			{

				int lr = 16;
				printf("IP ADDRESS is :%s\n",ipaddress);
				int r1 = sendto(newfd,ipaddress,lr,0,(struct sockaddr *)&clientserver,ser_len );
				printf("%d bytes sent\n",r1);
				qmsg[0]=0;
				r1 = sendto(newfd,qmsg,sizeof(int),0,(struct sockaddr *)&clientserver,ser_len );
				printf("%d bytes sent\n",r1);
				flag = 0;

				close(newfd);

			} else{

			//================================Contacting Root Server ==============================
			//Creation de la socket pour envoyer la requete au Root
			if( (rootfd = socket(AF_INET,SOCK_DGRAM,0) ) == -1 )
				{
					perror("socket");
					exit(-1);
				}

				//remplir la strucutre sockadd
				rootserver.sin_family= AF_INET;
				rootserver.sin_port= htons(rootportno);
				rootserver.sin_addr.s_addr = inet_addr(ip);
				memset( &(rootserver.sin_zero),'\0',8);

				//envois de la requete au root
				if( (sent = sendto(rootfd,rq1,600,100, (struct sockaddr *)&rootserver, sizeof(rootserver) ) ) == -1)
				{
					perror("send");
					exit(-1);
				}
				printf("%d bytes sent\n",sent );

			//Initialisation du timeout
        	struct timeval tv3;
          	tv3.tv_sec = timeout+5;
         	 tv3.tv_usec = 0;
          	setsockopt(rootfd, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv3, sizeof tv3);

				//reponse du root (port)
				if( (r = recvfrom(rootfd,extension,sizeof(int),0, (struct sockaddr *)&rootclient,&rootserver_length)) == -1)
				{
					perror("recvfrom");
					printf("No Entry for the Domain in the Root Server\n");
					exit(-1);
				}

				printf("le, port est is : %d\n",extension[0] );
				printf("%d bytes received\n", r );

				//timeout
       			 setsockopt(rootfd, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv3, sizeof tv3);

				//reponse du root (ip)
				char tldip[16];
				if( (r = recvfrom(rootfd,tldip,sizeof(tldip),0, (struct sockaddr *)&rootclient,&rootserver_length)) == -1)
				{
					perror("recvfrom");
					printf("No Entry for the Domain in the Root Server\n");
					exit(-1);
				}
      		  setsockopt(rootfd, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv3, sizeof tv3);

				//reponse du root de la forme <identifiant de transaction>|<horodatage>|<nom>|\<code>|<domaine>
				char rep1[1000];
				if( (r = recvfrom(rootfd,rep1,sizeof(rep1),0, (struct sockaddr *)&rootclient,&rootserver_length)) == -1)
				{
					perror("recvfrom");
					printf("No Entry for the Domain in the Root Server\n");
					exit(-1);
				}
				//infos
				printf("rep1 : %s \n" , rep1);
				tldip[r]='\0';
				printf("tld ip is : %s\n",tldip );
				printf("%d bytes received\n", r );

				//cas d'erreur, l'extension du nom de domaine est non trouvée
				if(extension[0]==4567)
				{
					char ipbuffer1[]="1.0.0.0";
					int r2 = sendto(newfd,ipbuffer1,strlen(ipbuffer1),0,(struct sockaddr *)&clientserver,ser_len );
					printf("%d bytes received\n",r2);
					qmsg[0]=qmsg[0]+3; //1 send and 2 recv between local and root server.
					int r1 = sendto(newfd,qmsg,sizeof(int),0,(struct sockaddr *)&clientserver,ser_len );
					printf("%d bytes sent\n",r1);

					close(newfd);
					exit(-1);
				}

				qmsg[0]=qmsg[0]+3;
				printf("Number of query messages between root and local server is %d\n",qmsg[0]);
				close(rootfd);
		//========================================================================================
				//===============portnumber of tld is in extension[0]=============================

				//=================================================TLD SERVER==========================

				//Creation de la socket
				if( (tldfd = socket(AF_INET,SOCK_DGRAM,0) )== -1 )
				{
					perror("socket");
					exit(-1);
				}

				//remplir la strucutre sockadd
				tldserver.sin_family = AF_INET;
				tldserver.sin_port = htons(extension[0]);
				tldserver.sin_addr.s_addr = inet_addr(tldip);
				memset( &(tldserver.sin_zero),'\0',8 );

				//Création de la requete, sous la forme <identifiant de transaction>|<horodatage>|<nom>, comme demandé
					char rq2[700];
					int id2 = rand()%10;
					int horodatage2 = rand()%1000;
					snprintf(rq2,700,"%d %s %d %s %s" ,id2, "|", horodatage2, "|", copiebuffer2 );
					printf("requete 2 : %s \n", rq2);

				//envois de la requete
				if( (sent = sendto(tldfd,rq2,strlen(rq2),0,(struct sockaddr *)&tldserver,sizeof(tldserver) ) ) ==-1)
				{
					perror("send");
					exit(-1);
				}
				printf("%d bytes sent\n", sent);

		 //Initialisation du timeout
       	 struct timeval tv1;
          tv1.tv_sec = timeout+10;
          tv1.tv_usec = 0;
          setsockopt(tldfd, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv1, sizeof tv1);

			//reception du port
			if( (r = recvfrom(tldfd,authportnumber,sizeof(int),0, (struct sockaddr *)&tldserver, &tldserver_length)) ==-1)
				{
					perror("recvfrom");
					printf("No entry found in the Tld server\n");
					exit(-1);
				}
				printf("%d port number\n", authportnumber[0]);
				printf("%d bytes received\n",r );

		char authportip[16];
        setsockopt(tldfd, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv1, sizeof tv1);

		//reception de l'IP
		if( (r = recvfrom(tldfd,authportip,sizeof(authportip),0, (struct sockaddr *)&tldserver, &tldserver_length)) ==-1)
		{
			perror("recvfrom");
			printf("No entry found in the Tld server\n");
			exit(-1);
		}
		authportip[r]='\0';
		printf("%s port IP adddre\n", authportip);
		printf("%d bytes received\n",r );


		char rep2[1000];
		//timeout
        setsockopt(tldfd, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv1, sizeof tv1);


		/*reception de la reponse de la forme
		<identifiant de transaction>|<horodatage>|<nom>|\<code>|<domaine>,<IP1>,<port1>
		|...|<domaine>,<IPn>,<portn>*/
		if( (r = recvfrom(tldfd,rep2,sizeof(rep2),0, (struct sockaddr *)&rootclient,&rootserver_length)) == -1)
		{
			perror("recvfrom");
			printf("No Entry for the Domain in the Root Server\n");
			exit(-1);
		}
		printf("rep2 : %s \n" , rep2);

		//cas d'erreur, le sous-domaine est non trouvé
		if(authportnumber[0]==4567)
		{
			char ipbuffer1[]="1.0.0.0";
			int r2 = sendto(newfd,ipbuffer1,strlen(ipbuffer1),0,(struct sockaddr *)&clientserver,ser_len );
			printf("%d bytes received\n",r2);
			qmsg[0]=qmsg[0]+3;
			int r1 = sendto(newfd,qmsg,sizeof(int),0,(struct sockaddr *)&clientserver,ser_len );
			printf("%d bytes sent\n",r1);

			close(newfd);
			exit(-1);
		}
			qmsg[0]=qmsg[0]+3;
			printf("Number of query messages between tld and local server is %d\n",qmsg[0]);
			close(tldfd);

	//================================================================================================================
				// ==================AUTH SERVER=========================
				int authfd;
				struct sockaddr_in authserver;
				char ipbuffer[16];
				int authserver_length = sizeof(authserver);

			//Creation de la socket
				if( (authfd = socket(AF_INET,SOCK_DGRAM,0) )== -1 )
				{
					perror("socket");
					exit(-1);
				}

			//remplir la strucutre sockadd
				authserver.sin_family = AF_INET;
				authserver.sin_port = htons(authportnumber[0]);
				authserver.sin_addr.s_addr = inet_addr(authportip);
				memset( &(authserver.sin_zero),'\0',8 );
				printf("%s\n",buffer );

			//creation de la requete de la forme <identifiant de transaction>|<horodatage>|<nom>
					char rq3[512];
					int id3 = rand()%10;
					int horodatage3 = rand()%1000;
					snprintf(rq3,512,"%d %s %d %s%s" ,id3, "|", horodatage3, "|", copiebuffer2 );
					printf("requete 3 : %s \n", rq3);


			//envois de la requete à authorative
			if( (sent = sendto(authfd,rq3,strlen(rq3),0,(struct sockaddr *)&authserver,sizeof(authserver) ) ) ==-1)
				{
					perror("send");
					exit(-1);
				}
				printf("%d bytes sent\n", sent);

				int bufflen = 512;
				//Initialisation du timeout
       		 struct timeval tv2;
        	tv2.tv_sec = timeout+15;
         	 tv2.tv_usec = 0;
          setsockopt(authfd, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv2, sizeof tv2);

		  	//reception de l'IP
				if( (r = recvfrom(authfd,ipbuffer,bufflen,0, (struct sockaddr *)&authserver, &authserver_length)) == -1)
				{
					perror("recvfrom");
					printf("No Entry Found in the Authoritative Server\n");
					exit(-1);
				}

				//cas de l'IP non trouvée -> Erreur
				ipbuffer[r]='\0';
				char errip[]="1.0.0.0";
				if((strcmp(errip,ipbuffer)) == 0)
				{
					int r1 = sendto(newfd,errip,strlen(errip),0,(struct sockaddr *)&clientserver,ser_len );
					printf("%d bytes received\n",r1);
					qmsg[0]=qmsg[0]+2;
					r1 = sendto(newfd,qmsg,sizeof(int),0,(struct sockaddr *)&clientserver,ser_len );
					printf("%d bytes sent\n",r1);

					close(newfd);
				exit(-1);
				}

				//Envois de l'IP et du nombre de bits échangés au Client pour les afficher
				printf("%s IP ADDRESS\n", ipbuffer);
				printf("%d bytes received\n",r );
				qmsg[0]=qmsg[0]+2;
				printf("Number of query messages between author and local server is %d\n",qmsg[0]);
				close(authfd);

				int r1 = sendto(newfd,ipbuffer,r,0,(struct sockaddr *)&clientserver,ser_len );
				printf("%d bytes received\n",r1);

					r1 = sendto(newfd,qmsg,sizeof(int),0,(struct sockaddr *)&clientserver,ser_len );
					printf("%d bytes sent\n",r1);

				close(newfd);

				//Remplir le cache, si jamais on réutilise le meme nom de domaine
				FILE *fppp;
				fppp = fopen("Fichiers_txt/localcache.txt","a");
				fprintf(fppp," %s %s\n",copiebuffer2,ipbuffer);
				fclose(fppp);
				sleep(5);

				printf("------------------------------------\n");

			}}
	}

			}	}
