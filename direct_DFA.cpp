#include "pch.h"
#include<stdio.h>
#include<string.h>
#include<stdlib.h>

const int maxn = 15;


//����ջ
typedef struct OP_char
{
	char ch;
	struct OP_char *next;
}OP_char;

typedef struct Node
{
	//�ýڵ������
	char ch;
	//���Һ�����Ϊ�˹�������﷨��ʱ��Ҫ
	struct Node *lchild;
	struct Node *rchild;
	//next�ڵ���Ϊ��ջ����Ҫ
	struct Node *next;
	//�ýڵ��λ��
	int position;
}Node;

//����ջ
typedef struct OP_Stack
{
	OP_char *top;
	OP_char *bottom;
}OP_Stack;

//NFAջ
typedef struct NFA_Stack
{
	Node *top;
	Node *bottom;
}NFA_Stack;

//str�Ǻ�׺��������ʽ
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


//����һ��������﷨��
Node *create_abstract_syntax_tree(char *str, OP_Stack *op_s, NFA_Stack *nfa_s)
{
	int len = strlen(str);
	//Ϊÿһ����������ڵ㹹���λ���±�
	int position = 1;
	for (int i = 0; i < len; i++)
	{
		//�ò����ַ��������
		if (test_op(str[i]))
		{
			merge_node(str[i], nfa_s);
		}
		//�ò����ַ��Ƿ������
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
		//����'*'�ڵ�
		Node *p = (Node*)malloc(sizeof(Node));
		p->ch = op;
		//��Ϊ'*'������������ԣ����Ծͽ���ֻʹ����һ�����ӽڵ���Ϣ
		p->lchild = tmp;
		p->rchild = NULL;
		p->position = 0;
		push_node(nfa_s, p);
		break;
	}
	//��Ϊ'+'��'|'��ʵ�ǲ���
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



//�ж��ǲ��������
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

//��������ʽ������ʽ�����ӷ�'+'
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
		//�����ַ�����һ�������м����һ����+��
		if (!test_op(str[i]) && !test_op(str[i + 1]))
		{
			ans[j] = '+';
			j++;
		}
		//��Ϊ�鱾�ϳ����﷨����������������ģ�������ʹ������������ﵽ���Ч��
		//����ϸϸһ�룬�������������ӵġ�����'*'�Ǿ������������ġ�������������֮�󣬾�ֱ�Ӱ�
		//'*'������������ȡ���ˣ�ʹ��һ����ʾ��'+'������
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
	//����϶������ڴ����һ�㡣��Ϊת��Ϊ��׺�����ʱ�򣬾�û�У�����
	char *ans = (char*)malloc(sizeof(char)*len);
	memset(ans, 0, sizeof(char)*len);
	int j = 0;
	for (int i = 0; i < len; i++)
	{
		//����Ԫ��Ϊ�����
		if (test_op(str[i]))
		{
			//ջ��Ԫ�طǿ�
			if (!empty_op(op_s))
			{
				//�������Ԫ����')'Ҫ���⴦��
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
					//����'('
					OP_char *tmp = pop_op(op_s);
					free(tmp);
					tmp = NULL;
				}
				//�������Ԫ�ز�����')'
				else if (str[i] != ')')
				{
					int priority = test_priority(get_top(op_s)->ch, str[i]);
					//str[i]��ջ��Ԫ�ص����ȼ��ͻ�����ȣ��͵ò��ϳ�ջ��Ȼ���Լ���ջ
					//����priority=1����ʾҪ��ջ��=0��ʾҪ��ջ
					if (!priority)
					{
						while (!priority)
						{
							OP_char *tmp = pop_op(op_s);
							ans[j] = tmp->ch;
							j++;
							free(tmp);
							tmp = NULL;
							//ÿ���ڶ�ջ������ʱ���ȼ��ջ�ǲ���Ϊ�գ�Ȼ����ȥ��������һ����ϰ��
							//�����Ƕ������ڴ���һ��while֮���ѭ�������ʱ���ر�ʵ��
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
					//ջ��Ԫ�����ȼ���
					else if (priority)
					{
						push_op(op_s, str[i]);
					}
				}

			}
			//���Ϊ�գ�����ջ
			else if (empty_op(op_s))
			{
				push_op(op_s, str[i]);
			}
		}
		//����Ԫ�ز�Ϊ�����
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

//ch1Ϊջ��Ԫ�أ�ch2Ϊ����Ԫ��
//ch2���ȼ�����ch1��return 1,��ʾch2Ӧ����ջ
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
