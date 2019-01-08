/*
 * =====================================================================================
 *
 *       Filename:  vlog.c
 *
 *    Description:  打印等级的分配
 *
 *        Version:  1.0
 *        Created:  2018年12月06日 19时22分02秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *        Company:  
 *
 * =====================================================================================
 */
#include "common.h"

static pthread_rwlock_t vlog_rwlock = PTHREAD_RWLOCK_INITIALIZER;
static enum vlog_destination vlog_dest = VLF_CONSOLE;

static pthread_once_t  once = PTHREAD_ONCE_INIT;
static LIST_HEAD(vlog_module);

void _vlog_init(void)
{

}

/**
 *各模块插入总的链表中
 * */
void vlog_insert_module(struct list_head* node)
{
    /**< 判断有没有存在*/

    list_add(node, &vlog_module);
}

/**
 *具体实现函数
 * */
void vlog_valist(const struct vlog_module *module, int level,const char *message, va_list args)
{
    int save_errno = errno;

    pthread_rwlock_rdlock(&vlog_rwlock);
    if(vlog_dest == VLF_SYSLOG) 
    {
        //int syslog_level = syslog_levels[level];
        //vsyslog(syslog_level,message,args);
        /*  < TODO print log to syslog */
    } 
    else if(vlog_dest == VLF_CONSOLE)
    {
        vfprintf(stdout,message,args);
    }
    else if(vlog_dest == VLF_FILE) 
    {
        /*  < TODO print log to log-file */
    }
    pthread_rwlock_unlock(&vlog_rwlock);

    errno = save_errno;
}

/**
 *typedef char *va_list;
 *va_start宏，获取可变参数列表的第一个参数的地址（list是类型为va_list的指针，param1是可变参数最左边的参数）：
 *va_arg宏，获取可变参数的当前参数，返回指定类型并将指针指向下一参数（mode参数描述了当前参数的类型）：
 *va_end宏，清空va_list可变参数列表：
 * */

void vlog(const struct vlog_module *module,int level,const char *message, ...)
{
    va_list args;
    va_start(args, message);
    vlog_valist(module, level, message, args);
    va_end(args);
}

void vlog_init(void)
{
    pthread_once(&once, _vlog_init);   
}

