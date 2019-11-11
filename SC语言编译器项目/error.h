#ifndef ERROR_H
#define ERROR_H

#include<stdarg.h>

//错误级别
enum e_ErrorLevel
{
	LEVEL_WARNING,
	LEVEL_ERROR
};

//工作阶段
enum e_WorkStage
{
	STAGE_COMPILE,
	STAGE_LINK
};

//异常处理阶段
void handle_exception(int stage, int level, char *fmt, va_list ap);

//编译警告处理
void warning(char *fmt, ...);

//编译致命错误处理
void error(char *fmt, ...);

//提示错误
void expect(char *msg);

//跳过单词c，取下一个单词。假如当前单词不是c，提示错误
void skip(int c);

//取得单词v所代表的源码字符串
char *get_tkstr(int v);

//链接错误处理
void link_error(char *fmt, ...);

#endif
