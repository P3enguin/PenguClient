# include <stdio.h>
# include <stdlib.h>
# include <vector>
# include <sys/ioctl.h>
# include <sys/poll.h>
# include <sys/socket.h>
# include <sys/time.h>
# include <netinet/in.h>
# include <errno.h>
# include <iostream>
# include <unistd.h>
# include <cstring>
# include <fcntl.h>
# include <arpa/inet.h>
# include <sstream>
#include <sys/types.h>
#include <sys/stat.h>

int flag = 1;

// Sending the 
void	sendMessage(int fd,const std::string &msg)
{
	size_t total = 0;
	while (total != msg.length())
	{
		ssize_t nb = send(fd,msg.c_str() + total,msg.length() - total, 0);
		if (nb == -1)
        {
			std::cout << "sending error" << std::endl;
            return ;
        }
		total += nb;
	}
}

// thread function for Recieving answers from the server 
void    *reciving(void *arg)
{
    int rc;
    int *sock = (int *)arg; 
    char buff[1024];
    while (flag)
    {
        memset(buff,0,1024);
        rc = recv(*sock,buff,sizeof(buff),0);
        if (rc == 0)
        {
            std::cout << "Server closed " << std::endl;
            close(*sock);
            flag = 0;
            exit(0) ;
        }
        std::cout << buff ;
    }
    return NULL;
}

int main(int argc,char **argv)
{
    if (argc != 3)
    {
        std::cout << "Usage : ./PenguClient 'IP' 'PORT' " << std::endl;
        return 1;
    }

    int sock;
    uint16_t port;

    std::stringstream ss;
    struct sockaddr_in addr;

    std::string msg;

    ss << argv[2];
    ss >> port;
    if ((sock = socket(AF_INET,SOCK_STREAM,0)) < 0)
    {
        std::cout << " Error while creating sockets " << std::endl;
        return 1;
    }
    
    memset(&addr,0,sizeof(addr));
    addr.sin_family = AF_INET;
    if (inet_pton(AF_INET, argv[1],&(addr.sin_addr)) < 0)
    {
        std::cout << "Server are not availabe" << std::endl;
        return 1;
    }
    addr.sin_port = htons(port);
    if ((connect(sock,(struct sockaddr *)&addr,sizeof(addr))) < 0)
    {
        std::cout << "can not connect to server ..." << std::endl;
        return 1;
    }

    pthread_t id;
    pthread_create(&id,NULL,reciving,&sock);
    while (flag)
    {
        std::getline(std::cin,msg);
        msg += "\r\n";
        sendMessage(sock,msg);
        msg.clear();
    }
    close (sock);
}