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


//栈的初始化(本质上就是一个DyString的数组)
//size是这个栈的大小，start是开始符号
DyString *init_stack(int size, DyString *start);

//push 栈
void push_stack(DyString *stack, DyString val);

//pop 栈
void pop_stack(DyString *stack);

//get_top 栈
DyString gettop_stack(DyString *stack);

//判断栈是不是空
int is_empty_stack(DyString *stack);

//将char *转化为DyString类型
DyString charstr_to_DyString(char *src);

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
//返回值：'#'结束标记->2 非终结符->1,终结符->0,空串->-1,不存在返回-2
int find_vt(char *str);

//初始化VTfirsts数组
void init_first_set(DyString *Tstring,DyString *Vstring);

//根据每一个候选式来求FIRST集合，本质上get_first()就是多个候选式的组合
//curindex就是代表当前想要求FIRST集的文法符号在FIRST集合中的下标
//VTfirst就是用来存放first集合的地方
//flag是是否改变的标志，也就是说，是否更新了curindex的FIRST集合
//返回值:
//0:str不可以推导出空串
//1:str可以推导出空串
//-1:str本身就是空串
int get_first_candidate(int curindex, First_set *first_set,int *flag, const char *str);

//根据'|'来拆分产生式
//src的范围是[start,end);这里的end就是'|'或者就是产生式结尾
//返回值是所得到字串的长度
int split_Pstring(char *dst, const char *src, char *split_str, int start);

void get_first2(DyString *Pstring);

//根据每一个表达式来求FIRST集合
void get_first(DyString *Pstring);

//找到一个文法符号在First集合(VFirst)中下标
int find_first_index(const char *val);

//得到一个完整的文法符号（主要是终结符），因为像id这样子的是一个
//得将其完全辨别出来。返回值是完整字符的长度，便于之后指针移动
//src下标在[start,end]之间
//返回值：-1就是代表出错，>0就是strlen(dst)
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

//因为寻找空串这个操作比较频繁，那么以后寻找空串就使用下面这个函数
int find_empty_string(const char *str);

//返回值：1代表可以在src中有空串，0代表src中没有空串
//flag来判断是否实现了复制元素，1表示复制了，0表示没有复制
int copy_follow_first(DyString *dst, DyString *src,int *flag,int *first_follow_flag);

//得到各个非终结符的Follow集合
void get_follow(DyString *Pstring);

//打印FOLLOW集合中的元素
void display_follow(Follow_set arr[], int num);

//判断两个字符串有没有交集
int is_intersection(const char *strA, const char *strB);

//判断这个产生式可以不可以分割
int is_spilt(const char *str);

//利用已经求出来的单个文法符号的FIRST集合来求候选式的FIRST集合
//其实这个函数和get_first_candidate()差不多。这个是当初没有设计好
//str是要求的候选式，vtfirst是整个文法符号的first集合
void get_string_first(First_set *cur_first_set, int curindex, const char *str, First_set *vtfirst);

//判断是不是一个LL(1)文法
int is_LL1(DyString *Pstring, DyString *Vstring, DyString *Tstring, First_set *vtfirst, Follow_set *vfollow);

//初始化预测分析表
char*** init_analysis_table(int vnum, int tnum);

//构造预测分析表
/*
	analysis_table:分析表
	Pstring：用户输入的产生式,后面的xnum，就是他们的数量
	Vstring：用户输入的非终结符集合
	Tstring：用户输入的终结符结合
	vtfirst：所求出的终结符和非终结符的first集合
	vfollow：所求的非终结符的Follow集合
*/
void create_predictive_analysis_table(char ***analysis_table, DyString *Pstring, int pnum, DyString *Vstring, int vnum, DyString *Tstring, int tnum, First_set *vtfirst, Follow_set *vfollow);

//添加数据到预测分析表中
//first是对于某个候选式得出的FIRST集合
//index是这个候选式在这个集合中的下标（一般来说，我这就是0）
void add_data_to_analysis_table(char ***analysis_table, const char *candidate_set, char left_canditate, const char *right_candidate, DyString *Vstring, int vnum, DyString *Tstring, int tnum);

//确定终结符或者非终结符在分析表中的下标
int find_analysis_index(char *str, DyString *Vstring, int vnum, DyString *Tstring, int tnum);

//打印分析表
void display_analysis_table(char ***analysis_table, DyString *Vstring, int vnum, DyString *Tstring, int tnum);

//判断一个句子是不是可以由上面的文法推出来
int LL1Parsing(char *input, char ***analysis_table, DyString *analysis_stack, DyString *Vstring, int vnum, DyString *Tstring, int tnum);
#endif
