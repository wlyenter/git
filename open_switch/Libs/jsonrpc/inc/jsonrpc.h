/*
 * =====================================================================================
 *
 *       Filename:  jsonrpc.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2018年12月19日 19时30分57秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *        Company:  
 *
 * =====================================================================================
 */
#ifndef __JSONRPC_H
#define __JSONRPC_H

#include "json.h"

enum jsonrpc_msg_type{
    JSONRPC_REQUEST,       /**< 请求*/
    JSONRPC_NOTIFY,        /**< 通知*/
    JSONRPC_REPLY,         /**< 成功返回*/
    JSONRPC_ERROR,         /**< 错误返回*/
};

struct jsonrpc_msg{
    enum jsonrpc_msg_type type;  /**< 消息类型*/
    char* method;                /**< 命令名称*/   
    struct json *params;         /**< 命令参数*/
    struct json *result;         /**< 成功返回的结果*/
    struct json *error;          /**< 失败返回的结果*/
    struct json *id;             /**< 命令的标识码*/  
};

struct jsonrpc;

struct jsonrpc* jsonrpc_open();

#endif

