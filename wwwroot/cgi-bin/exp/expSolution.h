#pragma once

#include<string>
#include<cmath>
#include<iostream>
#include<cctype>
#include<queue>
#include<stack>
#include<string>
#include<cstring>
#include<cstdlib>
#include<cstdio>
#include"BinaryTree.h"
using namespace std;

#define IS_NUM_CH(c) (isdigit(c)!=0)//�Ƿ�Ϊ�����ַ�
#define IS_FUN_CH(c) ((c)>='A' && (c)<='F')//�Ƿ�Ϊ�����ַ�
#define IS_X(c) (c=='x')//�Ƿ�ΪΪδ֪��x
#define IS_OP(c) (c=='+'||c=='-'||c=='*'||c=='/' || c=='^')//�ж��Ƿ�Ϊ�����
#define IS_INVALID_CH(c) ((!IS_FUN_CH(c))&&(!IS_X(c))&&(!IS_OP(c))&&(!IS_NUM_CH(c)) && (c!='.')\
	&& (c != '(') && (c != ')'))//�Ƿ�Ϊ�Ϸ���ĸ�ַ�


void RepFun(string& str);
void DelSapce(string& str);
void Preprocessor(string& str);
void InfixToPostfix(const string exp, queue<string>& postFix);
BinaryTreeNode<string>* CreateBinTreeExp(queue<string>& postFix);
BinaryTreeNode<string>* compute(BinaryTreeNode<string>* pNode, BinaryTreeNode<string>* val);
BinaryTreeNode<string>* dao(BinaryTreeNode<string>* pRoot);
BinaryTreeNode<string>* calculate(string& exp);
string ExpResult(string& exp);
