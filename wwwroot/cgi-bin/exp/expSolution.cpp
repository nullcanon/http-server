
#include"expSolution.h"

/*************************表达式预处理*********************************/
// 替换字符串中的函数
// 是cos->A sin->B tan->C cot->D ln->E log->F 
void RepFun(string& str)
{
	if (str == "")
		throw "表达式为空";

	const char* strs[] = { "cos", "sin", "tan", "cot","ln","log"};
	const char* chs[] = { "A", "B", "C", "D", "E", "F" };
	size_t i = 0;
	while (i<sizeof(strs)/sizeof(strs[0])) 
	{
		if (str.find(strs[i]) == string::npos)
		{
			++i;
			continue;
		}
		str.replace(str.find(strs[i]), strlen(strs[i]), chs[i]);
		++i;
	}
}





//去掉字符串中所有空格' '
void DelSapce(string& str)
{
	for (string::iterator it = str.begin(); it != str.end(); ++it)
	{
		if (*it == ' ')
			str.erase(it);
	}
}

//预处理字符串未知数为 x
// 1. 5x --> 5*x  cosxsinx --> ax * bx
//    如果未知数前面是一个数字字符，为未知数前面添加*
//	  如果未知数后面是一个函数字符，为未知数后面添加*
// 2.-x --> (0-x)
// 3. +5-x --> 5-x
//    如果第一个字符为‘+’，删除即可

//isalnum  isalpha
//如果是字母，1.是x 
		  
void Preprocessor(string& str)
{
	size_t cur = 0;
	if (str == "")
		throw "表达式为空";
	RepFun(str);
	DelSapce(str);
	// +5-x --> 5-x
	if (str[0] == '+')
		str.erase(0, 1);

	// 5x --> 5*x  cosxsinx --> ax * bx  x^2x --> x^2 * x
	while (cur < str.length())
	{
		if (IS_INVALID_CH(str[cur]))
			throw "表达式错误1";//含有非法字符
		if (IS_X(str[cur]))
		{
			if (IS_X(str[cur]))
			{
				//1.
				//判断x前一个字符是否为数字字符和函数字符
				//如果前一个字符不是数字字符也不是函数字符也不是运算符号(^x没处理)，return false
				//如果前一个字符是数字字符，在x前面添加*
				if (cur > 0)
				{
					char pre_ch = str[cur - 1];
					if (IS_NUM_CH(pre_ch))
						str.insert(cur, "*");
				}
				//2.
				//判断x后一个字符是否为函数字符或x
				//如果不是函数字符、x、运算符return false
				//如果后一个字符是函数字符，在x后面添加*
				if (cur < str.length() - 1)
				{
					char next_ch = str[cur + 1];
					if (IS_FUN_CH(next_ch))
						str.insert(cur + 1, "*");
				}
			}
		}
		cur++;
	}
	cur = 0;
	while (cur < str.length())
	{
		// -x --> (0-x)  , -5x --> 0-5*x , -Ax --> 0-Ax
		//如果碰到 - 
		//1.前面是数字或者x，不进行处理
		//2.前面没有字符，从-前面到下一个 操作符 或者 结束位置 为止添加0
		//3.前面是（，在（后面添加0
		if (str[cur] == '-')
		{
			if (cur > 0)
			{
				char pre_ch = str[cur - 1];
				if (IS_NUM_CH(pre_ch) || IS_X(pre_ch))
				{
					cur++;
					continue;
				}
				else if (pre_ch == '(')
				{
					str.insert(cur, "0");
				}
				/*else
					throw "表达式错误2";*/
			}
			else
				str.insert(0, "0");
			
		}
		cur++;
	}
}

/****利用调度场算法将中缀表达式转化为后缀表达式*****/


//操作符优先级
static int OP_PRECED(char c)
{
	switch (c)
	{
	case'*': case'/': return 1;
	case'^': return 2;
	case'+': case'-':return 0;
	}
	return -1;
}
//解析是否为合格的数字字符串
static bool ParseFloat(const string str)
{
    (void)str;
	return true;
}

void InfixToPostfix(const string exp,queue<string>& postFix)
{
	stack<string> stack;
	for (size_t i = 0; i < exp.length(); ++i)
	{
		//数字
		if (IS_NUM_CH(exp[i]))
		{
			// 123 , 1.23
			// 将一个数字提取出来构建成字符串
			// 向后读取，当前读取的是.或者读取的是数字，一直向下读取
			string tmp;
			size_t j = i;
			for (; j < exp.length() && (exp[j] == '.' || IS_NUM_CH(exp[j])); ++j)
				tmp.insert(tmp.end(), 1, exp[j]);
			
			if (!ParseFloat(tmp))
				throw "表达式错误3";//数字不合格
			postFix.push(tmp);
			i = j-1;
		}
		//特殊字符 e x
		else if (exp[i] == 'e' || exp[i] == 'x')
			postFix.push(string(1,exp[i]));
		//函数分割符 ，
		else if (exp[i] == ',')
		{
			//从栈当中不断地弹出操作符并且放入输出队列中去，
			//直到栈顶部的元素为一个左括号为止。如果一直没有遇到左括号，
			//那么要么是分隔符放错了位置，要么是括号不匹配。
			bool mk = false;
			while (!stack.empty() && stack.top() != string(1, '('))
			{
				if (stack.top() == string(1, '('))
				{
					mk = true;
					break;
				}
				postFix.push(stack.top());
				stack.pop();
			}
			if (!mk)
				throw "表达式错误4";//括号不匹配
		}
		//运算符
		else if (IS_OP(exp[i]))
		{
			char o1 = exp[i];
			if (!stack.empty())
			{
				char o2 = stack.top()[0];
				while (IS_OP(o2) && /*o2>=o1*/OP_PRECED(o2) >= OP_PRECED(o1))
				{
					postFix.push(stack.top());
					stack.pop();
					if (stack.empty())
						break;
					o2 = stack.top()[0];
				}
			}
			stack.push(string(1, o1));
		}
		//左括号
		else if (exp[i] == '(')
			stack.push(string(1, '('));
		//右括号
		else if (exp[i] == ')')
		{
			//  从栈当中不断地弹出操作符并且放入输出队列中，直到栈顶部的元素为左括号为止。
			//	将左括号从栈的顶端弹出，但并不放入输出队列中去。
			//	如果此时位于栈顶端的记号表示一个函数，那么将其弹出并放入输出队列中去。
			//	如果在找到一个左括号之前栈就已经弹出了所有元素，那么就表示在表达式中存在不匹配的括号。
			while (!stack.empty() && stack.top()[0] != '(')
			{
				postFix.push(stack.top());
				stack.pop();
			}
			if (stack.empty())
				throw "表达式错误5";//括号不匹配
			if (!stack.empty() && stack.top()[0] == '(')
			{
				stack.pop();
				if (!stack.empty() && IS_FUN_CH(stack.top()[0]))
				{
					postFix.push(stack.top());
					stack.pop();
				}
			}
		}
		//函数字符
		else if (IS_FUN_CH(exp[i]))
			stack.push(string(1, exp[i]));
		else
			throw "表达式错误6";
	}
	if (!stack.empty())
	{
		if (stack.top()[0] == '(' || stack.top()[0] == ')')
			throw "表达式错误7";//括号不匹配
		else
		{
			while (!stack.empty())
			{
				postFix.push(stack.top());
				stack.pop();
			}
		}
	}
}
/************构建二叉表达树************************/
//  1.依次读取表达式；
//  2. 如果是操作数，则将该操作数压入栈中；
//  3. 如果是操作符，则弹出栈中的两个操作数，第一个弹出的操作数作为右孩子，
//     第二个弹出的操作数作为左孩子；然后再将该操作符压入栈中。
// 构建二叉树栈，出来完成后，栈中只剩下唯一一个元素，此为表达式二叉树
// 如果该二叉树没有左右子树为错误
BinaryTreeNode<string>* CreateBinTreeExp(queue<string>& postFix)
{
	//TODO if (postFix.empty()) 抛异常
	stack<BinaryTreeNode<string>*> stack;
	BinaryTreeNode<string>* expBinTree;
	while (!postFix.empty())
	{
		string s = postFix.front();
		if (IS_NUM_CH(s[0]) || IS_X(s[0]) || s[0] == 'e')
		{
			stack.push(new BinaryTreeNode<string>(s));
			postFix.pop();
		}
		else
		{
			if (s[0] == '(' || s[0] == ')')	throw "表达式错误8";
			expBinTree = new BinaryTreeNode<string>(s);
			postFix.pop();
			expBinTree->_pRight = stack.top();
			stack.pop();
			if (IS_OP(s[0]) || s[0] == 'F')
			{
				expBinTree->_pLeft = stack.top();
				stack.pop();
				//TODO,判断左操作数，错误抛异常
			}
			stack.push(expBinTree);
		}
	}
	expBinTree = stack.top();
	//TODO 如果expBinTree的左和右都为空，抛异常
	//TODO 如果stack里面的元素大于1，抛异常
	return expBinTree;
}
//char* strs[] = { "cos", "sin", "tan", "cot", "ln", "log" };
//char* chs[] = { "A", "B", "C", "D", "E", "F" };
/****************求利用二叉表达树值******************/
static double op(char op, double left, double right)
{
	switch (op)
	{
	case'+': return left + right;
	case'-': return left - right;
	case'*': return left * right;
	case'/': //if (right == 0) return false;异常
		return left / right;
	case'^': return pow(left, right);
	case'A': return cos(right);
	case'B': return sin(right);
	case'C': return tan(right);
	case'D': return 1/tan(right);
	case'E': return log(right);
	case'F': return log(right) / log(left);
	default:
		//return false;异常
		throw "表达式错误9";
		break;
	}
}

BinaryTreeNode<string>* compute(BinaryTreeNode<string>* pNode,BinaryTreeNode<string>* val)
{
	if (pNode == NULL)
		return  (new BinaryTreeNode<string>("0"));
	char ch[16] = { 0 };
	if (pNode->_pLeft == NULL && pNode->_pRight != NULL)
	{
		sprintf(ch, "%8f", op(pNode->_data[0], 0,
			atof(compute(pNode->_pRight, val)->_data.c_str())));
		return (new BinaryTreeNode<string>(ch));
	}
	if (pNode->_pLeft != NULL && pNode->_pRight != NULL)
	{
		sprintf(ch, "%8f", op(pNode->_data[0], atof(compute(pNode->_pLeft, val)->_data.c_str()),
			atof(compute(pNode->_pRight, val)->_data.c_str())));
		return (new BinaryTreeNode<string>(ch));
	}
	switch (pNode->_data[0])
	{
	case'x':
		return val;
	case'e':
		sprintf(ch, "%8f", exp(1.0));
		return (new BinaryTreeNode<string>(ch));
	default:
		return pNode;
		break;
	}
}

/**********************利用二叉表达树求导函数树****************/


BinaryTreeNode<string>* dao(BinaryTreeNode<string>* pRoot)
{
	char c = pRoot->_data[0];
	BinaryTreeNode<string>* ret;
	switch (c)
	{
	//case'+':
	//case'-':
	//	// (l+r)' = l'+r'
	//	// (l-r)' = l'-r'1
	//	// 空间在这边管理，构造函数里面进行指针赋值
	//	ret = new BinaryTreeNode<string>(pRoot->_data, dao(pRoot->_pLeft), dao(pRoot->_pRight));
	//	break;
	//case'*':
	//	//(l*r)' = l'*r + l*r'
	//{
	//		   ret = new BinaryTreeNode<string>("+");
	//		   ret->_pLeft = new BinaryTreeNode<string>("*", dao(pRoot->_pLeft), pRoot->_pRight);
	//		   ret->_pRight = new BinaryTreeNode<string>("*", pRoot->_pLeft, dao(pRoot->_pRight));
	//}
	//	break;
	//

	case'+':
	case'-':
	{
			   // (l+r)' = l'+r'
			   // (l-r)' = l'-r'
			   BinaryTreeNode<string>* l =dao(pRoot->_pLeft);
			   BinaryTreeNode<string>* r = dao(pRoot->_pRight);
			   ret = new BinaryTreeNode<string>(pRoot->_data, l, r);
	}
		break;
	case'*':
	{
			   //(l*r)' = l'*r + l*r'
			   BinaryTreeNode<string>* left = new BinaryTreeNode<string>("*",dao(pRoot->_pLeft),pRoot->_pRight);
			   BinaryTreeNode<string>* right = new BinaryTreeNode<string>("*", pRoot->_pLeft, dao(pRoot->_pRight));
			   ret = new BinaryTreeNode<string>("+",left,right);
			  
	}
		break;

	//case'+':
	//case'-':
	//{
	//		   // (l+r)' = l'+r'
	//		   // (l-r)' = l'-r'
	//		   BinaryTreeNode<string>* l = new BinaryTreeNode<string>(dao(pRoot->_pLeft)->_data);
	//		   BinaryTreeNode<string>* r = new BinaryTreeNode<string>(dao(pRoot->_pRight)->_data);
	//		   ret = new BinaryTreeNode<string>(pRoot->_data, l, r);
	//}
	//	break;
	//case'*':
	//{
	//		   //(l*r)' = l'*r + l*r'
	//		   ret = new BinaryTreeNode<string>("+");
	//		   BinaryTreeNode<string>* l1 = new BinaryTreeNode<string>(dao(pRoot->_pLeft)->_data);
	//		   BinaryTreeNode<string>* r1 = new BinaryTreeNode<string>(dao(pRoot->_pRight)->_data);
	//		   BinaryTreeNode<string>* l2 = new BinaryTreeNode<string>(pRoot->_pLeft->_data);
	//		   BinaryTreeNode<string>* r2 = new BinaryTreeNode<string>(pRoot->_pRight->_data);
	//		   ret->_pLeft = new BinaryTreeNode<string>("*", l1, r2);
	//		   ret->_pRight = new BinaryTreeNode<string>("*", l2, r1);
	//}
	//	break;

	case'/':
		//(l/r)' = (l'*r - l*r') / (r*r)
		ret = new BinaryTreeNode<string>("/");
		ret->_pLeft = new BinaryTreeNode<string>("-");
		ret->_pLeft->_pLeft = new BinaryTreeNode<string>("*", dao(pRoot->_pLeft), pRoot->_pRight);
		ret->_pLeft->_pRight = new BinaryTreeNode<string>("*", pRoot->_pLeft, dao(pRoot->_pRight));
		ret->_pRight = new BinaryTreeNode<string>("*", pRoot->_pRight, pRoot->_pRight);
		break;
	case'^':
		ret = new BinaryTreeNode<string>("*");
		//(l^r)' = (l^r) * (ln(l)*r)'
		//右子树中含未知数
		if (pRoot->_pRight->_data[0] == 'x')
		{
			ret->_pLeft = pRoot;
			ret->_pRight = dao(new BinaryTreeNode<string>("*", 
				new BinaryTreeNode<string>("E", NULL, pRoot->_pLeft), pRoot->_pRight));
		}
		//(l^c)' = (l'*c) * l^(c-1)
		else
		{
			ret->_pLeft = new BinaryTreeNode<string>("*", dao(pRoot->_pLeft), pRoot->_pRight);
			ret->_pRight = new BinaryTreeNode<string>("^", pRoot->_pLeft, 
				new BinaryTreeNode<string>("-", pRoot->_pRight, new BinaryTreeNode<string>("1")));
		}
		break;
	case'A':
		//cos --> cos(r)' = 0-sin(r)*r'
		ret = new BinaryTreeNode<string>("-", new BinaryTreeNode<string>("0"),NULL);
		ret->_pRight = new BinaryTreeNode<string>("*",
			new BinaryTreeNode<string>("B",NULL,pRoot->_pRight),dao(pRoot->_pRight));
		break;
	case'B':
		//sin --> sin(r)' = cos(r)*r'
		ret = new BinaryTreeNode<string>("*");
		ret->_pLeft = new BinaryTreeNode<string>("A", NULL, pRoot->_pRight);
		ret->_pRight = dao(pRoot->_pRight);
		break;
	case'C':
		//tan --> tan(r)' = r' / (cos(r) * cos(r))
		ret = new BinaryTreeNode<string>("/", dao(pRoot->_pRight),NULL);
		ret->_pRight = new BinaryTreeNode<string>("*", new BinaryTreeNode<string>("A", NULL, pRoot->_pRight), 
			new BinaryTreeNode<string>("A", NULL, pRoot->_pRight));
		break;
	case'D':
		//cot --> cot(r)' = 0-r'/(sin(r)*sin(r))
		ret = new BinaryTreeNode<string>("-", new BinaryTreeNode<string>(0), NULL);
		ret->_pRight = new BinaryTreeNode<string>("/", dao(pRoot->_pRight), NULL);
		ret->_pRight->_pRight = new BinaryTreeNode<string>("*", new BinaryTreeNode<string>("B", NULL, pRoot->_pRight),
			new BinaryTreeNode<string>("B", NULL, pRoot->_pRight));
		break;
	case'E':
		//ln --> ln(r)' = r'/r
		ret = new BinaryTreeNode<string>("/", dao(pRoot->_pRight), pRoot->_pRight);
		break;
	case'F':
		//log --> log(l,r) = r'/(r*ln(l))
		ret = new BinaryTreeNode<string>("/", dao(pRoot->_pRight), NULL);
		ret->_pRight = new BinaryTreeNode<string>("*", pRoot->_pRight, new BinaryTreeNode<string>("E", NULL, pRoot->_pLeft));
		break;
	case'x':
		//x' = 1
		ret = new BinaryTreeNode<string>("1");
		break;
	default:
		//常量的导数为0
		ret = new BinaryTreeNode<string>("0");
		break;
	}
	return ret;
}

static BinaryTreeNode<string>* XdivY(BinaryTreeNode<string>* X, BinaryTreeNode<string>* Y)
{
	char ch[16] = { 0 };
	sprintf(ch, "%8f", (atof(X->_data.c_str()) / atof(Y->_data.c_str())));
	return (new BinaryTreeNode<string>(ch));
}

static BinaryTreeNode<string>* XsubY(BinaryTreeNode<string>* X, BinaryTreeNode<string>* Y)
{
	char ch[16] = { 0 };
	sprintf(ch, "%8f", (atof(X->_data.c_str()) - atof(Y->_data.c_str())));
	return (new BinaryTreeNode<string>(ch));
	return X;
}

BinaryTreeNode<string>* calculate(string& exp)
{
	Preprocessor(exp);
    size_t i=0;
    for(;i<exp.length();++i)
        if(IS_INVALID_CH(exp[i]))
            throw "表达式不合法";
	queue<string> postFix;
	InfixToPostfix(exp, postFix);
	BinaryTreeNode<string>* t = CreateBinTreeExp(postFix);//构建表达式树
	BinaryTreeNode<string>* dt = dao(t); //构建导函数树
	
	BinaryTreeNode<string>* x0 = new BinaryTreeNode<string>("1");
	BinaryTreeNode<string>* x1 = new BinaryTreeNode<string>("2");
	int j = 0;
	while (abs(atof(XsubY(x0,x1)->_data.c_str())) > 0.00000009)
	{
		x0->_data = x1->_data;
		x1->_data = XsubY(x0 , XdivY(compute(t, x0) , compute(dt,x0)))->_data;
		if (++j > 1000)
			break;
	}
	return x1;
}

string ExpResult(string& exp)
{
	string ret;
	try{
        cout<<"in try front"<<endl;
		ret = calculate(exp)->_data;
        cout<<"in try last"<<endl;
    cout<<ret<<endl;    
        return ret;
	}
	catch (const char* str)
	{
        ret = string(str);
	}
    cout<<ret<<endl;    
	return ret;
}

