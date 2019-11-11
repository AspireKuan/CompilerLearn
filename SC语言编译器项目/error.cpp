#include"pch.h"
#include"error.h"
#include"DyArray.h"
#include"LexicalAnalysis.h"
#include"DyString.h"
#include<stdio.h>
#include<stdlib.h>  
#include<stdarg.h>

extern int line_num;		//行号
extern int token_num;		//当前单词的种别码
extern DyArray tktable;	//单词表
extern DyString sourcestr;	//单词源码字符
extern char* filename;


void handle_exception(int stage, int level, char *fmt, va_list ap)
{
	char buf[1024];
	vsprintf(buf, fmt, ap);
	if (stage == STAGE_COMPILE)
	{
		if (level == LEVEL_WARNING)
		{
			printf("%s(第%d行)：编译警告：%s!\n", filename, line_num, buf);
		}
		else
		{
			printf("%s(第%d行)：编译错误：%s!\n", filename, line_num, buf);
			return;
		}
	}
	else
	{
		printf("链接错误\n");
		exit(-1);
	}
}

void warning(char *fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	handle_exception(STAGE_COMPILE, LEVEL_WARNING, fmt, ap);
	va_end(ap);
}

void error(char *fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	handle_exception(STAGE_COMPILE, LEVEL_ERROR, fmt, ap);
	va_end(ap);
}


void expect(char *msg)
{
	error((char*)"缺少%s", msg);
}

void skip(int c)
{
	if (token_num != c)
	{
		error((char*)"缺少'%s'", get_tkstr(c));
	}
	get_token();
}

char *get_tkstr(int v)
{
	if (v > tktable.count)
	{
		return NULL;
	}
	else if (v >= TK_CINT && v <= TK_CSTR)
	{
		return sourcestr.data;
	}
	else
	{
		//这里是有一个强制转化的，因为单词表中都是void**类型
		return ((Token*)tktable.data[v])->spelling;
	}
}

void link_error(char *fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	handle_exception(STAGE_LINK, LEVEL_ERROR, fmt, ap);
	va_end(ap);
}