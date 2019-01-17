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
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>  
#include <arpa/inet.h>

#include "cJSON.h"

#define PARAM_MAX  4

/**< 封装json格式*/
char* json_func(int argc, char** argv)
{
    char* str_result = NULL; 
    cJSON *result_root = cJSON_CreateObject();
    /**< argc == 1 命令码本身*/
    cJSON_AddStringToObject(result_root, "method", argv[0]);
    if(argc == 2)
    {
        cJSON_AddStringToObject(result_root, "params", argv[1]);
    }
    else if(argc > 2)
    {
        cJSON *params_root = cJSON_CreateStringArray((const char**)&argv[1], argc-1); 
        cJSON_AddItemToObject(result_root, "params", params_root);
    }

    cJSON *id = cJSON_CreateNumber(1);
    cJSON_AddItemToObject(result_root, "id", id);

    str_result = cJSON_Print(result_root);
    cJSON_Delete(result_root);

    return str_result;
}

/**
 *功能：解析命令码
 *输入：命令字符串 
 *输出：命令+参数 总个数
 *返回：数据结构
 * */
char** parse_func(char* data, int* num)
{
    /**< 裁剪字符串*/
    if(!data){
        printf("go here\n");
        return NULL;
    }
    char* tmpdata = strdup(data);

    char* p = NULL;
    char* delim = " ";
    int i = 0;
    char* buf = NULL;

    /**< 分配最大参数个数 + 命令自身*/
    char **tmp = calloc(PARAM_MAX +1, sizeof(char *));

    /*  *< 分割字符串*/ 
    p = strtok(tmpdata, delim);
    while(p!=NULL)
    {
        buf = p;
        tmp[i] = calloc(strlen(buf)+1, sizeof(char));
        strcpy(tmp[i], buf);
        i++;
        p=strtok(NULL,delim);
    }
    *num = i;

    free(tmpdata);

    return tmp;
}

/**< free 数据*/
void freeExplode(char **arr, int size)
{
    int i;
    for(i = 0; i < size; i++)
    {
        if(arr[i])
            free(arr[i]);
    }

    free(arr);
}

int json_send(int fd, char* data)
{
    int num = 0;
    char** arr = NULL;
    /**< 解析数据*/
    arr = parse_func(data, &num);
    if(!arr || (num == 0))
    {
        freeExplode(arr, 5);
        return -1;
    }
    /**< 封装json格式*/
    char* buf = json_func(num, arr);

    //printf("buf = %s\n", buf);
    /**< 发送数据*/
    write(fd, buf, strlen(buf));
    free(buf);

    freeExplode(arr, 5);


    return 0;
}

int tcp_proc()
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
    int ret = 0;
    while(1)
    {
        memset(buf, 0, sizeof(buf));
        printf("send#");  
        fflush(stdout);  
        ssize_t len=read(0,buf,sizeof(buf)-1);
        buf[len-1]=0;
        if(!strncmp(buf, "exit", strlen("exit"))){
            printf("client exit \n ");
            break;
        }
        /**< 封装jsonrpc 格式*/
        /**< 发送*/
        ret = json_send(fd, buf);
        if(ret)
            continue;
        len = read(fd, buf, sizeof(buf)-1);
        buf[len-1]=0;  
        printf("recv# %s \n", buf);  
    }

    /**< 4. close*/
    close(fd);
    return 0;
}


int main(int argc, char* argv[])
{
    /**< cli具体实现*/
    tcp_proc();
    return 0;
}

