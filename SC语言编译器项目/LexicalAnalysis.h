#ifndef LEXICALANAYSIS_H
#define LEXICALANAYSIS_H

enum e_TokenCode
{
	//������ͷָ���
	TK_PLUS,		// +����
	TK_MINUS,		// -����
	TK_MULTIPLE,	// *����
	TK_DIVIDE,		// /����
	TK_MOD,			// %����
	TK_EQ,			// ==����
	TK_NEQ,			// !=����
	TK_LT,			// <����
	TK_LEQ,			// <=����
	TK_GT,			// >����
	TK_GEQ,			// >=����
	TK_ASSIGN,		// =����
	TK_POINTSTO,	// ->����
	TK_DOT,			// .����
	TK_AND,			// &����
	TK_OPENPA,		// (
	TK_CLOSEPA,		// )
	TK_OPENBR,		// [
	TK_CLOSEBR,		// ]
	TK_BEGIN,		// {
	TK_END,			// }
	TK_SEMICOLON,	// ;
	TK_COMMA,		// ,
	TK_ELLIPSIS,	// ...�βοɱ�ʱ���õ�
	TK_EOF,			//�ļ�������


	//����
	TK_CINT,		//���ͳ���
	TK_CCHAR,		//�ַ��ͳ���
	TK_CSTR,		//�ַ�������


	//�ؼ���
	KW_CHAR,		// char�ؼ���
	KW_SHORT,		// short�ؼ���
	KW_INT,			// int�ؼ���
	KW_VOID,		// void �ؼ���
	KW_IF,			// if�ؼ���
	KW_ELSE,		//else �ؼ���
	KW_FOR,			//for�ؼ���
	KW_STRUCT,		//struct�ؼ���
	KW_BREAK,		//break�ؼ���
	KW_CONTINUE,	//continue�ؼ���
	KW_RETURN,		//return �ؼ���
	KW_SIZEOF,		//sizeof�ؼ���



	//��ʶ��
	KW_IDENT

};

//��������鱾��˵�ķ���һ��token
typedef struct Token
{
	int TK_code;					//�ֱ���
	struct Token *next;				//ʹ����ʽ������hash��ͻ
	char *spelling;					//ָ���token��ֵ
	struct Symbol *sym_struct;
	struct Symbol *sym_identifier;
}Token;

//��������

//���ʱ�ĳ�ʼ��
void init_lex();

//��������ؼ��֣�����ֱ�ӷ��뵥�ʱ�
Token* tkword_direct_insert(Token *tk);

//���ҵ��ʱ����Ƿ��иõ���
Token* find_word(char *str);

//��ʶ�����뵥�ʱ�
Token* tkword_insert(char *str);

//����Linux�е�hash����
int elf_hash(char *key);

//ʶ��ÿһ������
void get_token();

//Ԥ�������
void process();

//�����հ��ַ�
void skip_white_space();

//����ע������
void parse_comment();

//������ʶ��
void parse_identifier();

//��������
void parse_num();

//�����ַ���
void parse_string(char sep);

//�ж��Ƿ�Ϊ����
int is_digit(char ch);

//�ж��Ƿ�Ϊ��ĸ(a-z,A-Z)�����»���( _ )
int is_nodigit(char ch);

//��ʼ������
void init();

//�ͷŵ��ʱ�Ŀռ�
void cleanup();



#endif

