#ifndef LEXICALANAYSIS_H
#define LEXICALANAYSIS_H

enum e_TokenCode
{
	//运算符和分隔符
	TK_PLUS,		// +运算
	TK_MINUS,		// -运算
	TK_MULTIPLE,	// *运算
	TK_DIVIDE,		// /运算
	TK_MOD,			// %运算
	TK_EQ,			// ==运算
	TK_NEQ,			// !=运算
	TK_LT,			// <运算
	TK_LEQ,			// <=运算
	TK_GT,			// >运算
	TK_GEQ,			// >=运算
	TK_ASSIGN,		// =运算
	TK_POINTSTO,	// ->运算
	TK_DOT,			// .运算
	TK_AND,			// &运算
	TK_OPENPA,		// (
	TK_CLOSEPA,		// )
	TK_OPENBR,		// [
	TK_CLOSEBR,		// ]
	TK_BEGIN,		// {
	TK_END,			// }
	TK_SEMICOLON,	// ;
	TK_COMMA,		// ,
	TK_ELLIPSIS,	// ...形参可变时会用到
	TK_EOF,			//文件结束符


	//常量
	TK_CINT,		//整型常量
	TK_CCHAR,		//字符型常量
	TK_CSTR,		//字符串常量


	//关键字
	KW_CHAR,		// char关键字
	KW_SHORT,		// short关键字
	KW_INT,			// int关键字
	KW_VOID,		// void 关键字
	KW_IF,			// if关键字
	KW_ELSE,		//else 关键字
	KW_FOR,			//for关键字
	KW_STRUCT,		//struct关键字
	KW_BREAK,		//break关键字
	KW_CONTINUE,	//continue关键字
	KW_RETURN,		//return 关键字
	KW_SIZEOF,		//sizeof关键字



	//标识符
	KW_IDENT

};

//这个就是书本上说的返回一个token
typedef struct Token
{
	int TK_code;					//种别码
	struct Token *next;				//使用链式来处理hash冲突
	char *spelling;					//指向该token的值
	struct Symbol *sym_struct;
	struct Symbol *sym_identifier;
}Token;

//函数声明

//单词表的初始化
void init_lex();

//运算符，关键字，常量直接放入单词表
Token* tkword_direct_insert(Token *tk);

//查找单词表中是否有该单词
Token* find_word(char *str);

//标识符插入单词表
Token* tkword_insert(char *str);

//采用Linux中的hash函数
int elf_hash(char *key);

//识别每一个单词
void get_token();

//预处理程序
void process();

//跳过空白字符
void skip_white_space();

//处理注释内容
void parse_comment();

//解析标识符
void parse_identifier();

//解析整数
void parse_num();

//解析字符串
void parse_string(char sep);

//判断是否为数字
int is_digit(char ch);

//判断是否为字母(a-z,A-Z)或者下划线( _ )
int is_nodigit(char ch);

//初始化工作
void init();

//释放单词表的空间
void cleanup();



#endif

