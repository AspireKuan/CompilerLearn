#include"pch.h"
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include"LexicalAnalysis.h"
#include"DyArray.h"
#include"DyString.h"
#include"error.h"

#define MAXKEY 1024

//ȫ�ֱ���
DyArray tktable;	//���ʱ�
Token *tk_hashtable[MAXKEY];
int tkvalue;		//���ʵ��ֱ���
DyString tkstr;		//�����ַ���
DyString sourcestr;	//����Դ���ַ�
char ch;			//��ǰȡ���ĳ�����ַ�
int token_num;		//��ǰ���ʵ��ֱ���
int line_num;		//�к�
FILE* fin;			//�ļ���ʶ��
char *filename = (char*)"D://Temporary//HelloWorld.c";

int main(int argc, char **argv)
{
	fin = fopen("D://Temporary//HelloWorld.c", "rb");
	if (!fin)
	{
		printf("���ܴ�%s�ļ�", argv[1]);
		return 0;
	}
	tktable.data = NULL;
	init();
	//debug
	//for (int i = 0; i < 40; i++)
	//{
	//	printf("%p\t", (Token*)tktable.data[i]);
	//	printf("%s\n", ((Token*)tktable.data[i])->spelling);
	//}
	ch = getc(fin);
	
	do
	{
		get_token();
		printf("%s",get_tkstr(token_num));
	} while (token_num != TK_EOF);
	printf("\n����������%d\n", line_num);
	cleanup();
	fclose(fin);
	printf("%s�ʷ������ɹ�\n", filename);
	return 0;
}


void init_lex()
{
	Token *tk;
	//���ڵı����������ʹ��(char*)ǿ��ת�����£�"+"���Ƶľͻᱨ����Ϊ�������const char*
	static Token keywords[] = {
	{TK_PLUS,		NULL,	  (char*)"+",	NULL,	NULL},
	{TK_MINUS,		NULL,	  (char*)"-",	NULL,	NULL},
	{TK_MULTIPLE,	NULL,	  (char*)"*",	NULL,	NULL},
	{TK_DIVIDE,		NULL,	  (char*) "/",	NULL,	NULL},
	{TK_MOD,		NULL,	  (char*)"%",	NULL,	NULL},
	{TK_EQ,			NULL,	  (char*)"==",	NULL,	NULL},
	{TK_NEQ,		NULL,	  (char*)"!=",	NULL,	NULL},
	{TK_LT,			NULL,	  (char*)"<",	NULL,	NULL},
	{TK_LEQ,		NULL,	  (char*)"<=",	NULL,	NULL},
	{TK_GT,			NULL,	  (char*)">",	NULL,	NULL},
	{TK_GEQ,		NULL,	  (char*)">=",	NULL,	NULL},
	{TK_ASSIGN,		NULL,	  (char*)"=",	NULL,	NULL},
	{TK_POINTSTO,	NULL,	  (char*)"->",	NULL,	NULL},
	{TK_DOT,		NULL,	  (char*)".",	NULL,	NULL},
	{TK_AND,		NULL,	  (char*)"&",	NULL,	NULL},
	{TK_OPENPA,		NULL,	  (char*)"(",	NULL,	NULL},
	{TK_CLOSEPA,	NULL,	  (char*)")",	NULL,	NULL},
	{TK_OPENBR,		NULL,	  (char*)"[",	NULL,	NULL},
	{TK_CLOSEBR,	NULL,	  (char*)"]",	NULL,	NULL},
	{TK_BEGIN,		NULL,	  (char*)"{",	NULL,	NULL},
	{TK_END,		NULL,	  (char*)"}",	NULL,	NULL},
	{TK_SEMICOLON,	NULL,	  (char*)";",	NULL,	NULL},
	{TK_COMMA,		NULL,	  (char*)",",	NULL,	NULL},
	{TK_ELLIPSIS,	NULL,	(char*)"...",	NULL,	NULL},
	{TK_EOF,		NULL,	 (char*)"End_Of_File",	NULL,	NULL},

	{TK_CINT,		NULL,	 	(char*)"���ͳ���",	NULL,	NULL},
	{TK_CCHAR,		NULL,		(char*)"�ַ�����",	NULL,	NULL},
	{TK_CSTR,		NULL,		(char*)"�ַ�������",	NULL,	NULL},

	{KW_CHAR,		NULL,		(char*)"char",	NULL,	NULL},
	{KW_SHORT,		NULL,		(char*)"short",	NULL,	NULL},
	{KW_INT,		NULL,		(char*)"int",	NULL,	NULL},
	{KW_VOID,		NULL,		(char*)"void",	NULL,	NULL},
	{KW_STRUCT,		NULL,		(char*)"struct",	NULL,	NULL},

	{KW_IF,			NULL,		(char*)"if"	,	NULL,	NULL},
	{KW_ELSE,		NULL,		(char*)"else",	NULL,	NULL},
	{KW_FOR,		NULL,		(char*)"for",	NULL,	NULL},
	{KW_CONTINUE,	NULL,		(char*)"continue",	NULL,	NULL},
	{KW_BREAK,		NULL,		(char*)"break",	NULL,	NULL},
	{KW_RETURN,		NULL,		(char*)"return",	NULL,	NULL},
	{KW_SIZEOF,		NULL,		(char*)"sizeof",	NULL,	NULL},
	//���һ������ȫ����ΪNULL��ԭ�����Ϊ�˷���֮���forѭ�������������ж�
	{0,				NULL,	NULL,	NULL,		NULL}
	};
	//��ΪĿǰ�ҵ������������Ϊrealloc()���������⡣
	//�����Ҿ����ȷ���һ���ܴ�Ŀռ䣬���֮�����realloc()������������
	init_dyarray(&tktable, 100);
	//init_dyarray(&tktable, 8);
	int i = 0;
	for (tk = &keywords[0]; tk->spelling != NULL; tk++)
	{
		//debug
		//printf("%p\t%d\n", tk,i);
		//i++;
		tkword_direct_insert(tk);
	}
}

Token* tkword_direct_insert(Token *tk)
{
	int hash_key = 0;
	insert_dyarray(&tktable, (void*)tk);
	hash_key = elf_hash(tk->spelling);
	tk->next = tk_hashtable[hash_key];
	tk_hashtable[hash_key] = tk;
	return tk;
}

Token* tkword_insert(char *str)
{
	//���ȿ�������ַ���str��û���ڵ��ʱ���ڣ������ڲŲ���
	//���������˵���ոյ��ַ�����һ���ؼ��֣����Ǳ�ʶ��
	Token *tk = find_word(str);
	if (tk == NULL)
	{
		tk = (Token*)malloc(sizeof(Token));
		if (tk == NULL)
		{
			printf("�ڴ����ʧ��\n");
			return NULL;
		}
		int len = strlen(str) + 1;
		tk->spelling = (char*)malloc(sizeof(char)*len);
		memset(tk->spelling, 0, len);
		strcpy(tk->spelling, str);
		//tk->TK_code = tktable.count - 1;
		tk->TK_code = tktable.count;
		int hash_key = elf_hash(str);
		tk->next = tk_hashtable[hash_key];
		tk_hashtable[hash_key] = tk;
		tk->sym_identifier = NULL;
		tk->sym_struct = NULL;
		insert_dyarray(&tktable, tk);
	}
	return tk;
}

Token* find_word(char *str)
{
	//�����ҵ����ַ�����hash_key
	int hash_key = elf_hash(str);
	Token *tk1 = NULL;
	//����hash_key�������Ƿ��Ƿ���ڸ�Ԫ��
	for (tk1 = tk_hashtable[hash_key]; tk1; tk1 = tk1->next)
	{
		if (!strcmp(str, tk1->spelling))
		{
			return tk1;
		}
	}
	return NULL;
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
	return h % MAXKEY;
}

void get_token()
{
	process();
	switch (ch)
	{
		case 'a': case 'b': case 'c': case 'd': case 'e': case 'f': case 'g':
		case 'h': case 'i': case 'j': case 'k': case 'l': case 'm': case 'n':
		case 'o': case 'p': case 'q': case 'r': case 's': case 't':
		case 'u': case 'v': case 'w': case 'x': case 'y': case 'z':
		case 'A': case 'B': case 'C': case 'D': case 'E': case 'F': case 'G':
		case 'H': case 'I': case 'J': case 'K': case 'L': case 'M': case 'N':
		case 'O': case 'P': case 'Q': case 'R': case 'S': case 'T':
		case 'U': case 'V': case 'W': case 'X': case 'Y': case 'Z':
		case '_':
		{
			Token *tk = NULL;
			parse_identifier();
			tk = tkword_insert(tkstr.data);
			token_num = tk->TK_code;
			break;
		}

		case '0': case '1': case '2': case '3':
		case '4': case '5': case '6': case '7':
		case '8': case '9':
		{
			parse_num();
			token_num = TK_CINT;
			break;
		}

		case '+':
			ch = getc(fin);
			token_num = TK_PLUS;
			break;
		case '-':
			ch = getc(fin);
			if (ch == '>')
			{
				token_num = TK_POINTSTO;
				ch = getc(fin);
			}
			else
				token_num = TK_MINUS;
			break;
		case '/':
			token_num = TK_DIVIDE;
			ch = getc(fin);
			break;
		case '%':
			token_num = TK_MOD;
			ch = getc(fin);
			break;
		case '=':
			ch = getc(fin);
			if (ch == '=')
			{
				token_num = TK_EQ;
				ch = getc(fin);
			}
			else
				token_num = TK_ASSIGN;
			break;
		case '!':
			ch = getc(fin);
			if (ch == '=')
			{
				token_num = TK_NEQ;
				ch = getc(fin);
			}
			else
				error((char*)"�ݲ�֧��'!'(�ǲ�����)");
			break;
		case '<':
			ch = getc(fin);
			if (ch == '=')
			{
				token_num = TK_LEQ;
				ch = getc(fin);
			}
			else
				token_num = TK_LT;
			break;
		case '>':
			ch = getc(fin);
			if (ch == '=')
			{
				token_num = TK_GEQ;
				ch = getc(fin);
			}
			else
				token_num = TK_GT;
			break;
		case '.':
			ch = getc(fin);
			if (ch == '.')
			{
				ch = getc(fin);
				if (ch != '.')
					error((char*)"ʡ�Ժ�ƴд����");
				else
					token_num = TK_ELLIPSIS;
				ch = getc(fin);
			}
			else
			{
				token_num = TK_DOT;
			}
			break;
		case '&':
			token_num = TK_AND;
			ch = getc(fin);
			break;
		case ';':
			token_num = TK_SEMICOLON;
			ch = getc(fin);
			break;
		case ']':
			token_num = TK_CLOSEBR;
			ch = getc(fin);
			break;
		case '}':
			token_num = TK_END;
			ch = getc(fin);
			break;
		case ')':
			token_num = TK_CLOSEPA;
			ch = getc(fin);
			break;
		case '[':
			token_num = TK_OPENBR;
			ch = getc(fin);
			break;
		case '{':
			token_num = TK_BEGIN;
			ch = getc(fin);
			break;
		case ',':
			token_num = TK_COMMA;
			ch = getc(fin);
			break;
		case '(':
			token_num = TK_OPENPA;
			ch = getc(fin);
			break;
		case '*':
			token_num = TK_MULTIPLE;
			ch = getc(fin);
			break;
		case '\'':
			parse_string(ch);
			token_num = TK_CCHAR;
			tkvalue = *(char *)tkstr.data;
			break;
		case '\"':
		{
			Token *tk = NULL;
			parse_string(ch);
			tk = tkword_insert(tkstr.data);
			token_num = TK_CSTR;
			break;
		}
		case EOF:
			token_num = TK_EOF;
			break;
		default:
			//�����ַ�������ַ���ֻ���������Դ���ַ�����
			//��������ֵ�Դ�������λ��
			error((char*)"����ʶ���ַ�:%c", ch); 
			ch = getc(fin);
			break;
	}
}

void process()
{
	while (1)
	{
		if (ch == ' ' || ch == '\t' || ch == '\r')
		{
			skip_white_space();
		}
		else if (ch == '/')
		{
			//��Ϊ������'/'������˵���ܳ���/**/,���Գ�ǰ��ȡһ���ַ�
			ch = getc(fin);
			if (ch == '*')
			{
				parse_comment();
			}
			else
			{
				ungetc(ch, fin);
				ch = '/';
				break;
			}
		}
		else
		{
			break;
		}
	}
}

void skip_white_space()
{
	while (ch == ' ' || ch == '\t' || ch == '\r')
	{
		//ע�⣬����Ļ��н���������Windows�µĻ��У�û�д���Linux��Mac����Ļ���
		if (ch == '\r')
		{
			ch = getc(fin);
			if (ch != '\n')
				return;
			line_num++;
		}
		printf("%c", ch);
		ch = getc(fin);
	}
}


void parse_comment()
{
	ch = getc(fin);
	do
	{
		do
		{
			if (ch == '\n' || ch == '*' || ch == EOF)
			{
				break;
			}
			else
			{
				ch = getc(fin);
			}
		} while (1);
		if (ch == '\n')
		{
			line_num++;
			ch = getc(fin);
		}
		else if (ch == '*')
		{
			ch = getc(fin);
			if (ch == '/')
			{
				ch = getc(fin);
				return;
			}
		}
		else
		{
			printf("û�б��䵽ע�͵Ľ�����\n");
		}

	} while (1);
}

void parse_identifier()
{
	reset_dystring(&tkstr);
	charcat_dystring(&tkstr, ch);
	ch = getc(fin);
	while (is_digit(ch) || is_nodigit(ch))
	{
		charcat_dystring(&tkstr, ch);
		ch = getc(fin);
	}
	//�Լ��ֶ�����ȥ�����Ӱ�ȫ
	charcat_dystring(&tkstr, '\0');
}

void parse_num()
{
	reset_dystring(&tkstr);
	reset_dystring(&sourcestr);
	do
	{
		charcat_dystring(&tkstr, ch);
		charcat_dystring(&sourcestr, ch);
		ch = getc(fin);
	} while (is_digit(ch));
	if (ch == '.')
	{
		do
		{
			charcat_dystring(&tkstr, ch);
			charcat_dystring(&sourcestr, ch);
			ch = getc(fin);
		} while (is_digit(ch));
	}
	charcat_dystring(&tkstr, '\0');
	charcat_dystring(&sourcestr, '\0');
}

void parse_string(char sep)
{
	char c;
	reset_dystring(&tkstr);
	reset_dystring(&sourcestr);
	charcat_dystring(&sourcestr, sep);
	ch = getc(fin);
	for (;;)
	{
		if (ch == sep)
		{
			break;
		}
		else if (ch == '\\')
		{
			charcat_dystring(&sourcestr, ch);
			charcat_dystring(&tkstr, ch);
			ch = getc(fin);
			//����ת���ַ�
			switch (ch)
			{
				case '0':
					c = '\0';
					break;
				case 'a':
					c = '\a';
					break;
				case 'b':
					c = '\b';
					break;
				case 't':
					c = '\t';
					break;
				case 'n':
					c = '\n';
					break;
				case 'v':
					c = '\v';
					break;
				case 'f':
					c = '\f';
					break;
				case 'r':
					c = '\r';
					break;
				case '\"':
					c = '\"';
					break;
				case '\'':
					c = '\'';
					break;
				case '\\':
					c = '\\';
					break;
				default:
					c = ch;
					if (c >= '!' && c <= '~')
						warning((char*)"�Ƿ�ת���ַ�: \'\\%c\'", c); // 33-126 0x21-0x7E����ʾ�ַ�����
					else
						warning((char*)"�Ƿ�ת���ַ�: \'\\0x%x\'", c);
					break;
			}
			charcat_dystring(&tkstr, c);
			charcat_dystring(&sourcestr, ch);
			ch = getc(fin);
		}
		else
		{
			charcat_dystring(&tkstr, ch);
			charcat_dystring(&sourcestr, ch);
			ch = getc(fin);
		}
	}
	charcat_dystring(&tkstr, sep);
	charcat_dystring(&tkstr, '\0');
	charcat_dystring(&sourcestr, sep);
	charcat_dystring(&sourcestr, '\0');
	ch = getc(fin);
}


int is_digit(char ch)
{
	if (ch >= '0' && ch <= '9')
	{
		return 1;
	}
	return 0;
}

int is_nodigit(char ch)
{
	if ((ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z') || ch=='_')
	{
		return 1;
	}
	return 0;
}

void init()
{
	line_num = 1;
	init_lex();
}

void cleanup()
{
	for (int i = KW_IDENT; i < tktable.count; i++)
	{
		free(tktable.data[i]);
		tktable.data[i] = NULL;
	}
	free(tktable.data);
	tktable.data = NULL;
}