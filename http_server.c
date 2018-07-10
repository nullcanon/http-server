#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<sys/stat.h>
#include<sys/sendfile.h>
#include<sys/wait.h>
#include<fcntl.h>
#include<pthread.h>

#include"http_server.h"

typedef struct sockaddr sockaddr;
typedef struct sockaddr_in sockaddr_in;

// 从socket中读取一行数据放到buf中
// 读取失败返回-1
// 成功返回字符个数
int ReadLine(int sock,char buf[],ssize_t size)
{
    // 一次读一个字符，读到\n \r \r\n返回
    char c = '\0';
    ssize_t i = 0;
    while(i < size-1 && c != '\n'){
        ssize_t read_size = recv(sock,&c,1,0);
        if(read_size <= 0){
            return -1;
        }
        if(c == '\r'){
            // 判断下一个字符是否为\n
            recv(sock,&c,1,MSG_PEEK);// MSG_PEEK只读不取出
            if(c == '\n'){
                // \r\n
                recv(sock,&c,1,0);// 读取\n
            }else{
                // \r
                c = '\n';
            }
        }
        buf[i++] = c; 
    }
    buf[i] = '\0';
    return i;
}

int Split(char input[],const char* split_char,char* output[],int output_size)
{
    int i = 0;
    char* tmp;
    char* pch;
    pch = strtok_r(input,split_char,&tmp);
    while(pch != NULL){
        if(i >= output_size){
            return i;
        }
        output[i++] = pch;
        pch = strtok_r(NULL,split_char,&tmp);
    }
    return i;
}

int ParseFirstLine(char first_line[],char** url,char** method)
{
    // 按照空格进行切分
    char* tok[10];
    // 把切分的元素存到tok中，返回值为tok中元素个数
    int tok_size = Split(first_line," ",tok,10);
    if(tok_size != 3){
        printf("Split falid tok_size=%d\n",tok_size);
        return -1;
    }
    *method = tok[0];
    *url = tok[1];
    return 0;
}

int ParseQueryString(char* url,char** url_path,char** query_string)
{
    *url_path = url;
    char* p = url;
    for(;*p != '\0'; ++p){
        if(*p == '?'){
            *p = '\0';
            *query_string = p+1;
            return 0;
        }
    }
    *query_string = NULL;
    return 0;
}

int ParseHead(int sock,int* content_length)
{
    char buf[SIZE] = {0};
    // 1.循环从sock读取一行
    // 2.判断当前行是否为Content-Length
    // 3.如果是把value读取出来
    // 3.不是直接丢弃
    // 5.读到空行，循环结束
    while(1){
        ssize_t read_size = ReadLine(sock,buf,sizeof(buf));
        // 读取失败
        if(read_size <=  0){
            return -1;
        }
        // 读完
        if(strcmp(buf,"\n") == 0){
            return 0;
        }
        // 判断当前行是否为Content-Length
        const char* content_length_str = "Content-Length:";
        if(content_length != NULL 
            && strncmp(buf,content_length_str,strlen(content_length_str)) == 0){
            *content_length = atoi(buf+strlen(content_length_str));
        }
    } 
    return 0;
}

void Handler404(int sock)
{
    const char* first_line = "HTTP/1.1 404 Not Found\n";
    const char* blank_line = "\n";
    const char* type_line = "Content-Type: text/html; charset=UTF-8\n";
    const char* html = "<head><meta charset=\"utf-8\"></head>"
       " <h1>页面没了嘤嘤嘤</h1>";
    send(sock,first_line,strlen(first_line),0);
    send(sock,type_line,strlen(type_line),0);
    send(sock,blank_line,strlen(blank_line),0);
    send(sock,html,strlen(html),0);
}

int IsDir(char* path)
{
    struct stat st;
    int ret = stat(path,&st);
    if(ret < 0){
        return 0;
    }
    if(S_ISDIR(st.st_mode)){
        return 1;
    }
    return 0;
}

void HandlerFilePath(const char* url_path,char file_path[])
{
    sprintf(file_path,"./wwwroot%s",url_path);

    // 如果url_path是目录，给其追加index.html
    if(file_path[strlen(file_path)-1] == '/'){
        strcat(file_path,"index.html");
    }
    // url_path是/image这样的非/结尾的目录
    if(IsDir(file_path)){
        strcat(file_path,"/index.html");
    }
}

ssize_t GetFileSize(const char* file_path)
{
    struct stat st;
    int ret = stat(file_path,&st);
    if(ret < 0){
        return 0;
    }
    return st.st_size;
}

int WriteStaticFile(int sock,const char* file_path)
{
    int fd = open(file_path,O_RDONLY);
    if(fd < 0){
        perror("open");
        return 404;
    }

    // 把构造出的HTTP相应写入socket中
    // 1.写入首行
    const char* first_line = "HTTP/1.1 200 OK\n";
    send(sock,first_line,strlen(first_line),0);
    // 2.写入header
    // const char* type_line = "Content-Type: text/html; charset=UTF-8\n";
    // const char* type_line = "Content-Type: image/jpg; charset=UTF-8\n";
    // send(sock,type_line,strlen(type_line),0);
    // 3.写入空行
    const char* blank_line = "\n";
    send(sock,blank_line,strlen(blank_line),0);
    // 4.写入body(文件内容)
    /*
    ssize_t i = 0;
    for(; i<file_size;++i){
        char c;
        read(fd,&c,1);
        send(sock,&c,1,0);
    }
    */
    sendfile(sock,fd,NULL,GetFileSize(file_path));
    close(fd);
    return 200;
}

int HandlerStaticFile(int sock,Request* req)
{
    // 根据url_path获取文件在服务器上的真实路径
    char file_path[SIZE] = {0};
    HandlerFilePath(req->url_path,file_path);
    // 读取文件，把文件内容写到socket之中
    int err_code = WriteStaticFile(sock,file_path);
    return err_code;
}

int  HandlerCGIFather(int sock,int father_read,int father_write,int child_pid,Request* req)
{
    // 把POST请求的body写入管道
    if(strcasecmp(req->method,"POST") == 0){
        int i = 0;
        char c = '\0';
        for(; i<req->content_length; ++i){
            read(sock,&c,1);
            write(father_write,&c,1);
        }
    }
    // 构造HTTP响应
    const char* first_line = "HTTP/1.1 200 OK\n";
    send(sock,first_line,strlen(first_line),0);
    const char* type_line = "Content-Type: text/html; charset=UTF-8\n";
    send(sock,type_line,strlen(type_line),0);
    const char* blank_line = "\n";
    send(sock,blank_line,strlen(blank_line),0);
    // 循环从管道中读取数据写入socket
    char c = '\0';
    while(read(father_read,&c,1) > 0){
        send(sock,&c,1,0);
    }
    // 回收子进程的资源
    waitpid(child_pid,NULL,0);
    return 200;
}

int HandlerCGIChild(int child_read,int child_write,Request* req)
{
    // 设置环境变量
    char method_env[SIZE] = {0};
    sprintf(method_env,"REQUEST_METHOD=%s",req->method);
    putenv(method_env);
    /* GET设置QUERY_STRING; POST设置CONTENT_LENGTH*/
    if(strcasecmp(req->method,"GET") == 0){
        char query_string_env[SIZE] = {0};
        sprintf(query_string_env,"QUERY_STRING=%s",req->query_string);
    }else{
        char content_length_env[SIZE] = {0};
        sprintf(content_length_env,"CONTENT_LENGTH=%d",req->content_length);
    }
    
    // 把标准输入输出重定向到管道
    dup2(child_read,0);
    dup2(child_write,1);
    // 对子进程进行程序替换
    char file_path[SIZE] = {0};
    HandlerFilePath(req->url_path,file_path);
    execl(file_path,file_path,NULL);
    exit(1);
    return 200;
}
        
int HandlerCGI(int sock,Request* req)
{
    // 创建一对匿名管道
    int fd1[2],fd2[2];
    int ret = pipe(fd1);
    int err_code = 200;
    if(ret < 0){
        return 404;
    }
    ret = pipe(fd2);
    if(ret < 0){
        close(fd1[0]);
        close(fd1[1]);
        return 404;
    }
    int father_read = fd1[0];
    int child_write = fd1[1];
    int father_write = fd2[1];
    int child_read = fd2[0];
    // 创建子进程
    ret = fork();
    // 父子进程各自执行不同逻辑
    if(ret > 0){
        // father
        close(child_read);
        close(child_write);
        err_code = HandlerCGIFather(sock,father_read,father_write,ret,req);
    }else if(0 == ret){
        // child
        close(father_read);
        close(father_write);
        err_code = HandlerCGIChild(child_read,child_write,req);
    }else{
        perror("fork");
        close(fd1[0]);
        close(fd1[1]);
        close(fd2[0]);
        close(fd2[1]);
        return 404;
    }
    // 错误处理
    return err_code;
}

// 处理请求
void HandlerRequest(int64_t clt_sock)
{
    int err_code = 200;
    // 1.读取并解析请求（反序列化）
    Request req;
    memset(&req,0,sizeof(req));
    //  ① 从socket中读取首行
    if(ReadLine(clt_sock,req.first_line,sizeof(req.first_line)) < 0){
        // TODO 失败处理
        err_code = 404;
        goto END;
    }
    //  ② 解析首行，从首行中解析出url和method
    if(ParseFirstLine(req.first_line,&req.url,&req.method) < 0){
        // TODO 失败处理
        err_code = 404;
        goto END;
    }
    //  ③ 解析url,从url中解析出 url_path,query_string
    if(ParseQueryString(req.url,&req.url_path,&req.query_string)< 0){
        //TODO 失败处理
        err_code = 404;
        goto END;
    }
    //  ④ 处理Header,这里只读取Content-Length
    if(ParseHead(clt_sock,&req.content_length) < 0) {
        //TODO 失败处理
        err_code = 404;
        goto END;
    }
    printf("method:%s\n",req.method);
    printf("url_path:%s\n",req.url_path);
    printf("query_string%s\n",req.query_string);
    printf("content_length%d\n",req.content_length);
    // 2.生成页面
    // 3.把生成结果写回客户端
    if(strcmp(req.method,"GET") == 0 && req.query_string == NULL){
        err_code = HandlerStaticFile(clt_sock,&req);
    }else if(strcmp(req.method,"GET") == 0 && req.query_string != 0){
        err_code = HandlerCGI(clt_sock,&req);
    }else if(strcmp(req.method,"POST") == 0){
        err_code = HandlerCGI(clt_sock,&req);
    }else{
        // TODO 错误处理
        err_code = 404;
        goto END;
    }
    // 错误处理：返回404 HTTP响应
END:
    if(err_code != 200){
        Handler404(clt_sock);
    }
    close(clt_sock);
}

// 线程入口函数
void* ThreadEntry(void* arg)
{
    int64_t clt_sock = (int64_t)arg;
    HandlerRequest(clt_sock);
    return NULL;
}

void HttpServerStart(const char* ip,short port)
{
    int listen_sock = socket(AF_INET,SOCK_STREAM,0);
    if(listen_sock < 0){
        perror("socket");
        return;
    }

    // 重用TIME_WAIT 连接
    int opt = 1;
    setsockopt(listen_sock,SOL_SOCKET,SO_REUSEADDR,&opt,sizeof(opt));

    sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(ip);
    addr.sin_port = htons(port);
    int ret = bind(listen_sock,(sockaddr*)&addr,sizeof(addr));
    if(ret < 0){
        perror("bind");
        return;
    }
    ret = listen(listen_sock,5);
    if(ret < 0){
        perror("listen");
        return;
    }
    printf("ServerInit OK!\n");
    while(1){
        sockaddr_in clt_adr;
        socklen_t clt_adr_len = sizeof(clt_adr);
        int64_t clt_sock = accept(listen_sock,(sockaddr*)&clt_adr,&clt_adr_len);
        if(clt_sock < 0){
            perror("accept");
            continue;
        }

        //使用多线程来处理客户端
        pthread_t tid;
        pthread_create(&tid,NULL,ThreadEntry,(void*)clt_sock);
        pthread_detach(tid);
    }
}

int main(int argc,char* argv[])
{
    if(argc != 3){
        printf("Usage ./http_server [ip] [port]\n");
        exit(1);
    }
    HttpServerStart(argv[1],atoi(argv[2]));
    return 0;
}
