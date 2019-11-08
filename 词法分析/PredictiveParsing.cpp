#include"pch.h"
#include"DyString.h"
#include"PredictiveParsing.h"
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<stdlib.h>

#define MAXCHARNUM 16		//每一个终结符，非终结符，表达式的最大长度
#define HASHSIZE 512		//hash表的大小
#define MAXVTPNUM 32		//所有终结符，或非终结符，或表达式的个数
#define EMPTYSTRING '@'		//使用‘@’来代替空串

//全局变量
//非终结符和终结符的hash表
Node *VNode[HASHSIZE];
Node *TNode[HASHSIZE];
//非终结符，终结符，表达式的个数(一定要小于MAXVTPNUM)
int Vnum, Tnum, Pnum;
//文法符号的FIRST集合
First_set VTfirst[MAXVTPNUM];
//每个非终结符的FOLLOW集合
Follow_set Vfollow[MAXVTPNUM];

int main()
{
	DyString *Vstring = (DyString*)malloc(sizeof(DyString) * MAXVTPNUM);
	DyString *Tstring = (DyString*)malloc(sizeof(DyString) * MAXVTPNUM);
	DyString *Pstring = (DyString*)malloc(sizeof(DyString) * MAXVTPNUM);
	DyString* S = (DyString*)malloc(sizeof(DyString));//因为开始符号仅仅只有一个
	if (!Vstring || !Tstring || !Pstring)
	{
		printf("malloc error\n");
		exit(-1);
	}
	

	//接收文法G
	init(Vstring, Tstring, Pstring, S);

	//为文法G中的非终结符和终结符构造hash表
	establish_hash(Vstring, Vnum,VNode);
	establish_hash(Tstring, Tnum, TNode);

	printf("\n*******************************\n");

	//测试FIRST算法
	//初始化VTfirst集合
	init_first_set(Tstring,Vstring);

	//根据文法的表达式来计算FIRST集合
	//get_first(Pstring);
	get_first2(Pstring);
	display_first(VTfirst, Tnum + Vnum);
	
	printf("\n*******************************\n");

	//测试FOLLOW算法
	init_follow_set(Vstring,S);
	get_follow(Pstring);
	display_follow(Vfollow, Vnum);

	printf("\n*******************************\n");
	//测试判断LL(1)文法的算法
	if (is_LL1(Pstring,Vstring,Tstring, VTfirst, Vfollow))
	{
		printf("该文法是LL(1)文法\n\n");
		char*** analysis_table = init_analysis_table(Vnum, Tnum);
		create_predictive_analysis_table(analysis_table, Pstring, Pnum,Vstring, Vnum,Tstring,Tnum, VTfirst, Vfollow);
		display_analysis_table(analysis_table, Vstring,Vnum, Tstring,Tnum);
		printf("\n*******************************\n");
		
		//测试预测分析程序
		printf("请输入您要测试的句子：");
		char *input = (char*)malloc(sizeof(char)*MAXCHARNUM);
		memset(input, 0, sizeof(char)*MAXCHARNUM);
		scanf("%s", input);
		input[strlen(input)] = '#';//给输入串添加上一个结束标记
		DyString *analysis_stack=init_stack(MAXVTPNUM, S);
		int result=LL1Parsing(input, analysis_table, analysis_stack,Vstring,Vnum,Tstring,Tnum);
		input[strlen(input)-1] = '\0';
		if (result)
		{
			printf("\n%s是该文法下的句子\n",input);
		}
		else
		{
			printf("\n%s不是该文法下的句子\n",input);
		}
	}
	return 0;
}

DyString *init_stack(int size,DyString *start)
{
	DyString *res = (DyString*)malloc(sizeof(DyString)*size);
	if (!res)
	{
		printf("%d:内存分配失败", __LINE__);
		exit(1);
	}
	//用第0个元素来当作整个栈的标记
	res[0].length = 1;//因为res[0]不能使用。
	res[0].capacity = size-1;
	res[0].data = NULL;
	for (int i = 1; i < size; i++)
	{
		init_dystring(res+i, MAXVTPNUM);
	}
	//同时将'#'和开始符号入栈
	DyString ends;
	init_dystring(&ends, 8);
	charcat_dystring(&ends, '#');
	push_stack(res, ends);
	push_stack(res, *start);
	return res;
}

void push_stack(DyString *stack, DyString val)
{
	if (stack[0].capacity == 0)
	{
		printf("%d:栈没有容量了\n", __LINE__);
		exit(1);
	}
	stack[stack[0].length] = val;
	stack[0].length++;
	stack[0].capacity--;
}

void pop_stack(DyString *stack)
{
	if (stack[0].length == 1)
	{
		printf("%d:没有元素，无法出栈\n", __LINE__);
		exit(1);
	}
	stack[0].length--;
	stack[0].capacity++;
}

DyString gettop_stack(DyString *stack)
{
	if (stack[0].length == 1)
	{
		printf("%d:栈中没有元素，无法获得栈顶元素\n", __LINE__);
		exit(1);
	}
	return stack[stack[0].length-1];
}

int is_empty_stack(DyString *stack)
{
	if (stack[0].length == 1)
	{
		return 1;
	}
	return 0;
}

DyString charstr_to_DyString(char *src)
{
	int len = strlen(src);
	DyString res;
	init_dystring(&res, 8);
	strcat_dystring(&res, src);
	return res;
}

void init(DyString *Vstring, DyString *Tstring, DyString *Pstring, DyString *S)
{
	printf("请输入文法的非终结符集合：");
	init_VTPset(Vstring,&Vnum);
	printf("请输入文法的终结符结合：");
	init_VTPset(Tstring,&Tnum);
	printf("请输入文法的表达式集合：\n");
	init_VTPset(Pstring,&Pnum);
	printf("请输入文法的开始符号：");
	char ch;
	scanf("%c", &ch);
	init_dystring(S, MAXCHARNUM);
	charcat_dystring(S, ch);
	return;
}

void init_VTPset(DyString *str,int *i)
{
	char ch;
	scanf("%c", &ch);
	while (ch != '\n')
	{
		init_dystring(&(str[*i]), MAXCHARNUM);
		while (ch != ' ' && ch !='\n')
		{
			charcat_dystring(&(str[*i]), ch);
			scanf("%c", &ch);
		}
		if (ch == ' ')
		{
			scanf("%c", &ch);
			(*i)++;
		}
	}
	//因为最后这个i是要个数的，我们习惯个数从1开始计算
	//所以，在返回的时候++;
	if (i)
	{
		(*i)++;
	}
	return;
}

void init_first_set(DyString *Tstring,DyString *Vstring)
{
	for (int i = 0; i < MAXVTPNUM; i++)
	{
		VTfirst[i].src = (char*)malloc(MAXCHARNUM);
		VTfirst[i].data = (DyString*)malloc(sizeof(DyString));
		if (!VTfirst[i].src || !VTfirst[i].data)
		{
			printf("内存分配失败\n");
			exit(-1);
		}
		VTfirst[i].flag = 0;
		memset(VTfirst[i].src, 0, MAXCHARNUM);
		memset(VTfirst[i].data, 0, sizeof(DyString));
		init_dystring(VTfirst[i].data, MAXVTPNUM);
	}
	//初始化所有终结符的first集合
	for (int i = 0; i < Tnum; i++)
	{
		VTfirst[i].src = Tstring[i].data;
		for (int j = 0; j < Tstring[i].length; j++)
		{
			charcat_dystring(VTfirst[i].data, Tstring[i].data[j]);
		}
	}
	//初始化所有非终结符集合
	for (int i = Tnum,j=0; i < Vnum+Tnum; i++,j++)
	{
		VTfirst[i].src = Vstring[j].data;
	}
	return;
}

void establish_hash(DyString *str,int num,Node **arr)
{
	int hashkey = 0;
	Node *node =NULL;
	for (int i=0; i < num; i++)
	{
		node = (Node*)malloc(sizeof(Node));
		hashkey = elf_hash(str[i].data);
		node->data = &str[i];
		node->next = arr[hashkey];
		arr[hashkey] = node;
	}
}

int elf_hash(char *key)
{
	int h = 0;
	int g = 0;
	while (*key)
	{
		h = (h << 4) + (*key++);
		g = h & 0xf0000000;
		if (g)
		{
			h ^= g >> 24;
		}
		h &= ~g;
	}
	return h % HASHSIZE;
}

int find_vt(char *str)
{
	const char ch1[2] = { '@','\0' };
	const char ch2[2] = { '#','\0' };
	//因为空串既不属于终结符也不属于非终结符
	if (!strcmp(str, ch1))
	{
		return -1;//代表是一个空串
	}
	if (!strcmp(str, ch2))
	{
		return 2;
	}
	int keyhash = elf_hash(str);
	//看看是不是非终结符
	if(VNode[keyhash])
	{
		for (Node *p = VNode[keyhash]; p; p = p->next)
		{
			if (!strcmp(p->data->data, str))
			{
				return 1;
			}
		}
		
	}
	//看看是不是终结符
	if (TNode[keyhash])
	{
		for (Node *p = TNode[keyhash]; p; p = p->next)
		{
			if (!strcmp(p->data->data, str))
			{
				return 0;
			}
		}
	}
	//证明该文法符号不存在
	return -2;
}

int get_first_candidate(int curindex,First_set *first_set,int *flag,const char *str)
{
	int ans = 0;//默认是str无法推导出空串
	char nextchar[MAXVTPNUM];//nextchar[]就是当前产生式的可能FIRST元素
	int nextindex = 0;
	int len = strlen(str);
	int offset = 0;
	int result = 0;//标识现在curchar是一个终结符还是非终结符
	for (int i = 0; i<len; i++)
	{
		memset(nextchar, 0, MAXVTPNUM);
		offset =get_complete_vt(nextchar, str, i, len - 1);
		//test bug
		if (offset == -1)
		{
			printf("curindex:%d", curindex);
			printf("%d 出现问题", __LINE__);
			system("pause");
		}
		i += offset - 1;
		//根据result分析这个文法符号的情况
		result = find_vt(nextchar);
		
		//非终结符的情况
		if (result == 1)
		{
			nextindex = find_first_index(nextchar);
			//证明现在这个非终结符first集合里面有数据,并且增加过元素
			//if (first_set[nextindex].data->length != 0 && first_set[nextindex].flag)
			if (first_set[nextindex].flag)
			{
				//看看first_set[index].data是否可以推导出'@'
				int is_null = 0;
				is_null = copy_follow_first(first_set[curindex].data, first_set[nextindex].data, flag, &first_set[curindex].flag);
				//如果不能推导出空串,结束该产生式的扫描
				if (!is_null)
				{
					ans = 0;//因为可能被前一个非终结符该成ans=1了
					return ans;
				}
				//如果能推导出空串,先改变ans值，然后继续扫描
				else
				{
					ans = 1;
				}
			}
			//没有数据复制，但是还得先判断它可不可以推出空串
			else if (first_set[nextindex].data->length == 0 || first_set[nextindex].flag == 0)
			{
				//这里为什么要判断一下，见笔记
				//证明它也可以推出空串，所以还得继续扫描
				if (find_empty_string(first_set[nextindex].data->data) != -1)
				{
					ans = 1;
					continue;
				}
				ans = 0;
				return ans;
			}
		}
		//终结符的情况
		else if (result == 0)
		{
			if (find_string(first_set[curindex].data, nextchar)==-1)
			{
				//因为有可能有id这样的两个字符的终结符
				strcat_dystring(first_set[curindex].data, nextchar);
				//*flag = 1;
				flag !=NULL ? *flag= 1:NULL;
				first_set[curindex].flag = 1;
			}
			//在这里做出ans改变
			ans = 0;
			return ans;//因为这里说明式可以一个终结符为FIRST的，所以后面的也不用测试了
		}
		//空串情况
		else if (result == -1)
		{
			if (find_empty_string(first_set[curindex].data->data) == -1)
			{
				strcat_dystring(first_set[curindex].data, nextchar);
				//*flag = 1;
				flag != NULL ? *flag = 1 : NULL;
				first_set[curindex].flag = 1;
				ans = -1;
				return ans;
			}
			
		}
	}
	return ans;
}

int split_Pstring(char *dst, const char *src, char *split_str,int start)
{
	//在这个函数调用前，dst一定要被初始化
	int index = 0;
	//因为目前这个就是用来切割'|'的，没有想要扩展，所以下面就直接使用split_str[0]了
	for (int i = start; src[i]!='\0' && src[i]!=split_str[0]; i++,index++)
	{
		dst[index] = src[i];
	}
	dst[index] = '\0';//谨慎起见
	return index;
}

void get_first2(DyString *Pstring)
{
	char curchar[2];//产生式左边的非终结符
	int curindex = 0;//curchar在FIRST集合中的下标
	int flag = 1;//判断是不是有文法符号更新了FIRSR集合
	int changing_count = 0;//表示第几次扫描整个产生式。尤其是我的算法仅仅只需要第一次扫描的时候需要判断终结符和空串，之后的扫描仅仅只看非终结符
	char split_char[2] = { '|','\0' };
	while (flag)
	{
		flag = 0;
		changing_count++;
		for (int i = 0; i < Pnum; i++)
		{
			int is_null = 1;//该产生式是否可以推出空串，默认可以
			//获取产生式左部的一个非终结符；这里必须要保证非终结符仅仅只有一个字母表达式左部仅仅只有一个字符
			curchar[0] = Pstring[i].data[0];
			curchar[1] = '\0';
			curindex = find_first_index(curchar);

			//分割该产生式
			char subPstring[MAXVTPNUM];
			memset(subPstring, 0, MAXVTPNUM);
			//每一次开始循环的时候都是设置为没有更新first集合
			VTfirst[curindex].flag = 0;
			int start = 0;
			for (int j = 3; j < Pstring[i].length; j++)
			{
				start = j;
				j+=split_Pstring(subPstring, Pstring[i].data, split_char,start);
				is_null = get_first_candidate(curindex, VTfirst,&flag, subPstring);
				//证明这个产生式可以推出空串
				//因为每执行一次get_first_candidate()就是一个产生式
				if (is_null == 1)
				{
					if (find_empty_string(VTfirst[curindex].data->data) == -1)
					{
						charcat_dystring(VTfirst[curindex].data, '@');
					}
				}
			}
		}
	}
	return;
}

void get_first(DyString *Pstring)
{
	int flag = 1;//判断是不是有文法符号更新了FIRSR集合
	int curindex = 0;//curindex就是代表当前想要求FIRST集的文法符号在FIRST集合中的下标
	int nextindex = 0;//代表可以成为curFIRSR集合中的文法符号在FIRST集合中的下标
	int changing_count = 0;//表示第几次扫描整个产生式。尤其是我的算法仅仅只需要第一次扫描的时候需要判断终结符和空串，之后的扫描仅仅只看非终结符
	while (flag)
	{
		flag = 0;
		changing_count++;
		for (int i = 0; i < Pnum; i++)
		{
			//获取产生式左部的一个非终结符；这里必须要保证非终结符仅仅只有一个字母表达式左部仅仅只有一个字符
			char vch[2];
			vch[0] = Pstring[i].data[0];
			vch[1] = '\0';
		
			curindex = find_first_index(vch);
			//每一次开始循环的时候都是设置为没有更新first集合
			VTfirst[curindex].flag = 0;

			char tmp[MAXVTPNUM];
			memset(tmp, 0, MAXVTPNUM);
			int n = 1;
			//从产生式的右部的第一个文法符号开始
			for (int j = 3; j < Pstring[i].length; j++)
			{
				int n = get_complete_vt(tmp, (const char*)&Pstring[i].data[0], j, Pstring[i].length - 1);
				int result = find_vt(tmp);
				j += n - 1;//因为上面可能一次多读了几个字符，所以j的位置就得移动了

				//分析这个文法符号的情况
				//非终结符的情况
				if (result == 1)
				{
					nextindex = find_first_index(tmp);
					//证明现在这个非终结符first集合里面有数据,并且增加过元素
					if (VTfirst[nextindex].data->length != 0 && VTfirst[nextindex].flag)
					{
						//看看VTfirst[index].data是否可以推导出'@'
						int is_null = 0;
						is_null = copy_follow_first(VTfirst[curindex].data, VTfirst[nextindex].data, &flag, &VTfirst[curindex].flag);
						//如果不能推导出空串，就到找到vch下一个产生式
						if (!is_null)
						{
							while (Pstring[i].data[j] != '|' && j < Pstring[i].length)
							{
								j++;
							}
						}
						//能推导出空串
						else if (is_null)
						{
							//如果这个条件满足，说明该表达式可以推出一个空串
							if (j >= Pstring[i].length - 1)
							{
								int is_contain_empty = 0;//表示没有包含空串
								char ch4[2] = { '@','\0' };
								find_string(VTfirst[curindex].data, ch4) == -1 ?
									is_contain_empty = 0 : is_contain_empty = 1;
								//看看现在curindex里面有没有空串，没有就添加进去
								if (!is_contain_empty)
								{
									strcat_dystring(VTfirst[curindex].data, ch4);
								}
							}
							//否则的话就是去看下一个文法符号，也就是顶层for()j++做的事情
							continue;
						}
					}
					//没有数据，就到找到vch下一个产生式（下面的A就是vch）
					//例如：A->Bc|D,如果FIRST(B)暂时没有数据，就将i移到‘|’符号处
					else if (VTfirst[nextindex].data->length == 0 || VTfirst[nextindex].flag == 0)
					{
						while (Pstring[i].data[j] != '|' && j < Pstring[i].length)
						{
							j++;
						}
					}
				}
				//终结符的情况
				else if (result == 0)
				{
					//仅仅只有第一次扫描的时候需要考虑终结符
					if (changing_count == 1)
					{
						//因为有可能有id这样的两个字符的终结符
						strcat_dystring(VTfirst[curindex].data, tmp);
						flag = 1;
						VTfirst[curindex].flag = 1;
					}
					while (Pstring[i].data[j] != '|' && j < Pstring[i].length)
					{
						j++;
					}
				}
				//空串情况
				else if (result == -1)
				{
					if (changing_count == 1)
					{
						charcat_dystring(VTfirst[curindex].data, tmp[0]);
						flag = 1;
						VTfirst[curindex].flag = 1;
					}
					while (Pstring[i].data[j] != '|' && j < Pstring[i].length)
					{
						j++;
					}
				}
			}
		}
	}
	return;
}

int find_first_index(const char *val)
{
	for (int i = 0; i<Vnum+Tnum && VTfirst[i].src[0]!='\0'; i++)
	{
		//找到了刚刚所对应非终结符的first集合
		if (!strcmp(VTfirst[i].src, val))
		{
			return i;
		}
	}
	return -1;
}

int get_complete_vt(char *dst, const char *src,const int start,int end)
{
	//证明是从前往后复制获取一个完整的文法符号
	int n = 1;
	if(start <=end)
	{
		int i = start;
		strncpy(dst, &src[i], n);
		int result = find_vt(dst);
		//找到下一个文法符号
		//直到发现这个文法符号是终结符或者非终结符
		while (result == -2)
		{
			n++;
			//一般来说，不能出现这样的情况。如果有的话可能就是src没有包含了一些非文法符号的字符
			//但是写这个就是防止程序出现bug
			//因为n是长度，而end是一个下标,所以这里得先构造出一个长度出来
			if (n >end-start+1)
			{
				return -1;
			}
			memset(dst, 0, strlen(dst));//清除刚刚拷贝的东西
			strncpy(dst, &src[i], n);
			result = find_vt(dst);
		}
		return n;
	}
	//否则就是从后往前获取一个完整的的文法符号
	else
	{
		for (int i = start; i >=end; i--)
		{
			n = 1;
			strncpy(dst, &src[i], n);
			int result = find_vt(dst);
			while (result == -2)
			{
				n++;
				i--;
				memset(dst, 0, strlen(dst));//清除刚刚拷贝的东西
				strncpy(dst,src+i, n);
				result = find_vt(dst);
			}
			break;
		}
		return n;
	}
}

void display_first(First_set arr[], int num)
{
	for (int i = 0; i < num; i++)
	{
		printf("%s   的FIRST集合是{ ",arr[i].src);
		int n = 0;
		char tmp[MAXCHARNUM];
		for (int j = 0; j < arr[i].data->length; j++)
		{
			memset(tmp, 0, sizeof(char)*MAXCHARNUM);
			n = get_complete_vt(tmp, arr[i].data->data, j, arr[i].data->length);
			j = j + n - 1;
			//控制格式打印
			j == arr[i].data->length - 1 ?
				printf("%s", tmp) : printf("%s,", tmp);
		}
		printf(" }\n");
	}
	return;
}

void init_follow_set(DyString *Vstring, DyString *S)
{
	for (int i = 0; i < Vnum; i++)
	{
		Vfollow[i].ch = Vstring[i].data[0];//因为我默认非终结符仅仅只能有一个字符
		Vfollow[i].data = (DyString*)malloc(sizeof(DyString));
		if (!Vfollow[i].data)
		{
			printf("内存分配失败\n");
			exit(1);
		}
		init_dystring(Vfollow[i].data, MAXCHARNUM);
		//因为对于FOLLOW集合来说，我们一开始需要的是他们的FIRST集合，所以一开始flag默认都是1
		Vfollow[i].flag = 1;
		//说明是开始符号，首先添加一个'#'到FOLLOW集合中去
		if (Vfollow[i].ch == S->data[0])
		{
			charcat_dystring(Vfollow[i].data, '#');
		}
	}
	return;
}

int find_follow_index(const char val)
{
	for (int i = 0; i < Vnum; i++)
	{
		if (Vfollow[i].ch == val)
		{
			return i;
		}
	}
	return -1;
}

char get_variable(char *src, int *sindex)
{
	char ch[2] = { '\0','\0' };
	while (*sindex >= 0)
	{
		memset(ch, 0, 2);
		ch[0] = src[* sindex];
		if (find_vt(ch) == 1)
		{
			return src[*sindex];
		}
		(*sindex)--;
	}
	//一般来说不会走到这一步
	return ch[0];
}

int copy_follow_first(DyString *dst, DyString *src, int *flag, int *first_follow_flag)
{
	int is_null = 0;
	int tmp_flag = 1;
	for (int k1 = 0; k1 < src->length; k1++)
	{
		tmp_flag = 0;
		char ch1[MAXCHARNUM];
		memset(ch1, 0, MAXCHARNUM);
		int n1 = get_complete_vt(ch1, src->data, k1, src->length - 1);
		k1 += n1 - 1;
		if (ch1[0] == '@')
		{
			is_null = 1;
			continue;
		}
		for (int k2 = 0; k2 < dst->length; k2++)
		{
			char ch2[MAXCHARNUM];
			memset(ch2, 0, MAXCHARNUM);
			int n2 = get_complete_vt(ch2, dst->data, k2, dst->length - 1);
			k2 += n2 - 1;
			if (!strcmp(ch1, ch2))
			{
				tmp_flag = 1;
				break;
			}
		}
		if (!tmp_flag)
		{
			//*flag = 1;
			flag != NULL ? *flag = 1 : NULL;
			*first_follow_flag = 1;
			strcat_dystring(dst, ch1);
		}
	}
	return is_null;
}

int find_string(const DyString *strA, const char *strB)
{
	int tmp_flag = -1;
	for (int i = 0; i < strA->length; i++)
	{
		char ch[MAXCHARNUM];
		memset(ch, 0, MAXCHARNUM);
		int n = get_complete_vt(ch, strA->data, i, strA->length - 1);
		while (n == -1)
		{
			i++;
			if (i >= strA->length)
			{
				printf("%d：无法处理字符串：%s", __LINE__,strA->data);
				return tmp_flag;
			}
			memset(ch, 0, sizeof(char)*MAXCHARNUM);
			n = get_complete_vt(ch, strA->data, i, strA->length - 1);
		}
		i += n - 1;
		if (!strcmp(ch, strB))
		{
			tmp_flag = i;
			break;
		}
	}
	return  tmp_flag;
}

int find_empty_string(const char *str)
{
	char empty= '@';
	for (int i = 0; str[i]!='\0'; i++)
	{
		if (empty == str[i])
		{
			return i;
		}
	}
	return -1;
}

void get_follow(DyString *Pstring)
{
	int flag = 1;//判断是不是有文法符号更新了FOLLOW集合
	int changing_count = 0;//和get_first()中的含义一样
	char curch;//curcahr为当前产生式中准备求的FOLLOW(curchar)
	int curindex = 0;//curindex为当前非终结符在Vfollow[]中的下标
	char nextchar[MAXCHARNUM];//为curchar后面一个文法符号，因为有可能是终结符，所以使用了一个数组
	int nextindex = 0;//nextindex为curindex的后面一个文法符号在FOLLOW集合或者FIRST集合中的下标
	while (flag)
	{
		changing_count++;
		flag = 0;
		for (int i = 0; i <Pnum; i++)
		{
			char vch = Pstring[i].data[0];//表达式的左部
			int last_character_is_end = 1;//0就是不可以，1就是可以
			int is_null = 0;//0代表nextchar中没有空串，反之1代表有空串

			//这里写了一个魔数3，是因为输入的产生式中非终结符仅仅只能是一个字符
			for (int j = Pstring[i].length - 1; j >= 3; j--)
			{
				curch = get_variable(Pstring[i].data, &j);
				//下面的操作都是针对于产生式左部的
				if (j < 3)
				{
					continue;
				}
				curindex = find_follow_index(curch);
				//为什么是要changing_count > 1才执行这个操作，见你配套的思考笔记
				if (changing_count > 1)
				{
					Vfollow[curindex].flag = 0;
				}
				//如果j是该表达式最后一个下标 || curch的后面一个字符是'|',或者是'@'，那么说明curch是该产生式的最后一个字符
				if (j == Pstring[i].length - 1 || Pstring[i].data[j + 1] == '|' || Pstring[i].data[j + 1] == '@')
				{
					nextindex = find_follow_index(vch);
					last_character_is_end = 1;
					//应该要把Vfollow[nextindex]值复制给Vfollow[curindex]
					if (Vfollow[nextindex].data->length > 0)
					{
						copy_follow_first(Vfollow[curindex].data, Vfollow[nextindex].data,&flag, &Vfollow[curindex].flag);
					}
				}
				//否则就从j处往后面找到一个终结符或者非终结符
				else
				{
					memset(nextchar, 0, MAXCHARNUM);
					//因为curchar在产生式的下标式j,所以后面一个文法符号的下标就是j+1
					//进入这个else,就可以表达j+1肯定不为 '|' 了
					get_complete_vt(nextchar, Pstring[i].data, j + 1, Pstring[i].length - 1);
					nextindex = find_first_index(nextchar);
					int result = find_vt(nextchar);
					//curchar后面是一个非终结符
					if (result == 1)
					{
						
						//遍历nextchar的FIRST,curchar的FOLLOW,将非重复的非终结符复制过去;空字符串也不能要
						is_null = copy_follow_first(Vfollow[curindex].data, VTfirst[nextindex].data,&flag,&Vfollow[curindex].flag);
						//说明curchar可以成为该表达式最后一个非终结符
						if (is_null && last_character_is_end)
						{
							nextindex = find_follow_index(vch);
							copy_follow_first(Vfollow[curindex].data, Vfollow[nextindex].data,&flag, &Vfollow[curindex].flag);
						}
					}
					//curchar后面是一个终结符
					else if (result == 0 && changing_count==1)
					{
						if (find_string(Vfollow[curindex].data, nextchar)==-1)
						{
							flag = 1;
							strcat_dystring(Vfollow[curindex].data, nextchar);
						}
						
					}
				}
				
				//现在来设置curchar到底可不可以要下一个（是从右往左的方向，所以是左字符）字符为空
				//所以last_character_is_end现在代表的就是本字符
				//只有当上一字符可以为最后一个字符的时候要设置
				if (last_character_is_end && j>3)
				{
					char tmp_curch[2];
					memset(tmp_curch, 0, 2);
					tmp_curch[0] = curch;
					int index = find_first_index(tmp_curch);
					char empty_string[2] = { '@','\0' };
					
					find_string(VTfirst[index].data, empty_string) == -1 ?
						last_character_is_end = 0 : last_character_is_end= 1;
				}
			}
		}
	}
	return;
}

void display_follow(Follow_set arr[], int num)
{
	for (int i = 0; i < num; i++)
	{
		printf("%c   的FOLLOW集合是{ ", arr[i].ch);
		int n = 0;
		char tmp[MAXCHARNUM];
		for (int j = 0; j < arr[i].data->length; j++)
		{
			memset(tmp, 0, sizeof(char)*MAXCHARNUM);
			n = get_complete_vt(tmp, arr[i].data->data, j, arr[i].data->length);
			j = j + n - 1;
			//控制格式打印
			j == arr[i].data->length - 1 ?
				printf("%s", tmp) : printf("%s,", tmp);
		}
		printf(" }\n");
	}
	return;
}

int is_intersection(const char *strA, const char *strB)
{
	int father[256];
	memset(father, -1, sizeof(int) * 256);
	int flag = 0;//表示没有交集
	for (int i = 0; strA[i] !='\0'; i++)
	{
		father[strA[i]] = strA[i];
	}
	for (int i = 0; i < strB[i] != '\0'; i++)
	{
		if (father[strB[i]] != -1)
		{
			flag = 1;
			break;
		}
	}
	return flag;
}

int is_spilt(const char *str)
{
	char splitchar = '|';
	for (int i = 0; str[i] != '\0'; i++)
	{
		if (str[i] == splitchar)
		{
			return 1;
		}
	}
	return 0;
}

void get_string_first(First_set *cur_first_set,int curindex, const char *str, First_set *vtfirst)
{
	char nextchar[MAXVTPNUM];//nextchar[]就是当前产生式的可能FIRST元素
	int nextindex = 0;
	int len = strlen(str);
	int offset = 0;
	int result = 0;//标识现在curchar是一个终结符还是非终结符
	for (int i = 0; i < len; i++)
	{
		memset(nextchar, 0, MAXVTPNUM);
		offset = get_complete_vt(nextchar, str, i, len - 1);
		//test bug
		if (offset == -1)
		{
			printf("curindex:%d", curindex);
			printf("%d 出现问题", __LINE__);
			system("pause");
		}
		i += offset - 1;
		//根据result分析这个文法符号的情况
		result = find_vt(nextchar);
		//非终结符的情况
		if (result == 1)
		{
			nextindex = find_first_index(nextchar);
			//看看first_set[index].data是否可以推导出'@'
			int is_null = 0;
			is_null = copy_follow_first(cur_first_set[curindex].data, vtfirst[nextindex].data, NULL, &vtfirst[curindex].flag);
			//如果不能推导出空串,结束该产生式的扫描
			if (!is_null)
			{
				return;
			}
			//如果能推导出空串,继续扫描
			else
			{
				continue;
			}
		}
		//终结符的情况
		else if (result == 0)
		{
			
			strcat_dystring(cur_first_set[curindex].data, nextchar);
			return ;
		}
		//空串情况
		else if (result == -1)
		{
			strcat_dystring(cur_first_set[curindex].data, nextchar);
			return;
		}
	}
}

int is_LL1(DyString *Pstring,DyString *Vstring,DyString *Tstring,First_set *vtfirst,Follow_set *vfollow)
{
	char split_char[2] = { '|','\0' };
	int maxright = 8;//定义一个非终结符最多可以有8个右部
	char **res = (char**)malloc(sizeof(char*)*maxright);
	if (res == NULL)
	{
		printf("%d:内存分配失败\n",__LINE__);
		exit(1);
	}
	memset(res, 0, sizeof(char*)*maxright);
	for (int i = 0; i < maxright; i++)
	{
		res[i] = (char*)malloc(sizeof(char)*MAXVTPNUM);
		if (!res[i])
		{
			printf("%d:内存分配失败\n", __LINE__);
			exit(1);
		}
		memset(res[i], 0, sizeof(char)*MAXVTPNUM);
	}
	int index = 0;//可以分分解出来的下标
	int start = 3;//开始分解的下标，分解从右部开始
	int cnt = 0;//一个产生式分解出来的个数
	First_set first_set[2];//暂时用来存放两个产生式右部的FIRST集合
	for (int i = 0; i < 2; i++)
	{
		first_set[i].src = (char*)malloc(sizeof(char));
		memset(first_set[i].src, 0, sizeof(char));
		first_set[i].data = (DyString*)malloc(sizeof(DyString));
		memset(first_set[i].data, 0, sizeof(DyString));
		init_dystring(first_set[i].data, MAXVTPNUM);
		first_set[i].flag = 0;
	}
	for (int i = 0; i < Pnum; i++)
	{
		index = 0;
		start = 3;
		cnt = 0;
		char vch[2] = { '\0','\0' };//产生式左部非终结符
		vch[0] = Pstring[i].data[0];
		//首先判断左部FIRST集合和FOLLOW集合的
		if (find_empty_string(vtfirst[find_first_index(vch)].data->data)!=-1)
		{
			if (is_intersection(
				vtfirst[find_first_index(vch)].data->data,
				vfollow[find_follow_index(vch[0])].data->data))
			{
				printf("%c的FIRSR集合和FOLLOW集合在FIRST可以推出空串的前提下有交集", vch[0]);
				return 0;
			}
			
		}
		if (is_spilt(Pstring[i].data))
		{
			while (start < Pstring[i].length)
			{
				start += split_Pstring(res[index], Pstring[i].data, split_char, start) + 1;
				index++;
				cnt++;
			}
			for (int i = 0; i < cnt; i++)
			{
				//memset(&(first_set[0].data->data), 0, sizeof(DyString));
				//清空两个都要清空
				memset(first_set[0].data->data, 0, sizeof(char)*strlen(first_set[0].data->data));
				first_set[0].data->length = 0;
				get_string_first(first_set,0, res[i], vtfirst);
				for (int j = i + 1; j < cnt; j++)
				{
					memset(first_set[1].data->data, 0, sizeof(char)*strlen(first_set[1].data->data));
					first_set[1].data->length = 0;
					get_string_first(first_set, 1,res[j],vtfirst);
					if (is_intersection(first_set[0].data->data, first_set[1].data->data))
					{
						printf("FIRST(%s)和FIRST(%s)有交集\n", res[i], res[j]);
						return 0;
					}
				}
			}
		}
	}
	return 1;
}

char*** init_analysis_table(int vnum,int tnum)
{
	char ***ans = (char***)malloc(sizeof(char**)*vnum);
	if (!ans)
	{
		printf("%d:内存分配失败\n",__LINE__);
		exit(1);
	}
	memset(ans, 0, sizeof(char**)*vnum);
	for (int i = 0; i < vnum; i++)
	{
		//因为还有一个'#'还要添加一列
		ans[i] = (char**)malloc(sizeof(char*)*(tnum+1));
		if (!ans[i])
		{
			printf("%d:内存分配失败\n", __LINE__);
			exit(1);
		}
		memset(ans[i], 0, sizeof(char*)*(tnum+1));
		for (int j = 0; j < tnum+1; j++)
		{
			ans[i][j] = (char*)malloc(sizeof(char)*MAXCHARNUM);
			if (!ans[i][j])
			{
				printf("%d:内存分配失败\n", __LINE__);
				exit(1);
			}
			memset(ans[i][j], 0, sizeof(char)*MAXCHARNUM);

		}
	}
	return ans;
}

void create_predictive_analysis_table(char ***analysis_table,DyString *Pstring,int pnum,DyString *Vstring,int vnum,DyString *Tstring,int tnum,First_set *vtfirst,Follow_set *vfollow)
{
	char tmp_candidata[MAXCHARNUM];//存放暂时被分割的产生式(没有产生式左部)
	memset(tmp_candidata, 0, sizeof(char)*MAXCHARNUM);
	First_set tmp_candidata_first[1];//存放产生式的FIRST集合
	tmp_candidata_first[0].src = (char*)malloc(sizeof(char));
	tmp_candidata_first[0].flag = 0;
	tmp_candidata_first[0].data = (DyString*)malloc(sizeof(DyString));
	init_dystring(tmp_candidata_first[0].data,MAXCHARNUM);

	char split_str[2] = { '|','\0' };
	char left_candidate[2] = { '\0','\0' };//用来存放表达式左部非终结符
	char connect_str[3] = { '-','>','\0' };
	int n = 0;
	for (int i = 0; i < pnum; i++)
	{
		left_candidate[0] = Pstring[i].data[0];
		//清空之前的数据
		memset(tmp_candidata_first[0].data->data, 0, tmp_candidata_first[0].data->length);
		tmp_candidata_first[0].data->length = 0;
		if (is_spilt(Pstring[i].data))
		{
			for (int j = 3; j < Pstring[i].length; j++)
			{
				memset(tmp_candidata_first[0].data->data, 0, tmp_candidata_first[0].data->length);
				tmp_candidata_first[0].data->length = 0;
				n = split_Pstring(tmp_candidata, Pstring[i].data, split_str, j);
				j += n;
				//获取到候选式的first集合
				get_string_first(tmp_candidata_first, 0, tmp_candidata, vtfirst);
				if (find_empty_string(tmp_candidata_first->data->data) != -1)
				{
					//将候选式的左部的FOLLOW集合添加到分析表对应的位置上
					add_data_to_analysis_table(analysis_table, vfollow[find_follow_index(left_candidate[0])].data->data, left_candidate[0],tmp_candidata, Vstring, vnum, Tstring, tnum);
				}
				else
				{
					//将候选式FIRST加入到分析表的对应位置上
					add_data_to_analysis_table(analysis_table, tmp_candidata_first[0].data->data, left_candidate[0],tmp_candidata, Vstring, vnum, Tstring, tnum);
				}
			}
		}
		else
		{
			memset(tmp_candidata, 0, strlen(tmp_candidata));
			//获取到候选式的first集合
			get_string_first(tmp_candidata_first, 0, Pstring[i].data, vtfirst);
			split_Pstring(tmp_candidata, Pstring[i].data, connect_str, 3);
			if (find_empty_string(tmp_candidata_first->data->data) != -1)
			{
				//将候选式的左部的FOLLOW集合添加到分析表对应的位置上
				add_data_to_analysis_table(analysis_table, vfollow[find_follow_index(left_candidate[0])].data->data, left_candidate[0],tmp_candidata, Vstring, vnum, Tstring, tnum);
			}
			else
			{
				//将候选式FIRST加入到分析表的对应位置上
				add_data_to_analysis_table(analysis_table, tmp_candidata_first[0].data->data, left_candidate[0],tmp_candidata, Vstring, vnum, Tstring, tnum);
			}
		}
	}
}

void add_data_to_analysis_table(char ***analysis_table, const char *candidate_set, char left_canditate,const char *right_candidate,DyString *Vstring,int vnum,DyString *Tstring,int tnum)
{
	int x_index, y_index;//分别是left_candidate和right_candidate的在analysis_table中的横纵下标
	char x_string[2] = { '\0','\0' };
	x_string[0] = left_canditate;
	x_index = find_analysis_index(x_string,Vstring,vnum,Tstring,tnum);
	
	//当前在该FIRST集合中找到非终结符集合
	//因为一个非终结符可能是多个字符
	char y_string[MAXCHARNUM];
	memset(y_string, 0, sizeof(char)*MAXCHARNUM);
	int offset = 0;
	int len = strlen(candidate_set);
	for (int i = 0; i < candidate_set[i]!='\0'; i++)
	{
		offset=get_complete_vt(y_string, candidate_set, i, len - 1);
		i += offset - 1;
		y_index = find_analysis_index(y_string, Vstring, vnum,Tstring,tnum);
		//这个时候是因为这个y_string是一个空串，不应该在这里处理。
		if (y_index == -1)
		{
			continue;
		}
		strcpy(analysis_table[x_index][y_index], right_candidate);
	}
	return;
}

int find_analysis_index(char *str, DyString *Vstring, int vnum,DyString *Tstring,int tnum)
{
	char tmp[MAXCHARNUM];
	memset(tmp, 0, sizeof(char)*MAXCHARNUM);
	//在这里，没有必要使用get_complete_vt()函数的返回值
	//来测试一个改变i的值，而是每一次都是从0开始获取。
	//原因去看自己的对应想法
	for (int i = 0; i < vnum; i++)
	{
		memset(tmp, 0, sizeof(char)*strlen(tmp));
		//注意这个函数的参数0，是指每一次都是从Vstring[i].data起点处开始获取一个完整的终结符或者非终结符
		get_complete_vt(tmp, Vstring[i].data, 0, Vstring[i].length - 1);
		if (!strcmp(str, tmp))
		{
			return i;
		}
	}
	for (int i = 0; i < tnum; i++)
	{
		memset(tmp, 0, sizeof(char)*strlen(tmp));
		get_complete_vt(tmp, Tstring[i].data, 0, Tstring[i].length - 1);
		if (!strcmp(str, tmp))
		{
			return i;
		}
	}
	if (str[0] == '#')
	{
		return tnum;
	}
	return -1;
}

void display_analysis_table(char*** analysis_table, DyString *Vstring,int vnum, DyString *Tstring,int tnum)
{
	printf("V\\T |\t");
	for (int i = 0; i < tnum; i++)
	{
		printf("%s \t|\t", Tstring[i].data);
	}
	printf("#\t|\n");
	for (int i = 0; i <(tnum+1)*2; i++)
	{
		printf("--------");
	}
	printf("\n");
	for (int i = 0; i < vnum; i++)
	{
		printf("%c |\t", Vstring[i].data[0]);
		for (int j = 0; j < tnum+1; j++)
		{
			printf("%s \t|\t",analysis_table[i][j]);
		}
		printf("\n\n");
	}
	for (int i = 0; i < (tnum + 1) * 2; i++)
	{
		printf("--------");
	}
	printf("\n");
	return;
}

int LL1Parsing(char *input, char ***analysis_table,DyString *analysis_stack, DyString *Vstring, int vnum, DyString *Tstring, int tnum)
{
	int i = 0;
	DyString topchar;//符号栈栈顶的元素
	int x_index = 0,y_index = 0;//分别表示栈顶元素在分析表中的横坐标和纵坐标
	char tmpchar[MAXCHARNUM];//用于保存输入串中的一个完整的文法符号
	memset(tmpchar, 0, sizeof(char)*MAXCHARNUM);
	int offset = 0;
	while (input[i] != '\0')
	{
		topchar = gettop_stack(analysis_stack);
		memset(tmpchar, 0, sizeof(char)*strlen(tmpchar));
		offset = get_complete_vt(tmpchar, input, i, strlen(input) - 1);
		//栈顶元素和输出串最左未配备字符相等
		if (!strcmp(topchar.data, tmpchar))
		{
			pop_stack(analysis_stack);
			i += offset;
		}
		//不相等的话就得根据分析表来给符号栈进入元素
		//注意，你在分析表中得到的是一整个式子的右部，但是每一次入栈的元素只能是一个终结符或者非终结符
		else
		{
			x_index = find_analysis_index(topchar.data, Vstring, vnum, Tstring, tnum);
			y_index = find_analysis_index(tmpchar, Vstring, vnum, Tstring, tnum);
			char* tmp=analysis_table[x_index][y_index];
			//出现错误情况
			if (strlen(tmp) == 0)
			{
				return 0;
			}
			//首先将栈顶元素出栈
			pop_stack(analysis_stack);
			//然后把分析表中每一个完整文法符号入栈
			for (int j = strlen(tmp) - 1; j >= 0; j--)
			{
				memset(tmpchar, 0, strlen(tmpchar));
				offset = get_complete_vt(tmpchar, tmp, j, 0);
				j = j - offset + 1;
				//如果推导出的是空串，是不将其入栈的
				if (tmpchar[0] != '@')
				{
					push_stack(analysis_stack, charstr_to_DyString(tmpchar));
				}
			}
		}
	 }
	if (is_empty_stack(analysis_stack))
	{
		return 1;
	}
	else
	{
		return 0;
	}
	
}