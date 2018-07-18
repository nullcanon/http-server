#include"cgi_base.h"

int main()
{
    char buf[1024*4] = {0};
    int ret = GetQueryString(buf);
//    printf("after GetQueryString fun\n");
    if(ret < 0){
        fprintf(stderr,"GetQueryString failed\n");
        exit(1);
    }

    int a,b;
   // printf("%s\n",buf);
    sscanf(buf, "a=%d&b=%d", &a, &b);
    printf("a=%d,b=%d\n",a,b);
    int sum = a + b;

    // 按照http格式返回，父进程已经把首行、header、空行
    // 写回了客户端，因此这里只返回body
    printf("<h1>sum=%d</h1>",sum);
    return 0;
}
