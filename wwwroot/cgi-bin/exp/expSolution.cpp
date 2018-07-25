
#include"expSolution.h"

/*************************���ʽԤ����*********************************/
// �滻�ַ����еĺ���
// ��cos->A sin->B tan->C cot->D ln->E log->F 
void RepFun(string& str)
{
	if (str == "")
		throw "���ʽΪ��";

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





//ȥ���ַ��������пո�' '
void DelSapce(string& str)
{
	for (string::iterator it = str.begin(); it != str.end(); ++it)
	{
		if (*it == ' ')
			str.erase(it);
	}
}

//Ԥ�����ַ���δ֪��Ϊ x
// 1. 5x --> 5*x  cosxsinx --> ax * bx
//    ���δ֪��ǰ����һ�������ַ���Ϊδ֪��ǰ�����*
//	  ���δ֪��������һ�������ַ���Ϊδ֪���������*
// 2.-x --> (0-x)
// 3. +5-x --> 5-x
//    �����һ���ַ�Ϊ��+����ɾ������

//isalnum  isalpha
//�������ĸ��1.��x 
		  
void Preprocessor(string& str)
{
	size_t cur = 0;
	if (str == "")
		throw "���ʽΪ��";
	RepFun(str);
	DelSapce(str);
	// +5-x --> 5-x
	if (str[0] == '+')
		str.erase(0, 1);

	// 5x --> 5*x  cosxsinx --> ax * bx  x^2x --> x^2 * x
	while (cur < str.length())
	{
		if (IS_INVALID_CH(str[cur]))
			throw "���ʽ����1";//���зǷ��ַ�
		if (IS_X(str[cur]))
		{
			if (IS_X(str[cur]))
			{
				//1.
				//�ж�xǰһ���ַ��Ƿ�Ϊ�����ַ��ͺ����ַ�
				//���ǰһ���ַ����������ַ�Ҳ���Ǻ����ַ�Ҳ�����������(^xû����)��return false
				//���ǰһ���ַ��������ַ�����xǰ�����*
				if (cur > 0)
				{
					char pre_ch = str[cur - 1];
					if (IS_NUM_CH(pre_ch))
						str.insert(cur, "*");
				}
				//2.
				//�ж�x��һ���ַ��Ƿ�Ϊ�����ַ���x
				//������Ǻ����ַ���x�������return false
				//�����һ���ַ��Ǻ����ַ�����x�������*
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
		//������� - 
		//1.ǰ�������ֻ���x�������д���
		//2.ǰ��û���ַ�����-ǰ�浽��һ�� ������ ���� ����λ�� Ϊֹ���0
		//3.ǰ���ǣ����ڣ��������0
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
					throw "���ʽ����2";*/
			}
			else
				str.insert(0, "0");
			
		}
		cur++;
	}
}

/****���õ��ȳ��㷨����׺���ʽת��Ϊ��׺���ʽ*****/


//���������ȼ�
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
//�����Ƿ�Ϊ�ϸ�������ַ���
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
		//����
		if (IS_NUM_CH(exp[i]))
		{
			// 123 , 1.23
			// ��һ��������ȡ�����������ַ���
			// ����ȡ����ǰ��ȡ����.���߶�ȡ�������֣�һֱ���¶�ȡ
			string tmp;
			size_t j = i;
			for (; j < exp.length() && (exp[j] == '.' || IS_NUM_CH(exp[j])); ++j)
				tmp.insert(tmp.end(), 1, exp[j]);
			
			if (!ParseFloat(tmp))
				throw "���ʽ����3";//���ֲ��ϸ�
			postFix.push(tmp);
			i = j-1;
		}
		//�����ַ� e x
		else if (exp[i] == 'e' || exp[i] == 'x')
			postFix.push(string(1,exp[i]));
		//�����ָ�� ��
		else if (exp[i] == ',')
		{
			//��ջ���в��ϵص������������ҷ������������ȥ��
			//ֱ��ջ������Ԫ��Ϊһ��������Ϊֹ�����һֱû�����������ţ�
			//��ôҪô�Ƿָ����Ŵ���λ�ã�Ҫô�����Ų�ƥ�䡣
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
				throw "���ʽ����4";//���Ų�ƥ��
		}
		//�����
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
		//������
		else if (exp[i] == '(')
			stack.push(string(1, '('));
		//������
		else if (exp[i] == ')')
		{
			//  ��ջ���в��ϵص������������ҷ�����������У�ֱ��ջ������Ԫ��Ϊ������Ϊֹ��
			//	�������Ŵ�ջ�Ķ��˵������������������������ȥ��
			//	�����ʱλ��ջ���˵ļǺű�ʾһ����������ô���䵯�����������������ȥ��
			//	������ҵ�һ��������֮ǰջ���Ѿ�����������Ԫ�أ���ô�ͱ�ʾ�ڱ��ʽ�д��ڲ�ƥ������š�
			while (!stack.empty() && stack.top()[0] != '(')
			{
				postFix.push(stack.top());
				stack.pop();
			}
			if (stack.empty())
				throw "���ʽ����5";//���Ų�ƥ��
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
		//�����ַ�
		else if (IS_FUN_CH(exp[i]))
			stack.push(string(1, exp[i]));
		else
			throw "���ʽ����6";
	}
	if (!stack.empty())
	{
		if (stack.top()[0] == '(' || stack.top()[0] == ')')
			throw "���ʽ����7";//���Ų�ƥ��
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
/************������������************************/
//  1.���ζ�ȡ���ʽ��
//  2. ����ǲ��������򽫸ò�����ѹ��ջ�У�
//  3. ����ǲ��������򵯳�ջ�е���������������һ�������Ĳ�������Ϊ�Һ��ӣ�
//     �ڶ��������Ĳ�������Ϊ���ӣ�Ȼ���ٽ��ò�����ѹ��ջ�С�
// ����������ջ��������ɺ�ջ��ֻʣ��Ψһһ��Ԫ�أ���Ϊ���ʽ������
// ����ö�����û����������Ϊ����
BinaryTreeNode<string>* CreateBinTreeExp(queue<string>& postFix)
{
	//TODO if (postFix.empty()) ���쳣
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
			if (s[0] == '(' || s[0] == ')')	throw "���ʽ����8";
			expBinTree = new BinaryTreeNode<string>(s);
			postFix.pop();
			expBinTree->_pRight = stack.top();
			stack.pop();
			if (IS_OP(s[0]) || s[0] == 'F')
			{
				expBinTree->_pLeft = stack.top();
				stack.pop();
				//TODO,�ж�����������������쳣
			}
			stack.push(expBinTree);
		}
	}
	expBinTree = stack.top();
	//TODO ���expBinTree������Ҷ�Ϊ�գ����쳣
	//TODO ���stack�����Ԫ�ش���1�����쳣
	return expBinTree;
}
//char* strs[] = { "cos", "sin", "tan", "cot", "ln", "log" };
//char* chs[] = { "A", "B", "C", "D", "E", "F" };
/****************�����ö�������ֵ******************/
static double op(char op, double left, double right)
{
	switch (op)
	{
	case'+': return left + right;
	case'-': return left - right;
	case'*': return left * right;
	case'/': //if (right == 0) return false;�쳣
		return left / right;
	case'^': return pow(left, right);
	case'A': return cos(right);
	case'B': return sin(right);
	case'C': return tan(right);
	case'D': return 1/tan(right);
	case'E': return log(right);
	case'F': return log(right) / log(left);
	default:
		//return false;�쳣
		throw "���ʽ����9";
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

/**********************���ö��������󵼺�����****************/


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
	//	// �ռ�����߹������캯���������ָ�븳ֵ
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
		//�������к�δ֪��
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
		//�����ĵ���Ϊ0
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
            throw "���ʽ���Ϸ�";
	queue<string> postFix;
	InfixToPostfix(exp, postFix);
	BinaryTreeNode<string>* t = CreateBinTreeExp(postFix);//�������ʽ��
	BinaryTreeNode<string>* dt = dao(t); //������������
	
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

