/*
 * =====================================================================================
 *
 *       Filename:  jsonrpc.c
 *
 *    Description:  jsonrpc库
 *
 *        Version:  1.0
 *        Created:  2018年12月19日 16时40分31秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *        Company:  
 *
 * =====================================================================================
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "byteq.h"
#include "json.h"
#include "jsonrpc.h"



/**
 *定义jsonrpc对象结构，包含一条连接session上的数据传输及处理
 * */
struct jsonrpc
{
    struct session *pstSession;       /**< 该jsonrpc 拥有的流对象*/  
    char* name;                       /**< jsonrpc 名称*/
    int   status;                     /**< jsonrpc 状态*/

    /**< input*/
    struct byteq  input;             /**< 输入数据的缓存区对象*/ 
    uint8_t input_buffer[512];       /**< 缓存区*/
    struct json_parser* parser;      /**< 该jsonrpc 的json解析器，用于解析收到的数据*/

    /**< output*/

};

struct jsonrpc* jsonrpc_open()
{
    struct jsonrpc *rpc;



    return rpc;
}


