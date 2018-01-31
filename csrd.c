#include <netdb.h>
#include <netinet/in.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/time.h>

#include <sys/socket.h>
#include <sys/select.h>
#include <pthread.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "interface.h"

#define MAX_MESSAGE 128
#define MAX_MEMBER 32
#define MAX_ROOM 32

int handle_request(const char* command, const int port);
void* start_server(void* data);
struct Reply create(const char* chatroom);
struct Reply join(const char* chatroom);
struct Reply list(void);
struct Reply del(const char* chatroom);

typedef struct 
{
    // Name of chatroom
    char room_name[256];
    // Port number to join the chatroom
    int port_num;
    // # of members in chatroom
    int num_members;
    // Slave sockets
    int slave_socket[MAX_MEMBER];
} chat_room;

chat_room room_db[MAX_ROOM];

int port;

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
        perror("Command master socket");
        exit(1);
    }

    // Allow for reuse of address
    int opt = 1;
    if (setsockopt(m_sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(int)) < 0)
    {
        perror("Set socket options");
        exit(1);
    }

    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    port = atoi(argv[2]);
    serv_addr.sin_port = htons(port++);

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

    printf("Server running\n");

    // Run the server
    while (1)
    {
        memcpy(&rfds, &afds, sizeof(rfds));

        int n_socks;

        // https://www.ibm.com/support/knowledgecenter/en/ssw_i5_54/rzab6/xnonblock.htm

        // nfds should be heighest number file descriptor + 1
        // rfds is read file set
        // select(...) monitors multiple file descriptors, and returns # of file descriptors in sets
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
        }

        for (int fd = 0; fd < nfds; fd++)
        {
            // If the socket has activity on it
            if (fd != m_sock && FD_ISSET(fd, &rfds))
            {
                char* command = (char*) malloc((MAX_DATA) * sizeof(char));

                read(fd, command, MAX_DATA);

                // TODO: Handle request... with sends and receives
                if (handle_request(command, fd) > 0)
                {
                    // Close the file descriptor
                    if (close(fd) < 0)
                    {
                        perror("Close");
                        exit(1);
                    }
                }

                // Remove fd from set
                FD_CLR(fd, &afds);
            }
        }
    }
}

int handle_request(const char* command, const int fd)
{
    if (strncmp(command, "CREATE ", 7) == 0)
    {
        // TODO: Check if already exists
        
        pthread_t th;
        pthread_attr_t ta;
        pthread_attr_init(&ta);
        pthread_attr_setdetachstate(&ta, PTHREAD_CREATE_DETACHED);

        // Create the new server on the new socket
        int* port_ptr = new int;
        *port_ptr = port;
        pthread_create(&th, &ta, start_server, (void*) port_ptr);
        
        // Send port back to client
        char pstr[5];
        sprintf(pstr, "%d", port);
        write(fd, pstr, strlen(pstr));

        return 0;
    }
    else if (strncmp(command, "JOIN ", 5) == 0)
    {
        // TODO: Search if server exists, then get port
        char pstr[6];
        sprintf(pstr, "%d", port);
        write(fd, pstr, strlen(pstr));

        return 1;
    }
}

void* start_server(void* data)
{
    int m_sock, s_sock;

    int port = *((int*)data);

    struct sockaddr_in cr_addr, cli_addr;

    if ((m_sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("Chatroom master socket");
        exit(1);
    }

    int opt = 1;
    if (setsockopt(m_sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(int)) < 0)
    {
        perror("Set socket options");
        exit(1);
    }

    bzero((char*) &cr_addr, sizeof(cr_addr));
    cr_addr.sin_family = AF_INET;
    cr_addr.sin_addr.s_addr = INADDR_ANY;
    cr_addr.sin_port = htons(port++);

    if (bind(m_sock, (struct sockaddr*) &cr_addr, sizeof(cr_addr)) < 0)
    {
        perror("Chatroom socket bind");
        exit(1);
    }

    if (listen(m_sock, 20) < 0)
    {
        perror("Chatroom socket listen");
        exit(1);
    }

    fd_set rfds, afds;
    int nfds = FD_SETSIZE;

    FD_ZERO(&afds);
    FD_SET(m_sock, &afds);

    char* message = (char*) malloc((MAX_DATA) * sizeof(char));

    printf("Chatroom socket running\n");

    // Run the chatroom
    while (1)
    {
        memcpy(&rfds, &afds, sizeof(rfds));

        int n_socks;

        if ((n_socks = select(nfds, &rfds, 0, 0, 0)) < 0)
        {
            perror("Chatroom select");
            exit(1);
        }

        if (FD_ISSET(m_sock, &rfds))
        {
            socklen_t size = sizeof(cli_addr);
            if ((s_sock = accept(m_sock, (struct sockaddr*) &cli_addr, &size)) < 0)
            {
                perror("Chatroom accept");
                exit(1);
            }

            // Add socket to set of slave sockets
            FD_SET(s_sock, &afds);
        }

        for (int fd = 0; fd < nfds; fd++)
        {
            // If the socket has activity on it
            if (fd != m_sock && FD_ISSET(fd, &rfds))
            {
                read(fd, message, MAX_MESSAGE);

                // Trying to broadcast message to all others
                // for (int kfd = 0; fd < nfds; fd++)
                // {
                    // if (fd != kfd)
                    // {
                        // write(fd, message, MAX_MESSAGE);
                    // }
                // }

                write(fd, message, MAX_MESSAGE);
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

