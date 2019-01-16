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

typedef cJSON* unixctl_cb_func(jrpc_context *ctx, int argc, const char* argv[], void* aux); 
void unixctl_server_create();
void unixctl_command_register(const char* name, const char* usage, int min_args, int max_args, 
                              unixctl_cb_func *cb, void* aux);

#endif
