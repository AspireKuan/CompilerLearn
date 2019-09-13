#include "pch.h"
#include<stdio.h>
#include<string.h>
#include<stdlib.h>


//封装以下，可以增加安全性。不会出现任意的类型转化
//同时为了满足通用栈的设计
typedef struct OP_char
{
	char ch;
	struct OP_char *next;
}OP_char;

typedef struct Node
{
	int front;
	int rear;
	char ch;
	struct Node *next;
}Node;

//符号栈
typedef struct OP_Stack
{
	OP_char *top;
	OP_char *bottom;
}OP_Stack;

//NFA栈
typedef struct NFA_Stack
{
	Node *top;
	Node *bottom;
}NFA_Stack;

//状态集合
typedef struct state
{
	char *state_set;//状态集合
	int flag;//是否被访问过标志
	struct state* next;//指向下一个状态集合
}state;

state *Dstates = NULL;
//max_len是输入的正则表达式的长度，mxan是出现的状态的数目
const int max_len = 10;
const int maxn = 15;

//adj[x][y]='a';表示状态点x->状态y中间可以通过接收'a'来转化
//adj[x][y]=-1;表示状态点x->状态y可以直接转化
//adj[x][y]=0;表示状态点x->状态y不可以转化
int adj[maxn][maxn];



int test_op(char ch);
void init_OP_stack(OP_Stack **stack);
void init_NFA_stack(NFA_Stack **stack);
void push_op(OP_Stack *op_s, char ch);
void push_node(NFA_Stack *nfa_s, Node *tmp);
Node *pop_node(NFA_Stack *nfa_s);
OP_char *pop_op(OP_Stack *op_s);
OP_char *get_top(OP_Stack *op_s);
int empty_op(OP_Stack *s);
char *add_connection(char *str);
char *infixTOpostfix(char *str, OP_Stack *op_s);
int test_priority(char ch1, char ch2);
void merge_nfa(NFA_Stack *nfa_s, char ch, int *index);
void print_state(int adj[][maxn], int start);
void dfs(int adj[][maxn], int start, int *state);
void subset_construction(state *Dstates, char *str, int start);
state* find_no_flag_state(state *Dstates);
int find_state(state *Dstates, state *val);
state* closure_start(int start);
state* closure(state *p);
state* move(state *s, char ch);
char *dfs_get_state(int index, int condition);
void state_bubble_sort(char *str);
void swap(char *p, char *q);



int main()
{
	printf("Enter your regular experssion:");
	char *tmp1 = (char*)malloc(sizeof(char)*max_len);
	memset(tmp1, 0, sizeof(char)*max_len);
	scanf("%s", tmp1);
	char *tmp2 = add_connection(tmp1);
	free(tmp1);
	tmp1 = NULL;
	//这里要有两个栈，运算符栈和NFA栈
	OP_Stack *op_s = (OP_Stack*)malloc(sizeof(OP_Stack));
	NFA_Stack *nfa_s = (NFA_Stack*)malloc(sizeof(NFA_Stack));
	init_OP_stack(&op_s);
	init_NFA_stack(&nfa_s);
	//将中缀的正则表达式转化为后缀的正则表达式
	char *reg = infixTOpostfix(tmp2, op_s);
	free(tmp2);
	tmp2 = NULL;
	//开始处理后缀正则表达式
	int len = strlen(reg);
	//这个用来作为最开始的指向
	int start = 0;
	//用来生产状态的表示index
	//因为这个是大家共用的，所以要保证一个人改变了，其他
	//人都知道。所以下面使用指针转递（注意使用全局函数
	//没有这个效果。）

	int index = 0;
	for (int i = 0; i < len; i++)
	{
		//注意：这个for是你已经将正则表达式转化为后缀的
		//正则表达式了，所以，这里是没有'('和')'的
		//如果此时是一个运算符
		if (test_op(reg[i]))
		{
			merge_nfa(nfa_s, reg[i], &index);
		}
		//非运算符操作
		else
		{
			//创建NFA节点
			index++;
			Node *tmp = (Node*)malloc(sizeof(Node));
			tmp->front = index;
			index++;
			tmp->rear = index;
			tmp->ch = reg[i];
			//构建图
			adj[tmp->front][tmp->rear] = reg[i];
			//将刚刚生成的NFA节点入NFA栈
			push_node(nfa_s, tmp);
		}
	}

	//Test
	for (int i = 0; i < maxn; i++)
	{
		for (int j = 0; j < maxn; j++)
		{
			printf("%d\t", adj[i][j]);
		}
		printf("\n");
	}

	//现在NFA栈中唯一节点就是，我需要使用他来遍历这个图
	start = pop_node(nfa_s)->front;
	//print_state(adj, start);

	//初始化状态集合
	Dstates = (state*)malloc(sizeof(state));
	Dstates->next = NULL;
	Dstates->state_set = NULL;
	Dstates->flag = 1;//因为Dstates是一个带有头节点的链表，所以这里将头节点的访问标志设置为1
	subset_construction(Dstates, reg, start);
	//遍历集合链表
	for (state *p = Dstates->next; p != NULL; p = p->next)
	{
		printf("%s\n", p->state_set);
	}
	return 0;
}




//判断是不是运算符
int test_op(char ch)
{
	if (ch == '*' || ch == '|' || ch == '+' || ch == '(' || ch == ')')
	{
		return 1;
	}
	return 0;
}

void init_OP_stack(OP_Stack **stack)
{
	OP_char *tmp = (OP_char*)malloc(sizeof(OP_char));
	tmp->next = NULL;
	(*stack)->bottom = tmp;
	(*stack)->top = tmp;
}

void init_NFA_stack(NFA_Stack **stack)
{
	Node *tmp = (Node*)malloc(sizeof(Node));
	tmp->next = NULL;
	(*stack)->bottom = tmp;
	(*stack)->top = tmp;
}

void push_op(OP_Stack *op_s, char ch)
{
	OP_char *tmp = (OP_char*)malloc(sizeof(OP_char));
	tmp->ch = ch;
	tmp->next = op_s->top;
	op_s->top = tmp;
}

void push_node(NFA_Stack *nfa_s, Node *tmp)
{
	tmp->next = nfa_s->top;
	nfa_s->top = tmp;
}

Node *pop_node(NFA_Stack *nfa_s)
{
	if (nfa_s->top != nfa_s->bottom)
	{
		Node *res = nfa_s->top;
		nfa_s->top = nfa_s->top->next;
		return res;
	}
	return NULL;
}

OP_char *pop_op(OP_Stack *op_s)
{
	if (op_s->top != op_s->bottom)
	{
		OP_char *res = op_s->top;
		op_s->top = op_s->top->next;
		return res;
	}
	return NULL;
}

int empty_op(OP_Stack *s)
{
	if (s->bottom == s->top)
	{
		return 1;
	}
	return 0;
}

OP_char *get_top(OP_Stack *op_s)
{
	if (!empty_op(op_s))
	{
		return op_s->top;
	}
	return NULL;
}


//给正则表达式加上显式的连接符'+'
char *add_connection(char *str)
{
	int len = strlen(str);
	int i = 0, j = 0;
	char *ans = (char*)malloc(sizeof(char)*maxn);
	memset(ans, 0, sizeof(char)*maxn);
	while (i < len - 1)
	{
		ans[j] = str[i];
		j++;
		//两个字符连在一起，所以中间插入一个‘+’
		if (!test_op(str[i]) && !test_op(str[i + 1]))
		{
			ans[j] = '+';
			j++;
		}
		i++;
	}
	ans[j] = str[i];
	return ans;
}

char *infixTOpostfix(char *str, OP_Stack *op_s)
{
	int len = strlen(str);
	//这里肯定分配内存多了一点。因为转化为后缀正则的时候，就没有（）了
	char *ans = (char*)malloc(sizeof(char)*len);
	memset(ans, 0, sizeof(char)*len);
	int j = 0;
	for (int i = 0; i < len; i++)
	{
		//测试元素为运算符
		if (test_op(str[i]))
		{
			//栈顶元素非空
			if (!empty_op(op_s))
			{
				//如果测试元素是')'要特殊处理
				if (str[i] == ')')
				{
					while (get_top(op_s)->ch != '(')
					{
						OP_char *tmp = pop_op(op_s);
						ans[j] = tmp->ch;
						j++;
						free(tmp);
						tmp = NULL;
					}
					//弹出'('
					OP_char *tmp = pop_op(op_s);
					free(tmp);
					tmp = NULL;
				}
				//如果测试元素不是是')'
				else if (str[i] != ')')
				{
					int priority = test_priority(get_top(op_s)->ch, str[i]);
					//str[i]比栈顶元素的优先级低或者相等，就得不断出栈。然后自己入栈
					//所以priority=1，表示要入栈，=0表示要出栈
					if (!priority)
					{
						while (!priority)
						{
							OP_char *tmp = pop_op(op_s);
							ans[j] = tmp->ch;
							j++;
							free(tmp);
							tmp = NULL;
							//每次在对栈操作的时候，先检查栈是不是为空，然后再去操作，是一个好习惯
							//尤其是对于现在处于一个while之类的循环里面的时候，特别实用
							if (!empty_op(op_s))
							{
								priority = test_priority(get_top(op_s)->ch, str[i]);
							}
							else
							{
								priority = 1;
							}

						}
						push_op(op_s, str[i]);
					}
					//栈顶元素优先级高
					else if (priority)
					{
						push_op(op_s, str[i]);
					}
				}

			}
			//如果为空，则入栈
			else if (empty_op(op_s))
			{
				push_op(op_s, str[i]);
			}
		}
		//测试元素不为运算符
		else
		{
			ans[j] = str[i];
			j++;
		}
	}
	while (!empty_op(op_s))
	{
		OP_char *tmp = pop_op(op_s);
		ans[j] = tmp->ch;
		j++;
		free(tmp);
		tmp = NULL;
	}
	return ans;
}

//ch1为栈顶元素，ch2为测试元素
//ch2优先级高于ch1，return 1,表示ch2应该入栈
//'(' > '*' > '|' > '+' > ')'
int test_priority(char ch1, char ch2)
{
	switch (ch1)
	{
	case '(':
		return 1;
	case '*':
		if (ch2 == '(')
		{
			return 1;
		}
		return 0;
	case '|':
		if (ch2 == '(' || ch2 == '*')
		{
			return 1;
		}
		return 0;
	case '+':
		if (ch2 == '(' || ch2 == '*' || ch2 == '|')
		{
			return 1;
		}
		return 0;
	case ')':
		return 0;
	}
}

//合并现在NFA栈中前两个状态点
void merge_nfa(NFA_Stack *nfa_s, char ch, int *index)
{
	switch (ch)
	{
	case '|':
	{
		//因为是'|'所以创建一个节点
		(*index)++;
		Node *head = (Node*)malloc(sizeof(Node));
		head->front = *index;
		(*index)++;
		head->rear = *index;
		Node *tmp1 = pop_node(nfa_s);
		Node *tmp2 = pop_node(nfa_s);
		//改变图结构
		adj[head->front][tmp1->front] = -1;
		adj[head->front][tmp2->front] = -1;
		adj[tmp1->rear][head->rear] = -1;
		adj[tmp2->rear][head->rear] = -1;
		//释放刚刚pop出来的节点的空间
		free(tmp1);
		tmp1 = NULL;
		free(tmp2);
		tmp2 = NULL;
		push_node(nfa_s, head);
		break;
	}

	case '*':
	{
		//这里还是采用龙书上面的状态，但是其实我觉得是多余的
		(*index)++;
		Node *head = (Node*)malloc(sizeof(Node));
		head->front = *index;
		//将NFA栈中的两个节点出栈
		Node *tmp1 = pop_node(nfa_s);
		Node *tmp2 = pop_node(nfa_s);
		//改变图结构
		adj[head->front][tmp2->front] = -1;
		adj[tmp2->rear][tmp1->front] = -1;
		adj[tmp2->rear][tmp2->front] = -1;
		adj[head->front][tmp1->front] = -1;
		//将最后的节点入栈
		head->rear = tmp1->rear;
		//释放内存单元
		free(tmp1);
		tmp1 = NULL;
		free(tmp2);
		tmp2 = NULL;
		push_node(nfa_s, head);
		break;

	}
	case '+':
	{
		//这个运算符是不用创建状态的,反而要使index--
		//将NFA栈中的两个节点出栈
		Node *tmp1 = pop_node(nfa_s);
		Node *tmp2 = pop_node(nfa_s);
		//改变图的结构
		adj[tmp2->rear][tmp1->front] = tmp1->ch;
		//原来的关系就不能使用了，所以要给adj图复原
		adj[tmp1->front][tmp1->rear] = 0;
		(*index)--;
		tmp2->rear = tmp1->front;
		//释放内存单元
		free(tmp1);
		tmp1 = NULL;
		push_node(nfa_s, tmp2);
		break;
	}

	}
}

//打印NFA状态图(在我自己的策略之中，并没有使用状态0。这个之后我有
//其他用处)
void print_state(int adj[][maxn], int start)
{
	int index = 1;
	int visit[maxn];
	memset(visit, 0, maxn * sizeof(int));
	dfs(adj, start, &index);
}


void dfs(int adj[][maxn], int start, int *state)
{
	for (int i = 1; i < maxn; i++)
	{
		if (adj[start][i] != 0)
		{

		}
	}
}

//收集正则表达式的中的所有字符
char *getchar_set(char *str)
{
	int len = strlen(str);
	//这里考虑的是正则表达式最大的字符数量就是len个，可能没有len个。因为还有一些运算符
	char *character = (char*)malloc(sizeof(char)*(len + 1));
	memset(character, 0, sizeof(char)*(len + 1));
	int j = 0;
	//暂时开一个数组来bi'ai's
	int flag[128];
	memset(flag, 0, sizeof(int) * 128);
	for (int i = 0; i < len; i++)
	{
		if (!test_op(str[i]) && !flag[str[i]])
		{
			character[j] = str[i];
			j++;
			flag[str[i]] = 1;
		}
	}
	character[j] = '\0';
	//我觉得这里可以在返回之前得先将character排序
	return character;
}

//子集构造算法将NFA转化为DFA
//Dstates是整个状态集合，str是正则表达式，start是NFA开始状态
void subset_construction(state *Dstates, char *str, int start)
{
	//为了要Dstates最开始有一个状态，所以在这里首先将初始状态集合获取
	//首先获取NFA状态的
	state *s = closure_start(start);
	//将开始状态s链入Dstates
	s->next = Dstates->next;
	Dstates->next = s;
	//获取整个正则表达式中的符号(不是运算符)
	char *char_set = getchar_set(str);
	int len = strlen(char_set);
	//printf("NFA状态\t\t|DFA状态\t\t|");
	//for (int i = 0; i < len; i++)
	//{
	//	printf("%c\t|", char_set[i]);
	//}
	//printf("\n");
	while (state* p = find_no_flag_state(Dstates))
	{
		//将状态集合p设置为已经访问
		p->flag = 1;
		for (int i = 0; i < len; i++)
		{
			//其实这里的tmp2的内存要分配的多一点，不然的话后面的strcat()就会出现问题。
			//因为你得算法都是很精确的在分配内存，所以导致tmp2的内存不够
			state *tmp1 = move(p, char_set[i]);
			state *tmp2 = closure(tmp1);
			if (tmp1 != NULL)
			{
				strcat(tmp2->state_set, tmp1->state_set);
				free(tmp1);
				tmp1 = NULL;
			}

			if (!find_state(Dstates, tmp2))
			{
				tmp2->flag = 0;
				tmp2->next = Dstates->next;
				Dstates->next = tmp2;
			}
			//此时tmp2就是一个已经存在的状态，没有必要保存了，可以释放它的内存空间了
			else
			{
				free(tmp2);
				tmp2 = NULL;
			}
		}
	}
}

state* find_no_flag_state(state *Dstates)
{
	state *p = Dstates;
	for (; p != NULL; p = p->next)
	{
		if (!(p->flag))
		{
			return p;
		}
	}
	return NULL;
}

//查找状态val是否已在状态Dstates中
//存在返回1，不存在返回0
int find_state(state *Dstates, state *val)
{
	//Dstates是一个带有头节点的链表
	state *p = Dstates->next;
	//在查找状态的时候，不能仅仅看看这个字符串是否相等，因为有的状态集合中所有状态都是相等的，
	//只是排序不同而已。所以，我们要首先对这里的状态集合排序后比较
	state_bubble_sort(val->state_set);
	for (; p != NULL; p = p->next)
	{
		state_bubble_sort(p->state_set);
		if (strcmp(val->state_set, p->state_set) == 0)
		{
			return 1;
		}
	}
	return 0;
}

//注意针对于各种在“子集构造法”中的三个算法，一定要使用dfs来实现。因为针对于“空状态”而言，
//不管有没有对应的字符，都是可以直接到达的。
//对dfs_get_state()做出判断，其实这个也是一个好习惯。对于返回指针的函数，尤其是你还要使用这个返回
//指针的时候，一定要做是否为NULL的判断。这样子才是最安全的做法

//获取初始状态集合
state* closure_start(int start)
{
	state *ans = NULL;
	ans = (state*)malloc(sizeof(state));
	ans->flag = 0;
	ans->next = NULL;
	int len = 0;
	char *p = dfs_get_state(start, -1);
	//因为有可能开始状态没有直接的空状态，所以此时p==NULL
	if (p)
	{
		len = strlen(p);
	}
	//+1是为了保证start状态也可以加进去
	ans->state_set = (char*)malloc(sizeof(char)*(len + 1));
	memset(ans->state_set, 0, sizeof(char)*(len + 1));
	ans->state_set[0] = start + '0';
	if (p)
	{
		strcat(ans->state_set, p);
		free(p);
		p = NULL;
	}

	return ans;
}

//NFA状态转化。一个状态集合p，通过"空字符"可以得到的状态集合
//其实这里的代码是可以和上面的,下面的代码合并的。这个动作之后优化的时候执行
state* closure(state *p)
{
	state *ans = NULL;
	int len_state_set = strlen(p->state_set);
	//这里采用一个局部变量先多开一点内存，然后复制给ans->state_set就可以了
	//因为p的内存可以在这里释放掉，但是ans的内存要长久用，所以分配内存的时候要精确一点
	char *tmp = (char*)malloc(sizeof(char)*maxn);
	memset(tmp, 0, sizeof(char)*maxn);
	for (int i = 0; i < len_state_set; i++)
	{
		char *str = dfs_get_state(p->state_set[i] - '0', -1);
		//因为可能返回的是一个NULL,说明状态p->state_set[i]不能通过"空状态"转化
		if (str != NULL)
		{
			strcat(tmp, str);
			free(str);
			str = NULL;
		}
	}
	int len = strlen(tmp);
	ans = (state*)malloc(sizeof(state));
	ans->flag = 0;
	ans->next = NULL;
	//因为将来这里还要返回给上级的调用，还会有一些数据strcat()过来，所以这里并不仅仅分配len+1个内存
	//而是分配maxn+1个内存单元
	ans->state_set = (char*)malloc(sizeof(char)*(maxn + 1));
	memset(ans->state_set, 0, sizeof(char)*(maxn + 1));
	strcpy(ans->state_set, tmp);
	ans->state_set[maxn] = '\0';
	return ans;
}

//NFA状态转化。集合状态s通过ch可以转化的状态
state* move(state *s, char ch)
{
	int len_s = strlen(s->state_set);
	char *tmp = (char*)malloc(sizeof(char)*maxn);
	memset(tmp, 0, sizeof(char)*maxn);
	for (int i = 0; i < len_s; i++)
	{
		//因为现在状态是由“数字”和“小写字母”组成，所以这里还得判断一下状态
		int index = s->state_set[i];
		if (index >= 'a')
		{
			index = index - 'a' + 10;
		}
		else
		{
			index = index - '0';
		}
		char *str = dfs_get_state(index, ch);
		//因为可能返回的是一个NULL,说明状态p->state_set[i]不能通过"ch状态"转化
		if (str != NULL)
		{
			strcat(tmp, str);
			free(str);
			str = NULL;
		}

	}
	int len = strlen(tmp);
	state *ans = (state*)malloc(sizeof(state));
	ans->flag = 0;
	ans->next = NULL;
	ans->state_set = (char*)malloc(sizeof(char)*len + 1);
	memset(ans->state_set, 0, sizeof(char)*(len + 1));
	strcpy(ans->state_set, tmp);
	ans->state_set[len] = '\0';
	free(tmp);
	tmp = NULL;
	return ans;


}

char *dfs_get_state(int index, int condition)
{
	//这里就是一个保守估计,多以仅仅分配maxn/2+1的内存大小
	char *copy_state = (char*)malloc(sizeof(char)*(maxn / 2 + 1));
	memset(copy_state, 0, sizeof(char)*(maxn / 2 + 1));

	char *ans = (char*)malloc(sizeof(char)*(maxn / 2 + 1));
	memset(ans, 0, sizeof(char)*(maxn / 2 + 1));
	int len = 0;
	for (int i = 0; i < maxn; i++)
	{
		if (adj[index][i] == condition)
		{
			if (i + '0' > '9')
			{
				ans[len] = 'a' + (i + '0' - '9' - 1);
			}
			else
			{
				ans[len] = i + '0';
			}

			len++;
			//仅仅对于“空状态”来说需要递归调用这个函数，对于接收特定的字符的状态仅仅只调用一次
			if (condition == -1)
			{
				copy_state = dfs_get_state(i, condition);
				if (copy_state != NULL)
				{
					strcat(ans, copy_state);
					//释放内存
					free(copy_state);
					copy_state = NULL;
				}
				len = strlen(ans);
			}

		}
	}
	if (strlen(ans) == 0)
	{
		free(copy_state);
		copy_state = NULL;
		free(ans);
		ans = NULL;
		return NULL;
	}
	else
	{
		//至于这里可能会有一个隐含的bug,因为str值在最上层的dfs调用的时候可能会溢出
		//这个取决于你在上面的保守估计问题
		return ans;
	}
}


//字符串排序。采用冒泡排序算法实现
void state_bubble_sort(char *str)
{
	//lastIndex这是是一个记录最后需要交换顺序的边界点
	//这是为了克服有些有序，有些无序的元素
	int lastIndex = 0;
	/*
	因为IndexBound为num-1,j<IndexBound
	所以，j最大值为num-2;所以a[j+1]还是
	在数组下标还是没有越界
	*/
	int num = strlen(str);
	int IndexBound = num - 1;
	bool flag = true;
	for (int i = 0; i < num; i++)
	{
		flag = true;
		for (int j = 0; j < IndexBound; j++)
		{
			if (str[j] < str[j + 1])
			{
				swap(&str[j], &str[j + 1]);
				flag = false;
				lastIndex = j;
			}
		}
		IndexBound = lastIndex;
		if (flag)
			break;
	}
}

void swap(char *p, char *q)
{
	char tmp;
	tmp = *p;
	*p = *q;
	*q = tmp;
}