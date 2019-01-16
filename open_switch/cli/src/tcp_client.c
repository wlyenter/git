/*
 * =====================================================================================
 *
 *       Filename:  tcp_client.c
 *
 *    Description:  tcp客户端代码
 *
 *        Version:  1.0
 *        Created:  2018年06月14日 14时23分15秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *        Company:  
 *
 * =====================================================================================
 */
#include <stdio.h>  
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>  
#include <arpa/inet.h>

int main(int argc, char* argv[])
{
    int fd = -1;
    /**< 1.create fd*/
    /**< sockfd = socket(int socket_family, int socket_type, int protocol);*/
    /**< family:AF_INET~IPV4  AF_INET6~IPV6 AF_LOCAL~unix域*/
    /**< type:SOCK_STREAM~字节流(TCP) SOCK_DGRAM~数据报(UDP)*/
    /**< protocl: IPPROTO_TCP, IPPROTO_UDP*/
    fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP); 
    if(fd < 0)
    {
        perror("socket");
        exit(1);
    }

    /**< 2. connect server*/
    struct sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_port = htons(8000);
    server.sin_addr.s_addr = inet_addr("127.0.0.1");

    /**< int connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen);*/
    if(connect(fd, (struct sockaddr*)&server, (socklen_t)sizeof(server)) < 0)
    {
        perror("connect");   
        exit(2);
    }
    /**< 3. 连接操作*/
    char buf[1024] = {0};
    while(1)
    {
        memset(buf, 0, sizeof(buf));
        printf("send#");  
        fflush(stdout);  
        ssize_t len=read(0,buf,sizeof(buf)-1);
        buf[len-1]=0;
        if(!strcmp(buf, "exit")){
            printf("client exit \n ");
            break;
        }
        /**< 封装jsonrpc 格式*/

        /**< 发送*/
        write(fd,buf,len);
        len = read(fd, buf, sizeof(buf)-1);
        buf[len-1]=0;  
        printf("recv# %s \n", buf);  
    }

    /**< 4. close*/
    close(fd);
    return 0;
}

