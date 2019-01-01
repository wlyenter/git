/*
 * =====================================================================================
 *
 *       Filename:  byteq.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2018年12月19日 18时58分03秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *        Company:  
 *
 * =====================================================================================
 */
#ifndef __BYTEQ_H
#define __BYTEQ_H

#include <stdint.h>

/**
 *环形缓存区结构体
 * */
struct byteq{
    uint8_t *buffer;       /**< 环形缓存区*/
    unsigned int size;     /**< 缓存区长度*/
    unsigned int head;     /**< 缓存区头部*/
    unsigned int tail;     /**< 缓存区尾部*/
};

#endif

