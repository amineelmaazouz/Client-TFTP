#include "fonctions.h"

void getFile(char *fileName, struct sockaddr *sock_addr, int sockfd) {
	
    int requestLength = (int)(2 + strlen(fileName) + 1 + 5 + 1);
    char buffer[BUFFER_SIZE];
    int sendSize;
    int recvSize;
    char opcode;
    socklen_t addrSize;
    unsigned short blockId; // Unsigned short because it occupies 2 bytes
    unsigned short blockCounter = 0;
    FILE *fileOut;

    struct sockaddr sock_in_addr;

    // Construct the request packet
    buffer[0] = 0;
    buffer[1] = OPCODE_RRQ;
    strcpy(&buffer[2], fileName);
    strcpy(&buffer[2 + strlen(fileName) + 1], "octet");

    // Open the file for writing and reading
    if ((fileOut = fopen(fileName, "w+")) == NULL) {
        printf("Error: unable to open file %s\n", fileName);
        return;
    }

    // Send the request packet to the server
    sendSize = sendto(sockfd, buffer, requestLength, 0, sock_addr, sizeof(struct sockaddr));
    if (sendSize == -1 || sendSize != requestLength) {
        printf("Error while sendto: %d\n", sendSize);
        return;
    }

    // Receive and process data packets
    while (1) {
        addrSize = sizeof(struct sockaddr);
        recvSize = recvfrom(sockfd, buffer, BUFFER_SIZE, 0, &sock_in_addr, &addrSize);
        if (recvSize == -1) {
            printf("Error while recvfrom: %d\n", recvSize);
        }
        opcode = buffer[1];

        if (opcode == OPCODE_ERROR) {
            printf("Error opcode %s\n", &buffer[4]);
            return;
        }

        // Extract the block ID from the received packet
        blockId = (unsigned short)buffer[2] | (unsigned short)buffer[3];

        requestLength = 4; // Set the request length to 2 bytes

        // Construct and send an ACK packet
        buffer[0] = 0;
        buffer[1] = OPCODE_ACK;
        sendSize = sendto(sockfd, buffer, requestLength, 0, &sock_in_addr, sizeof(struct sockaddr));

        printf("Received block id %d: %d bytes\n", blockId, recvSize - 4);

        // Write data to the file if conditions are met
        if ((recvSize - 4 > 0) && (blockId > blockCounter)) {
            fwrite(&buffer[4], 1, recvSize - 4, fileOut);
            blockCounter++;
        }

        // Check if the last block has less than 512 bytes, indicating the end of the file
        if (recvSize - 4 < 512) {
            fclose(fileOut);
            return;
        }
    }
}

int main(int argc, char *args[]) {
    int s;
    struct addrinfo hints;
    struct addrinfo *res;
    char PORT[10];
    char ipbuf[INET_ADDRSTRLEN];
    int sockfd;

    memset(&hints, 0, sizeof(struct addrinfo));

    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_protocol = IPPROTO_UDP;

    if (argc != 3) {
        printf("Usage: getftp <host> <local file>\n");
        return 0;
    }
    printf("Host: %s, File: %s\n", args[1], args[2]);
    sprintf(PORT, "%d", 1069); // For now, manually set the port to use

    // Get address information for the specified host and port
    if ((s = getaddrinfo(args[1], PORT, &hints, &res)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(s));
        exit(EXIT_FAILURE);
    }

    // Convert the server address to a human-readable format
    inet_ntop(AF_INET, &((struct sockaddr_in *)res->ai_addr)->sin_addr, ipbuf, sizeof(ipbuf));
    printf("Connecting to server: %s\n", ipbuf);

    // Create a UDP socket
    if ((sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol)) == -1) {
        printf("Error while creating socket\n");
    }

    // Perform the file transfer
    getFile(args[2], res->ai_addr, sockfd);

    // Clean up and close the socket
    close(sockfd);
    freeaddrinfo(res);
}
