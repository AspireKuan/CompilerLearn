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


//ջ�ĳ�ʼ��(�����Ͼ���һ��DyString������)
//size�����ջ�Ĵ�С��start�ǿ�ʼ����
DyString *init_stack(int size, DyString *start);

//push ջ
void push_stack(DyString *stack, DyString val);

//pop ջ
void pop_stack(DyString *stack);

//get_top ջ
DyString gettop_stack(DyString *stack);

//�ж�ջ�ǲ��ǿ�
int is_empty_stack(DyString *stack);

//��char *ת��ΪDyString����
DyString charstr_to_DyString(char *src);

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
//����ֵ��'#'�������->2 ���ս��->1,�ս��->0,�մ�->-1,�����ڷ���-2
int find_vt(char *str);

//��ʼ��VTfirsts����
void init_first_set(DyString *Tstring,DyString *Vstring);

//����ÿһ����ѡʽ����FIRST���ϣ�������get_first()���Ƕ����ѡʽ�����
//curindex���Ǵ���ǰ��Ҫ��FIRST�����ķ�������FIRST�����е��±�
//VTfirst�����������first���ϵĵط�
//flag���Ƿ�ı�ı�־��Ҳ����˵���Ƿ������curindex��FIRST����
//����ֵ:
//0:str�������Ƶ����մ�
//1:str�����Ƶ����մ�
//-1:str������ǿմ�
int get_first_candidate(int curindex, First_set *first_set,int *flag, const char *str);

//����'|'����ֲ���ʽ
//src�ķ�Χ��[start,end);�����end����'|'���߾��ǲ���ʽ��β
//����ֵ�����õ��ִ��ĳ���
int split_Pstring(char *dst, const char *src, char *split_str, int start);

void get_first2(DyString *Pstring);

//����ÿһ�����ʽ����FIRST����
void get_first(DyString *Pstring);

//�ҵ�һ���ķ�������First����(VFirst)���±�
int find_first_index(const char *val);

//�õ�һ���������ķ����ţ���Ҫ���ս��������Ϊ��id�����ӵ���һ��
//�ý�����ȫ������������ֵ�������ַ��ĳ��ȣ�����֮��ָ���ƶ�
//src�±���[start,end]֮��
//����ֵ��-1���Ǵ������>0����strlen(dst)
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

//��ΪѰ�ҿմ���������Ƚ�Ƶ������ô�Ժ�Ѱ�ҿմ���ʹ�������������
int find_empty_string(const char *str);

//����ֵ��1���������src���пմ���0����src��û�пմ�
//flag���ж��Ƿ�ʵ���˸���Ԫ�أ�1��ʾ�����ˣ�0��ʾû�и���
int copy_follow_first(DyString *dst, DyString *src,int *flag,int *first_follow_flag);

//�õ��������ս����Follow����
void get_follow(DyString *Pstring);

//��ӡFOLLOW�����е�Ԫ��
void display_follow(Follow_set arr[], int num);

//�ж������ַ�����û�н���
int is_intersection(const char *strA, const char *strB);

//�ж��������ʽ���Բ����Էָ�
int is_spilt(const char *str);

//�����Ѿ�������ĵ����ķ����ŵ�FIRST���������ѡʽ��FIRST����
//��ʵ���������get_first_candidate()��ࡣ����ǵ���û����ƺ�
//str��Ҫ��ĺ�ѡʽ��vtfirst�������ķ����ŵ�first����
void get_string_first(First_set *cur_first_set, int curindex, const char *str, First_set *vtfirst);

//�ж��ǲ���һ��LL(1)�ķ�
int is_LL1(DyString *Pstring, DyString *Vstring, DyString *Tstring, First_set *vtfirst, Follow_set *vfollow);

//��ʼ��Ԥ�������
char*** init_analysis_table(int vnum, int tnum);

//����Ԥ�������
/*
	analysis_table:������
	Pstring���û�����Ĳ���ʽ,�����xnum���������ǵ�����
	Vstring���û�����ķ��ս������
	Tstring���û�������ս�����
	vtfirst����������ս���ͷ��ս����first����
	vfollow������ķ��ս����Follow����
*/
void create_predictive_analysis_table(char ***analysis_table, DyString *Pstring, int pnum, DyString *Vstring, int vnum, DyString *Tstring, int tnum, First_set *vtfirst, Follow_set *vfollow);

//������ݵ�Ԥ���������
//first�Ƕ���ĳ����ѡʽ�ó���FIRST����
//index�������ѡʽ����������е��±꣨һ����˵���������0��
void add_data_to_analysis_table(char ***analysis_table, const char *candidate_set, char left_canditate, const char *right_candidate, DyString *Vstring, int vnum, DyString *Tstring, int tnum);

//ȷ���ս�����߷��ս���ڷ������е��±�
int find_analysis_index(char *str, DyString *Vstring, int vnum, DyString *Tstring, int tnum);

//��ӡ������
void display_analysis_table(char ***analysis_table, DyString *Vstring, int vnum, DyString *Tstring, int tnum);

//�ж�һ�������ǲ��ǿ�����������ķ��Ƴ���
int LL1Parsing(char *input, char ***analysis_table, DyString *analysis_stack, DyString *Vstring, int vnum, DyString *Tstring, int tnum);
#endif
