// read_keyword_identifier.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include "pch.h"
#include<stdio.h>
#include<string.h>
#include<stdlib.h>


const int maxn = 7;
int adj[maxn][maxn];
int main()
{
	printf("输入你的正则表达式：");
	char *reg = (char*)malloc(sizeof(char) * maxn);
	scanf("%s", reg);
	char *str = (char*)malloc(sizeof(char) * maxn);
	printf("输入你要校验的字符串：");
	scanf("%s", str);
	int len_reg = strlen(reg);
	for (int i = 0; i < len_reg; i++)
	{
		int x = reg[i] - 'a';
		int y = reg[i + 1] - 'a';
		//这里保证*就是在图的最后一行或者最后一列
		//因为上面的运算可能出现负数
		x=(x < 0) ? maxn-1 : x;
		y=(y < 0) ? maxn-1 : y;
		adj[x][y]++;
	}
	int len_str = strlen(str);
	int flag = 1;
	for (int i = 0; i < len_str; i++)
	{
		int x = str[i] - 'a';
		int y = str[i + 1] - 'a';
		x=(x < 0) ? maxn-1 : x;
		y=(y < 0) ? maxn-1 : y;
		//此时正则表达式中有显示的str[i]str[i+1]
		if (adj[x][y] >= 1)
		{
			adj[x][y]--;
			continue;
		}
		//此时正则表达式中是a*,并且输入str[i]==str[i+1]
		else if (adj[x][2] >= 1 && x==y)
		{
			continue;
		}
		//此时正则表达式中是ab*a,但是输入的是aa
		else if (adj[x][maxn - 1] >= 1 && adj[maxn - 1][y] >= 1)
		{
			continue;
		}
		flag = 0;
		break;
	}
	if (flag)
	{
		printf("匹配成功\n");
	}
	else
	{
		printf("匹配失败\n");
	}
	return 0;
}

/*
//在这个模式下面，你还的显式的超前看几个字符。这个样子话导致程序写出来
//比较别扭。所以就放弃的这个写法
const int maxn = 3;
int adj[maxn][maxn];
int main()
{
	printf("输入你的正则表达式：");
	char *reg= (char*)malloc(sizeof(char) * 10);
	scanf("%s", reg);
	char *str = (char*)malloc(sizeof(char)*10);
	printf("输入你要校验的字符串：");
	scanf("%s", str);
	int len = strlen(str);
	int j = 0;
	int flag = 1;
	for (int i = 0; i < len; i++)
	{
		if (reg[j] == str[i])
		{
			j++;
			continue;
		}
		else
		{
			if (reg[j] == '*')
			{
				if (j >= 1 && reg[j - 1] == str[i])
				{
					continue;
				}
				else if(reg[j+1]==str[i])
				{
					j += 2;
					continue;
				}
				flag = 0;
				break;
			}
		}
	}
	if (j >= strlen(reg) - 1 && flag)
	{
		printf("匹配成功\n");
	}
	else
	{
		printf("匹配失败\n");
	}
	
}
*/

/*
#define KEYWORD 0
#define OP 1
#define CONST 2
#define ID 3


typedef struct token
{
	int key;
	char *value;
};

//这个是用来将词素转化为邻接矩阵
//我的设计就是：字母，数字，运算符，下划线，小数点
const int maxn = 5;
int adj[maxn][maxn];

int test_keyword_indentifier(char *str);

int main()
{
	printf("请输入你要测试的词素：");
	char *str = (char*)malloc(sizeof(char));
	if (str == NULL)
	{
		printf("内存分配失败\n");
		exit(-1);
	}
	scanf("%s", str);
	int num=test_keyword_indentifier(str);
	switch (num)
	{
	case 1:
		printf("这个是一个C语言的关键字\n");
		break;
	case 2:
		printf("这个是一个C语言合法标识符\n");
		break;
	case 3:
		printf("这个是一个C语言的运算符\n");
		break;
	default:
		printf("输入不符合C语言标准\n");
		break;
	}
	return 0;
}

int test_keyword_indentifier(char *str)
{
	int length = strlen(str);

}

int test_letter(char ch)
{
	if ((ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z'))
	{
		return 1;
	}
	return 0;
}

int test_digit(char ch)
{
	if (ch >= '0' && ch <= '9')
	{
		return 1;
	}
	return 0;
}

int test_operation(char ch)
{
	if (ch == '>' || ch == '<' || ch == '=' || ch == '-' || ch == '+' || ch == '*' || ch == '\\')
	{
		return 1;
	}
	return 0;
}

int test_other(char ch)
{
	if (ch == '.' || ch == '_')
	{
		return 1;
	}
	return 0;
}

token get_token(char *str)
{
	token *retToken = (token*)malloc(sizeof(token));
	if (retToken == NULL)
	{
		printf("内存分配失败\n");
		exit(-1);
	}
	int len = strlen(str);
	retToken->value = (char*)malloc(sizeof(char)*len);
	//遍历这个词素，形成他自己的图
	for (int i = 0; i < len; i++)
	{
		if (test_digit(str[i]))
		{
			adj[1][1] = 1;
		}
		else if (test_letter(str[i]))
		{
			adj[0][0] = 1;
		}
		else if (test_operation(str[i]))
		{
			adj[2][2] = 1;
		}
		else if (test_other(str[i]))
		{
			adj[3][3] = 1;
		}
	}
}
*/