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
//终结符，非终结符，表达式的个数(一定要小于MAXVTPNUM)
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

	//为文法G中的终结符和非终结符构造hash表
	establish_hash(Vstring, Vnum,VNode);
	establish_hash(Tstring, Tnum, TNode);

	printf("*******************************\n");

	//测试FIRST算法
	//初始化VTfirst集合
	init_first_set(Tstring,Vstring);

	//根据文法的表达式来计算FIRST集合
	get_first(Vstring, Pstring);
	display_first(VTfirst, Tnum + Vnum);
	
	printf("*******************************\n");

	//测试FOLLOW算法
	init_follow_set(Vstring,S);
	get_follow(Pstring);
	display_follow(Vfollow, Vnum);
	return 0;
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

//返回值：'#'结束标记->2 非终结符->1,终结符->0,空串->-1,不存在返回-2
//这个函数的问题：
//1.因为如果不是终结符就是非终结符，所以对于每一次调用
//仅仅调用一次即可。这里的函数第二个参数应该传入TNode
//2.我一开始是这样定义的：int find_vt(DyString *data, Node **arr);
//但是对于表达式来说，它可能仅仅就只需要检测部分符号，所以
//传入一个DyString不太好处理
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

void get_first(DyString *Vstring, DyString *Pstring)
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
								is_contain_empty=find_string(VTfirst[curindex].data, ch4);
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
			memset(dst, 0, MAXCHARNUM);//清除刚刚拷贝的东西
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
				memset(dst, 0, MAXVTPNUM);//清除刚刚拷贝的东西
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
	char ch[2];
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
			*flag = 1;
			*first_follow_flag = 1;
			strcat_dystring(dst, ch1);
		}
	}
	return is_null;
}

int find_string(const DyString *strA, const char *strB)
{
	int tmp_flag = 0;
	for (int i = 0; i < strA->length; i++)
	{
		char ch[MAXCHARNUM];
		memset(ch, 0, MAXCHARNUM);
		int n = get_complete_vt(ch, strA->data, i, strA->length - 1);
		i += n - 1;
		if (!strcmp(ch, strB))
		{
			tmp_flag = 1;
			break;
		}
	}
	return  tmp_flag;
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
						if (!find_string(Vfollow[curindex].data, nextchar))
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
					last_character_is_end = 0;
					for (int i = 0; i < VTfirst[index].data->length; i++)
					{
						//虽然first集合里面有像id这样的非终结符，但是
						//'@'(空字符串仅仅只有一个)，所以就直这样比较了
						if (VTfirst[index].data->data[i] == '@')
						{
							last_character_is_end = 1;
							is_null = 1;
							break;
						}
					}
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
