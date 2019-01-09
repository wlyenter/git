/*
 * =====================================================================================
 *
 *       Filename:  unixctl.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2019年01月05日 11时49分23秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *        Company:  
 *
 * =====================================================================================
 */
#ifndef __UNIXCTL_H
#define __UNIXCTL_H

typedef struct{
    int error_code;
    char* error_message;
}jrpc_context;

struct bufferevent;
typedef cJSON* unixctl_cb_func(struct bufferevent *bev, cJSON* param, cJSON* ip); 


#endif
