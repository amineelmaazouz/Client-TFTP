#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <fcntl.h>

#define ERROR_MSG "Usage : %s host:port file \n Please try again... \n"
#define ERROR_CONNECT "Unable to connect on host \n"
#define ERROR_FROM_SERVER "Server has return an error : \n%s\n"
#define ERROR_RECVFROM "Server not found \n"
#define ERROR_SENDTO "Impossible Sent \n"
#define END_TRANSMISSION "Received %d bytes in %d block in %d ms"
#define MAX_NAME_SIZE 64
#define MAX_RRQ_SIZE 128
#define MAX_BUF_RECEIVE 516
#define MAX_BUF_SEND 2048
#define OCTET 0

#define OPCODE_RRQ      1
#define OPCODE_WRQ      2
#define OPCODE_DATA     3
#define OPCODE_ACK      4
#define OPCODE_ERROR    5

#define BUFFER_SIZE     516 // Because 2 bytes for opcode + 2 bytes for block id + 512 bytes for data

int connectTo(char *, char *, char *, int, int);
//void sendRRQ(int, char *, int);
int receiveOneBlock(int sfd, int ffd);
void receiveOneFile(int sfd, int ffd);
int sendOneBlock(int sfd, int ffd);
void sendOneFile(int sfd, int ffd);
