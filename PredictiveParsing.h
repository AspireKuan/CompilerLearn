#ifndef PREDICTIVEPARSING_H
#define PREDICTIVEPARSING_H

typedef struct Node
{
	DyString *data;		
	Node *next;//Ϊ���hash��ͻ���ڵ�ָ��
}Node;


typedef struct First_set
{
	char *src;//ȷ������һ���ս�����߷��ս����first���ϡ���Ϊ��id�����ķ��ս���п��ܲ�ֹһ���ַ�������ʹ��char*����������
	int flag;//�������һ���Ƿ���¹�
	DyString *data;//ʵ��first���ϵ�����
}First_set;

typedef struct Follow_set
{
	char ch;//ȷ������һ�����ս����FOLLOW����
	int flag;//�������һ���Ƿ���¹�
	DyString *data;//ʵ�ʵ�follow���ϵ�����
}Follow_set;

//���е��ս�������ս�������ʽ����ʼ���Ŷ��ǲ���DyString
//��ʼ��������һ���ķ�G=(V,T,P,S),x����(V��T)
//������ý���һ������ķ�������ָ���ָ�룬����ʹ�ö��DyString *str�����ڴ�������
void init(DyString *Vstring,DyString *Tstring,DyString *Pstring,DyString *S);

//����Ӧ�ļ��ϵ�Ԫ�ش��������н��գ���ŵ���Ӧ�ļ�����
void init_VTPset(DyString *dystring,int *i);

//Ϊ�ս���ͷ��ս������һ��hash�������ڼ��ٲ���
//��Ϊstr��һ�����飬����num������Ԫ�صĸ���
void establish_hash(DyString *str,int num,Node **arr);

//Linux�е�hash�����������LexicalAnalsis.c��Ҳ�����ˣ�
int elf_hash(char *key);

//ͨ��hash����ٲ�������ķ����ŵ������ս�����Ƿ��ս��
int find_vt(char *str);

//��ʼ��VTfirsts����
void init_first_set(DyString *Tstring,DyString *Vstring);

//����ÿһ�����ʽ����FIRST����
void get_first(DyString *VString,DyString *Pstring);

//�ҵ�һ���ķ�������First����(VFirst)���±�
int find_first_index(const char *val);

//�õ�һ���������ķ����ţ���Ҫ���ս��������Ϊ��id�����ӵ���һ��
//�ý�����ȫ������������ֵ�������ַ��ĳ��ȣ�����֮��ָ���ƶ�
int get_complete_vt(char *dst, const char *src, const int start, int end);

//��ӡ�ս���ͷ��ս����FIRST����
void display_first(First_set arr[], int num);

//��ʼ��Follow��������
void init_follow_set(DyString *Vstring, DyString *S);

////�ҵ�һ�����ս������FoLLOW�����±�
int find_follow_index(const char val);

//�õ�һ�����ս������Ϊ�����get_follow()��ȥ�ж����ս����
char get_variable(char *src, int *sindex);

//��һ���ַ���A�����ַ���B�Ƿ����
//��ʵ���������copy_follow_first()�����е��ظ���
//�����ڸ��Ƶ����ս����ʱ�򣬾͵�Ҫ�ж�һ���Ƿ��ظ�
//�����һ�����������Ѱ��strA���Ƿ��пմ�
int find_string(const DyString *strA, const char *srtB);

//����ֵ��1���������src���пմ���0����src��û�пմ�
//flag���ж��Ƿ�ʵ���˸���Ԫ�أ�1��ʾ�����ˣ�0��ʾû�и���
int copy_follow_first(DyString *dst, DyString *src,int *flag,int *first_follow_flag);

//�õ��������ս����Follow����
void get_follow(DyString *Pstring);

//��ӡFOLLOW�����е�Ԫ��
void display_follow(Follow_set arr[], int num);

#endif
