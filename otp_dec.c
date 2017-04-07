#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <fcntl.h>

void error(const char *msg) { perror(msg); exit(0); } // Error function used for reporting issues
void setStringFromFile(char* file_in, char* toString, char* postfix);
void sendMessageToServer(int socket, char* sendString);
void getMessageFromServer(int socket, int portNum);
void validateInput(char* inputString);

int main(int argc, char *argv[])
{
    int socketFD, portNumber, charsWritten, charsRead;
    struct sockaddr_in serverAddress;
    struct hostent* serverHostInfo;
    char buffer[70100];
    int file_descriptor;
    ssize_t nread;

    // sendString will consist of who$decryptString#keyString@
    char* whoString = "otp_dec$";
    char decryptString[70010];
    memset(decryptString, '\0', 70010);
    char keyString[70010];
    memset(keyString, '\0', 70010);
    char sendString[140100];
    memset(sendString, '\0', 140100);

    setStringFromFile(argv[1], decryptString, "#");
    setStringFromFile(argv[2], keyString, "@");

    int encStrLen = strlen(decryptString);
    int keyStrLen = strlen(keyString);
    if(keyStrLen < encStrLen){ fprintf(stderr,"Error: key '%s' is too short\n", argv[2]); exit(1); }  //Check that key is not shorter than string in argv[1]

    validateInput(decryptString);

    strcpy(sendString, whoString);
    strcat(sendString, decryptString);
    strcat(sendString, keyString);

    if (argc < 4) { fprintf(stderr,"USAGE: %s file_in key port\n", argv[0]); exit(0); } // Check usage & args
    // Set up the server address struct
    memset((char*)&serverAddress, '\0', sizeof(serverAddress)); // Clear out the address struct
    portNumber = atoi(argv[3]); // Get the port number, convert to an integer from a string
    serverAddress.sin_family = AF_INET; // Create a network-capable socket
    serverAddress.sin_port = htons(portNumber); // Store the port number
    serverHostInfo = gethostbyname("localhost");
    if (serverHostInfo == NULL) { fprintf(stderr, "CLIENT: ERROR, no such host\n"); exit(0); }
    memcpy((char*)&serverAddress.sin_addr.s_addr, (char*)serverHostInfo->h_addr, serverHostInfo->h_length); // Copy in the address

    // Set up the socket
    socketFD = socket(AF_INET, SOCK_STREAM, 0); // Create the socket
    if (socketFD < 0) error("CLIENT: ERROR opening socket");
    // Connect to server
    if (connect(socketFD, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) < 0) // Connect socket to addy
        error("CLIENT: ERROR connecting");

    // Send to and receive from server
    sendMessageToServer(socketFD, sendString);
    getMessageFromServer(socketFD, portNumber);

    close(socketFD); // Close the socket
    return 0;
}


void setStringFromFile(char* file_in, char* toString, char* postfix) {
    char buffer[70100];
    int file_descriptor;
    ssize_t nread;

    file_descriptor = open(file_in, O_RDONLY);

    if (file_descriptor == -1){
        printf("Hull breach - open() failed on \"%s\"\n", file_in);
        perror(file_in);
    }

    /* read file, parse file */
    memset(buffer, '\0', sizeof(buffer)); // Clear out the array before using it
    lseek(file_descriptor, 0, SEEK_SET); // Reset the file pointer to the beginning of the file
    nread = read(file_descriptor, buffer, sizeof(buffer));
    close(file_descriptor);

    strcpy(toString, buffer);
    strcat(toString, postfix);
}

void sendMessageToServer(int socket, char* sendString){
    int charsWritten;
    charsWritten = send(socket, sendString, strlen(sendString), 0); // Write to the server
    if (charsWritten < 0) error("CLIENT: ERROR writing to socket");
    if (charsWritten < strlen(sendString)) printf("CLIENT: WARNING: Not all data written to socket!\n");
}

void getMessageFromServer(int socket, int portNum){
    char buffer[140100];
    memset(buffer, '\0', sizeof(buffer)); // Clear out the buffer

    int charsRead;
    charsRead = recv(socket, buffer, sizeof(buffer) - 1, 0); // Read data from the socket, leaving \0 at end
    if (charsRead < 0) error("CLIENT: ERROR reading from socket");
    if(strcmp(buffer, "Connection refused") == 0){
        fprintf(stderr, "Error: could not contact opt_dec_d on port %d\n", portNum);
        exit(2);
    } else {
        fprintf(stdout, buffer);
    }
}

void validateInput(char* inputString){
    int i;
    for(i = 0; i < strlen(inputString) - 2; i++){       // -2 for "\n#"
        if((inputString[i] < 64 || inputString[i] > 90) && inputString[i] != 32){
            fprintf(stderr, "opt_dec error: input contains bad characters\n");
            exit(1);
        }
    }
}
