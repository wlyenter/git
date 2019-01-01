/*
 * =====================================================================================
 *
 *       Filename:  main.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2018年12月05日 16时59分14秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *        Company:  
 *
 * =====================================================================================
 */
#include "common.h"

VLOG_DEFINE_THIS_MODULE(main)

void usage(char* argv[])
{
    fprintf(stderr, "Usage: %s [-v levels] [-d]\n",argv[0]);
    fprintf(stderr, "-d : run as a daemon\n");
    fprintf(stderr, "-h : display this help massage\n");
    fprintf(stderr, "-v : set log levels\n");
}


int main(int argc, char* argv[])
{

    int opt;
    int levels;
    bool daemonize = false;
    
    /**< getopt 解析命令行函数，带:的后面需要带参数*/
    while((opt = getopt(argc, argv, "hdv:")) != -1)
    {
        switch(opt)
        {   
            case 'h': /**< 帮助信息*/ 
                usage(argv);
                break;
            case 'd': /**< 是否后台运行*/
                daemonize = true;
                break;
            case 'v': /**< 打印等级*/                
                levels = atoi(optarg); /**< optarg当前选项参数字符串*/
                break;
            default: /**< 其它处理*/
                fprintf(stderr, "Usage: %s [-v levels] [-n] name\n",argv[0]);
                exit(EXIT_FAILURE);
        }
    }
    /**< log 打印等级*/
    vlog_init();
    VLOG_ERR("hello");
    VLOG_WARN("hello");
    VLOG_INFO("hello");
    VLOG_DBG("hello");
    /**< unix/tcp 初始化*/
    unix_server_create();

    exit(EXIT_SUCCESS);
}



