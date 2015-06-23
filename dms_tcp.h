// ---------------------------------------------------------------------------
//  Includes for all the program files to see
// ---------------------------------------------------------------------------
#include <strings.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <pthread.h>
#include <iostream>
#include <assert.h>

using namespace std;

#define MAX_BUFLEN                      1024

#define SERVER_PORT_STUDY 8910
#define SERVER_PORT 8912

#define MAX_THREAD                      128
#define GDI_TRUE                        1
#define GDI_FALSE                       0

typedef struct Rline
{
        int 	rl_cnt;					/*initialize to 0*/
        char 	*rl_bufptr;				/*initialize to rl_buf*/
        char	rl_buf[MAX_BUFLEN];
}RLINE;

typedef struct  gdi_thread
{
        pthread_t   thread_id;
        //pthread_startroutine_t    handler;
        //pthread_addr_t      argv;
        //char        thread_name[23];
        //char        creat_flag;
        int fdinThread;
}GDI_THREAD;

// ---------------------------------------------------------------------------
//  This is a simple class that lets us do easy (though not terribly efficient)
//  trancoding of XMLCh data to local code page for display.
// ---------------------------------------------------------------------------
class TCPClass
{
public :
        // -----------------------------------------------------------------------
        //  Constructors and Destructor
        // -----------------------------------------------------------------------
        TCPClass();
//        TCPClass(char *portname,char *hostname);
        ~TCPClass();

        // -----------------------------------------------------------------------
        //  Getter methods
        // -----------------------------------------------------------------------
        int 	Srv_Init(u_short port);
        int 	Cli_Init(u_short port,const char *server_ip);
        int	Srv_Accept();
        int	Srv_Accept(int listfd);
        int	GetListenfd();
        int	GetAcceptfd();
        int 	GetSocketfd();

        // -----------------------------------------------------------------------
        //  Read and Write methods
        // -----------------------------------------------------------------------
        int	Recv(int fd,char *buff,int maxlen);
        int	readn(int fd,char *buff,int maxlen);
        int	ReadLine(int fd,char *buff,int maxlen);
        int	my_read(RLINE *tsd, int fd, char *ptr);
        int 	Send(int fd,const char *buff,int maxlen);


private :
        // -----------------------------------------------------------------------
        //  Private data members
        //
        //  xxxFaaa
        //      This is the local code page form of the string.
        // -----------------------------------------------------------------------
        int     socketFd;
        int     listenFd;
        int     acceptFd;
        int     bufsize;
//        char    portn[MAX_CHAR];
//        char    hostn[MAX_CHAR];
        char    buff[100];
        char    ipaddress[30];
//        struct	servent *sp;
//        struct	hostent *hp;
        struct  sockaddr_in     srvAddr;
        struct  sockaddr_in     cliAddr;
};



