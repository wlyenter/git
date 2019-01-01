/*
 * =====================================================================================
 *
 *       Filename:  unixctl.c
 *
 *    Description:  unix/ip协议通信
 *
 *        Version:  1.0
 *        Created:  2018年12月06日 19时25分34秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *        Company:  
 *
 * =====================================================================================
 */
#include "common.h"

#include <event2/util.h>
#include <event2/event.h>
#include <event2/listener.h>
#include <event2/bufferevent.h>
#include <event2/buffer.h>

VLOG_DEFINE_THIS_MODULE(unixctl);

/**< 读操作的回调函数具体实现*/
void unix_read_cb(struct bufferevent *bev, void *arg)
{
    char msg[4096] = {0};

    size_t len = bufferevent_read(bev, msg, sizeof(msg)-1);

    msg[len] = '\0';
    VLOG_INFO("rcv data : %s", msg);
    char reply[] = "I has read your data";  
    bufferevent_write(bev, reply, strlen(reply) );

    /**< 1.创建一个关联的jsonrpc对象*/
    struct jsonrpc* rpc = jsonrpc_open();

    /**< 2.处理jsonrpc对象缓存对象*/
    /**< 3.接收一条json格式消息*/
#if 0
    struct jsonrpc_msg *pstmsg;
    /**< 3.1 接收到的信息放在json_parse里*/
    int n = json_parse_feed(rpc->parser, msg, len);
    /**< 3.2 解析成jsonrpcmsg格式*/
    pstmsg = jsonrpc_parse_received_message(rpc);
    /**< 3.3 命令解析并处理*/
    process_command(bev, pstmsg);
#endif
    cJSON *root;
    char *end_ptr = NULL;

    if ((root = cJSON_Parse_Stream(msg, &end_ptr)) != NULL) {
        char * str_result = cJSON_Print(root);
        printf("Valid JSON Received:\n%s\n", str_result);
        free(str_result);
    }
    


    /**< 4.消息类型处理*/
    /**< 5.查找命令对象并调用回调函数*/

}

/**< 错误事件的回调函数具体实现*/
void unix_event_cb(struct bufferevent *bev, short events, void* arg)
{
    if(events & BEV_EVENT_EOF)
    {
        VLOG_INFO("connection closed");
    }
    else if(events & BEV_EVENT_ERROR)
    {
        VLOG_ERR("connection error"); 
    }
    else
    {
        VLOG_DBG("go on here"); 
    }

    /**< 这将自动close套接字和free读写缓冲区*/
    bufferevent_free(bev);
}

/**< 有新的客户端连接到服务器当此函数被调用时，
 * libevent已经帮我们accept了这个客户端。该客户端的文件描述符为fd
 * evconnlistener *listener, 
 * evutil_socket_t fd,
 * struct sockaddr *sock, int socklen, 
 * void *arg
 */
void unix_listener_cb(struct evconnlistener* listener, evutil_socket_t fd,
                      struct sockaddr *sock, int socklen, void *arg)
{
    VLOG_INFO("accept a client %d, from[%s]", fd, "127.0.0.1");

    struct event_base *base = (struct event_base*)arg;

    /**< 为这个客户端分配一个bufferevent*/
    struct bufferevent *bev = bufferevent_socket_new(base, fd, BEV_OPT_CLOSE_ON_FREE);

    /**< 为bufferevent 设置回调函数*/
    bufferevent_setcb(bev, unix_read_cb, NULL, unix_event_cb, NULL);

    /**< 启用bufferevent的读事件*/
    bufferevent_enable(bev, EV_READ);

}


/**
 *通过libevent建立tcp server
 * */
void unix_server_create()
{
    struct sockaddr_in local;
    memset(&local, 0, sizeof(struct sockaddr_in));
    local.sin_family = AF_INET;
    local.sin_port = htons(8000);
    local.sin_addr.s_addr = inet_addr("127.0.0.1");

    /**< 函数分配且返回一个具有默认配置的event_base*/
    struct event_base *base = event_base_new();

    /**
     * struct event_base *base,
     * evconnlistener_cb cb, 回调函数，当有新的客户端请求连接时，该函数就会调用 
     * void *ptr, 回调函数的参数
     * unsigned flags, LEV_OPT_CLOSE_ON_FREE：当连接监听器释放时，会自动关闭底层的socket
     *                 LEV_OPT_THREADSAFE：为连接监听器分配锁。这样可以确保线程安全
     * int backlog, backlog 是任何时刻网络栈允许处于还未接受状态的最大未决连接数
     *              如果 backlog 是负的,libevent 会试图挑选一个较好的值;
     *              如果为0,libevent 认为已经对提供的套接字调用了 listen()
     * const struct sockaddr *sa, 
     * int socklen
     * */
    struct evconnlistener *listener = evconnlistener_new_bind(base, unix_listener_cb, base, 
                                                              LEV_OPT_REUSEABLE|LEV_OPT_CLOSE_ON_FREE,
                                                              -1, (struct sockaddr*)&local, sizeof(struct sockaddr_in));



    /**< 运行 event_base 直到其中没有已经注册的事件为止。
     * 执行循环的时候,函数重复地检查是否有任何已经注册的事件被触发*/
    event_base_dispatch(base);


    /**< 使用完 listener 之后,进行释放*/
    evconnlistener_free(listener);

    /**< 使用完 event_base 之后,使用 event_base_free()进行释放*/
    event_base_free(base);
}



/**< 初始化*/


