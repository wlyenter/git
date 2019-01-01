/*******************************************************************************
 Copyright (c) 2017, Hangzhou joyware Technologies Co., Ltd. All rights reserved.
--------------------------------------------------------------------------------
                              
  Project Code: 
   Module Name: 
  Date Created:2017-05-16
         Author: 
   Description: define standard type definition file 

--------------------------------------------------------------------------------
  Modification History
  DATE        NAME             DESCRIPTION
--------------------------------------------------------------------------------
  2017-05-16     created 

*******************************************************************************/


/*******************************************************************************
*                 File Comments                                                
*******************************************************************************/
#ifndef   _INCLUDE_MW_TYPE_DEF_H_
#define   _INCLUDE_MW_TYPE_DEF_H_

#ifdef  __cplusplus
extern "C"{
#endif

#if	!defined(NULL)
#if defined __GNUG__
#define NULL (__null)
#else
#if !defined(__cplusplus) && 0
#define NULL ((void*)0)
#else
#define NULL (0)
#endif
#endif
#endif

#if	(!defined(EOF) || (EOF!=(-1)))
#define EOF		(-1)
#endif

#undef FALSE
#if	(!defined(FALSE) || (0 != FALSE))
#define FALSE		0
#endif

#undef TRUE
#if	(!defined(TRUE) || (1 != TRUE))
#define TRUE		1
#endif

#ifndef _MWTYPE_DEF
#define _MWTYPE_DEF

typedef char                CHAR;
typedef int                 INT;
typedef int                 LONG;
typedef short               SHORT;
typedef float               FLOAT;

typedef	char		        INT8;
typedef	short		        INT16;
typedef	int                 INT32;
typedef	long long	        INT64;

typedef	unsigned char       UINT8;
typedef	unsigned short      UINT16;
typedef	unsigned int        UINT32;
typedef	unsigned long long  UINT64;

typedef	unsigned char	    UCHAR;
typedef unsigned short	    USHORT;
typedef	unsigned int	    UINT;
typedef unsigned long	    ULONG;
typedef unsigned long long  ULONG64;
typedef unsigned char       BYTE;

typedef	unsigned char	    uchar;
typedef unsigned short	    ushort;
typedef	unsigned int	    uint;
typedef unsigned long	    ulong;

typedef	int                 BOOL;
typedef unsigned short      BOOL_T;
typedef int                 ARGINT;

#ifndef OK
#define OK          (0)
#endif
#ifndef ERROR
#define ERROR		(-1)
#endif

#define BUF_SIZE8       8
#define BUF_SIZE16      16
#define BUF_SIZE32      32
#define BUF_SIZE64      64
#define BUF_SIZE128     128
#define BUF_SIZE256     256
#define BUF_SIZE512     512
#define BUF_SIZE1024    1024
#define BUF_SIZE2048    2048

#if defined(__KERNEL__)
#define ASSERT(_x)  do{         \
    if (!(_x)) {                \
        printk("Assert[%s][%d] %s\n", __FUNCTION__, __LINE__, #_x);       \
    }   \
}while(0)
#else
#define ASSERT(_x)  do{         \
    if (!(_x)) {                \
        printf("Assert[%s][%d] %s\n", __FUNCTION__, __LINE__, #_x);       \
    }   \
}while(0)

#endif


#endif

#define BOOL_TRUE  ((BOOL_T)1)
#define BOOL_FALSE ((BOOL_T)0)

#define ENUMTOSTR(VAL)  VAL##_STR
#define ENABLE_STR       "enable"
#define DISABLE_STR       "disable"
#define TRUE_STR           "true"
#define FALSE_STR          "flase"

/* typedef void will raise g++ compile error, zhouxiaochen */
/* typedef void		VOID; */
#define VOID        void

typedef int         PART_ID;

#define FAST        register
#define IMPORT      extern
#define LOCAL       static
#define CONST      const
#if defined(UT_TEST) || defined(UT_TEST_X86)
#define STATIC       
#else
#define STATIC   static
#endif
/* timeout defines */

#define NO_WAIT             (0)
#define WAIT_FOREVER        (-1)

/*#define OK          (0)*/
/*#define ERROR		(-1)*/
#define IN
#define INOUT
#define OUT
/*定义端口号，端口号包括LAN口，WAN口，光口，commbo口*/
typedef UINT32 port_num_t;

/*定义vlan id的类型*/
typedef UINT16 vlan_id_t;

/*定义用户端口号类型*/
/* user port num define : | device num(8bit) | slot num(8bit) | reserve(8bit) | port num(8bit) |*/
typedef UINT32 user_port_num_t;
typedef UINT32 agg_grp_num_t;

#define IP_ADDR_LEN     4
#define IP_STRING_LEN   15
#define IPV6_ADDR_LEN   16
#define IPV6_STRING_LEN 40

typedef UINT8 ip_address_t[IP_ADDR_LEN];
typedef INT8 ip_address_str_t[IP_STRING_LEN + 1];

#define MAC_ADDR_LEN  6
typedef UINT8 mac_address_t[MAC_ADDR_LEN];

#define VLAN_BUF_LEN   512
typedef UINT8 vid_list_t[VLAN_BUF_LEN];

#define IPV6_ADDR_LEN           16
typedef UINT8 ipv6_address_t[IPV6_ADDR_LEN];
typedef INT8 ipv6_address_str_t[IPV6_STRING_LEN + 1];

#ifdef  __cplusplus
}
#endif
#endif /* _INCLUDE_MW_TYPE_DEF_H_  */



