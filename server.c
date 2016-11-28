/*
    C socket server example
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>      // strlen
#include <sys/socket.h>  // socket
#include <arpa/inet.h>   // inet_addr
#include <unistd.h>      // write

int main(int argc , char *argv[])
{
    // Variables
    int ssock, csock, addrlen, ret;
    struct sockaddr_in server, client;
    char message[2000], reply[2000];

    short port = 8888;

    // Create a socket. Return value is a file descriptor for the socket.
    ssock = socket(AF_INET, SOCK_STREAM, 0);
    if (ssock == -1) {
        perror("Error creating socket\n");
        exit(0);
    }
    printf("Server socket created\n");

    // Set the server ip address, connection family and port. INADDR_ANY means
    // all the ip addresses of the server can be used to set up connection.
    server.sin_family      = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port        = htons(port);

    // Bind the socket for the connection
    if (bind(ssock, (struct sockaddr *) &server, sizeof(server)) < 0) {
        //print the error message
        perror("Error binding\n");
        close(ssock);
        exit(0);
    }
    printf("Binding done\n");

    // Listen for incoming connections
    listen(ssock, SOMAXCONN);
    printf("Listening...\n");

    printf("Waiting for incoming connections...\n");
    addrlen = sizeof(struct sockaddr_in);

    // Accept connection from an incoming client
    csock = accept(ssock, (struct sockaddr *) &client, (socklen_t *) &addrlen);
    if (csock < 0) {
        perror("Error accepting connections\n");
        exit(0);
    }
    printf("Connection accepted\n");

    // Keep communicating with client. Repetitively send messages.
/*
    while(1) {
        printf("Enter message: ");
        scanf("%s" , message);

        if (strcmp(message, "end") == 0) {
            break;
        }

        // Send the message to client
        if ((ret = write(csock, message, strlen(message))) <= 0) {
            perror("Error writing\n");
            break;
        }

        memset(reply, 0, sizeof(reply));
        // Receive a reply from the client. recv() is an alternative for read().
        // Similarly, send() can replace write().
        if ((ret = recv(csock, reply, sizeof(reply), 0)) <= 0) {
            perror("Error receiving\n");
            break;
        }

        printf("Client: %s\n", reply);
    }
*/
	while((ret=recv(csock, reply, sizeof(reply), 0)) > 0) {
		//reply[ret] = '\0';
		printf("Client: %s\n", reply);
		const size_t line_size = 300;
		char* line = (char *) malloc(line_size);
		/*
		FILE *fp = popen(reply, "r");
		fscanf(fp, "%s", result);
		*/
		strcat(reply, " > output.txt");
		system(reply);
		FILE *file = fopen("output.txt", "r");
		while(fgets(line, line_size, file) != NULL) {
			printf(line);
			//need to send line by line
		}
		//send some terminating message
		write(csock, line, strlen(line));
		free(line);
		fclose(file);
		//pclose(fp);
		memset(reply, 0, sizeof(reply));
	}

    close(csock);
    close(ssock);
    printf("End connection\n");

    return 0;
}
