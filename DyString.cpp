#include"pch.h"
#include"DyString.h"
#include<stdio.h>
#include<string.h>
#include<stdlib.h>

int init_dystring(DyString *str, int initsize)
{
	//在初始化的时候，str一定自己已经有了空间
	if (str == NULL)
	{
		return 0;
	}
	initsize=align(initsize);
	str->data = (char*)malloc(sizeof(char)*initsize);
	if (str->data == NULL)
	{
		printf("内存分配失败\n");
		return 0;
	}
	memset(str->data, 0, initsize);
	str->capacity = initsize;
	str->length = 0;
	return 1;
}

void free_dystring(DyString *str)
{
	if (str != NULL)
	{
		free(str->data);
		str->data = NULL;
	}
}

void reset_dystring(DyString *str)
{
	if (str != NULL)
	{
		free_dystring(str);
		init_dystring(str, 8);
	}
}

void realloc_dystring(DyString *str,int new_size)
{
	if (new_size <=str->capacity)
	{
		new_size = str->capacity * 2;
	}
	new_size = align(new_size);
	str->data=(char*)realloc(str->data, new_size);
	if (!str->data)
	{
		printf("内存分配失败\n");
		return;
	}
	str->capacity = new_size;
}

void charcat_dystring(DyString *str, char ch)
{
	int cnt = str->length + 1;
	if (cnt >= str->capacity)
	{
		realloc_dystring(str, str->capacity * 2);
	}
	str->data[str->length] = ch;
	str->length = cnt;
}

int align(int num)
{
	int align_size = 8;
	return ((num + align_size)&(~(align_size - 1)));
}