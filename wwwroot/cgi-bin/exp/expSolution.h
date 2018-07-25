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

#define IS_NUM_CH(c) (isdigit(c)!=0)//是否为数字字符
#define IS_FUN_CH(c) ((c)>='A' && (c)<='F')//是否为函数字符
#define IS_X(c) (c=='x')//是否为为未知数x
#define IS_OP(c) (c=='+'||c=='-'||c=='*'||c=='/' || c=='^')//判断是否为运算符
#define IS_INVALID_CH(c) ((!IS_FUN_CH(c))&&(!IS_X(c))&&(!IS_OP(c))&&(!IS_NUM_CH(c)) && (c!='.')\
	&& (c != '(') && (c != ')'))//是否为合法字母字符


void RepFun(string& str);
void DelSapce(string& str);
void Preprocessor(string& str);
void InfixToPostfix(const string exp, queue<string>& postFix);
BinaryTreeNode<string>* CreateBinTreeExp(queue<string>& postFix);
BinaryTreeNode<string>* compute(BinaryTreeNode<string>* pNode, BinaryTreeNode<string>* val);
BinaryTreeNode<string>* dao(BinaryTreeNode<string>* pRoot);
BinaryTreeNode<string>* calculate(string& exp);
string ExpResult(string& exp);
