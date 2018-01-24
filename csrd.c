#include <netdb.h>
#include <netinet/in.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/time.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "interface.h"

struct Reply create(const char *chatroom);
struct Reply join(const char *chatroom);
struct Reply list();
struct Reply delete(const char *chatroom);

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
    // TOOD: If it does, return the port number of the master socket of that chat room and the current number of memebers in the chatroom
    // TODO: Client will then connect to the chat room through that port
}

struct Reply list()
{
    // TODO: Return the names of all chatrooms
}

struct Reply delete(const char *chatroom)
{
    // TODO: Check if chatroom exits already
    // TOOD: If it does, send the warning message "chat room being deleted" to all connected clients before terminating their connections, closing the master socket, and deleting the entry
    // TODO: Inform the client about the result
}