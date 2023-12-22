#ifndef COMMAND_H
#define COMMAND_H
#include "util.h"


struct Command{
	bool is_back = false;	//�Ƿ��̨���� 
	int com_id = -1;		//ָ��ID�����ڵ��ö�Ӧ������-1Ϊ��Чָ�� 
	string option ;			//ָ��ѡ�������ѡ��� 
	string para_a ;			//����1 
	string para_b ;			//����2 
	bool is_in_relocate = false;	//�Ƿ������ض�λ 
	bool is_out_relocate = false;	//�Ƿ�����ض�λ 
	bool cover_or_add = false;		//����ض�λΪ׷�ӻ��߸��� , ����Ϊtrue 
	string relocated_file ;			//�ض�λ�ļ�Ŀ¼ 
	bool is_head = false ;			//ָ���Ƿ��ڶ�ͷ 
	bool is_tail = false ;			//ָ���Ƿ��ڶ�β 
	const char* out ;
	const char* in ;
	std::stack<std::string> commandStack; 
};

typedef int (*FunVector)(Command* com_block); 		//����һ������ָ�� 

//***********����ĺ������뵽���� ****************** 
int Redirection(Command* com_block);
int help(Command* com_block);
int pwd(Command* com_block);
int mv(Command* com_block);
int tune(Command* com_block);
int ls(Command* com_block);
int history(Command* com_block);
int rm(Command* com_block);
// ***********����ĺ������뵽���� ****************** 


//************���º�������**************** 
const int COM_NUM = 7;
//************����������**************** 
extern FunVector fv[];
//************���º������Ʊ�**************** 
	#define FUNCTION_LIST "Redirection","help","pwd","mv","ls","history","rm"

	#endif