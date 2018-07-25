#include"expSolution.h"

/***************以下为测试代码*********************/

void TestPreprocessor()
{
	string str("4-(-2.4xcos2)");
	Preprocessor(str);
	cout << str << endl;
}

void TestRepFun()
{
	string str("cos sin tan cot ln log");
	RepFun(str);
	cout << str << endl;
}

void TestInfixToPostfix()
{
	//string str("1.12-5x+1-sinx");
	//string str("(1+(-x))");
	string str("2x^4 - 3x + sin(x*x) - ln(x^3)");
	Preprocessor(str);

	queue<string> postFix;
	InfixToPostfix(str, postFix);
}

void TestDelSpace()
{
	string str("2x^4 - 3x + sin(x*x) - ln(x^3) + log(2,x) - 2");
	DelSapce(str);
}
//
//void TestCreateBinTreeExp()
//{
//	//string str("(1+2) * (x * (3+4))");
//	/*string str("x^2-4");
//	if (!Preprocessor(str))
//		cout << "!Pre" << endl;
//	queue<string> postFix;
//	if (!InfixToPostfix(str, postFix))
//		cout << "！IntoFix" << endl;
//	BinaryTreeNode<string>* t = CreateBinTreeExp(postFix);
//}*/

void Testcalculatet()
{
	//string str("x^2-9");
	//string str("sinx");
	//string str("2^x-4");
	//string str("sinx-1");
	//string str("2x^4 - 3x + sin(x*x) - ln(x^3)  - 2");//NO
	//string str("ln(x^3)");

	string str("2x ^ 4 - 3x + sin(x*x) - 2");
	calculate(str);
}

int main()
{
	//TODO 不能正确解析log(2,x),在表达式预处理处解决
	//TestPreprocessor();
	//TestRepFun();
	//TestDelSpace();
	//TestInfixToPostfix();
	//TestCreateBinTreeExp();
	//Testcalculatet();
	//printf("%d", 10E-9);
	//string str("2x ^ 4 - 3x + sin(x*x) - 2");

	//string str("2x^4-3x+sin(x*x)-2");
    string str("sinx");
	string s = ExpResult(str);
	cout << s << endl;
	return 0;
}
