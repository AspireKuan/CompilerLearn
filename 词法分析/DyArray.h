#ifndef DYARRAY_H
#define DYARRAY_H

//��̬���鶨��
typedef struct DyArray
{
	int count;		//��̬����Ԫ�ظ���
	int capacity;	//��̬����Ļ�������С
	void **data;	//ָ�����ݵ�ָ�����飨Ҳ����˵��data����ڴ��ŵ���ָ�����ݣ�
}DyArray;


//��������

//��ʼ����̬����
void init_dyarray(DyArray *da, int initsize);

//����̬�����������
void insert_dyarray(DyArray *da, void *data);

//���������ڴ�
void remalloc_dyarray(DyArray *da);

//�ͷŶ�̬���������ݣ��ǵý���ֻ��malloc�Ŀռ�����ʸ��ͷţ���̬�ڴ����޷��ͷŵģ�
void free_dyarray(DyArray *da);

//����Ĵ�СҪ���루����ÿһ�η���Ĵ�С������Ƶľ��Ƕ���8�ı�����
extern int align(int num);


#endif
