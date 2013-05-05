#ifndef _CGH_NET_IRC_H
#define _CGH_NET_IRC_H

#include <winsock2.h>
#include <iostream>
#include <string>
#include <sstream>
#include <fstream>

using namespace std;

//define color values
string IRC_COLOR = "";
string IRC_WHITE = "0";
string IRC_BLACK = "1";
string IRC_D_BLUE = "2";
string IRC_D_GREEN = "3";
string IRC_RED = "4";
string IRC_D_RED = "5";
string IRC_D_PURPLE = "6";
string IRC_ORANGE = "7";
string IRC_YELLOW = "8";
string IRC_GREEN = "9";
string IRC_D_CYAN = "10";
string IRC_CYAN = "11";
string IRC_BLUE = "12";
string IRC_PINK = "13";
string IRC_GREY = "14";
string IRC_SILVER = "15";

//define return values
#define IRC_CONNECT_SUCESS 0
#define IRC_CONNECT_ERROR_HOST 1
#define IRC_CONNECT_ERROR_RESOLVE 2

#define IRC_RECIEVE_ERROR -1

#define IRC_PING 2
#define IRC_PRIVMSG 3
#define IRC_NOTICE 4
#define IRC_QUIT 5
#define IRC_JOIN 6
#define IRC_PART 7
#define IRC_KICK 8
#define IRC_NICK 9
#define IRC_MODE 10
#define IRC_DCCFILE 11
#define IRC_NICK_INUSE 12
#define IRC_UNRECOGNIZED_MESSAGE 1

#define IRC_DCC_SEND_SUCESS 0
#define IRC_DCC_SEND_ERROR_CREATE_SOCKET 1
#define IRC_DCC_SEND_ERROR_BIND_SOCKET 2
#define IRC_DCC_SEND_ERROR_OPEN_SOCKET 3
#define IRC_DCC_SEND_ERROR_OPEN_FILE 4
#define IRC_DCC_SEND_ERROR_USER 5
#define IRC_DCC_SEND_ERROR_ACCEPT 6
#define IRC_DCC_SEND_ERROR_SEND 7

#define IRC_DCC_RECV_SUCCESS 0
#define IRC_DCC_RECV_ERROR_DISK 1
#define IRC_DCC_RECV_ERROR_WRITE 2
#define IRC_DCC_RECV_ERROR_OPEN_SOCKET 3
#define IRC_DCC_RECV_ERROR_SOCKET 4


class IRC
{
public:
    //constructor / destructor
    IRC();
    ~IRC();
    //functions
    IRC* getThis(){return this;};
    int Connect(const string Server,int Port);
    void Register(string Nick);
    void ChangeNick(string Nick);
    void Join(string Channel);
    void Part(string Channel);
    string RecvData();
    int ParseData(string Data);
    void PrivMsg(string Message,string Channel);
    void Notice(string Message,string Channel);
    void Mode(string Channel, string Modes, string Users);
    void Mode(string Channel, string Modes);
    int DCCSendFile(string File,string User,int Port);
    int DCCRecvFile(string File,string Host,int Port);
    void SendRaw(string Command);
    void Quit(string Reason);
    void Quit();
    void End(); // quit + wsa cleanup!
    int GetLastError();
    //parsed data structure definitions
    struct generic_irc_message
    {
        string User;
        string Message;
        string Channel;
        string Address;
    };
    struct dccfile
    {
        string User;
        string FileName;
        string Ip;
        int Port;
        int FileSize;
        string Address;
    };
    struct quit
    {
        string User;
        string Address;
    };
    struct generic_irc_action
    {
        string User;
        string Channel;
        string Address;
    };
    struct kick
    {
        string UserKicked;
        string User;
        string Channel;
        string Reason;
        string Address;
    };
    struct nick
    {
        string Old;
        string New;
        string Address;
    };
    struct mode
    {
        string User;
        string Channel;
        string Modes;
        string Params;
        string Address;
    };
    //structure declarations
    generic_irc_message s_privmsg;
    generic_irc_message s_notice;
    dccfile s_dccfile;
    quit s_quit;
    generic_irc_action s_join;
    generic_irc_action s_part;
    kick s_kick;
    nick s_nick;
    mode s_mode;
private:
    //winsock data
    SOCKET sock;
    WSADATA wsaData;
    sockaddr_in address;
    //other data
    string IRCHost;
    string IRCNick;
    string LocalIp;
    int LastError;
    //other functions
    string GetLocalIp();
    SOCKET CreateSock(string host, unsigned short port);
};






//constructor/destructor functions
IRC::IRC()
{
    if( WSAStartup( MAKEWORD(2, 2), &wsaData ) != NO_ERROR )
    {
        WSACleanup();
    }
    sock = socket( AF_INET, SOCK_STREAM, IPPROTO_TCP );
    if(sock==INVALID_SOCKET)
    {
        WSACleanup();
    }
    LocalIp = GetLocalIp();
}

IRC::~IRC()
{
    WSACleanup();
}

//irc functions
int IRC::Connect(const string Host,int Port)
{
    LPHOSTENT hostEntry;
    if (!(hostEntry = gethostbyname(Host.c_str())))
    {
        return IRC_CONNECT_ERROR_HOST;
    }
    address.sin_family = AF_INET;
    address.sin_addr = *((LPIN_ADDR)*hostEntry->h_addr_list);
    address.sin_port = htons( Port );

    if ( connect( sock, (SOCKADDR*) &address, sizeof( address ) ) == SOCKET_ERROR )
    {
        return IRC_CONNECT_ERROR_RESOLVE;
    }
    IRCHost = Host;
    return IRC_CONNECT_SUCESS;
}

void IRC::Register(string Nick)
{
    string temp;
    temp = "USER " + Nick + " 0 0 " + Nick + "\r\n";
    send(sock,temp.c_str(),temp.length(),0);
    temp = "NICK " + Nick + "\r\n";
    send(sock,temp.c_str(),temp.length(),0);
    IRCNick = Nick;
    return;
}

void IRC::ChangeNick(string Nick)
{
    string temp = "NICK " + Nick + "\r\n";
    send(sock,temp.c_str(),temp.length(),0);
    IRCNick = Nick;
    return;
}

void IRC::Join(string Channel)
{
    string temp = "JOIN " + Channel + "\r\n";
    send(sock,temp.c_str(),temp.length(),0);
    return;
}

void IRC::Part(string Channel)
{
    string temp = "PART " + Channel + "\r\n";
    send(sock,temp.c_str(),temp.length(),0);
    return;
}

string IRC::RecvData()
{
    char buf[4096];
    string temp;
    int i = recv(sock,buf,sizeof(buf),0);
    buf[i]='\0';
    if ((i==-1)||(i==0))
    {
        LastError = IRC_RECIEVE_ERROR;
        return "";
    }
    temp.assign(buf, strlen(buf));
    return temp;
}

int IRC::ParseData(string Data)
{
    string command, str1, str2, str3, temp, substring;
    string::size_type prev_pos = 0, pos = 0, temp_pos = 0, temp_prev_pos = 0;
    int cut=1;

    //determine if the data is followed by \r\n, \n or none, and set the appropriate cut value
    if (Data[Data.length()-1]=='\n')
    {
        cut++;
    }
    if (Data[Data.length()-2]=='\r')
    {
        cut++;
    }

    //extract sub-strings
    pos = Data.find(' ', pos);
    substring = Data.substr(prev_pos, pos-prev_pos);
    str1 = substring;
    prev_pos = ++pos;

    pos = Data.find(' ', pos);
    substring = Data.substr(prev_pos, pos-prev_pos);
    str2 = substring;
    prev_pos = ++pos;

    pos = Data.find(' ', pos);
    substring = Data.substr(prev_pos, pos-prev_pos);
    str3 = substring;
    prev_pos = ++pos;

    //check for ping
    if(str1=="PING")
    {
        temp = "PONG " + str2 + "\r\n";
        send(sock,temp.c_str(),temp.length(),0);
        return IRC_PING; //replyed to ping
    }

    //check for other messages
    if (str2=="PRIVMSG")   //DCC SEND " + File + " " + htonl(inet_addr(LocalIp.c_str())) + " " + Port + " " + length + "
    {
        temp = Data.substr(Data.find(":",1)+1,Data.length()-Data.find(":",1)-cut);

        if (temp.substr(0,9)=="DCC SEND")
        {
            temp_pos = temp.find(' ', temp_pos);
            substring = temp.substr(temp_prev_pos, temp_pos-temp_prev_pos);
            temp_prev_pos = ++temp_pos;

            temp_pos = temp.find(' ', temp_pos);
            substring = temp.substr(temp_prev_pos, temp_pos-temp_prev_pos);
            temp_prev_pos = ++temp_pos;

            temp_pos = temp.find(' ', temp_pos);
            substring = temp.substr(temp_prev_pos, temp_pos-temp_prev_pos);
            temp_prev_pos = ++temp_pos;

            s_dccfile.FileName = substring;

            temp_pos = temp.find(' ', temp_pos);
            substring = temp.substr(temp_prev_pos, temp_pos-temp_prev_pos);
            temp_prev_pos = ++temp_pos;

            s_dccfile.Ip = substring;

            temp_pos = temp.find(' ', temp_pos);
            substring = temp.substr(temp_prev_pos, temp_pos-temp_prev_pos);
            temp_prev_pos = ++temp_pos;

            s_dccfile.Port = atoi(substring.c_str());

            temp_pos = temp.find(' ', temp_pos);
            substring = temp.substr(temp_prev_pos, temp_pos-temp_prev_pos);
            temp_prev_pos = ++temp_pos;

            s_dccfile.FileSize = atoi(substring.c_str());

            s_dccfile.User = str1.substr(1,str1.find("!")-1);
            s_dccfile.Address = str1.substr(str1.find("!")+1,str1.length()-(str1.find("!")+1));

            return IRC_DCCFILE;
        }
        else
        {
            s_privmsg.User = str1.substr(1,str1.find("!")-1);
            s_privmsg.Message = temp;
            s_privmsg.Channel = str3;
            s_privmsg.Address = str1.substr(str1.find("!")+1,str1.length()-(str1.find("!")+1));
            return IRC_PRIVMSG;
        }
    }
    else if (str2=="NOTICE")
    {
        s_notice.User = str1.substr(1,str1.find("!")-1);
        s_notice.Message = Data.substr(Data.find(":",1)+1,Data.length()-Data.find(":",1)-cut);
        s_notice.Channel = str3;
        s_notice.Address = str1.substr(str1.find("!")+1,str1.length()-(str1.find("!")+1));
        return IRC_NOTICE;
    }
    else if (str2=="QUIT")
    {
        s_quit.User = str1.substr(1,str1.find("!")-1);
        s_quit.Address = str1.substr(str1.find("!")+1,str1.length()-(str1.find("!")+1));
        return IRC_QUIT;
    }
    else if (str2=="JOIN")
    {
        s_join.User = str1.substr(1,str1.find("!")-1);
        s_join.Address = str1.substr(str1.find("!")+1,str1.length()-(str1.find("!")+1));
        s_join.Channel = str3.substr(1,str3.length()-cut);
        return IRC_JOIN;
    }
    else if (str2=="PART")
    {
        s_part.User = str1.substr(1,str1.find("!")-1);
        s_part.Address = str1.substr(str1.find("!")+1,str1.length()-(str1.find("!")+1));
        s_part.Channel = str3.substr(0,str3.length()-(cut-1));
        return IRC_PART;
    }
    else if (str2=="KICK")
    {
        //in progress
        s_kick.User = str1.substr(1,str1.find("!")-1);
        s_kick.Channel = str3;
        s_kick.Address = str1.substr(str1.find("!")+1,str1.length()-(str1.find("!")+1));
        //get user who was kicked
        pos = Data.find(' ', pos);
        substring = Data.substr(prev_pos, pos-prev_pos);
        prev_pos = ++pos;
        s_kick.UserKicked = substring;
        s_kick.Reason = Data.substr(pos+1,Data.length()-pos-cut);
        return IRC_KICK;
    }
    else if (str2=="NICK")
    {
        s_nick.Old = str1.substr(1,str1.find("!")-1);
        s_nick.New = Data.substr(Data.find(":",1)+1,Data.length()-Data.find(":",1)-3);
        s_nick.Address = str1.substr(str1.find("!")+1,str1.length()-(str1.find("!")+1));
        return IRC_NICK;
    }
    else if (str2=="MODE")
    {
        s_mode.User = str1.substr(1,str1.find("!")-1);
        s_mode.Channel = str3;
        //get modes
        pos = Data.find(' ', pos);
        substring = Data.substr(prev_pos, pos-prev_pos);
        s_mode.Modes = substring;
        prev_pos = ++pos;
        //get params
        s_mode.Params = Data.substr(pos,Data.length()-pos-cut);
        s_mode.Address = str1.substr(str1.find("!")+1,str1.length()-(str1.find("!")+1));
        return IRC_MODE;
    }
    else if (str2=="433")
    {
        temp = Data.substr(Data.find(":",1)+1,27);
        if (temp=="Nickname is already in use.")
        {
            return IRC_NICK_INUSE;
        }
        else
        {
            return IRC_UNRECOGNIZED_MESSAGE;
        }
    }
    else
    {
        return IRC_UNRECOGNIZED_MESSAGE;
    }
}

void IRC::PrivMsg(string Message,string Channel)
{
    string temp = "PRIVMSG " + Channel + " :" + Message + "\r\n";
    send(sock,temp.c_str(),temp.length(),0);
    return;
}

void IRC::Notice(string Message,string Channel)
{
    string temp = "NOTICE " + Channel + " :" + Message + "\r\n";
    send(sock,temp.c_str(),temp.length(),0);
    return;
}

void IRC::Mode(string Channel, string Modes, string Users)
{
    string temp = "MODE " + Channel + " " + Modes + " " + Users + "\r\n";
    send(sock,temp.c_str(),temp.length(),0);
    return;
}

void IRC::Mode(string Channel, string Modes)
{
    string temp = "MODE " + Channel + " " + Modes + "\r\n";
    send(sock,temp.c_str(),temp.length(),0);
    return;
}

int IRC::DCCSendFile(string File,string User,int Port)
{
    cout << "Attempting to send " << File << " to " << User << " on port " << Port << endl;

    string temp;
    char buffer[1024];
    SOCKET ssock, csock;
    HANDLE testfile;

    ssock = socket( AF_INET, SOCK_STREAM, IPPROTO_TCP );
    if ( ssock == INVALID_SOCKET )
    {
        return IRC_DCC_SEND_ERROR_CREATE_SOCKET;
    }
    sockaddr_in saddress;
    saddress.sin_family = AF_INET;
    saddress.sin_addr.s_addr = INADDR_ANY;
    saddress.sin_port = htons( Port );
    if ( bind ( ssock, (SOCKADDR*) &saddress, sizeof( saddress) ) == SOCKET_ERROR )
    {
        closesocket(ssock);
        return IRC_DCC_SEND_ERROR_BIND_SOCKET;
    }
    if ( listen ( ssock, 1 ) == SOCKET_ERROR )
    {
        closesocket(ssock);
        return IRC_DCC_SEND_ERROR_OPEN_SOCKET;
    }

    testfile = CreateFile(File.c_str(),GENERIC_READ,FILE_SHARE_READ,0,OPEN_EXISTING,0,0);
    if (testfile == INVALID_HANDLE_VALUE)
    {
        closesocket(ssock);
        return IRC_DCC_SEND_ERROR_OPEN_FILE;
    }

    int length = GetFileSize(testfile,NULL);

    //temp = "PRIVMSG " + User + " :DCC SEND " + File + " " + htonl(inet_addr(LocalIp.c_str())) + " " + Port + " " + length + "\r\n";

    string sLength;
    stringstream out;
    out << length;
    sLength = out.str();

    string sPort;
    stringstream outPort;
    outPort << Port;
    sPort = outPort.str();

    string sIp;
    stringstream outIp;
    outIp << htonl(inet_addr(LocalIp.c_str()));
    sIp = outIp.str();

    temp = "PRIVMSG " + User + " :DCC SEND " + File + " ";
    temp += sIp;
    temp += " " + sPort + " " + sLength + "\r\n";

    cout << temp << endl;

    send(sock,temp.c_str(),temp.length(),0);

    TIMEVAL timeout;
    timeout.tv_sec = 60;
    timeout.tv_usec = 0;
    fd_set fd_struct;
    FD_ZERO(&fd_struct);
    FD_SET(ssock, &fd_struct);

    if (select(0, &fd_struct, NULL, NULL, &timeout) <= 0)
    {
        closesocket(ssock);
        closesocket(csock);
        CloseHandle(testfile);
        return IRC_DCC_SEND_ERROR_USER;
    }

    csock = accept( ssock, NULL, NULL );
    if ( (csock == INVALID_SOCKET ) || (csock == SOCKET_ERROR) )
    {
        closesocket(ssock);
        closesocket(csock);
        return IRC_DCC_SEND_ERROR_ACCEPT;
    }

    closesocket(ssock);

    int Fsend, bytes_sent;
    unsigned int move;
    unsigned __int64 totalbytes = 0;
    DWORD mode = 0;

    while (length)
    {
        Fsend = 1024;
        if (Fsend>length)
            Fsend=length;
        move = 0-length;

        memset(buffer,0,sizeof(buffer));
        SetFilePointer(testfile, move, NULL, FILE_END);
        ReadFile(testfile, buffer, Fsend, &mode, NULL);

        bytes_sent = send(csock, buffer, Fsend, 0);
        totalbytes += bytes_sent;

        if (recv(csock,buffer ,sizeof(buffer), 0) < 1 || bytes_sent < 1)
        {
            closesocket(csock);
            return IRC_DCC_SEND_ERROR_SEND;
        }
        length = length - bytes_sent;
    }

    if (testfile != INVALID_HANDLE_VALUE)
        CloseHandle(testfile);

    if (ssock > 0)
        closesocket(ssock);
    closesocket(csock);

    return IRC_DCC_SEND_SUCESS;
}

int IRC::DCCRecvFile(string File,string Host,int Port)
{
    char buffer[4096];
    string tmpfile;
    char tmpBuff[MAX_PATH];

    int received = 0;
    unsigned long received2;

    FILE *infile;

    SOCKET ssock;

    GetSystemDirectory(tmpBuff, sizeof(tmpBuff));
    tmpfile.assign(tmpBuff,strlen(tmpBuff));
    tmpfile = tmpfile + "\\" + File;

    while (1)
    {
        //HANDLE testfile = CreateFile(tmpfile.c_str(),GENERIC_WRITE,FILE_SHARE_READ,0,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,0);
        HANDLE testfile = CreateFile(File.c_str(),GENERIC_WRITE,FILE_SHARE_READ,0,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,0);
        if (testfile == INVALID_HANDLE_VALUE)
        {
            //Error unable to write file to disk.
            return IRC_DCC_RECV_ERROR_DISK;
        }
        CloseHandle(testfile);

        //if ((infile = fopen(tmpfile.c_str(),"a+b")) == NULL) {
        if ((infile = fopen(File.c_str(),"a+b")) == NULL)
        {
            //Error opening file for writing.
            return IRC_DCC_RECV_ERROR_WRITE;
        }

        if ((ssock = CreateSock(Host.c_str(),Port)) == INVALID_SOCKET)
        {
            //Error opening socket.
            return IRC_DCC_RECV_ERROR_OPEN_SOCKET;
        }
        DWORD err = 1;
        while (err != 0)
        {
            memset(buffer,0,sizeof(buffer));

            err = recv(ssock, buffer, sizeof(buffer), 0);
            if (err == 0)
                break;
            if (err == SOCKET_ERROR)
            {
                //Socket error.

                fclose(infile);
                closesocket(ssock);

                return IRC_DCC_RECV_ERROR_SOCKET;
            }

            fwrite(buffer,1,err,infile);
            received = received + err;
            received2 = htonl(received);
            send(ssock,(char *)&received2 , 4, 0);
        }
        break;
    }

    if (infile != NULL)
        fclose(infile);
    if (ssock > 0)
        closesocket(ssock);

    return IRC_DCC_RECV_SUCCESS;
}

void IRC::SendRaw(string Command)
{
    send(sock,Command.c_str(),Command.length(),0);
    return;
}

void IRC::Quit(string Reason)
{
    string temp = "QUIT " + IRCHost + " :" + Reason + "\r\n";
    send(sock,temp.c_str(),temp.length(),0);
    closesocket(sock);
    return;
}

void IRC::Quit()
{
    send(sock,"QUIT\r\n",7,0);
    closesocket(sock);
    return;
}

void IRC::End()
{
    Quit();
    WSACleanup();
    return;
}

SOCKET IRC::CreateSock(string host, unsigned short port)
{
    SOCKET ssock;
    if ((ssock = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET)
        return INVALID_SOCKET;

    SOCKADDR_IN ssin;
    memset(&ssin, 0, sizeof(ssin));
    ssin.sin_family = AF_INET;
    ssin.sin_port = htons(port);

    IN_ADDR in;
    in.s_addr = inet_addr(host.c_str());
    LPHOSTENT Hostent = NULL;
    if (in.s_addr == INADDR_NONE)
        Hostent = gethostbyname(host.c_str());
    if (Hostent == NULL && in.s_addr == INADDR_NONE)
        return INVALID_SOCKET;
    ssin.sin_addr = ((Hostent != NULL)?(*((LPIN_ADDR)*Hostent->h_addr_list)):(in));

    if (connect(ssock, (LPSOCKADDR) &ssin, sizeof(ssin)) == SOCKET_ERROR)
    {
        closesocket(ssock);
        return INVALID_SOCKET;
    }

    return (ssock);
}

int IRC::GetLastError()
{
    int i;
    i = LastError;
    LastError = 0;
    return i;
}

//other functions
string IRC::GetLocalIp()
{
    string ip;
    char ac[80];
    if (gethostname(ac, sizeof(ac)) == SOCKET_ERROR)
    {
        return "0.0.0.0";
    }

    struct hostent *phe = gethostbyname(ac);
    if (phe == 0)
    {
        return "0.0.0.0";
    }

    struct in_addr addr;
    memcpy(&addr, phe->h_addr_list[0], sizeof(struct in_addr));
    ip.assign(inet_ntoa(addr),strlen(inet_ntoa(addr)));

    return ip;
}

#endif
