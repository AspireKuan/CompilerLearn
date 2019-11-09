#ifndef NFA_CONVERSION_DFA
#define NFA_CONVERSION_DFA
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

 extern const int max_len;
 extern const int maxn;

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
void print_state(int **adj, int start);
void dfs(int **adj, int start, int *state);
void subset_construction(state *Dstates, char *str, int start);
state* find_no_flag_state(state *Dstates);
int find_state(state *Dstates, state *val);
state* closure_start(int start);
state* closure(state *p);
state* move(state *s, char ch);
char *dfs_get_state(int index, int condition);
void state_bubble_sort(char *str);
void swap(char *p, char *q);

#endif
