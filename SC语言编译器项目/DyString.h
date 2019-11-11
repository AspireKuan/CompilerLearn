#ifndef DYSTRING_H
#define DYSTRING_H

typedef struct DyString
{
	int length;
	int capacity;
	char *data;
}DyString;


//��ʼ���ַ���
int init_dystring(DyString *str, int initsize);

//��ʼ���ַ�����

//�ͷ��ַ����Ŀռ�
void free_dystring(DyString *str);

//�����ַ���
void reset_dystring(DyString *str);

//���·����ַ����ռ�
void realloc_dystring(DyString *str, int new_size);

//׷�ӵ����ַ����ַ�����
void charcat_dystring(DyString *str, char ch);

//׷��һ��char*���ַ�����
void strcat_dystring(DyString *dst, char *src);

//����Ĵ�СҪ���루����ÿһ�η���Ĵ�С������Ƶľ��Ƕ���8�ı�����
int align(int num);

#endif


