#ifndef SOCKET_H
#define SOCKET_H

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <string>
#include <arpa/inet.h>

const int MAXHOSTNAME = 200;
const int MAXCONNECTIONS = 5;
const int MAXRECV = 500;

class Socket
{
    public:
        Socket();
        ~Socket();

        // Server initialization
        bool do_create();
        bool do_bind(const int port);
        bool do_listen();
        bool do_accept(Socket&);

        // Client initialization
        bool do_connect(const std::string host, const int port);

        // Data Transimission
        bool do_send(const std::string);
        int do_receive(std::string&);

        bool is_open() { return m_sock != -1; }
        int sock() { return m_sock; };

    private:
        int m_sock;
        sockaddr_in m_addr;
};

#endif

