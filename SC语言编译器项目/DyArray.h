#ifndef DYARRAY_H
#define DYARRAY_H

//动态数组定义
typedef struct DyArray
{
	int count;		//动态数组元素个数
	int capacity;	//动态数组的缓冲区大小
	void **data;	//指向数据的指针数组（也就是说，data这块内存存放的是指针数据）
}DyArray;


//函数声明

//初始化动态数组
void init_dyarray(DyArray *da, int initsize);

//给动态数组插入数据
void insert_dyarray(DyArray *da, void *data);

//重新扩大内存
void remalloc_dyarray(DyArray *da);

//释放动态数组中数据（记得仅仅只有malloc的空间才用资格释放，像静态内存是无法释放的）
void free_dyarray(DyArray *da);

//分配的大小要对齐（就是每一次分配的大小，我设计的就是都是8的倍数）
extern int align(int num);


#endif
