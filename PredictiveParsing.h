#ifndef PREDICTIVEPARSING_H
#define PREDICTIVEPARSING_H

typedef struct Node
{
	DyString *data;		
	Node *next;//为解决hash冲突存在的指针
}Node;


typedef struct First_set
{
	char *src;//确定是哪一个终结符或者非终结符的first集合。因为像id这样的非终结符有可能不止一个字符，所以使用char*来保存数据
	int flag;//看看最近一次是否更新过
	DyString *data;//实际first集合的数据
}First_set;

typedef struct Follow_set
{
	char ch;//确定是哪一个非终结符的FOLLOW集合
	int flag;//看看最近一次是否更新过
	DyString *data;//实际的follow集合的数据
}Follow_set;

//所有的终结符，非终结符，表达式，开始符号都是采用DyString
//初始化，接收一个文法G=(V,T,P,S),x属于(V并T)
//这里采用建立一个数组的方法不是指针的指针，而是使用多给DyString *str分配内存来做的
void init(DyString *Vstring,DyString *Tstring,DyString *Pstring,DyString *S);

//将对应的集合的元素从输入流中接收，存放到对应的集合中
void init_VTPset(DyString *dystring,int *i);

//为终结符和非终结符建立一个hash表，有利于加速查找
//因为str是一个数组，所以num是数组元素的个数
void establish_hash(DyString *str,int num,Node **arr);

//Linux中的hash函数（这个在LexicalAnalsis.c中也定义了）
int elf_hash(char *key);

//通过hash表快速查找这个文法符号到底是终结符还是非终结符
int find_vt(char *str);

//初始化VTfirsts数组
void init_first_set(DyString *Tstring,DyString *Vstring);

//根据每一个表达式来求FIRST集合
void get_first(DyString *VString,DyString *Pstring);

//找到一个文法符号在First集合(VFirst)中下标
int find_first_index(const char *val);

//得到一个完整的文法符号（主要是终结符），因为像id这样子的是一个
//得将其完全辨别出来。返回值是完整字符的长度，便于之后指针移动
int get_complete_vt(char *dst, const char *src, const int start, int end);

//打印终结符和非终结符的FIRST集合
void display_first(First_set arr[], int num);

//初始化Follow集合数组
void init_follow_set(DyString *Vstring, DyString *S);

////找到一个非终结符符号FoLLOW集合下标
int find_follow_index(const char val);

//得到一个非终结符（因为免得在get_follow()中去判断中终结符）
char get_variable(char *src, int *sindex);

//在一个字符串A中找字符串B是否存在
//其实这个函数和copy_follow_first()函数有点重复了
//但是在复制单个终结符的时候，就得要判断一下是否重复
//后来我还将其用在了寻查strA中是否含有空串
int find_string(const DyString *strA, const char *srtB);

//返回值：1代表可以在src中有空串，0代表src中没有空串
//flag来判断是否实现了复制元素，1表示复制了，0表示没有复制
int copy_follow_first(DyString *dst, DyString *src,int *flag,int *first_follow_flag);

//得到各个非终结符的Follow集合
void get_follow(DyString *Pstring);

//打印FOLLOW集合中的元素
void display_follow(Follow_set arr[], int num);

#endif
