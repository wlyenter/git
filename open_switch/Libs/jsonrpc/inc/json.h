/*
 * =====================================================================================
 *
 *       Filename:  json.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2018年12月19日 19时23分28秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *        Company:  
 *
 * ===============================================================================
*/
#ifndef __JSON_H
#define __JSON_H

/**< json 类型*/
enum json_type{
    JSON_NULL,        /* null */
    JSON_FALSE,       /* false */
    JSON_TRUE,        /* true */
    JSON_OBJECT,      /* {"a":b, "c":d, } */
    JSON_ARRAY,       /* [1,2,3,...] */
    JSON_INTENGER,    /* 123 */
    JSON_REAL,        /* 123.456 */
    JSON_STRING,      /* "..." */
    JSON_N_TYPES,     /* */
};

/**< json的数组结构*/
struct json_array{
    size_t n, n_allocted;
    struct json **elems;
};

/**< 定义一个json对象*/
struct json{
    enum json_type type;              /**< json 对象类型*/
    size_t count;                     /**< json 对象的引用计数*/
    union{
        //struct shash *object;       /**< 对应json object 类型*/
        struct json_array array;
        long long int intenger;
        double real;
        char *string;
    }u;
};

/**< json 解析器结构*/
struct json_parser{
    int flags;

};

#endif

