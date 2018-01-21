/*************************************************************************
	> File Name: reg.c
	> Author:UgoLi 
	> Mail: 2653920896@qq.com
	> Created Time: 2017年10月10日 星期二 09时44分41秒
 ************************************************************************/

#include<stdio.h>
#include<regex.h>
#include<stdlib.h>
#include<string.h>

int main(int argc,char** argv){
    if(argc<2){
        printf("用法:%s<HTML文件>",argv[0]);
        return EXIT_FAILURE;
    }
    FILE* file=fopen(argv[1],"r");
    if(!file){
        perror("fopen");
        return EXIT_FAILURE;
    }
    if(fseek(file,0,SEEK_END)==-1){
        perror("fseek");
        return EXIT_FAILURE;
    }
    long size=ftell(file);
    if(size==-1){
        perror("ftell");
        return EXIT_FAILURE;
    }
    char* buf=(char*)malloc(size+1);
    if(!buf){
        perror("malloc");
        return EXIT_FAILURE;
    }
    if(fseek(file,0,SEEK_SET)==-1){
        perror("fseek");
        return EXIT_FAILURE;
    }
    if(fread(buf,1,size,file)!=size){
        perror("fread");
        return EXIT_FAILURE;
    }
    fclose(file);
    regex_t ex;
    int error=regcomp(&ex,"href=\"\\s*\\([^ \">]*\\)\\s*\"",0);
    if(error){
        char errIfo[1024];
        regerror(error,&ex,errIfo,sizeof(errIfo));
        printf("regcomp:%s\n",errIfo);
        return EXIT_FAILURE;
    }
    const char* html=buf;
    regmatch_t match[2];
    while(regexec(&ex,html,2,match,0)!=REG_NOMATCH){
        size_t len=match[1].rm_eo-match[1].rm_so;
        char* url=(char*)malloc(len+1);
        memcmp(url,html,len);
        url[len]='\0';
        printf("%s\n",url);
        free(url);
        free(buf);
        html +=len+match[0].rm_eo-match[1].rm_eo;
    }

    return EXIT_SUCCESS;
}
