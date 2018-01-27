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
#define PORT 3490

struct Reply create(const char *chatroom);
struct Reply join(const char *chatroom);
struct Reply list(void);
struct Reply del(const char *chatroom);

int main(int argc, char** argv)
{
    // TODO: Condition for when to get angry
    if (false) 
    {
        fprintf(stderr, "usage: enter host address and port number\n");
        exit(0);
    }

    // TODO: socket(), bind(), listen(), accept(),
    //       select(), recv(), send(), close()

    // TODO: Listen on a well-known port to take commands, loop through below

    int m_sock, s_sock;
    unsigned int message_len;
    char buffer[MAX_MESSAGE];

    struct addrinfo hints, *res;
    struct sockaddr_in self, fsin;

    // TODO: Call getaddrinfo();
    // getaddrinfo(NULL, PORT, &hints, &res);

    // NOTE: Put this on the client?
    // struct addrinfo server;
    // memset(&server, 0, sizeof(server));
    // server.ai_family = AF_INET;         // IPv4
    // server.ai_socktype = SOCK_STREAM;   // TCP socket

    // Create iPv4 TCP file descriptor
    if ((m_sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("Master socket");
        // TODO: Use errno?
        exit(-1);
    }

    bzero(&self, sizeof(self));
    self.sin_family = AF_INET;
    self.sin_port = htons(PORT);
    self.sin_addr.s_addr = INADDR_ANY;

    // TODO: Fix this bind
    if (bind(m_sock, (struct sockaddr*) &self, sizeof(self)) < 0)
    {
        perror("Socket bind");
        exit(-1);
    }

    // TODO: Listen
    if (listen(m_sock, 20) < 0)
    {
        perror("Socket listen");
        exit(-1);
    }

    fd_set rfds, afds;
    int nfds = FD_SETSIZE;

    FD_ZERO(&afds);
    FD_SET(m_sock, &afds);  // Add master socket

    // Run the server
    while (1)
    {
        memcpy(&rfds, &afds, sizeof(rfds)); // Copy
        select(nfds, &rfds, 0, 0, 0);

        if (FD_ISSET(m_sock, &rfds))
        {
            socklen_t size = sizeof(fsin);
            if ((s_sock = accept(m_sock, (struct sockaddr*) &fsin, &size)) < 0)
            {
                perror("Accept");
                exit(-1);
            }

            FD_SET(s_sock, &afds);
        }

        for (int fd = 0; fd < nfds; fd++)
        {
            if (fd != m_sock && FD_ISSET(fd, &rfds))
            {
                // TODO: Handle request... with sends and receives
                // send(s_sock, some stuff, );
                close(fd);
                FD_CLR(fd, &afds);
            }
        }
    }
}

struct Reply create(const char *chatroom)
{
    // TODO: Check if chatroom exists already
    // TODO: If not, create a new master socket
    // TODO: Create an entry for the new chat room in the local database, and store the name and the port number of the new chat room
    // TODO: Return a result to inform the client
}

struct Reply join(const char *chatroom)
{
    // TODO: Check if chatroom exists already
    // TODO: If it does, return the port number of the master socket of that chat room and the current number of memebers in the chatroom
    // TODO: Client will then connect to the chat room through that port
}

struct Reply list()
{
    // TODO: Return the names of all chatrooms
}

struct Reply del(const char *chatroom)
{
    // TODO: Check if chatroom exits already
    // TODO: If it does, send the warning message "chat room being deleted" to all connected clients before terminating their connections, closing the master socket, and deleting the entry
    // TODO: Inform the client about the result
}

