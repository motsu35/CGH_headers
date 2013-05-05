#ifndef Socket_H_INCLUDED
#define Socket_H_INCLUDED
/************
Coded by Motsu35. have fun and play safe :)
************/
#include <iostream>
#include "WinSock2.h"
using namespace std;

const int STRLEN = 256;

class Socket
{
protected:
    WSADATA wsaData;
    SOCKET mySocket;
    SOCKET myBackup;
    SOCKET acceptSocket;
    sockaddr_in myAddress;
public:
    Socket();
    ~Socket();
    bool SendData( char* );
    bool RecvData( char*, int );
    void CloseConnection();
    void GetAndSendMessage();
    bool kill();
};

class ServerSocket : public Socket
{
public:
    bool Listen();
    bool Bind( int port );
    void StartHosting( int port );
    bool GetLocalIP(void);
    bool GetHostName(void);
};

class ClientSocket : public Socket
{
public:
    bool ConnectToServer( const char *ipAddress, int port );
};



/********************************************************/



///SOCKET

bool Socket::kill()
{
    closesocket(mySocket);

    if (WSACleanup() == SOCKET_ERROR)
    {
        cerr<<"WSACleanup failed with error: "<<WSAGetLastError()<<'\n';
        return false;
    }
    else
    {
        if(1)
        {
            cerr<<"WSACleanup compleated with no errors! \n";
        }
        return true;
    }
}

Socket::Socket()
{
    if( WSAStartup( MAKEWORD(2, 2), &wsaData ) != NO_ERROR )
    {
        cerr<<"Socket Initialization: Error with WSAStartup\n";
        system("pause");
        kill();
        exit(1);
    }

    //Create a socket
    mySocket = socket( AF_INET, SOCK_STREAM, IPPROTO_TCP );

    if ( mySocket == INVALID_SOCKET )
    {
        cerr<<"Socket Initialization: Error creating socket"<<endl;
        system("pause");
        kill();
        exit(1);
    }

    myBackup = mySocket;
}

Socket::~Socket()
{
    kill();
}

bool Socket::SendData( char *buffer )
{
    send( mySocket, buffer, strlen( buffer ), 0 );
    return true;
}

bool Socket::RecvData( char *buffer, int size )
{
    int i = recv( mySocket, buffer, size, 0 );
    buffer[i] = '\0';
    return true;
}

void Socket::CloseConnection()
{
    //cout<<"CLOSE CONNECTION"<<endl;
    closesocket( mySocket );
    mySocket = myBackup;
}

void Socket::GetAndSendMessage()
{
    char message[STRLEN];
    cin.ignore();//without this, it gets the return char from the last cin and ignores the following one!
    cout<<"Send > ";
    cin.get( message, STRLEN );
    SendData( message );
}

///SERVER

void ServerSocket::StartHosting( int port )
{
    Bind( port );
    Listen();
}

bool ServerSocket::Listen()
{
    //cout<<"LISTEN FOR CLIENT..."<<endl;

    if ( listen ( mySocket, 1 ) == SOCKET_ERROR )
    {
        cerr<<"ServerSocket: Error listening on socket\n";
        kill();
        return false;
    }

    //cout<<"ACCEPT CONNECTION..."<<endl;

    acceptSocket = accept( myBackup, NULL, NULL );
    while ( acceptSocket == SOCKET_ERROR )
    {
        acceptSocket = accept( myBackup, NULL, NULL );
    }
    mySocket = acceptSocket;
    return true;
}

bool ServerSocket::Bind( int port )
{
    myAddress.sin_family = AF_INET;
    myAddress.sin_addr.s_addr = inet_addr( "0.0.0.0" );
    myAddress.sin_port = htons( port );

    //cout<<"BIND TO PORT "<<port<<endl;

    if ( bind ( mySocket, (SOCKADDR*) &myAddress, sizeof( myAddress) ) == SOCKET_ERROR )
    {
        cerr<<"ServerSocket: Failed to connect\n";
        kill();
        return false;
    }
    return true;
}

bool ServerSocket::GetHostName(void)
{
    char ac[80];

    struct hostent *phe = gethostbyname(ac);
    if (phe == 0) {
        cerr << "Yow! Bad host lookup." << endl;
        return false;
    }

    for (int i = 0; phe->h_addr_list[i] != 0; ++i) {
        struct in_addr addr;
        memcpy(&addr, phe->h_addr_list[i], sizeof(struct in_addr));
        cout << "Address " << i << ": " << inet_ntoa(addr) << endl;
    }
    return true;

}

bool ServerSocket::GetLocalIP(void)
{
    char ac[80];

    struct hostent *phe = gethostbyname(ac);
    if (phe == 0)
    {
        cerr << "Yow! Bad host lookup." << endl;
        return false;
    }

    for (int i = 0; phe->h_addr_list[i] != 0; ++i)
    {
        struct in_addr addr;
        memcpy(&addr, phe->h_addr_list[i], sizeof(struct in_addr));
        cout << "Address " << i << "- " << inet_ntoa(addr) << endl;
    }
    return true;

}

///CLIENT

bool ClientSocket::ConnectToServer( const char *ipAddress, int port )
{
    myAddress.sin_family = AF_INET;
    myAddress.sin_addr.s_addr = inet_addr( ipAddress );
    myAddress.sin_port = htons( port );

    //cout<<"CONNECTED"<<endl;

    if ( connect( mySocket, (SOCKADDR*) &myAddress, sizeof( myAddress ) ) == SOCKET_ERROR )
    {
        cerr<<"ClientSocket: Failed to connect\n";
        kill();
        return false;
    }
    return true;
}
#endif
