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

struct bufferevent;
typedef void unixctl_cb_func(struct bufferevent *bev, int argc, const char* argv[], void* aux); 


#endif
