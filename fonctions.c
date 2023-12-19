#include "fonctions.h"

//Fonction de création de la socket -- renvoie le socket file descriptor
int connectTo(char * serverName, char * portASCII, char *fileName, int mode, int wrq){
	int status;
	int sfd;  //socket file descriptor
	int connect_status;
	struct addrinfo *ptr;
		
	struct addrinfo hints;
	struct addrinfo *result;
	
	memset(&hints, 0, sizeof(struct addrinfo));
	
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_DGRAM; //type de la socket
	hints.ai_protocol = IPPROTO_UDP; //on utilise le protocole UDP
	
	status = getaddrinfo(serverName, portASCII, &hints, &result);
	if (status != 0){
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(status));
        exit(EXIT_FAILURE);
    }
	
	
	ptr = result;

	while (ptr != NULL){
		sfd = socket(ptr->ai_family,ptr->ai_socktype,ptr->ai_protocol);	
		ptr = ptr->ai_next;
	}
	
	freeaddrinfo(result);


//Fonction de création de la requete
	char * rrq;
	int BUFFSIZE;
	
	rrq = calloc(MAX_RRQ_SIZE, sizeof(char)); //on alloue la mémoire qu'on va utiliser

	//création de la requête
	rrq[1] = wrq;
	strcpy(rrq + 2, fileName);
	
	switch(mode) {
		case OCTET :
			strcpy(rrq + 3 + strlen(fileName), "octet");
			BUFFSIZE = 4 + strlen(fileName) + strlen("octet");
			break;
	}
	
	sendto(sfd, rrq, BUFFSIZE, 0, result->ai_addr, result->ai_addrlen);
	
	return sfd;

}

//Fonction qui reçoit un bloc (sfd : socket file descriptor ; ffd : file descriptor)
int receiveOneBlock(int sfd, int ffd) {
	//ssize_t recvfrom(int s, void *buf, size_t len, int flags, struct stoackaddr *from, stocklen_t *fromlen);
	int count;
	struct sockaddr addr;
	socklen_t add_size;
	char *buf;
	static int block = 1;
	
	buf = calloc(MAX_BUF_RECEIVE, sizeof(char));
	
	count = recvfrom(sfd, buf, MAX_BUF_RECEIVE, 0, &addr, &add_size);
	
	if (count == -1){
		printf(ERROR_RECVFROM); 
		exit (EXIT_FAILURE); 
	}
	
	if((buf[0]!=0) ||(buf[1]!=3)){
		printf(ERROR_FROM_SERVER, buf+4); //+4 : qd c'est une trame d'erreur, le serveur envoie "0005" puis 2 octets d'erreur puis l'explicitation du message d'erreur, donc le message d'erreur "clair" se trouve 4 rang après
		exit(EXIT_FAILURE);
	}
	
	char acq[4] = {0, 4};
	acq[2] = buf[2];
	acq[3] = buf[3];
	
	sendto(sfd, acq, 4, 0, &addr, add_size);
	
	if (((buf[2]<<8)+buf[3])==block){
		write(ffd, buf+4, count-4);
		block++;
	}
	
	free(buf);
	
	return count;
}

void receiveOneFile(int sfd, int ffd){
	int count;
	int receiveTotal;
	int blockTotal = 0;
	int timeTotal;
	struct timespec start, stop;
	
	clock_gettime(CLOCK_REALTIME, &start);
	
	do{
		count = receiveOneBlock(sfd, ffd);
		blockTotal ++;
		receiveTotal += count;
	}while(count==MAX_BUF_RECEIVE);
	
	clock_gettime(CLOCK_REALTIME, &stop);
	timeTotal = (stop.tv_nsec - start.tv_nsec)/100000;
	
	printf(END_TRANSMISSION, receiveTotal, blockTotal, timeTotal);
}

//Fonction qui envoie un bloc (sfd : socket file descriptor ; ffd : file descriptor)
int sendOneBlock(int sfd, int ffd) {
	int count;
	struct sockaddr addr;
	socklen_t add_size;
	char *buf;
	static int block = 1;
	
	buf = calloc(MAX_BUF_SEND, sizeof(char));
	
	count = sendto(sfd, buf, MAX_BUF_RECEIVE, 0, &addr, add_size);
	
	if (count == -1){
		printf(ERROR_SENDTO); 
		exit (EXIT_FAILURE); 
	}
	
	if((buf[0]!=0) ||(buf[1]!=4)){
		printf(ERROR_FROM_SERVER, buf+4); 
		exit(EXIT_FAILURE);
	}
	
	char acq[4] = {0, 4};
	acq[2] = buf[2];
	acq[3] = buf[3];
	
	sendto(sfd, acq, 4, 0, &addr, add_size);
	
	if (((buf[2]<<8)+buf[3])==block){
		write(ffd, buf+4, count-4);
		block++;
	}
	
	free(buf);
	
	return count;
}

void sendOneFile(int sfd, int ffd){
	int count;
	int sendTotal;
	int blockTotal = 0;
	int timeTotal;
	struct timespec start, stop;
	
	clock_gettime(CLOCK_REALTIME, &start);
	
	do{
		count = sendOneBlock(sfd, ffd);
		blockTotal ++;
		sendTotal += count;
	}while(count==MAX_BUF_SEND);
	
	clock_gettime(CLOCK_REALTIME, &stop);
	timeTotal = (stop.tv_nsec - start.tv_nsec)/100000;
	
	printf(END_TRANSMISSION, sendTotal, blockTotal, timeTotal);
}
