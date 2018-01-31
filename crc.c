#include <netdb.h>
#include <netinet/in.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/time.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include "interface.h"

#define MAX_MESSAGE 128
using namespace std;

/*
 * TODO: IMPLEMENT BELOW THREE FUNCTIONS
 */
int connect_to(const char *host, const int port);
struct Reply process_command(const int sockfd, char* command);
void process_chatmode(const char* host, const int port);

int main(int argc, char** argv) 
{
	if (argc != 3) {
		fprintf(stderr,
				"usage: enter host address and port number\n");
		exit(1);
	}

    display_title();
    
	while (1) {
	
		int sockfd = connect_to(argv[1], atoi(argv[2]));
    
		char command[MAX_DATA];
        get_command(command, MAX_DATA);

		struct Reply reply = process_command(sockfd, command);
		display_reply(command, reply);
		
		touppercase(command, strlen(command) - 1);
		if (strncmp(command, "JOIN", 4) == 0) {
			printf("Now you are in the chatmode\n");
			process_chatmode(argv[1], reply.port);
		}
	
		close(sockfd);
    }

    return 0;
}

/*
 * Connect to the server using given host and port information
 *
 * @parameter host    host address given by command line argument
 * @parameter port    port given by command line argument
 * 
 * @return socket fildescriptor
 */
int connect_to(const char *host, const int port)
{
	// ------------------------------------------------------------
	// GUIDE :
	// In this function, you are suppose to connect to the server.
	// After connection is established, you are ready to send or
	// receive the message to/from the server.
	// 
	// Finally, you should return the socket fildescriptor
	// so that other functions such as "process_command" can use it
	// ------------------------------------------------------------

	int sockfd;

    struct sockaddr_in serv_addr;

    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("Client socket");
        exit(1);
    }

    bzero((char *) &serv_addr, sizeof(serv_addr));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);

    if (connect(sockfd, (struct sockaddr*) &serv_addr, sizeof(serv_addr)) < 0)
    {
        perror("Client connect");
        exit(1);
    }

	return sockfd;
}

/*
 * Send an input command to the server and return the result
 *
 * @parameter sockfd   socket file descriptor to commnunicate
 *                     with the server
 * @parameter command  command will be sent to the server
 *
 * @return    Reply    
 */
struct Reply process_command(const int sockfd, char* command)
{
	struct Reply reply;
	reply.status = FAILURE_UNKNOWN;

	// Don't waste a message sent to the server if the command isn't even valid
    if (strncmp(command, "CREATE ", 7) != 0 && strncmp(command, "DELETE ", 7) != 0 &&
	    strncmp(command, "JOIN ", 5) != 0 && strncmp(command, "LIST", 4) != 0)
    {
        reply.status = FAILURE_INVALID;

        return reply;
    }
		
    write(sockfd, command, strlen(command));
	
    // Receive the command from the server
    char buffer[1024] = {0};
	string buf;
    if (read(sockfd, buffer, MAX_DATA) != 0)
    {
		buf=buffer;
		printf("ON CLIENT: %s\n", buffer);
		if(buf.substr(0, 7) == "SUCCESS"){
			reply.status = SUCCESS;
			if (strncmp(command, "JOIN", 4) == 0) {
                reply.num_member = atoi(buf.substr(8, 9).c_str());
				reply.port = atoi(buf.substr(10).c_str());
			} 
			else if (strncmp(command, "LIST", 4) == 0) {
				stpcpy(reply.list_room, buf.substr(8).c_str());
			}
			else if (strncmp(command, "DELETE", 7) == 0) {
				stpcpy(reply.list_room, buf.substr(8).c_str());
			}
		}
		else if(buf.substr(0, 22) == "FAILURE_ALREADY_EXISTS"){
			reply.status = FAILURE_ALREADY_EXISTS;
		}
		else if(buf.substr(0, 18) == "FAILURE_NOT_EXISTS"){
			reply.status = FAILURE_NOT_EXISTS;
		}
		else if(buf.substr(0, 15) == "FAILURE_INVALID"){
			reply.status = FAILURE_INVALID;
		}
		else{
			reply.status = FAILURE_UNKNOWN;
		}	
    }

	return reply;
}

/* 
 * Get into the chat mode
 * 
 * @parameter host     host address
 * @parameter port     port
 */
void process_chatmode(const char* host, const int port)
{
	// ------------------------------------------------------------
	// GUIDE 1:
	// In order to join the chatroom, you are supposed to connect
	// to the server using host and port.
	// You may re-use the function "connect_to".
	// ------------------------------------------------------------

	// ------------------------------------------------------------
	// GUIDE 2:
	// Once the client have been connected to the server, we need
	// to get a message from the user and send it to server.
	// At the same time, the client should wait for a message from
	// the server.
	// ------------------------------------------------------------
	
    // ------------------------------------------------------------
    // IMPORTANT NOTICE:
    // 1. To get a message from a user, you should use a function
    // "void get_message(char*, int);" in the interface.h file
    // 
    // 2. To print the messages from other members, you should use
    // the function "void display_message(char*)" in the interface.h
    //
    // 3. Once a user entered to one of chatrooms, there is no way
    //    to command mode where the user  enter other commands
    //    such as CREATE,DELETE,LIST.
    //    Don't have to worry about this situation, and you can 
    //    terminate the client program by pressing CTRL-C (SIGINT)
	// ------------------------------------------------------------
	printf("Connecting to %s:%d\n", host, port);

	int fd = connect_to(host, port);

	printf("Connected to %s:%d\n", host, port);

	while (1)
	{
		char message[MAX_MESSAGE];
		get_message(message, MAX_MESSAGE);

		write(fd, message, MAX_MESSAGE);
		read(fd, message, MAX_MESSAGE);

		printf("Message %s sent!\n", message);
	}
}

