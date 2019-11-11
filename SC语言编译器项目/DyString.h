#ifndef DYSTRING_H
#define DYSTRING_H

typedef struct DyString
{
	int length;
	int capacity;
	char *data;
}DyString;


//初始化字符串
int init_dystring(DyString *str, int initsize);

//初始化字符数组

//释放字符串的空间
void free_dystring(DyString *str);

//重置字符串
void reset_dystring(DyString *str);

//重新分配字符串空间
void realloc_dystring(DyString *str, int new_size);

//追加当个字符到字符串中
void charcat_dystring(DyString *str, char ch);

//追加一个char*到字符串中
void strcat_dystring(DyString *dst, char *src);

//分配的大小要对齐（就是每一次分配的大小，我设计的就是都是8的倍数）
int align(int num);

#endif


