#include<cstdio>
#include<iostream>
#include<string>
#include <locale> 
#include<codecvt>

#include<unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include"expSolution.h"
#include"./../test/cgi_base.h"
#include"urlEncode.hpp"

using namespace std;


void ResultHtml(string& result)
{
    // 打开并读取result.html
    // 判断是否要替换成时间信息
    // 读到的同时写到标准输出上
    // 关闭文件
    int fd = open("./wwwroot/cgi-bin/exp/result.html",O_RDONLY);
    if(fd < 0)
    {
        fprintf(stderr,"result.html open faild\n");
        return;
    }
    char c = '\0';
    while(read(fd,&c,1) > 0)
    {
        if(c == '\1')
        {
            // 替换成计算结果
            std::cout<<result<<std::endl;
            // printf("%s",result.c_str());
        }
        else
        {
            write(1,&c,1);
        }
    }
    close(fd);
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

    
    std::string exp_url(buf+4,strlen(buf+4));   
    string exp = UrlDecode(exp_url);

    //     cout<<exp<<endl;
    string result = ExpResult(exp);
    // std::cout<<"|"<<exp<<"|"<<std::endl;
    //  std::cout<<"|"<<result<<"|"<<std::endl;
    ResultHtml(result);
    return 0;
}
