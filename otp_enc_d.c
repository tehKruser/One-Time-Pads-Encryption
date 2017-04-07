#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

void error(const char *msg) { perror(msg); exit(1); } // Error function used for reporting issues
void encryptInputString(char* inputString, char* keyString);
void forkChild(int establishedConnectionFD);
void reapChildren();

int main(int argc, char *argv[])
{
    int listenSocketFD, establishedConnectionFD, portNumber, charsRead;
    socklen_t sizeOfClientInfo;

    struct sockaddr_in serverAddress, clientAddress;
    if (argc < 2) { fprintf(stderr,"USAGE: %s port\n", argv[0]); exit(1); } // Check usage & args

    // Set up the address struct for this process (the server)
    memset((char *)&serverAddress, '\0', sizeof(serverAddress)); // Clear out the address struct
    portNumber = atoi(argv[1]); // Get the port number, convert to an integer from a string
    serverAddress.sin_family = AF_INET; // Create a network-capable socket
    serverAddress.sin_port = htons(portNumber); // Store the port number
    serverAddress.sin_addr.s_addr = INADDR_ANY; // Any address is allowed for connection to this process
    // Set up the socket
    listenSocketFD = socket(AF_INET, SOCK_STREAM, 0); // Create the socket
    if (listenSocketFD < 0) error("ERROR opening socket");

    // Enable the socket to begin listening
    if (bind(listenSocketFD, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) < 0) // Connect socket to port
        error("ERROR on binding");
    listen(listenSocketFD, 5); // Flip the socket on - it can now receive up to 5 connections

    while (1) {
        // Reap here
        reapChildren();

        // Accept a connection, blocking if one is not available until one connects
        sizeOfClientInfo = sizeof(clientAddress); // Get the size of the address for the client that will connect
        establishedConnectionFD = accept(listenSocketFD, (struct sockaddr *)&clientAddress, &sizeOfClientInfo); // Accept
        if (establishedConnectionFD < 0) error("ERROR on accept");

        forkChild(establishedConnectionFD);
    }
    close(listenSocketFD); // Close the listening socket

    return 0;
}

void forkChild(int establishedConnectionFD){
    pid_t spawnPid = -5;
    int childExitStatus = -5;
    spawnPid = fork();
    switch (spawnPid) {
        case -1: { perror("Hull Breach!\n"); exit(1); break; }
        case 0: {   // child process
            int charsRead;

            // Get the message from the client and display it
            char buffer[140100];
            memset(buffer, '\0', sizeof(buffer));
            charsRead = recv(establishedConnectionFD, buffer, sizeof(buffer), 0); // Read the client's message from the socket
            if (charsRead < 0) error("ERROR reading from socket");

            char encryptString[70010];
            memset(encryptString, '\0', 70010);
            char keyString[70010];
            memset(keyString, '\0', 70010);

            char* token = strtok(buffer, "$");
            //validate who on token
            if(strcmp(token, "otp_enc") != 0){
                charsRead = send(establishedConnectionFD, "Connection refused", 18, 0); // Send success back
                if (charsRead < 0) error("ERROR writing to socket");
            } else {
                token = strtok(NULL, "#");
                strcpy(encryptString, token);

                token = strtok(NULL, "@");
                strcpy(keyString, token);

                // encrypt string
                encryptInputString(encryptString, keyString);

                // Send a Success message back to the client
                charsRead = send(establishedConnectionFD, encryptString, strlen(encryptString), 0); // Send success back
                if (charsRead < 0) error("ERROR writing to socket");
            }

            close(establishedConnectionFD); // Close the existing socket which is connected to the client
            exit(0); break;
        }
        default: {  // parent process
            break;
        }
    }
}


void encryptInputString(char* inputString, char* keyString) {
    int i;
    for(i = 0; i < strlen(inputString) - 1; i++){
        int a = inputString[i];
        if(a == 32) {
            a = 27;
        }
        else {
            a -= 64;
        }

        int b = keyString[i];
        if(b == 32) {
            b = 27;
        }
        else {
            b -= 64;
        }

        int c = a + b;
        if(c > 27){
            c = c - 27;
        }

        if ( c == 27 ) {
            c = 32;
        } else {
            c += 64;
        }

        inputString[i] = c;
    }
}

void reapChildren(){
    int childExitStatus = -5;
    pid_t actualPid;

    while((actualPid = waitpid(-1, &childExitStatus, WNOHANG)) > 0){}
}
