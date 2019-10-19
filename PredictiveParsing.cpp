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
//�ս�������ս�������ʽ�ĸ���(һ��ҪС��MAXVTPNUM)
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

	//Ϊ�ķ�G�е��ս���ͷ��ս������hash��
	establish_hash(Vstring, Vnum,VNode);
	establish_hash(Tstring, Tnum, TNode);

	printf("*******************************\n");

	//����FIRST�㷨
	//��ʼ��VTfirst����
	init_first_set(Tstring,Vstring);

	//�����ķ��ı��ʽ������FIRST����
	get_first(Vstring, Pstring);
	display_first(VTfirst, Tnum + Vnum);
	
	printf("*******************************\n");

	//����FOLLOW�㷨
	init_follow_set(Vstring,S);
	get_follow(Pstring);
	display_follow(Vfollow, Vnum);
	return 0;
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

//����ֵ��'#'�������->2 ���ս��->1,�ս��->0,�մ�->-1,�����ڷ���-2
//������������⣺
//1.��Ϊ��������ս�����Ƿ��ս�������Զ���ÿһ�ε���
//��������һ�μ��ɡ�����ĺ����ڶ�������Ӧ�ô���TNode
//2.��һ��ʼ����������ģ�int find_vt(DyString *data, Node **arr);
//���Ƕ��ڱ��ʽ��˵�������ܽ�����ֻ��Ҫ��ⲿ�ַ��ţ�����
//����һ��DyString��̫�ô���
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

void get_first(DyString *Vstring, DyString *Pstring)
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
								is_contain_empty=find_string(VTfirst[curindex].data, ch4);
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
			memset(dst, 0, MAXCHARNUM);//����ոտ����Ķ���
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
				memset(dst, 0, MAXVTPNUM);//����ոտ����Ķ���
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
	char ch[2];
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
			*flag = 1;
			*first_follow_flag = 1;
			strcat_dystring(dst, ch1);
		}
	}
	return is_null;
}

int find_string(const DyString *strA, const char *strB)
{
	int tmp_flag = 0;
	for (int i = 0; i < strA->length; i++)
	{
		char ch[MAXCHARNUM];
		memset(ch, 0, MAXCHARNUM);
		int n = get_complete_vt(ch, strA->data, i, strA->length - 1);
		i += n - 1;
		if (!strcmp(ch, strB))
		{
			tmp_flag = 1;
			break;
		}
	}
	return  tmp_flag;
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
						if (!find_string(Vfollow[curindex].data, nextchar))
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
					last_character_is_end = 0;
					for (int i = 0; i < VTfirst[index].data->length; i++)
					{
						//��Ȼfirst������������id�����ķ��ս��������
						//'@'(���ַ�������ֻ��һ��)�����Ծ�ֱ�����Ƚ���
						if (VTfirst[index].data->data[i] == '@')
						{
							last_character_is_end = 1;
							is_null = 1;
							break;
						}
					}
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
