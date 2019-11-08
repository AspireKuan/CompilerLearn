#include"pch.h"
#include"DyString.h"
#include"PredictiveParsing.h"
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<stdlib.h>

#define MAXCHARNUM 16		//ÿһ���ս�������ս�������ʽ����󳤶�
#define HASHSIZE 512		//hash��Ĵ�С
#define MAXVTPNUM 32		//�����ս��������ս��������ʽ�ĸ���
#define EMPTYSTRING '@'		//ʹ�á�@��������մ�

//ȫ�ֱ���
//���ս�����ս����hash��
Node *VNode[HASHSIZE];
Node *TNode[HASHSIZE];
//���ս�����ս�������ʽ�ĸ���(һ��ҪС��MAXVTPNUM)
int Vnum, Tnum, Pnum;
//�ķ����ŵ�FIRST����
First_set VTfirst[MAXVTPNUM];
//ÿ�����ս����FOLLOW����
Follow_set Vfollow[MAXVTPNUM];

int main()
{
	DyString *Vstring = (DyString*)malloc(sizeof(DyString) * MAXVTPNUM);
	DyString *Tstring = (DyString*)malloc(sizeof(DyString) * MAXVTPNUM);
	DyString *Pstring = (DyString*)malloc(sizeof(DyString) * MAXVTPNUM);
	DyString* S = (DyString*)malloc(sizeof(DyString));//��Ϊ��ʼ���Ž���ֻ��һ��
	if (!Vstring || !Tstring || !Pstring)
	{
		printf("malloc error\n");
		exit(-1);
	}
	

	//�����ķ�G
	init(Vstring, Tstring, Pstring, S);

	//Ϊ�ķ�G�еķ��ս�����ս������hash��
	establish_hash(Vstring, Vnum,VNode);
	establish_hash(Tstring, Tnum, TNode);

	printf("\n*******************************\n");

	//����FIRST�㷨
	//��ʼ��VTfirst����
	init_first_set(Tstring,Vstring);

	//�����ķ��ı��ʽ������FIRST����
	//get_first(Pstring);
	get_first2(Pstring);
	display_first(VTfirst, Tnum + Vnum);
	
	printf("\n*******************************\n");

	//����FOLLOW�㷨
	init_follow_set(Vstring,S);
	get_follow(Pstring);
	display_follow(Vfollow, Vnum);

	printf("\n*******************************\n");
	//�����ж�LL(1)�ķ����㷨
	if (is_LL1(Pstring,Vstring,Tstring, VTfirst, Vfollow))
	{
		printf("���ķ���LL(1)�ķ�\n\n");
		char*** analysis_table = init_analysis_table(Vnum, Tnum);
		create_predictive_analysis_table(analysis_table, Pstring, Pnum,Vstring, Vnum,Tstring,Tnum, VTfirst, Vfollow);
		display_analysis_table(analysis_table, Vstring,Vnum, Tstring,Tnum);
		printf("\n*******************************\n");
		
		//����Ԥ���������
		printf("��������Ҫ���Եľ��ӣ�");
		char *input = (char*)malloc(sizeof(char)*MAXCHARNUM);
		memset(input, 0, sizeof(char)*MAXCHARNUM);
		scanf("%s", input);
		input[strlen(input)] = '#';//�����봮�����һ���������
		DyString *analysis_stack=init_stack(MAXVTPNUM, S);
		int result=LL1Parsing(input, analysis_table, analysis_stack,Vstring,Vnum,Tstring,Tnum);
		input[strlen(input)-1] = '\0';
		if (result)
		{
			printf("\n%s�Ǹ��ķ��µľ���\n",input);
		}
		else
		{
			printf("\n%s���Ǹ��ķ��µľ���\n",input);
		}
	}
	return 0;
}

DyString *init_stack(int size,DyString *start)
{
	DyString *res = (DyString*)malloc(sizeof(DyString)*size);
	if (!res)
	{
		printf("%d:�ڴ����ʧ��", __LINE__);
		exit(1);
	}
	//�õ�0��Ԫ������������ջ�ı��
	res[0].length = 1;//��Ϊres[0]����ʹ�á�
	res[0].capacity = size-1;
	res[0].data = NULL;
	for (int i = 1; i < size; i++)
	{
		init_dystring(res+i, MAXVTPNUM);
	}
	//ͬʱ��'#'�Ϳ�ʼ������ջ
	DyString ends;
	init_dystring(&ends, 8);
	charcat_dystring(&ends, '#');
	push_stack(res, ends);
	push_stack(res, *start);
	return res;
}

void push_stack(DyString *stack, DyString val)
{
	if (stack[0].capacity == 0)
	{
		printf("%d:ջû��������\n", __LINE__);
		exit(1);
	}
	stack[stack[0].length] = val;
	stack[0].length++;
	stack[0].capacity--;
}

void pop_stack(DyString *stack)
{
	if (stack[0].length == 1)
	{
		printf("%d:û��Ԫ�أ��޷���ջ\n", __LINE__);
		exit(1);
	}
	stack[0].length--;
	stack[0].capacity++;
}

DyString gettop_stack(DyString *stack)
{
	if (stack[0].length == 1)
	{
		printf("%d:ջ��û��Ԫ�أ��޷����ջ��Ԫ��\n", __LINE__);
		exit(1);
	}
	return stack[stack[0].length-1];
}

int is_empty_stack(DyString *stack)
{
	if (stack[0].length == 1)
	{
		return 1;
	}
	return 0;
}

DyString charstr_to_DyString(char *src)
{
	int len = strlen(src);
	DyString res;
	init_dystring(&res, 8);
	strcat_dystring(&res, src);
	return res;
}

void init(DyString *Vstring, DyString *Tstring, DyString *Pstring, DyString *S)
{
	printf("�������ķ��ķ��ս�����ϣ�");
	init_VTPset(Vstring,&Vnum);
	printf("�������ķ����ս����ϣ�");
	init_VTPset(Tstring,&Tnum);
	printf("�������ķ��ı��ʽ���ϣ�\n");
	init_VTPset(Pstring,&Pnum);
	printf("�������ķ��Ŀ�ʼ���ţ�");
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
	//��Ϊ������i��Ҫ�����ģ�����ϰ�߸�����1��ʼ����
	//���ԣ��ڷ��ص�ʱ��++;
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
			printf("�ڴ����ʧ��\n");
			exit(-1);
		}
		VTfirst[i].flag = 0;
		memset(VTfirst[i].src, 0, MAXCHARNUM);
		memset(VTfirst[i].data, 0, sizeof(DyString));
		init_dystring(VTfirst[i].data, MAXVTPNUM);
	}
	//��ʼ�������ս����first����
	for (int i = 0; i < Tnum; i++)
	{
		VTfirst[i].src = Tstring[i].data;
		for (int j = 0; j < Tstring[i].length; j++)
		{
			charcat_dystring(VTfirst[i].data, Tstring[i].data[j]);
		}
	}
	//��ʼ�����з��ս������
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

int find_vt(char *str)
{
	const char ch1[2] = { '@','\0' };
	const char ch2[2] = { '#','\0' };
	//��Ϊ�մ��Ȳ������ս��Ҳ�����ڷ��ս��
	if (!strcmp(str, ch1))
	{
		return -1;//������һ���մ�
	}
	if (!strcmp(str, ch2))
	{
		return 2;
	}
	int keyhash = elf_hash(str);
	//�����ǲ��Ƿ��ս��
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
	//�����ǲ����ս��
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
	//֤�����ķ����Ų�����
	return -2;
}

int get_first_candidate(int curindex,First_set *first_set,int *flag,const char *str)
{
	int ans = 0;//Ĭ����str�޷��Ƶ����մ�
	char nextchar[MAXVTPNUM];//nextchar[]���ǵ�ǰ����ʽ�Ŀ���FIRSTԪ��
	int nextindex = 0;
	int len = strlen(str);
	int offset = 0;
	int result = 0;//��ʶ����curchar��һ���ս�����Ƿ��ս��
	for (int i = 0; i<len; i++)
	{
		memset(nextchar, 0, MAXVTPNUM);
		offset =get_complete_vt(nextchar, str, i, len - 1);
		//test bug
		if (offset == -1)
		{
			printf("curindex:%d", curindex);
			printf("%d ��������", __LINE__);
			system("pause");
		}
		i += offset - 1;
		//����result��������ķ����ŵ����
		result = find_vt(nextchar);
		
		//���ս�������
		if (result == 1)
		{
			nextindex = find_first_index(nextchar);
			//֤������������ս��first��������������,�������ӹ�Ԫ��
			//if (first_set[nextindex].data->length != 0 && first_set[nextindex].flag)
			if (first_set[nextindex].flag)
			{
				//����first_set[index].data�Ƿ�����Ƶ���'@'
				int is_null = 0;
				is_null = copy_follow_first(first_set[curindex].data, first_set[nextindex].data, flag, &first_set[curindex].flag);
				//��������Ƶ����մ�,�����ò���ʽ��ɨ��
				if (!is_null)
				{
					ans = 0;//��Ϊ���ܱ�ǰһ�����ս���ó�ans=1��
					return ans;
				}
				//������Ƶ����մ�,�ȸı�ansֵ��Ȼ�����ɨ��
				else
				{
					ans = 1;
				}
			}
			//û�����ݸ��ƣ����ǻ������ж����ɲ������Ƴ��մ�
			else if (first_set[nextindex].data->length == 0 || first_set[nextindex].flag == 0)
			{
				//����ΪʲôҪ�ж�һ�£����ʼ�
				//֤����Ҳ�����Ƴ��մ������Ի��ü���ɨ��
				if (find_empty_string(first_set[nextindex].data->data) != -1)
				{
					ans = 1;
					continue;
				}
				ans = 0;
				return ans;
			}
		}
		//�ս�������
		else if (result == 0)
		{
			if (find_string(first_set[curindex].data, nextchar)==-1)
			{
				//��Ϊ�п�����id�����������ַ����ս��
				strcat_dystring(first_set[curindex].data, nextchar);
				//*flag = 1;
				flag !=NULL ? *flag= 1:NULL;
				first_set[curindex].flag = 1;
			}
			//����������ans�ı�
			ans = 0;
			return ans;//��Ϊ����˵��ʽ����һ���ս��ΪFIRST�ģ����Ժ����Ҳ���ò�����
		}
		//�մ����
		else if (result == -1)
		{
			if (find_empty_string(first_set[curindex].data->data) == -1)
			{
				strcat_dystring(first_set[curindex].data, nextchar);
				//*flag = 1;
				flag != NULL ? *flag = 1 : NULL;
				first_set[curindex].flag = 1;
				ans = -1;
				return ans;
			}
			
		}
	}
	return ans;
}

int split_Pstring(char *dst, const char *src, char *split_str,int start)
{
	//�������������ǰ��dstһ��Ҫ����ʼ��
	int index = 0;
	//��ΪĿǰ������������и�'|'�ģ�û����Ҫ��չ�����������ֱ��ʹ��split_str[0]��
	for (int i = start; src[i]!='\0' && src[i]!=split_str[0]; i++,index++)
	{
		dst[index] = src[i];
	}
	dst[index] = '\0';//�������
	return index;
}

void get_first2(DyString *Pstring)
{
	char curchar[2];//����ʽ��ߵķ��ս��
	int curindex = 0;//curchar��FIRST�����е��±�
	int flag = 1;//�ж��ǲ������ķ����Ÿ�����FIRSR����
	int changing_count = 0;//��ʾ�ڼ���ɨ����������ʽ���������ҵ��㷨����ֻ��Ҫ��һ��ɨ���ʱ����Ҫ�ж��ս���Ϳմ���֮���ɨ�����ֻ�����ս��
	char split_char[2] = { '|','\0' };
	while (flag)
	{
		flag = 0;
		changing_count++;
		for (int i = 0; i < Pnum; i++)
		{
			int is_null = 1;//�ò���ʽ�Ƿ�����Ƴ��մ���Ĭ�Ͽ���
			//��ȡ����ʽ�󲿵�һ�����ս�����������Ҫ��֤���ս������ֻ��һ����ĸ���ʽ�󲿽���ֻ��һ���ַ�
			curchar[0] = Pstring[i].data[0];
			curchar[1] = '\0';
			curindex = find_first_index(curchar);

			//�ָ�ò���ʽ
			char subPstring[MAXVTPNUM];
			memset(subPstring, 0, MAXVTPNUM);
			//ÿһ�ο�ʼѭ����ʱ��������Ϊû�и���first����
			VTfirst[curindex].flag = 0;
			int start = 0;
			for (int j = 3; j < Pstring[i].length; j++)
			{
				start = j;
				j+=split_Pstring(subPstring, Pstring[i].data, split_char,start);
				is_null = get_first_candidate(curindex, VTfirst,&flag, subPstring);
				//֤���������ʽ�����Ƴ��մ�
				//��Ϊÿִ��һ��get_first_candidate()����һ������ʽ
				if (is_null == 1)
				{
					if (find_empty_string(VTfirst[curindex].data->data) == -1)
					{
						charcat_dystring(VTfirst[curindex].data, '@');
					}
				}
			}
		}
	}
	return;
}

void get_first(DyString *Pstring)
{
	int flag = 1;//�ж��ǲ������ķ����Ÿ�����FIRSR����
	int curindex = 0;//curindex���Ǵ���ǰ��Ҫ��FIRST�����ķ�������FIRST�����е��±�
	int nextindex = 0;//������Գ�ΪcurFIRSR�����е��ķ�������FIRST�����е��±�
	int changing_count = 0;//��ʾ�ڼ���ɨ����������ʽ���������ҵ��㷨����ֻ��Ҫ��һ��ɨ���ʱ����Ҫ�ж��ս���Ϳմ���֮���ɨ�����ֻ�����ս��
	while (flag)
	{
		flag = 0;
		changing_count++;
		for (int i = 0; i < Pnum; i++)
		{
			//��ȡ����ʽ�󲿵�һ�����ս�����������Ҫ��֤���ս������ֻ��һ����ĸ���ʽ�󲿽���ֻ��һ���ַ�
			char vch[2];
			vch[0] = Pstring[i].data[0];
			vch[1] = '\0';
		
			curindex = find_first_index(vch);
			//ÿһ�ο�ʼѭ����ʱ��������Ϊû�и���first����
			VTfirst[curindex].flag = 0;

			char tmp[MAXVTPNUM];
			memset(tmp, 0, MAXVTPNUM);
			int n = 1;
			//�Ӳ���ʽ���Ҳ��ĵ�һ���ķ����ſ�ʼ
			for (int j = 3; j < Pstring[i].length; j++)
			{
				int n = get_complete_vt(tmp, (const char*)&Pstring[i].data[0], j, Pstring[i].length - 1);
				int result = find_vt(tmp);
				j += n - 1;//��Ϊ�������һ�ζ���˼����ַ�������j��λ�þ͵��ƶ���

				//��������ķ����ŵ����
				//���ս�������
				if (result == 1)
				{
					nextindex = find_first_index(tmp);
					//֤������������ս��first��������������,�������ӹ�Ԫ��
					if (VTfirst[nextindex].data->length != 0 && VTfirst[nextindex].flag)
					{
						//����VTfirst[index].data�Ƿ�����Ƶ���'@'
						int is_null = 0;
						is_null = copy_follow_first(VTfirst[curindex].data, VTfirst[nextindex].data, &flag, &VTfirst[curindex].flag);
						//��������Ƶ����մ����͵��ҵ�vch��һ������ʽ
						if (!is_null)
						{
							while (Pstring[i].data[j] != '|' && j < Pstring[i].length)
							{
								j++;
							}
						}
						//���Ƶ����մ�
						else if (is_null)
						{
							//�������������㣬˵���ñ��ʽ�����Ƴ�һ���մ�
							if (j >= Pstring[i].length - 1)
							{
								int is_contain_empty = 0;//��ʾû�а����մ�
								char ch4[2] = { '@','\0' };
								find_string(VTfirst[curindex].data, ch4) == -1 ?
									is_contain_empty = 0 : is_contain_empty = 1;
								//��������curindex������û�пմ���û�о���ӽ�ȥ
								if (!is_contain_empty)
								{
									strcat_dystring(VTfirst[curindex].data, ch4);
								}
							}
							//����Ļ�����ȥ����һ���ķ����ţ�Ҳ���Ƕ���for()j++��������
							continue;
						}
					}
					//û�����ݣ��͵��ҵ�vch��һ������ʽ�������A����vch��
					//���磺A->Bc|D,���FIRST(B)��ʱû�����ݣ��ͽ�i�Ƶ���|�����Ŵ�
					else if (VTfirst[nextindex].data->length == 0 || VTfirst[nextindex].flag == 0)
					{
						while (Pstring[i].data[j] != '|' && j < Pstring[i].length)
						{
							j++;
						}
					}
				}
				//�ս�������
				else if (result == 0)
				{
					//����ֻ�е�һ��ɨ���ʱ����Ҫ�����ս��
					if (changing_count == 1)
					{
						//��Ϊ�п�����id�����������ַ����ս��
						strcat_dystring(VTfirst[curindex].data, tmp);
						flag = 1;
						VTfirst[curindex].flag = 1;
					}
					while (Pstring[i].data[j] != '|' && j < Pstring[i].length)
					{
						j++;
					}
				}
				//�մ����
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
		//�ҵ��˸ո�����Ӧ���ս����first����
		if (!strcmp(VTfirst[i].src, val))
		{
			return i;
		}
	}
	return -1;
}

int get_complete_vt(char *dst, const char *src,const int start,int end)
{
	//֤���Ǵ�ǰ�����ƻ�ȡһ���������ķ�����
	int n = 1;
	if(start <=end)
	{
		int i = start;
		strncpy(dst, &src[i], n);
		int result = find_vt(dst);
		//�ҵ���һ���ķ�����
		//ֱ����������ķ��������ս�����߷��ս��
		while (result == -2)
		{
			n++;
			//һ����˵�����ܳ������������������еĻ����ܾ���srcû�а�����һЩ���ķ����ŵ��ַ�
			//����д������Ƿ�ֹ�������bug
			//��Ϊn�ǳ��ȣ���end��һ���±�,����������ȹ����һ�����ȳ���
			if (n >end-start+1)
			{
				return -1;
			}
			memset(dst, 0, strlen(dst));//����ոտ����Ķ���
			strncpy(dst, &src[i], n);
			result = find_vt(dst);
		}
		return n;
	}
	//������ǴӺ���ǰ��ȡһ�������ĵ��ķ�����
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
				memset(dst, 0, strlen(dst));//����ոտ����Ķ���
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
		printf("%s   ��FIRST������{ ",arr[i].src);
		int n = 0;
		char tmp[MAXCHARNUM];
		for (int j = 0; j < arr[i].data->length; j++)
		{
			memset(tmp, 0, sizeof(char)*MAXCHARNUM);
			n = get_complete_vt(tmp, arr[i].data->data, j, arr[i].data->length);
			j = j + n - 1;
			//���Ƹ�ʽ��ӡ
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
		Vfollow[i].ch = Vstring[i].data[0];//��Ϊ��Ĭ�Ϸ��ս������ֻ����һ���ַ�
		Vfollow[i].data = (DyString*)malloc(sizeof(DyString));
		if (!Vfollow[i].data)
		{
			printf("�ڴ����ʧ��\n");
			exit(1);
		}
		init_dystring(Vfollow[i].data, MAXCHARNUM);
		//��Ϊ����FOLLOW������˵������һ��ʼ��Ҫ�������ǵ�FIRST���ϣ�����һ��ʼflagĬ�϶���1
		Vfollow[i].flag = 1;
		//˵���ǿ�ʼ���ţ��������һ��'#'��FOLLOW������ȥ
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
	char ch[2] = { '\0','\0' };
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
	//һ����˵�����ߵ���һ��
	return ch[0];
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
			//*flag = 1;
			flag != NULL ? *flag = 1 : NULL;
			*first_follow_flag = 1;
			strcat_dystring(dst, ch1);
		}
	}
	return is_null;
}

int find_string(const DyString *strA, const char *strB)
{
	int tmp_flag = -1;
	for (int i = 0; i < strA->length; i++)
	{
		char ch[MAXCHARNUM];
		memset(ch, 0, MAXCHARNUM);
		int n = get_complete_vt(ch, strA->data, i, strA->length - 1);
		while (n == -1)
		{
			i++;
			if (i >= strA->length)
			{
				printf("%d���޷������ַ�����%s", __LINE__,strA->data);
				return tmp_flag;
			}
			memset(ch, 0, sizeof(char)*MAXCHARNUM);
			n = get_complete_vt(ch, strA->data, i, strA->length - 1);
		}
		i += n - 1;
		if (!strcmp(ch, strB))
		{
			tmp_flag = i;
			break;
		}
	}
	return  tmp_flag;
}

int find_empty_string(const char *str)
{
	char empty= '@';
	for (int i = 0; str[i]!='\0'; i++)
	{
		if (empty == str[i])
		{
			return i;
		}
	}
	return -1;
}

void get_follow(DyString *Pstring)
{
	int flag = 1;//�ж��ǲ������ķ����Ÿ�����FOLLOW����
	int changing_count = 0;//��get_first()�еĺ���һ��
	char curch;//curcahrΪ��ǰ����ʽ��׼�����FOLLOW(curchar)
	int curindex = 0;//curindexΪ��ǰ���ս����Vfollow[]�е��±�
	char nextchar[MAXCHARNUM];//Ϊcurchar����һ���ķ����ţ���Ϊ�п������ս��������ʹ����һ������
	int nextindex = 0;//nextindexΪcurindex�ĺ���һ���ķ�������FOLLOW���ϻ���FIRST�����е��±�
	while (flag)
	{
		changing_count++;
		flag = 0;
		for (int i = 0; i <Pnum; i++)
		{
			char vch = Pstring[i].data[0];//���ʽ����
			int last_character_is_end = 1;//0���ǲ����ԣ�1���ǿ���
			int is_null = 0;//0����nextchar��û�пմ�����֮1�����пմ�

			//����д��һ��ħ��3������Ϊ����Ĳ���ʽ�з��ս������ֻ����һ���ַ�
			for (int j = Pstring[i].length - 1; j >= 3; j--)
			{
				curch = get_variable(Pstring[i].data, &j);
				//����Ĳ�����������ڲ���ʽ�󲿵�
				if (j < 3)
				{
					continue;
				}
				curindex = find_follow_index(curch);
				//Ϊʲô��Ҫchanging_count > 1��ִ������������������׵�˼���ʼ�
				if (changing_count > 1)
				{
					Vfollow[curindex].flag = 0;
				}
				//���j�Ǹñ��ʽ���һ���±� || curch�ĺ���һ���ַ���'|',������'@'����ô˵��curch�Ǹò���ʽ�����һ���ַ�
				if (j == Pstring[i].length - 1 || Pstring[i].data[j + 1] == '|' || Pstring[i].data[j + 1] == '@')
				{
					nextindex = find_follow_index(vch);
					last_character_is_end = 1;
					//Ӧ��Ҫ��Vfollow[nextindex]ֵ���Ƹ�Vfollow[curindex]
					if (Vfollow[nextindex].data->length > 0)
					{
						copy_follow_first(Vfollow[curindex].data, Vfollow[nextindex].data,&flag, &Vfollow[curindex].flag);
					}
				}
				//����ʹ�j���������ҵ�һ���ս�����߷��ս��
				else
				{
					memset(nextchar, 0, MAXCHARNUM);
					//��Ϊcurchar�ڲ���ʽ���±�ʽj,���Ժ���һ���ķ����ŵ��±����j+1
					//�������else,�Ϳ��Ա��j+1�϶���Ϊ '|' ��
					get_complete_vt(nextchar, Pstring[i].data, j + 1, Pstring[i].length - 1);
					nextindex = find_first_index(nextchar);
					int result = find_vt(nextchar);
					//curchar������һ�����ս��
					if (result == 1)
					{
						
						//����nextchar��FIRST,curchar��FOLLOW,�����ظ��ķ��ս�����ƹ�ȥ;���ַ���Ҳ����Ҫ
						is_null = copy_follow_first(Vfollow[curindex].data, VTfirst[nextindex].data,&flag,&Vfollow[curindex].flag);
						//˵��curchar���Գ�Ϊ�ñ��ʽ���һ�����ս��
						if (is_null && last_character_is_end)
						{
							nextindex = find_follow_index(vch);
							copy_follow_first(Vfollow[curindex].data, Vfollow[nextindex].data,&flag, &Vfollow[curindex].flag);
						}
					}
					//curchar������һ���ս��
					else if (result == 0 && changing_count==1)
					{
						if (find_string(Vfollow[curindex].data, nextchar)==-1)
						{
							flag = 1;
							strcat_dystring(Vfollow[curindex].data, nextchar);
						}
						
					}
				}
				
				//����������curchar���׿ɲ�����Ҫ��һ�����Ǵ�������ķ������������ַ����ַ�Ϊ��
				//����last_character_is_end���ڴ���ľ��Ǳ��ַ�
				//ֻ�е���һ�ַ�����Ϊ���һ���ַ���ʱ��Ҫ����
				if (last_character_is_end && j>3)
				{
					char tmp_curch[2];
					memset(tmp_curch, 0, 2);
					tmp_curch[0] = curch;
					int index = find_first_index(tmp_curch);
					char empty_string[2] = { '@','\0' };
					
					find_string(VTfirst[index].data, empty_string) == -1 ?
						last_character_is_end = 0 : last_character_is_end= 1;
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
		printf("%c   ��FOLLOW������{ ", arr[i].ch);
		int n = 0;
		char tmp[MAXCHARNUM];
		for (int j = 0; j < arr[i].data->length; j++)
		{
			memset(tmp, 0, sizeof(char)*MAXCHARNUM);
			n = get_complete_vt(tmp, arr[i].data->data, j, arr[i].data->length);
			j = j + n - 1;
			//���Ƹ�ʽ��ӡ
			j == arr[i].data->length - 1 ?
				printf("%s", tmp) : printf("%s,", tmp);
		}
		printf(" }\n");
	}
	return;
}

int is_intersection(const char *strA, const char *strB)
{
	int father[256];
	memset(father, -1, sizeof(int) * 256);
	int flag = 0;//��ʾû�н���
	for (int i = 0; strA[i] !='\0'; i++)
	{
		father[strA[i]] = strA[i];
	}
	for (int i = 0; i < strB[i] != '\0'; i++)
	{
		if (father[strB[i]] != -1)
		{
			flag = 1;
			break;
		}
	}
	return flag;
}

int is_spilt(const char *str)
{
	char splitchar = '|';
	for (int i = 0; str[i] != '\0'; i++)
	{
		if (str[i] == splitchar)
		{
			return 1;
		}
	}
	return 0;
}

void get_string_first(First_set *cur_first_set,int curindex, const char *str, First_set *vtfirst)
{
	char nextchar[MAXVTPNUM];//nextchar[]���ǵ�ǰ����ʽ�Ŀ���FIRSTԪ��
	int nextindex = 0;
	int len = strlen(str);
	int offset = 0;
	int result = 0;//��ʶ����curchar��һ���ս�����Ƿ��ս��
	for (int i = 0; i < len; i++)
	{
		memset(nextchar, 0, MAXVTPNUM);
		offset = get_complete_vt(nextchar, str, i, len - 1);
		//test bug
		if (offset == -1)
		{
			printf("curindex:%d", curindex);
			printf("%d ��������", __LINE__);
			system("pause");
		}
		i += offset - 1;
		//����result��������ķ����ŵ����
		result = find_vt(nextchar);
		//���ս�������
		if (result == 1)
		{
			nextindex = find_first_index(nextchar);
			//����first_set[index].data�Ƿ�����Ƶ���'@'
			int is_null = 0;
			is_null = copy_follow_first(cur_first_set[curindex].data, vtfirst[nextindex].data, NULL, &vtfirst[curindex].flag);
			//��������Ƶ����մ�,�����ò���ʽ��ɨ��
			if (!is_null)
			{
				return;
			}
			//������Ƶ����մ�,����ɨ��
			else
			{
				continue;
			}
		}
		//�ս�������
		else if (result == 0)
		{
			
			strcat_dystring(cur_first_set[curindex].data, nextchar);
			return ;
		}
		//�մ����
		else if (result == -1)
		{
			strcat_dystring(cur_first_set[curindex].data, nextchar);
			return;
		}
	}
}

int is_LL1(DyString *Pstring,DyString *Vstring,DyString *Tstring,First_set *vtfirst,Follow_set *vfollow)
{
	char split_char[2] = { '|','\0' };
	int maxright = 8;//����һ�����ս����������8���Ҳ�
	char **res = (char**)malloc(sizeof(char*)*maxright);
	if (res == NULL)
	{
		printf("%d:�ڴ����ʧ��\n",__LINE__);
		exit(1);
	}
	memset(res, 0, sizeof(char*)*maxright);
	for (int i = 0; i < maxright; i++)
	{
		res[i] = (char*)malloc(sizeof(char)*MAXVTPNUM);
		if (!res[i])
		{
			printf("%d:�ڴ����ʧ��\n", __LINE__);
			exit(1);
		}
		memset(res[i], 0, sizeof(char)*MAXVTPNUM);
	}
	int index = 0;//���Էַֽ�������±�
	int start = 3;//��ʼ�ֽ���±꣬�ֽ���Ҳ���ʼ
	int cnt = 0;//һ������ʽ�ֽ�����ĸ���
	First_set first_set[2];//��ʱ���������������ʽ�Ҳ���FIRST����
	for (int i = 0; i < 2; i++)
	{
		first_set[i].src = (char*)malloc(sizeof(char));
		memset(first_set[i].src, 0, sizeof(char));
		first_set[i].data = (DyString*)malloc(sizeof(DyString));
		memset(first_set[i].data, 0, sizeof(DyString));
		init_dystring(first_set[i].data, MAXVTPNUM);
		first_set[i].flag = 0;
	}
	for (int i = 0; i < Pnum; i++)
	{
		index = 0;
		start = 3;
		cnt = 0;
		char vch[2] = { '\0','\0' };//����ʽ�󲿷��ս��
		vch[0] = Pstring[i].data[0];
		//�����ж���FIRST���Ϻ�FOLLOW���ϵ�
		if (find_empty_string(vtfirst[find_first_index(vch)].data->data)!=-1)
		{
			if (is_intersection(
				vtfirst[find_first_index(vch)].data->data,
				vfollow[find_follow_index(vch[0])].data->data))
			{
				printf("%c��FIRSR���Ϻ�FOLLOW������FIRST�����Ƴ��մ���ǰ�����н���", vch[0]);
				return 0;
			}
			
		}
		if (is_spilt(Pstring[i].data))
		{
			while (start < Pstring[i].length)
			{
				start += split_Pstring(res[index], Pstring[i].data, split_char, start) + 1;
				index++;
				cnt++;
			}
			for (int i = 0; i < cnt; i++)
			{
				//memset(&(first_set[0].data->data), 0, sizeof(DyString));
				//���������Ҫ���
				memset(first_set[0].data->data, 0, sizeof(char)*strlen(first_set[0].data->data));
				first_set[0].data->length = 0;
				get_string_first(first_set,0, res[i], vtfirst);
				for (int j = i + 1; j < cnt; j++)
				{
					memset(first_set[1].data->data, 0, sizeof(char)*strlen(first_set[1].data->data));
					first_set[1].data->length = 0;
					get_string_first(first_set, 1,res[j],vtfirst);
					if (is_intersection(first_set[0].data->data, first_set[1].data->data))
					{
						printf("FIRST(%s)��FIRST(%s)�н���\n", res[i], res[j]);
						return 0;
					}
				}
			}
		}
	}
	return 1;
}

char*** init_analysis_table(int vnum,int tnum)
{
	char ***ans = (char***)malloc(sizeof(char**)*vnum);
	if (!ans)
	{
		printf("%d:�ڴ����ʧ��\n",__LINE__);
		exit(1);
	}
	memset(ans, 0, sizeof(char**)*vnum);
	for (int i = 0; i < vnum; i++)
	{
		//��Ϊ����һ��'#'��Ҫ���һ��
		ans[i] = (char**)malloc(sizeof(char*)*(tnum+1));
		if (!ans[i])
		{
			printf("%d:�ڴ����ʧ��\n", __LINE__);
			exit(1);
		}
		memset(ans[i], 0, sizeof(char*)*(tnum+1));
		for (int j = 0; j < tnum+1; j++)
		{
			ans[i][j] = (char*)malloc(sizeof(char)*MAXCHARNUM);
			if (!ans[i][j])
			{
				printf("%d:�ڴ����ʧ��\n", __LINE__);
				exit(1);
			}
			memset(ans[i][j], 0, sizeof(char)*MAXCHARNUM);

		}
	}
	return ans;
}

void create_predictive_analysis_table(char ***analysis_table,DyString *Pstring,int pnum,DyString *Vstring,int vnum,DyString *Tstring,int tnum,First_set *vtfirst,Follow_set *vfollow)
{
	char tmp_candidata[MAXCHARNUM];//�����ʱ���ָ�Ĳ���ʽ(û�в���ʽ��)
	memset(tmp_candidata, 0, sizeof(char)*MAXCHARNUM);
	First_set tmp_candidata_first[1];//��Ų���ʽ��FIRST����
	tmp_candidata_first[0].src = (char*)malloc(sizeof(char));
	tmp_candidata_first[0].flag = 0;
	tmp_candidata_first[0].data = (DyString*)malloc(sizeof(DyString));
	init_dystring(tmp_candidata_first[0].data,MAXCHARNUM);

	char split_str[2] = { '|','\0' };
	char left_candidate[2] = { '\0','\0' };//������ű��ʽ�󲿷��ս��
	char connect_str[3] = { '-','>','\0' };
	int n = 0;
	for (int i = 0; i < pnum; i++)
	{
		left_candidate[0] = Pstring[i].data[0];
		//���֮ǰ������
		memset(tmp_candidata_first[0].data->data, 0, tmp_candidata_first[0].data->length);
		tmp_candidata_first[0].data->length = 0;
		if (is_spilt(Pstring[i].data))
		{
			for (int j = 3; j < Pstring[i].length; j++)
			{
				memset(tmp_candidata_first[0].data->data, 0, tmp_candidata_first[0].data->length);
				tmp_candidata_first[0].data->length = 0;
				n = split_Pstring(tmp_candidata, Pstring[i].data, split_str, j);
				j += n;
				//��ȡ����ѡʽ��first����
				get_string_first(tmp_candidata_first, 0, tmp_candidata, vtfirst);
				if (find_empty_string(tmp_candidata_first->data->data) != -1)
				{
					//����ѡʽ���󲿵�FOLLOW������ӵ��������Ӧ��λ����
					add_data_to_analysis_table(analysis_table, vfollow[find_follow_index(left_candidate[0])].data->data, left_candidate[0],tmp_candidata, Vstring, vnum, Tstring, tnum);
				}
				else
				{
					//����ѡʽFIRST���뵽������Ķ�Ӧλ����
					add_data_to_analysis_table(analysis_table, tmp_candidata_first[0].data->data, left_candidate[0],tmp_candidata, Vstring, vnum, Tstring, tnum);
				}
			}
		}
		else
		{
			memset(tmp_candidata, 0, strlen(tmp_candidata));
			//��ȡ����ѡʽ��first����
			get_string_first(tmp_candidata_first, 0, Pstring[i].data, vtfirst);
			split_Pstring(tmp_candidata, Pstring[i].data, connect_str, 3);
			if (find_empty_string(tmp_candidata_first->data->data) != -1)
			{
				//����ѡʽ���󲿵�FOLLOW������ӵ��������Ӧ��λ����
				add_data_to_analysis_table(analysis_table, vfollow[find_follow_index(left_candidate[0])].data->data, left_candidate[0],tmp_candidata, Vstring, vnum, Tstring, tnum);
			}
			else
			{
				//����ѡʽFIRST���뵽������Ķ�Ӧλ����
				add_data_to_analysis_table(analysis_table, tmp_candidata_first[0].data->data, left_candidate[0],tmp_candidata, Vstring, vnum, Tstring, tnum);
			}
		}
	}
}

void add_data_to_analysis_table(char ***analysis_table, const char *candidate_set, char left_canditate,const char *right_candidate,DyString *Vstring,int vnum,DyString *Tstring,int tnum)
{
	int x_index, y_index;//�ֱ���left_candidate��right_candidate����analysis_table�еĺ����±�
	char x_string[2] = { '\0','\0' };
	x_string[0] = left_canditate;
	x_index = find_analysis_index(x_string,Vstring,vnum,Tstring,tnum);
	
	//��ǰ�ڸ�FIRST�������ҵ����ս������
	//��Ϊһ�����ս�������Ƕ���ַ�
	char y_string[MAXCHARNUM];
	memset(y_string, 0, sizeof(char)*MAXCHARNUM);
	int offset = 0;
	int len = strlen(candidate_set);
	for (int i = 0; i < candidate_set[i]!='\0'; i++)
	{
		offset=get_complete_vt(y_string, candidate_set, i, len - 1);
		i += offset - 1;
		y_index = find_analysis_index(y_string, Vstring, vnum,Tstring,tnum);
		//���ʱ������Ϊ���y_string��һ���մ�����Ӧ�������ﴦ��
		if (y_index == -1)
		{
			continue;
		}
		strcpy(analysis_table[x_index][y_index], right_candidate);
	}
	return;
}

int find_analysis_index(char *str, DyString *Vstring, int vnum,DyString *Tstring,int tnum)
{
	char tmp[MAXCHARNUM];
	memset(tmp, 0, sizeof(char)*MAXCHARNUM);
	//�����û�б�Ҫʹ��get_complete_vt()�����ķ���ֵ
	//������һ���ı�i��ֵ������ÿһ�ζ��Ǵ�0��ʼ��ȡ��
	//ԭ��ȥ���Լ��Ķ�Ӧ�뷨
	for (int i = 0; i < vnum; i++)
	{
		memset(tmp, 0, sizeof(char)*strlen(tmp));
		//ע����������Ĳ���0����ָÿһ�ζ��Ǵ�Vstring[i].data��㴦��ʼ��ȡһ���������ս�����߷��ս��
		get_complete_vt(tmp, Vstring[i].data, 0, Vstring[i].length - 1);
		if (!strcmp(str, tmp))
		{
			return i;
		}
	}
	for (int i = 0; i < tnum; i++)
	{
		memset(tmp, 0, sizeof(char)*strlen(tmp));
		get_complete_vt(tmp, Tstring[i].data, 0, Tstring[i].length - 1);
		if (!strcmp(str, tmp))
		{
			return i;
		}
	}
	if (str[0] == '#')
	{
		return tnum;
	}
	return -1;
}

void display_analysis_table(char*** analysis_table, DyString *Vstring,int vnum, DyString *Tstring,int tnum)
{
	printf("V\\T |\t");
	for (int i = 0; i < tnum; i++)
	{
		printf("%s \t|\t", Tstring[i].data);
	}
	printf("#\t|\n");
	for (int i = 0; i <(tnum+1)*2; i++)
	{
		printf("--------");
	}
	printf("\n");
	for (int i = 0; i < vnum; i++)
	{
		printf("%c |\t", Vstring[i].data[0]);
		for (int j = 0; j < tnum+1; j++)
		{
			printf("%s \t|\t",analysis_table[i][j]);
		}
		printf("\n\n");
	}
	for (int i = 0; i < (tnum + 1) * 2; i++)
	{
		printf("--------");
	}
	printf("\n");
	return;
}

int LL1Parsing(char *input, char ***analysis_table,DyString *analysis_stack, DyString *Vstring, int vnum, DyString *Tstring, int tnum)
{
	int i = 0;
	DyString topchar;//����ջջ����Ԫ��
	int x_index = 0,y_index = 0;//�ֱ��ʾջ��Ԫ���ڷ������еĺ������������
	char tmpchar[MAXCHARNUM];//���ڱ������봮�е�һ���������ķ�����
	memset(tmpchar, 0, sizeof(char)*MAXCHARNUM);
	int offset = 0;
	while (input[i] != '\0')
	{
		topchar = gettop_stack(analysis_stack);
		memset(tmpchar, 0, sizeof(char)*strlen(tmpchar));
		offset = get_complete_vt(tmpchar, input, i, strlen(input) - 1);
		//ջ��Ԫ�غ����������δ�䱸�ַ����
		if (!strcmp(topchar.data, tmpchar))
		{
			pop_stack(analysis_stack);
			i += offset;
		}
		//����ȵĻ��͵ø��ݷ�������������ջ����Ԫ��
		//ע�⣬���ڷ������еõ�����һ����ʽ�ӵ��Ҳ�������ÿһ����ջ��Ԫ��ֻ����һ���ս�����߷��ս��
		else
		{
			x_index = find_analysis_index(topchar.data, Vstring, vnum, Tstring, tnum);
			y_index = find_analysis_index(tmpchar, Vstring, vnum, Tstring, tnum);
			char* tmp=analysis_table[x_index][y_index];
			//���ִ������
			if (strlen(tmp) == 0)
			{
				return 0;
			}
			//���Ƚ�ջ��Ԫ�س�ջ
			pop_stack(analysis_stack);
			//Ȼ��ѷ�������ÿһ�������ķ�������ջ
			for (int j = strlen(tmp) - 1; j >= 0; j--)
			{
				memset(tmpchar, 0, strlen(tmpchar));
				offset = get_complete_vt(tmpchar, tmp, j, 0);
				j = j - offset + 1;
				//����Ƶ������ǿմ����ǲ�������ջ��
				if (tmpchar[0] != '@')
				{
					push_stack(analysis_stack, charstr_to_DyString(tmpchar));
				}
			}
		}
	 }
	if (is_empty_stack(analysis_stack))
	{
		return 1;
	}
	else
	{
		return 0;
	}
	
}