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

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <event2/util.h>
#include <event2/event.h>
#include <event2/listener.h>
#include <event2/bufferevent.h>
#include <event2/buffer.h>

#include "shash.h"
#include "svec.h"

VLOG_DEFINE_THIS_MODULE(unixctl);

#define PROC_BUFF 8192
unsigned char proc_buff[PROC_BUFF];

/**< unixctl的命令描述*/
struct unixctl_command {
    char* usage;                /**< 命令的描述 */
    int min_args, max_args;     /**< 命令支持的最小/最大参数 */
    unixctl_cb_func* cb;        /**< 收到该命令后执行的回调函数 */
    void* aux;                  /**< 回调函数的参数 */
};

static struct shash commands = SHASH_INITIALIZER(&commands); /**< 初始化一张哈希表*/

/*
 *
 * http://www.jsonrpc.org/specification
 *
 * code message meaning
 * -32700   Parse error Invalid JSON was received by the server.
 *          An error occurred on the server while parsing the JSON text.
 * -32600   Invalid Request The JSON sent is not a valid Request object.
 * -32601   Method not found    The method does not exist / is not available.
 * -32602   Invalid params  Invalid method parameter(s).
 * -32603   Internal error  Internal JSON-RPC error.
 * -32000 to -32099 Server error    Reserved for implementation-defined server-errors.
 */

#define JRPC_PARSE_ERROR      -32700
#define JRPC_INVALID_REQUEST  -32600
#define JRPC_METHOD_NOT_FOUND -32601
#define JRPC_INVALID_PARAMS   -32603
#define JRPC_INTERNAL_ERROR   -32693

static int unixctl_json_send_response(struct bufferevent *bev,  char *response) 
{
    VLOG_DBG("JSON Response:\n%s\n", response);
    bufferevent_write(bev, response, strlen(response));
    return 0;
}

static int unixctl_json_send_error(struct bufferevent *bev, int code, char* message, cJSON * id) 
{
    int return_value = 0;
    cJSON *result_root = cJSON_CreateObject();
    cJSON *error_root = cJSON_CreateObject();
    cJSON_AddNumberToObject(error_root, "code", code);
    cJSON_AddStringToObject(error_root, "message", message);
    cJSON_AddItemToObject(result_root, "error", error_root);
    cJSON_AddItemToObject(result_root, "id", id);
    char * str_result = cJSON_Print(result_root);
    return_value = unixctl_json_send_response(bev, str_result);
    free(str_result);
    cJSON_Delete(result_root);
    free(message);
    return return_value;
}

static int unixctl_json_send_result(struct bufferevent *bev, cJSON * result, cJSON * id) 
{
    int return_value = 0;
    cJSON *result_root = cJSON_CreateObject();
    if (result)
        cJSON_AddItemToObject(result_root, "result", result);
    cJSON_AddItemToObject(result_root, "id", id);

    char * str_result = cJSON_Print(result_root);
    return_value = unixctl_json_send_response(bev, str_result);
    free(str_result);
    cJSON_Delete(result_root);
    return return_value;
}

/**< 查找命令对象并调用回调函数*/
static int unixctl_json_invoke_procedure(struct bufferevent *bev, char *name, cJSON *params, cJSON *id) 
{
    int params_num = 0;
    cJSON *returned = NULL;
    cJSON *elems = NULL;

    jrpc_context ctx;
    ctx.error_code = 0;
    ctx.error_message = NULL;

    /**< 获取参数个数*/
    if(params){
        params_num = cJSON_GetArraySize(params);
    }
    /**< 查找注册命令*/
    struct unixctl_command *command = shash_find_data(&commands, name);
    if(!command)
    {
        ctx.error_code = JRPC_METHOD_NOT_FOUND;
        ctx.error_message = 
            xasprintf("\"%s\" is not a valid command",name);
    }
    else if(params_num < command->min_args)
    {
        ctx.error_code = JRPC_INVALID_PARAMS;
        ctx.error_message = 
            xasprintf("\"%s\" command requires at least %d arguments", 
                      name, command->min_args);

    }
    else if(params_num > command->max_args)
    {
        ctx.error_code = JRPC_INVALID_PARAMS;
        ctx.error_message = 
            xasprintf("\"%s\" command takes at most %d arguments",
                      name, command->max_args);
    }
    else
    {
        /**< 调用回调函数*/
        struct svec argv = SVEC_EMPTY_INITIALIZER;
        int  i;

        // 将命令名以及参数都存放到svec结构中,形成一个字符串数组
        svec_add(&argv, name);
        for (i = 0; i < params_num; i++) {
            elems = cJSON_GetArrayItem(params, i);
            if (elems->type != cJSON_String) {
                ctx.error_code = JRPC_INVALID_PARAMS;
                ctx.error_message = 
                    xasprintf("\"%s\" command has non-string argument",name);
                break;
            }
            svec_add(&argv, elems->valuestring);
        }
        svec_terminate(&argv);

        // 如果以上操作无误,最终调用该命令对象的回调函数
        if (!ctx.error_code) {
            returned = command->cb(&ctx, argv.n, (const char **)argv.names, command->aux);
        }

        svec_destroy(&argv);
    }
    if (ctx.error_code)
        return unixctl_json_send_error(bev, ctx.error_code, ctx.error_message, id);
    else
        return unixctl_json_send_result(bev, returned, id);

    return 0;
}

/**< 消息类型解析处理函数*/
static int unixctl_json_eval_request(struct bufferevent *bev, cJSON *root) 
{
    cJSON *method, *params, *id;
    /* 获取 method 条目*/
    method = cJSON_GetObjectItem(root, "method");
    if (method != NULL && method->type == cJSON_String) {
        /* 获取 params 条目*/
        params = cJSON_GetObjectItem(root, "params");
        if (params == NULL|| params->type == cJSON_Array || params->type == cJSON_Object) 
        {
            /* 获取 id 条目*/
            id = cJSON_GetObjectItem(root, "id");
            if (id == NULL|| id->type == cJSON_String || id->type == cJSON_Number) 
            {
                //We have to copy ID because using it on the reply and deleting the response Object will also delete ID
                cJSON * id_copy = NULL;
                if (id != NULL)
                    id_copy = (id->type == cJSON_String) ? cJSON_CreateString(id->valuestring) :
                        cJSON_CreateNumber(id->valueint);
                VLOG_DBG("Method Invoked: %s\n", method->valuestring);
                return unixctl_json_invoke_procedure(bev,  method->valuestring, params, id_copy);
            }
        }
    }
    unixctl_json_send_error(bev, JRPC_INVALID_REQUEST, strdup("The JSON sent is not a valid Request object."), NULL);
    return -1;
}

cJSON* unixctl_list_command(jrpc_context *ctx, int argc, const char* argv[], void* aux)
{
    const struct shash_node **nodes = shash_sort(&commands);
    size_t i;

    memset(proc_buff, 0, sizeof(proc_buff));
    cJSON *result_root = cJSON_CreateObject();

    for (i = 0; i < shash_count(&commands); i++) {
        const struct shash_node *node = nodes[i];
        const struct unixctl_command *command = node->data;
        //strcat(proc_buff, command->name);
        //strcat(proc_buff, " ");
        cJSON_AddStringToObject(result_root, node->name, command->usage);
    }
    free(nodes);

    return result_root;   
}


/**< 读操作的回调函数具体实现*/
void unixctl_read_cb(struct bufferevent *bev, void *arg)
{
    char msg[4096] = {0};

    /**< 1.接收一条消息*/
    size_t len = bufferevent_read(bev, msg, sizeof(msg)-1);

    //msg[len] = '\0';
    VLOG_INFO("rcv data : %s", msg);

    cJSON *root;
    char *end_ptr = NULL;

    /**< 2. 接收到的信息按json格式解析到root里*/
    if ((root = cJSON_Parse_Stream(msg, &end_ptr)) != NULL) 
    {
        char * str_result = cJSON_Print(root);
        VLOG_DBG("Valid JSON Received:\n%s\n", str_result);
        free(str_result);
        if (root->type == cJSON_Object) {
            /**< 3. 命令解析并处理*/
            unixctl_json_eval_request(bev, root);
        }

        cJSON_Delete(root);
    }
    else 
    {
        // did we parse the all buffer? If so, just wait for more.
        // else there was an error before the buffer's end
        VLOG_WARN("INVALID JSON Received:\n---\n%s\n---\n",msg);
        unixctl_json_send_error(bev, JRPC_PARSE_ERROR,
                                strdup("Parse error. Invalid JSON was received by the server."),
                                NULL);
    }


}

/**< 错误事件的回调函数具体实现*/
void unixctl_event_cb(struct bufferevent *bev, short events, void* arg)
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
void unixctl_listener_cb(struct evconnlistener* listener, evutil_socket_t fd,
                         struct sockaddr *sock, int socklen, void *arg)
{
    VLOG_INFO("accept a client %d, from[%s]", fd, "127.0.0.1");

    struct event_base *base = (struct event_base*)arg;

    /**< 为这个客户端分配一个bufferevent*/
    struct bufferevent *bev = bufferevent_socket_new(base, fd, BEV_OPT_CLOSE_ON_FREE);

    /**< 为bufferevent 设置回调函数*/
    bufferevent_setcb(bev, unixctl_read_cb, NULL, unixctl_event_cb, NULL);

    /**< 启用bufferevent的读事件*/
    bufferevent_enable(bev, EV_READ);

}


/**
 *通过libevent建立tcp server
 * */
void unixctl_server_create()
{
    struct sockaddr_in local;
    memset(&local, 0, sizeof(struct sockaddr_in));
    local.sin_family = AF_INET;
    local.sin_port = htons(8000);
    local.sin_addr.s_addr = inet_addr("127.0.0.1");

    unixctl_command_register("list", "list-commands", 0, 0, unixctl_list_command, NULL);
    unixctl_command_register("ls", "", 0, 0, unixctl_list_command, NULL);
    unixctl_command_register("ps", "", 0, 0, unixctl_list_command, NULL);

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
    struct evconnlistener *listener = evconnlistener_new_bind(base, unixctl_listener_cb, base, 
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

/* Register a unixctl command
 * @name        command name
 * @usage       command usage
 * @min_args    min args this command support
 * @max_args    max args this command support
 * @cb          callback func when recv this command
 * @aux         arg for cb
 * */
void unixctl_command_register(const char* name, const char* usage, int min_args, int max_args, unixctl_cb_func *cb, void* aux)
{
    struct unixctl_command* command = calloc(1, sizeof(struct unixctl_command));
    if(!command){
        VLOG_ERR("Fail to allocate unixctl command: %s",strerror(errno));
        exit(EXIT_FAILURE);
    }
    /**< 判断哈希表里有没有信息注册*/
    struct unixctl_command* lookup = shash_find_data(&commands, name);
    if(lookup){
        return;
    }

    command->usage = strdup(usage);
    command->min_args = min_args;
    command->max_args = max_args;
    command->cb = cb;
    command->aux = aux;

    /**< 加入哈希表*/
    shash_add(&commands, name, command);
}





