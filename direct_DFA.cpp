#include "pch.h"
#include<stdio.h>
#include<string.h>
#include<stdlib.h>

const int maxn = 15;


//符号栈
typedef struct OP_char
{
	char ch;
	struct OP_char *next;
}OP_char;

typedef struct Node
{
	//该节点的数据
	char ch;
	//左右孩子是为了构造抽象语法树时需要
	struct Node *lchild;
	struct Node *rchild;
	//next节点是为了栈的需要
	struct Node *next;
	//该节点的位置
	int position;
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

//str是后缀的正则表达式
Node *create_abstract_syntax_tree(char *str, OP_Stack *op_s, NFA_Stack *nfa_s);

int test_op(char ch);
void init_OP_stack(OP_Stack **stack);
void init_NFA_stack(NFA_Stack **stack);
void push_node(NFA_Stack *nfa_s, Node *tmp);
Node *pop_node(NFA_Stack *nfa_s);
void push_op(OP_Stack *op_s, char ch);
OP_char *pop_op(OP_Stack *op_s);
OP_char *get_top(OP_Stack *op_s);
int empty_op(OP_Stack *s);
char *add_connection(char *str);
char *infixTOpostfix(char *str, OP_Stack *op_s);
int test_priority(char ch1, char ch2);
void merge_node(char op, NFA_Stack *nfa_s);



int main()
{
	printf("Enter your regular:");
	char *p = (char*)malloc(sizeof(char)*maxn);
	memset(p, 0, sizeof(char)*maxn);
	scanf("%s", p);
	p[strlen(p)] = '#';
	char *add_p = add_connection(p);
	free(p);
	p = NULL;
	OP_Stack *op_s = (OP_Stack*)malloc(sizeof(OP_Stack));
	NFA_Stack *nfa_s = (NFA_Stack*)malloc(sizeof(NFA_Stack));
	init_OP_stack(&op_s);
	char *post_reg = infixTOpostfix(add_p, op_s);
	free(add_p);
	add_p = NULL;
	printf("%s", post_reg);
	Node *root = create_abstract_syntax_tree(post_reg, op_s, nfa_s);
	return 0;
}


//创建一个抽象的语法树
Node *create_abstract_syntax_tree(char *str, OP_Stack *op_s, NFA_Stack *nfa_s)
{
	int len = strlen(str);
	//为每一个非运算符节点构造的位置下标
	int position = 1;
	for (int i = 0; i < len; i++)
	{
		//该测试字符是运算符
		if (test_op(str[i]))
		{
			merge_node(str[i], nfa_s);
		}
		//该测试字符是非运算符
		else
		{
			Node *tmp = (Node*)malloc(sizeof(Node));
			tmp->ch = str[i];
			tmp->position = position;
			position++;
			tmp->lchild = NULL;
			tmp->rchild = NULL;
			tmp->next = NULL;
			push_node(nfa_s, tmp);
		}
	}
	return pop_node(nfa_s);
}

void merge_node(char op, NFA_Stack *nfa_s)
{
	switch (op)
	{
	case '*':
	{
		Node *tmp = pop_node(nfa_s);
		//构建'*'节点
		Node *p = (Node*)malloc(sizeof(Node));
		p->ch = op;
		//因为'*'运算符的特殊性，所以就仅仅只使用了一个孩子节点信息
		p->lchild = tmp;
		p->rchild = NULL;
		p->position = 0;
		push_node(nfa_s, p);
		break;
	}
	//因为'+'和'|'其实是差不多的
	default:
	{
		Node *tmp1 = pop_node(nfa_s);
		Node *tmp2 = pop_node(nfa_s);
		Node *p = (Node*)malloc(sizeof(Node));
		p->ch = op;
		p->lchild = tmp1;
		p->rchild = tmp2;
		p->position = 0;
		push_node(nfa_s, p);
		break;
	}
	}
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
		//因为书本上抽线语法树是这样构造出来的，所以我使用这条语句来达到这个效果
		//但是细细一想，本来就是这样子的。明显'*'是具有连接能力的。现在这样做了之后，就直接把
		//'*'的连接能力给取消了，使用一个显示的'+'来连接
		else if (str[i] == '*')
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
