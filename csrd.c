#include <netdb.h>
#include <netinet/in.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/time.h>

#include <sys/socket.h>
#include <sys/select.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "interface.h"

#define MAX_MESSAGE 128

struct Reply create(const char* chatroom);
struct Reply join(const char* chatroom);
struct Reply list(void);
struct Reply del(const char* chatroom);

struct ChatRoom
{
    // name of chatroom
    char name[32];
    // port number to join the chatroom
    int port;
}

int main(int argc, char** argv)
{
    // TODO: Condition for when to get angry
    if (argc != 3) 
    {
        fprintf(stderr, "usage: enter host address and port number\n");
        exit(0);
    }

    // TODO: socket(), bind(), listen(), accept(),
    //       select(), recv(), send(), close()

    int m_sock, s_sock, max_sock;
    unsigned int message_len;
    char buffer[MAX_MESSAGE];

    struct addrinfo hints, *res;
    struct sockaddr_in serv_addr, cli_addr;

    // TODO: Call getaddrinfo();
    // getaddrinfo(NULL, PORT, &hints, &res);

    // Create IPv4 TCP file descriptor
    if ((m_sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("Master socket");
        exit(1);
    }

    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(atoi(argv[2]));

    if (bind(m_sock, (struct sockaddr*) &serv_addr, sizeof(serv_addr)) < 0)
    {
        perror("Socket bind");
        exit(1);
    }

    // Allow 20 pending connections
    if (listen(m_sock, 20) < 0)
    {
        perror("Socket listen");
        exit(1);
    }

    fd_set rfds, afds;
    int nfds = FD_SETSIZE;

    FD_ZERO(&afds);
    FD_SET(m_sock, &afds);  // Add master socket

    max_sock = m_sock;

    printf("Server running\n");

    // Run the server
    while (1)
    {
        memcpy(&rfds, &afds, sizeof(rfds));

        int n_socks;

        // https://www.ibm.com/support/knowledgecenter/en/ssw_i5_54/rzab6/xnonblock.htm

        if ((n_socks = select(nfds, &rfds, 0, 0, 0)) < 0)
        {
            perror("Select");
            exit(1);
        }

        if (FD_ISSET(m_sock, &rfds))
        {
            socklen_t size = sizeof(cli_addr);
            if ((s_sock = accept(m_sock, (struct sockaddr*) &cli_addr, &size)) < 0)
            {
                perror("Accept");
                exit(1);
            }

            // Add socket to set of slave sockets
            FD_SET(s_sock, &afds);

            if (s_sock > max_sock)
            {
                max_sock = s_sock;
            }
        }

        for (int fd = 0; fd < nfds; fd++)
        {
            if (fd != m_sock && FD_ISSET(fd, &rfds))
            {
                // TODO: Handle request... with sends and receives
                char mess[1024];
                read(s_sock, mess, 256);
                printf("ON SERVER: %s\n", mess);
                write(s_sock, mess, strlen(mess));
                // send(s_sock, some stuff, );
                close(fd);
                FD_CLR(fd, &afds);
            }
        }
    }
}

struct Reply create(const char* chatroom)
{
    // TODO: Check if chatroom exists already
    // TODO: If not, create a new master socket
    // TODO: Create an entry for the new chat room in the local database, and store the name and the port number of the new chat room
    // TODO: Return a result to inform the client
}

struct Reply join(const char* chatroom)
{
    // TODO: Check if chatroom exists already
    // TODO: If it does, return the port number of the master socket of that chat room and the current number of memebers in the chatroom
    // TODO: Client will then connect to the chat room through that port
}

struct Reply list()
{
    // TODO: Return the names of all chatrooms
}

struct Reply del(const char* chatroom)
{
    // TODO: Check if chatroom exits already
    // TODO: If it does, send the warning message "chat room being deleted" to all connected clients before terminating their connections, closing the master socket, and deleting the entry
    // TODO: Inform the client about the result
}

