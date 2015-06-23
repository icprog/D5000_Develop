// ---------------------------------------------------------------------------
//  Includes
// ---------------------------------------------------------------------------

//#if   _AIX_
#include "errno.h"
#include "dms_tcp.h"
//#include "Ps01_gdi_log.hxx"//需要写日志的类自己引用
//extern pthread_mutex_t logMutex;

static pthread_key_t rl_key;
static pthread_once_t rl_once = PTHREAD_ONCE_INIT;

static void readline_destructor(void *ptr)
{
        free(ptr);
}

static void readline_once(void)
{
        pthread_key_create(&rl_key, readline_destructor);
}


// ---------------------------------------------------------------------------
//  Local data
//
//  socketFd
//
//  acceptFd
// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//  TCPClass: Constructors and Destructor
// ---------------------------------------------------------------------------
TCPClass::TCPClass()
{
    cout <<"(TCPClass::TCPClass()) Start ... " <<endl;
}

//TCPClass::TCPClass(char *portname,char *hostname)
//{
//    cout <<"(TCPClass::TCPClass(char *portname,char *hostname)) PortName(" <<portname<<") HostName("<<hostname<<") ..."<<endl;
//    bzero(portn,MAX_CHAR);
//    bzero(hostn,MAX_CHAR);
//    strcpy(portn,portname);
//    strcpy(hostn,hostname);
//}

TCPClass::~TCPClass()
{
    cout <<"(TCPClass::~TCPClass) End ... " <<endl;
}

// ---------------------------------------------------------------------------
//  TCPClass: Service/Client Init methods
// ---------------------------------------------------------------------------
int TCPClass::Srv_Init(u_short port)
{
    int errorno;
#if 0
    if ( portname!=NULL )
    {
        if (strcmp(portname,portn)!=0)
        {
            strcpy(portn,portname);
        }
    }
#endif
    srvAddr.sin_family=AF_INET;

    srvAddr.sin_addr.s_addr=htonl(INADDR_ANY);

//    if ( portn!=NULL )
//    {
//        if ( (sp=getservbyname(portn,"tcp"))==NULL )
//        {
//            perror("TCPClass::Srv_init-> unkown service\n");
//            return(-1);
//        }
//    }
#ifdef _LINUX_
    cout<<"Linux system"<<endl;
    srvAddr.sin_port = htons(port);
#else
    cout<<"unix system"<<endl;
    srvAddr.sin_port = port;//sp->s_port;
#endif

#if     __sun

    if ((listenFd=socket(PF_INET,SOCK_STREAM,0))<0)
#else
    if ((listenFd=socket(AF_INET,SOCK_STREAM,0))<0)
#endif
    {
        perror("TCPClass::Srv_init-> can't create a socket");
        return(-1);
    }
#ifdef _DEBUG_
#ifdef _LINUX_
    cout <<"(TCPClass::Srv_Init) ListenFD(" <<listenFd<<") Port("<<ntohs(srvAddr.sin_port)<<") ..."<<endl;
#else
    cout <<"(TCPClass::Srv_Init) ListenFD(" <<listenFd<<") Port("<<srvAddr.sin_port<<") ..."<<endl;
#endif
#endif
    bufsize=131072;

    int opt=1;

#ifdef __sun
    if (0> (errorno=setsockopt(listenFd,SOL_SOCKET,SO_REUSEADDR,&opt,sizeof(opt))))
    {
        perror("TCPClass::Srv_init-> setsockopt err1 !");
    }
#endif

#ifndef _LINUX_
#ifndef __sun
    //if (setsockopt(listenFd,SOL_SOCKET,SO_REUSEPORT,&opt,sizeof(opt))<0)
    if (0>(errorno=setsockopt(listenFd,SOL_SOCKET,SO_REUSEADDR,&opt,sizeof(opt))))
    {
        perror("TCPClass::Srv_init-> setsockopt err !");
    }
#endif
#endif
#ifdef __sun
    if (0> (errorno=setsockopt(listenFd,SOL_SOCKET,SO_SNDBUF,&bufsize,sizeof(bufsize))))
    {
        perror("TCPClass::Srv_init-> setsockopt err2 !");
    }

#else
    if (0> (errorno=setsockopt(listenFd,SOL_SOCKET,SO_SNDBUF|SO_REUSEADDR,&bufsize,sizeof(bufsize))))
    {
        perror("TCPClass::Srv_init-> setsockopt err !");
    }

#endif
#ifdef __sun
    if (0> (errorno=setsockopt(listenFd,SOL_SOCKET,SO_RCVBUF,&bufsize,sizeof(bufsize))))
    {
        perror("TCPClass::Srv_init-> setsockopt err3 !");
    }

#else
    if (0> (errorno=setsockopt(listenFd,SOL_SOCKET,SO_RCVBUF|SO_REUSEADDR,&bufsize,sizeof(bufsize))))
    {
        perror("TCPClass::Srv_init-> setsockopt err !");
    }

#endif

    printf("errorno %d\n", errorno);
    if (0>(errorno=bind(listenFd,(struct sockaddr*)&srvAddr,sizeof(srvAddr))))
    {
        if ( errno==EADDRINUSE )
        {
            cout <<"the listen address is already in use "<< endl;
        }

        perror("TCPClass::Srv_init-> can't bind local address ");

        return(-1);
    }

    listen(listenFd,8);

    return GetListenfd();
}

int TCPClass::Cli_Init(u_short port,const char *server_ip)
{
	assert(server_ip != NULL);
	if(server_ip == NULL) return -1;
    // set port value
    srvAddr.sin_family = AF_INET;

    srvAddr.sin_port = port;
    srvAddr.sin_addr.s_addr = inet_addr(server_ip);

#if 0
    // get address
    if ( hostn!=NULL )
    {
        unsigned long inaddr;

        if ((inaddr=inet_addr(hostn))!=INADDR_NONE)
        {
            bcopy((char*)&inaddr,(char*)&srvAddr.sin_addr,sizeof(inaddr));
        } // the hostn is dotted address
        else
        {
#ifdef _DEBUG_
            cout <<" hostname is " <<endl;
#endif

            if ((hp=gethostbyname(hostn))==NULL)
            {
                perror("TCPClass::Cli_Init-> host name error");
                cout <<" err hostname is " <<hostn <<endl;
                return(-1);
            }

            bcopy((char*)hp->h_addr,(char*)&srvAddr.sin_addr,hp->h_length);
        } // the hostn is a host alias
    }
#endif
    // create socket
    if ( (socketFd=socket(AF_INET,SOCK_STREAM,0))<0 )
    {
        perror("TCPClass::Cli_Init-> can't create a TCP socket");
        close(socketFd);
        return -1;
    }

    if (connect(socketFd, (struct sockaddr *) &srvAddr, sizeof(srvAddr))<0)
    {
        perror("TCPClass::Cli_Init-> can't connet to server");
        close(socketFd);
        return -1;
    }

    return 1;
}

// ---------------------------------------------------------------------------
//  TCPClass: Server  methods
// ---------------------------------------------------------------------------
int TCPClass::Srv_Accept()
{
#ifdef _DEBUG_
    printf("(TCPClass::Srv_Accept) Start ...\n");
#endif
#if    _ALPHA_
    socklen_t len;
#elif  _AIX_
    socklen_t len;
#elif  __sun
    socklen_t len;
#elif  _ITANIUM_
    int32_t len;
#else //xh add 20090108
#ifdef _LINUX_
    socklen_t     len=0;
#else
    int     len=0;
#endif
#endif
    len = sizeof(cliAddr);
    acceptFd = accept(listenFd,(struct sockaddr*)&cliAddr,&len);

    printf("========%s(%d) connected to me!\r\n", inet_ntoa(cliAddr.sin_addr), ntohs(cliAddr.sin_port));

    bzero(buff,100);
    bzero(ipaddress,30);
    strcpy(ipaddress,inet_ntop(AF_INET,&cliAddr.sin_addr,buff,sizeof(buff)));
#ifdef _DEBUG_
    //cout <<" IPADDRESS is " <<ipaddress <<" BUFF is " <<buff<<endl;
#endif
 //   pthread_mutex_lock(&logMutex);
//    glog(E_CONFIG_TYPE_LOGGDI, E_LOG_PRIORITY_TYPE_MESSAGE, 1, "TCP/IP","$IPAddress$ %s $pthread$ %d \n", ipaddress, acceptFd);
//    pthread_mutex_unlock(&logMutex);

    return acceptFd;
}

int TCPClass::Srv_Accept(int listfd)
{
#if    _ALPHA_
    socklen_t len;
#elif  _AIX_
    socklen_t len;
#elif  __sun
    socklen_t len;
#elif  _ITANIUM_
    int32_t len;
#else //xh add 20090109
#ifdef _LINUX_
    socklen_t     len=0;
#else
    int     len=0;
#endif
#endif
    len = sizeof(cliAddr);
    acceptFd = accept(listfd,(struct sockaddr*)&cliAddr,&len);
    printf("========%s(%d) connected to me!\r\n", inet_ntoa(cliAddr.sin_addr), ntohs(cliAddr.sin_port));
    return GetAcceptfd();
}

// ---------------------------------------------------------------------------
//  TCPClass: Getter methods
// ---------------------------------------------------------------------------
int TCPClass::Recv(int fd,char *buff,int maxlen)
{
    int n;

again:
    n=readn(fd,buff,maxlen);

    if (n<0)
    {
        if (errno==EINTR)
        {
            errno=0;
            goto again;
        }

        return(n);
    }

    return (n);
}

int TCPClass::readn(int fd,char *buff,int maxlen)
{
    int nleft,nread;
    nleft=maxlen;

    while (nleft>0)
    {
        nread=read(fd,buff,nleft);

        if ( nread < 0 )
        {
            printf("recv err %d %d \n",errno,fd);

            if (errno==EINTR)
                continue;

            return(-1);
        }

        if ( nread == 0 )
        {
            if (errno==EINTR)
                continue;

            printf("recv no ------------   -----------    -----------   char %d %d\n",errno,fd);

            close(fd);

            return(0);
        }

        nleft-=nread;

        buff+=nread;
    }

    return (maxlen-nleft);
}

/* 慢速
int TCPClass::ReadLine(int fd,char *vptr,int maxlen)
{
        int n,rc;
        char  c,*ptr;
        ptr=(char *)vptr;
        for(n=1;n<maxlen;n++)
        {
                again:
                if((rc=read(fd,(void *)&c,1))==1)
                {
                        *ptr++=c;
                        if(c=='\n')
                                break;
                }
                else if(rc==0)
                {
                        if(n==1)
                                return(0);
                        else
                                break;
                }
                else
                {
                        if(errno==EINTR)
                                goto again;
                        perror("readline fail");
                        return(-1);
                }
        }
        ptr--;
        ptr--;
        if((*ptr)!='\r')
                ptr++;
        *ptr='\0';
        return(n);
}
*/
/*Change by Sundi 20090629 for Lost Message*/
int TCPClass::ReadLine(int fd,char *buff,int maxlen)
{
    int n,rc;
    char c,*ptr;
    RLINE *tsd;
    pthread_once(&rl_once,readline_once);

    if ( (tsd = (RLINE*)pthread_getspecific(rl_key)) == NULL)
    {
        tsd = (RLINE*)calloc(1,sizeof(RLINE));/* init 0*/
        pthread_setspecific(rl_key, tsd);
    }

    ptr=buff;

    for (n=1;n<maxlen;n++)
    {
        rc=my_read(tsd, fd, &c);

        if ( rc == 1 )
        {
            *ptr++=c;

            if ( c=='\n' )
            {
                break;
            }
        }
        else if ( rc==0 )
        {
            if ( n==1 )
            {
                return 0;
            }
            else
                break;
        }
        else
        {
            return -1;
        }
    }

    *ptr='\0';

    return n;
}

/*Change by Sundi 20090629 for Lost Message*/
int TCPClass::my_read(RLINE *tsd, int fd, char *ptr)
{
    if ( tsd->rl_cnt<=0 )
    {

again:
        tsd->rl_cnt = read(fd, tsd->rl_buf, MAX_BUFLEN);

        if ( tsd->rl_cnt<0 )
        {
            if ( errno==EINTR )
            {
                goto again;
            }

            return (-1);
        }
        else if ( tsd->rl_cnt == 0 )
        {
            return 0;
        }

        tsd->rl_bufptr = tsd->rl_buf;
    }

    tsd->rl_cnt--;

    *ptr = * tsd->rl_bufptr++;
    return 1;
}

int TCPClass::Send(int fd,const char *buff,int maxlen)
{
    int nleft,nwritten;
    nleft=maxlen;

    while (nleft>0)
    {
        nwritten=write(fd,buff,nleft);

        if (nwritten<=0)
        {
            return(nwritten);
        }

        nleft-=nwritten;

        buff+=nwritten;
    }

    return(maxlen-nleft);
}

int TCPClass::GetListenfd()
{
    return listenFd;
}

int TCPClass::GetAcceptfd()
{
    return acceptFd;
}

int TCPClass::GetSocketfd()
{
    return socketFd;
}

/* 线程安全+print
int TCPClass::ReadLine(int fd,char *buff,int maxlen)
{
        int n,rc;
        char c,*ptr;
        RLINE *tsd;
        pthread_once(&rl_once,readline_once);
        if( (tsd = (RLINE*)pthread_getspecific(rl_key)) == NULL)
        {
                tsd = (RLINE*)calloc(1,sizeof(RLINE));
                pthread_setspecific(rl_key, tsd);
        }
        ptr=buff;
                cout<<"maxlen="<<maxlen<<"fd"<<fd<<endl;
        for (n=1;n<maxlen;n++)
        {
                        rc=my_read(tsd, fd, &c);
                if ( rc == 1 )
                {
                        *ptr++=c;
                        if ( c=='\n' )
                        {
                                break;
                        }
                }
                else if ( rc==0 )
                {
                        if ( n==1 )
                        {
                                printf("rc = 0,n = 1 , fd = %d\n",fd);
                                return 0;
                        }
                        else
                                break;
                }
                else
                {
                        printf("re = -1,fd = %d\n",fd);
                        return -1;
                }
        }
        cout<<"n="<<n<<"fd"<<fd<<endl;
        //*ptr=0;
        *ptr='\0';
        return n;
}
int TCPClass::my_read(RLINE *tsd, int fd, char *ptr)
{
        static int read_cnt = 0;
        static char *read_ptr;
        static char read_buf[MAX_BUFLEN];
        if ( tsd->rl_cnt<=0 )
        {
again:
                                printf("ready to read fd = %d buff = %s leng = %d\n", fd, tsd->rl_buf, sizeof(tsd->rl_buf));
                                tsd->rl_cnt = read(fd, tsd->rl_buf, MAX_BUFLEN);
                                printf("end read fd = %d\n",fd);
                if ( tsd->rl_cnt<0 )
                {
                        printf("errno = %d,fd = %d\n",errno,fd);
                    if ( errno==EINTR )
                                        {
                                                cout<<"tcp read error errno=EINTR"<<endl;
                        goto again;
                                        }
                    return (-1);
                }
                else if ( tsd->rl_cnt == 0 )
                        {
                                printf("read_cnt==0 fd = %d\n",fd);
                        return 0;
                    }
                tsd->rl_bufptr = tsd->rl_buf;
                printf("fd = %d, buf = %s\n", fd, tsd->rl_bufptr);
        }
        tsd->rl_cnt--;
        *ptr = * tsd->rl_bufptr++;
        return 1;
}*/
/* 非线程安全
int TCPClass::ReadLine(int fd,char *buff,int maxlen)
{
        int n,rc;
        char c,*ptr;
        ptr=buff;
        cout<<"maxlen="<<maxlen<<"fd"<<fd<<endl;
        for (n=1;n<maxlen;n++)
        {
                if ( (rc=my_read(fd,&c))==1 )
                {
                        *ptr++=c;
                        if ( c=='\n' )
                        {
                                break;
                        }
                }
                else if ( rc==0 )
                {
                        if ( n==1 )
                        {
                                printf("rc = 0,n = 1 , fd = %d\n",fd);
                                return 0;
                        }
                        else
                                break;
                }
                else
                {
                        printf("re = -1,fd = %d\n",fd);
                        return -1;
                }
        }
        cout<<"n="<<n<<"fd"<<fd<<endl;
        //*ptr=0;
        *ptr='\0';
        return n;
}
int TCPClass::my_read(int fd,char *ptr)
{
        static int read_cnt = 0;
        static char *read_ptr;
        static char read_buf[MAX_BUFLEN];
        //bzero(read_buf,MAX_BUFLEN);
        //printf("read_cnt = %d fd = %d\n",read_cnt,fd);
        if ( read_cnt<=0 )
        {
again:
                                printf("ready to read fd = %d buff = %s leng = %d\n",fd,read_buf,sizeof(read_buf));
                                read_cnt=read(fd,read_buf,sizeof(read_buf));
                                printf("end read fd = %d\n",fd);
                if ( read_cnt<0 )
                {
                        printf("errno = %d,fd = %d\n",errno,fd);
                        if ( errno==EINTR )
                        {
                                cout<<"tcp read error errno=EINTR"<<endl;
                                goto again;
                        }
                        return (-1);
                }
                else if ( read_cnt==0 )
                        {
                                printf("read_cnt==0 fd = %d\n",fd);
                        return 0;
                    }
                read_ptr = read_buf;
                printf("ptr = %s, buf = %s\n",read_ptr,read_buf);
        }
        read_cnt--;
        *ptr = *read_ptr++;
        //printf("read_cnt>0 fd = %d\n",fd);
        return 1;
}
*/
