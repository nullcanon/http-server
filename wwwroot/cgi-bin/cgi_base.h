#pragma once

// CGI程序
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>

static int GetQueryString(char buf[])
{   
    // 1.从环境变量获取方法
    char* method = getenv("REQUEST_METHOD");
    if(method == NULL){
        fprintf(stderr,"err_getenv\n");
        exit(1);
    }

    // 判断是get还是post
    // get -read QUERY_STRING
    // post -read CONTENT_LENGTH
    //    printf("befor strcasecmp method and GET\n");// DEBUG
    if(strcasecmp(method,"GET") == 0){
        char* query_string = getenv("QUERY_STRING");
    //       printf("buf size %ld",strlen(buf));// DEBUG
        if(query_string == NULL){
            fprintf(stderr,"quer_string is null\n");
            return -1;
        }
        strcpy(buf,query_string);
    }else{
        char* content_length_str = getenv("CONTENT_LENGTH");
        if(content_length_str == NULL){
            fprintf(stderr,"content_lengtn env is null\n");
            return -1;
        }
        int content_length = atoi(content_length_str);

        // DEBUG
        // printf("content_length=%d",content_length);
        int i=0;
        for(;i < content_length; ++i){
            // 从管道读
            read(0,&buf[i],1);
        }
        buf[i] = '\0';
    }
    return 0;
}

