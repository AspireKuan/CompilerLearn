#include"pch.h"
#include"DyArray.h"
#include"LexicalAnalysis.h"
#include<stdio.h>
#include<stdlib.h>

void init_dyarray(DyArray *da, int initsize)
{
	//da传进来的时候一定已经分配空间过了,或者da->data已经分配过内存了
	//所以你在定义DyArray *da的时候，一定要da->data=NULL;
	if (da == NULL || da->data)
	{
		return;
	}
	if (initsize < 8)
	{
		initsize = 8;
	}
	else
	{
		initsize = align(initsize);
	}
	da->data = (void**)malloc(sizeof(void*)*initsize);
	if (da->data == NULL)
	{
		return;
	}
	da->capacity = initsize;
	da->count = 0;

}


void insert_dyarray(DyArray *da, void *data)
{
	int cnt = da->count + 1;
	//容量不足，要重新分配内存
	if (cnt >= da->capacity)
	{
		remalloc_dyarray(da);
	}
	da->data[cnt - 1] = data;
	da->count++;
}

void remalloc_dyarray(DyArray *da)
{
	int size = da->capacity + 8;
	da->data = (void**)realloc(da->data, size);
	da->capacity = size;
}


void free_dyarray(DyArray *da)
{
	void **p;
	for (p = da->data; da->count; --da->count, ++p)
	{
		if (*p)
		{
			free(*p);
			*p = NULL;
		}
	}
	free(da->data);
	da->data = NULL;
}