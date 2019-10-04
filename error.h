#ifndef ERROR_H
#define ERROR_H

#include<stdarg.h>

//���󼶱�
enum e_ErrorLevel
{
	LEVEL_WARNING,
	LEVEL_ERROR
};

//�����׶�
enum e_WorkStage
{
	STAGE_COMPILE,
	STAGE_LINK
};

//�쳣����׶�
void handle_exception(int stage, int level, char *fmt, va_list ap);

//���뾯�洦��
void warning(char *fmt, ...);

//��������������
void error(char *fmt, ...);

//��ʾ����
void expect(char *msg);

//��������c��ȡ��һ�����ʡ����統ǰ���ʲ���c����ʾ����
void skip(int c);

//ȡ�õ���v�������Դ���ַ���
char *get_tkstr(int v);

//���Ӵ�����
void link_error(char *fmt, ...);

#endif
