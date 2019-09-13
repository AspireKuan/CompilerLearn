#include "pch.h"
#include<stdio.h>
#include<string.h>
#include<stdlib.h>


//��װ���£��������Ӱ�ȫ�ԡ�����������������ת��
//ͬʱΪ������ͨ��ջ�����
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

//״̬����
typedef struct state
{
	char *state_set;//״̬����
	int flag;//�Ƿ񱻷��ʹ���־
	struct state* next;//ָ����һ��״̬����
}state;

state *Dstates = NULL;
//max_len�������������ʽ�ĳ��ȣ�mxan�ǳ��ֵ�״̬����Ŀ
const int max_len = 10;
const int maxn = 15;

//adj[x][y]='a';��ʾ״̬��x->״̬y�м����ͨ������'a'��ת��
//adj[x][y]=-1;��ʾ״̬��x->״̬y����ֱ��ת��
//adj[x][y]=0;��ʾ״̬��x->״̬y������ת��
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
	//����Ҫ������ջ�������ջ��NFAջ
	OP_Stack *op_s = (OP_Stack*)malloc(sizeof(OP_Stack));
	NFA_Stack *nfa_s = (NFA_Stack*)malloc(sizeof(NFA_Stack));
	init_OP_stack(&op_s);
	init_NFA_stack(&nfa_s);
	//����׺��������ʽת��Ϊ��׺��������ʽ
	char *reg = infixTOpostfix(tmp2, op_s);
	free(tmp2);
	tmp2 = NULL;
	//��ʼ�����׺������ʽ
	int len = strlen(reg);
	//���������Ϊ�ʼ��ָ��
	int start = 0;
	//��������״̬�ı�ʾindex
	//��Ϊ����Ǵ�ҹ��õģ�����Ҫ��֤һ���˸ı��ˣ�����
	//�˶�֪������������ʹ��ָ��ת�ݣ�ע��ʹ��ȫ�ֺ���
	//û�����Ч������

	int index = 0;
	for (int i = 0; i < len; i++)
	{
		//ע�⣺���for�����Ѿ���������ʽת��Ϊ��׺��
		//������ʽ�ˣ����ԣ�������û��'('��')'��
		//�����ʱ��һ�������
		if (test_op(reg[i]))
		{
			merge_nfa(nfa_s, reg[i], &index);
		}
		//�����������
		else
		{
			//����NFA�ڵ�
			index++;
			Node *tmp = (Node*)malloc(sizeof(Node));
			tmp->front = index;
			index++;
			tmp->rear = index;
			tmp->ch = reg[i];
			//����ͼ
			adj[tmp->front][tmp->rear] = reg[i];
			//���ո����ɵ�NFA�ڵ���NFAջ
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

	//����NFAջ��Ψһ�ڵ���ǣ�����Ҫʹ�������������ͼ
	start = pop_node(nfa_s)->front;
	//print_state(adj, start);

	//��ʼ��״̬����
	Dstates = (state*)malloc(sizeof(state));
	Dstates->next = NULL;
	Dstates->state_set = NULL;
	Dstates->flag = 1;//��ΪDstates��һ������ͷ�ڵ�������������ｫͷ�ڵ�ķ��ʱ�־����Ϊ1
	subset_construction(Dstates, reg, start);
	//������������
	for (state *p = Dstates->next; p != NULL; p = p->next)
	{
		printf("%s\n", p->state_set);
	}
	return 0;
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

//�ϲ�����NFAջ��ǰ����״̬��
void merge_nfa(NFA_Stack *nfa_s, char ch, int *index)
{
	switch (ch)
	{
	case '|':
	{
		//��Ϊ��'|'���Դ���һ���ڵ�
		(*index)++;
		Node *head = (Node*)malloc(sizeof(Node));
		head->front = *index;
		(*index)++;
		head->rear = *index;
		Node *tmp1 = pop_node(nfa_s);
		Node *tmp2 = pop_node(nfa_s);
		//�ı�ͼ�ṹ
		adj[head->front][tmp1->front] = -1;
		adj[head->front][tmp2->front] = -1;
		adj[tmp1->rear][head->rear] = -1;
		adj[tmp2->rear][head->rear] = -1;
		//�ͷŸո�pop�����Ľڵ�Ŀռ�
		free(tmp1);
		tmp1 = NULL;
		free(tmp2);
		tmp2 = NULL;
		push_node(nfa_s, head);
		break;
	}

	case '*':
	{
		//���ﻹ�ǲ������������״̬��������ʵ�Ҿ����Ƕ����
		(*index)++;
		Node *head = (Node*)malloc(sizeof(Node));
		head->front = *index;
		//��NFAջ�е������ڵ��ջ
		Node *tmp1 = pop_node(nfa_s);
		Node *tmp2 = pop_node(nfa_s);
		//�ı�ͼ�ṹ
		adj[head->front][tmp2->front] = -1;
		adj[tmp2->rear][tmp1->front] = -1;
		adj[tmp2->rear][tmp2->front] = -1;
		adj[head->front][tmp1->front] = -1;
		//�����Ľڵ���ջ
		head->rear = tmp1->rear;
		//�ͷ��ڴ浥Ԫ
		free(tmp1);
		tmp1 = NULL;
		free(tmp2);
		tmp2 = NULL;
		push_node(nfa_s, head);
		break;

	}
	case '+':
	{
		//���������ǲ��ô���״̬��,����Ҫʹindex--
		//��NFAջ�е������ڵ��ջ
		Node *tmp1 = pop_node(nfa_s);
		Node *tmp2 = pop_node(nfa_s);
		//�ı�ͼ�Ľṹ
		adj[tmp2->rear][tmp1->front] = tmp1->ch;
		//ԭ���Ĺ�ϵ�Ͳ���ʹ���ˣ�����Ҫ��adjͼ��ԭ
		adj[tmp1->front][tmp1->rear] = 0;
		(*index)--;
		tmp2->rear = tmp1->front;
		//�ͷ��ڴ浥Ԫ
		free(tmp1);
		tmp1 = NULL;
		push_node(nfa_s, tmp2);
		break;
	}

	}
}

//��ӡNFA״̬ͼ(�����Լ��Ĳ���֮�У���û��ʹ��״̬0�����֮������
//�����ô�)
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

//�ռ�������ʽ���е������ַ�
char *getchar_set(char *str)
{
	int len = strlen(str);
	//���￼�ǵ���������ʽ�����ַ���������len��������û��len������Ϊ����һЩ�����
	char *character = (char*)malloc(sizeof(char)*(len + 1));
	memset(character, 0, sizeof(char)*(len + 1));
	int j = 0;
	//��ʱ��һ��������bi'ai's
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
	//�Ҿ�����������ڷ���֮ǰ���Ƚ�character����
	return character;
}

//�Ӽ������㷨��NFAת��ΪDFA
//Dstates������״̬���ϣ�str��������ʽ��start��NFA��ʼ״̬
void subset_construction(state *Dstates, char *str, int start)
{
	//Ϊ��ҪDstates�ʼ��һ��״̬���������������Ƚ���ʼ״̬���ϻ�ȡ
	//���Ȼ�ȡNFA״̬��
	state *s = closure_start(start);
	//����ʼ״̬s����Dstates
	s->next = Dstates->next;
	Dstates->next = s;
	//��ȡ����������ʽ�еķ���(���������)
	char *char_set = getchar_set(str);
	int len = strlen(char_set);
	//printf("NFA״̬\t\t|DFA״̬\t\t|");
	//for (int i = 0; i < len; i++)
	//{
	//	printf("%c\t|", char_set[i]);
	//}
	//printf("\n");
	while (state* p = find_no_flag_state(Dstates))
	{
		//��״̬����p����Ϊ�Ѿ�����
		p->flag = 1;
		for (int i = 0; i < len; i++)
		{
			//��ʵ�����tmp2���ڴ�Ҫ����Ķ�һ�㣬��Ȼ�Ļ������strcat()�ͻ�������⡣
			//��Ϊ����㷨���Ǻܾ�ȷ���ڷ����ڴ棬���Ե���tmp2���ڴ治��
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
			//��ʱtmp2����һ���Ѿ����ڵ�״̬��û�б�Ҫ�����ˣ������ͷ������ڴ�ռ���
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

//����״̬val�Ƿ�����״̬Dstates��
//���ڷ���1�������ڷ���0
int find_state(state *Dstates, state *val)
{
	//Dstates��һ������ͷ�ڵ������
	state *p = Dstates->next;
	//�ڲ���״̬��ʱ�򣬲��ܽ�����������ַ����Ƿ���ȣ���Ϊ�е�״̬����������״̬������ȵģ�
	//ֻ������ͬ���ѡ����ԣ�����Ҫ���ȶ������״̬���������Ƚ�
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

//ע������ڸ����ڡ��Ӽ����취���е������㷨��һ��Ҫʹ��dfs��ʵ�֡���Ϊ����ڡ���״̬�����ԣ�
//������û�ж�Ӧ���ַ������ǿ���ֱ�ӵ���ġ�
//��dfs_get_state()�����жϣ���ʵ���Ҳ��һ����ϰ�ߡ����ڷ���ָ��ĺ������������㻹Ҫʹ���������
//ָ���ʱ��һ��Ҫ���Ƿ�ΪNULL���жϡ������Ӳ����ȫ������

//��ȡ��ʼ״̬����
state* closure_start(int start)
{
	state *ans = NULL;
	ans = (state*)malloc(sizeof(state));
	ans->flag = 0;
	ans->next = NULL;
	int len = 0;
	char *p = dfs_get_state(start, -1);
	//��Ϊ�п��ܿ�ʼ״̬û��ֱ�ӵĿ�״̬�����Դ�ʱp==NULL
	if (p)
	{
		len = strlen(p);
	}
	//+1��Ϊ�˱�֤start״̬Ҳ���Լӽ�ȥ
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

//NFA״̬ת����һ��״̬����p��ͨ��"���ַ�"���Եõ���״̬����
//��ʵ����Ĵ����ǿ��Ժ������,����Ĵ���ϲ��ġ��������֮���Ż���ʱ��ִ��
state* closure(state *p)
{
	state *ans = NULL;
	int len_state_set = strlen(p->state_set);
	//�������һ���ֲ������ȶ࿪һ���ڴ棬Ȼ���Ƹ�ans->state_set�Ϳ�����
	//��Ϊp���ڴ�����������ͷŵ�������ans���ڴ�Ҫ�����ã����Է����ڴ��ʱ��Ҫ��ȷһ��
	char *tmp = (char*)malloc(sizeof(char)*maxn);
	memset(tmp, 0, sizeof(char)*maxn);
	for (int i = 0; i < len_state_set; i++)
	{
		char *str = dfs_get_state(p->state_set[i] - '0', -1);
		//��Ϊ���ܷ��ص���һ��NULL,˵��״̬p->state_set[i]����ͨ��"��״̬"ת��
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
	//��Ϊ�������ﻹҪ���ظ��ϼ��ĵ��ã�������һЩ����strcat()�������������ﲢ����������len+1���ڴ�
	//���Ƿ���maxn+1���ڴ浥Ԫ
	ans->state_set = (char*)malloc(sizeof(char)*(maxn + 1));
	memset(ans->state_set, 0, sizeof(char)*(maxn + 1));
	strcpy(ans->state_set, tmp);
	ans->state_set[maxn] = '\0';
	return ans;
}

//NFA״̬ת��������״̬sͨ��ch����ת����״̬
state* move(state *s, char ch)
{
	int len_s = strlen(s->state_set);
	char *tmp = (char*)malloc(sizeof(char)*maxn);
	memset(tmp, 0, sizeof(char)*maxn);
	for (int i = 0; i < len_s; i++)
	{
		//��Ϊ����״̬���ɡ����֡��͡�Сд��ĸ����ɣ��������ﻹ���ж�һ��״̬
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
		//��Ϊ���ܷ��ص���һ��NULL,˵��״̬p->state_set[i]����ͨ��"ch״̬"ת��
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
	//�������һ�����ع���,���Խ�������maxn/2+1���ڴ��С
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
			//�������ڡ���״̬����˵��Ҫ�ݹ����������������ڽ����ض����ַ���״̬����ֻ����һ��
			if (condition == -1)
			{
				copy_state = dfs_get_state(i, condition);
				if (copy_state != NULL)
				{
					strcat(ans, copy_state);
					//�ͷ��ڴ�
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
		//����������ܻ���һ��������bug,��Ϊstrֵ�����ϲ��dfs���õ�ʱ����ܻ����
		//���ȡ������������ı��ع�������
		return ans;
	}
}


//�ַ������򡣲���ð�������㷨ʵ��
void state_bubble_sort(char *str)
{
	//lastIndex������һ����¼�����Ҫ����˳��ı߽��
	//����Ϊ�˿˷���Щ������Щ�����Ԫ��
	int lastIndex = 0;
	/*
	��ΪIndexBoundΪnum-1,j<IndexBound
	���ԣ�j���ֵΪnum-2;����a[j+1]����
	�������±껹��û��Խ��
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