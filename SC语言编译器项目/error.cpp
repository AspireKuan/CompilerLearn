#include"pch.h"
#include"error.h"
#include"DyArray.h"
#include"LexicalAnalysis.h"
#include"DyString.h"
#include<stdio.h>
#include<stdlib.h>  
#include<stdarg.h>

extern int line_num;		//�к�
extern int token_num;		//��ǰ���ʵ��ֱ���
extern DyArray tktable;	//���ʱ�
extern DyString sourcestr;	//����Դ���ַ�
extern char* filename;


void handle_exception(int stage, int level, char *fmt, va_list ap)
{
	char buf[1024];
	vsprintf(buf, fmt, ap);
	if (stage == STAGE_COMPILE)
	{
		if (level == LEVEL_WARNING)
		{
			printf("%s(��%d��)�����뾯�棺%s!\n", filename, line_num, buf);
		}
		else
		{
			printf("%s(��%d��)���������%s!\n", filename, line_num, buf);
			return;
		}
	}
	else
	{
		printf("���Ӵ���\n");
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
	error((char*)"ȱ��%s", msg);
}

void skip(int c)
{
	if (token_num != c)
	{
		error((char*)"ȱ��'%s'", get_tkstr(c));
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
		//��������һ��ǿ��ת���ģ���Ϊ���ʱ��ж���void**����
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