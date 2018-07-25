#include<cstdio>
#include<iostream>
#include<string>

#include<unistd.h>
using namespace std;

#include"expSolution.h"
#include"./../test/cgi_base.h"
#include"urlEncode.hpp"

void ResultHtml(string& reuslt)
{
    // 打开并读取result.html
    // 判断是否要替换成时间信息
    // 读到的同时写到标准输出上
    // 关闭文件
    int fd = open("./result.html");

}

int main()
{
    char buf[1024] = {0};
    int ret = GetQueryString(buf);
    if(ret < 0)
    {
        fprintf(stderr,"GetQueryString faild\n");
        return 1;
    }

    printf("|%s|",buf);
    
    std::string exp_url(buf+4,strlen(buf+4));   
    string exp = UrlDecode(exp_url);

    // std::cout<<"|"<<exp<<"|"<<std::endl;
    string result = ExpResult(exp);
    // std::cout<<"|"<<exp<<"|"<<std::endl;
    // std::cout<<"|"<<result<<"|"<<std::endl;
    ResultHtml(result);
    return 0;
}
