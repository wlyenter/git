/*
 * =====================================================================================
 *
 *       Filename:  vlog.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2018年12月07日 14时02分10秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *        Company:  
 *
 * =====================================================================================
 */
#ifndef __VLOG_H
#define __VLOG_H

/*  < a log module. */
struct vlog_module {
    struct list_head node;   /**< 节点*/
    const char *name;        /**< 名称*/
    int level;               /**< 等级*/
};

void vlog_init(void);
void vlog_insert_module(struct list_head *node);
int vlog_set_destination(const char *string);
char *vlog_set_levels_from_string(const char *string);
void vlog(const struct vlog_module *module,int level,const char *message, ...);

/**< 打印内容的三个去向*/
#define VLOG_DESTINATIONS \
    VLOG_DESTINATION(SYSLOG) \
    VLOG_DESTINATION(CONSOLE) \
    VLOG_DESTINATION(FILE)


/*  < Destinatinos that we can log to. */
enum vlog_destination {
#define VLOG_DESTINATION(NAME) VLF_##NAME,
        VLOG_DESTINATIONS
#undef VLOG_DESTINATION
        VLOG_N_DEST
};

/**< 打印的几个等级*/
#define VLOG_LEVELS \
    VLOG_LEVEL(OFF,LOG_EMERG) \
    VLOG_LEVEL(ERR,LOG_ERR) \
    VLOG_LEVEL(WARN,LOG_WARNING) \
    VLOG_LEVEL(INFO,LOG_INFO) \
    VLOG_LEVEL(DBG,LOG_DEBUG) 

enum vlog_level {
#define VLOG_LEVEL(NAME,SYSLOG_LEVEL) VLL_##NAME,
    VLOG_LEVELS
#undef VLOG_LEVEL
    VLL_N_LEVELS
};


/**
 *  __attribute__可以设置函数属性(Function Attribute)、变量属性(Variable Attribute)和类型属性(Type Attribute)
 *  __attribute__前后都有两个下划线，并且后面会紧跟一对原括弧，括弧里面是相应的__attribute__参数
 *  若函数被设定为constructor属性，则该函数会在main（）函数执行之前被自动的执行
 *  若函数被设定为destructor属性，则该函数会在main（）函数执行之后或者exit（）被调用后被自动的执行
 */
/**< 每个模块的初始化init，意味着每个模块只有初始化后使用vlog才会生效*/
#define VLOG_DEFINE_THIS_MODULE(MODULE)             \
    static struct vlog_module this_module = {       \
        LIST_HEAD_INIT(this_module.node),           \
        #MODULE,                                    \
        VLL_INFO,                                   \
    };                                              \
    static void __attribute__((constructor))        \
    init_this_module_ ## MODULE(void) {             \
        vlog_insert_module(&this_module.node);      \
    }

/**
 *printf("\033[1;33m Hello World. \033[0m \n");
 颜色如下:
 none         = "\033[0m"
 black        = "\033[0;30m"
 dark_gray    = "\033[1;30m"
 blue         = "\033[0;34m"
 light_blue   = "\033[1;34m"
 green        = "\033[0;32m"
 light_green -= "\033[1;32m"
 cyan         = "\033[0;36m"
 light_cyan   = "\033[1;36m"
 red          = "\033[0;31m"
 light_red    = "\033[1;31m"
 purple       = "\033[0;35m"
 light_purple = "\033[1;35m"
 brown        = "\033[0;33m"
 yellow       = "\033[1;33m"
 light_gray   = "\033[0;37m"
 white        = "\033[1;37m"

 字背景颜色范围: 40--49         字颜色: 30--39
    40: 黑                         30: 黑
    41:红                          31: 红
    42:绿                          32: 绿
    43:黄                          33: 黄
    44:蓝                          34: 蓝
    45:紫                          35: 紫
    46:深绿                        36: 深绿
    47:白色                        37: 白色



输出特效格式控制：

\033[0m  关闭所有属性  
\033[1m   设置高亮度  
\03[4m   下划线  
\033[5m   闪烁  
\033[7m   反显  
\033[8m   消隐  
\033[30m   --   \033[37m   设置前景色  
\033[40m   --   \033[47m   设置背景色
****/

/**
 * __VA_ARGS__ 是一个可变参数的宏,实现思想就是宏定义中参数列表的最后一个参数为省略号（也就是三个点）
 * ##__VA_ARGS__ 宏前面加上##的作用在于，当可变参数的个数为0时，这里的##起到把前面多余的","去掉的作用,否则会编译出错
 * */

#define VLOG_ERR(fmt,...)   VLOG(VLL_ERR,"\033[0;31m%s|[%s][%d]: "fmt"\033[0m\n",\
                                 this_module.name,__func__,__LINE__, ## __VA_ARGS__)
#define VLOG_WARN(fmt,...)  VLOG(VLL_WARN,"\033[1;31m%s|[%s][%d]: "fmt"\033[0m\n",\
                                 this_module.name,__func__,__LINE__, ## __VA_ARGS__)
#define VLOG_INFO(fmt,...)  VLOG(VLL_INFO,"\033[1;32m%s|[%s][%d]: "fmt"\033[0m\n",\
                                 this_module.name,__func__,__LINE__, ## __VA_ARGS__)
#define VLOG_DBG(fmt,...)   VLOG(VLL_DBG,"\033[1;33m%s|[%s][%d]: "fmt"\033[0m\n",\
                                 this_module.name,__func__,__LINE__, ## __VA_ARGS__)

#define VLOG(LEVEL,fmt,...)                          \
    do{                                              \
        if(this_module.level >= LEVEL)               \
        vlog(&this_module,LEVEL,fmt,## __VA_ARGS__); \
    }while(0)

#endif



