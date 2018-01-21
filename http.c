/*************************************************************************
	> File Name: http.c
	> Author:UgoLi 
	> Mail: 2653920896@qq.com
	> Created Time: 2017年10月09日 星期一 14时13分22秒
 ************************************************************************/

#include<sys/socket.h>
#include<sys/types.h>
#include<arpa/inet.h>
#include<unistd.h>
#include<ctype.h>
#include<netinet/in.h>
#include<stdio.h>
#include<strings.h>
#include<string.h>
#include<stdlib.h>

int main(int argc,char** argv){
    if(argc<3){
        printf("用法:%s<主机IP><主机域名>""[<资源路径>]\n",argv[0]);
        return EXIT_FAILURE;
    }
    char const* ip=argv[1];
    char const* domain=argv[2];
    char const* path=argc<4?"":argv[3];
    int sockfd=socket(PF_INET,SOCK_STREAM,0);
    if(sockfd==-1){
        perror("socket");
        return EXIT_FAILURE;
    }
    struct sockaddr_in addr;
    bzero(&addr,sizeof(addr));
    addr.sin_family=AF_INET;
    addr.sin_port=htons(80);
    if(!inet_aton(ip,&addr.sin_addr)){
        perror("inet_aton");
        return EXIT_FAILURE;
    }
    if(connect(sockfd,(struct sockaddr*)&addr,sizeof(addr)-1)){
        perror("connect");
        return EXIT_FAILURE;
    }
    char request[1024];
    sprintf(request,
            "GET /%s HTTP/1.0\r\n"
            "Host:%s\r\n"
            "Accept:*/*\r\n"
            "Connection:Close\r\n\r\n",
           path,domain);
    if(send(sockfd,request,strlen(request),0)==-1){
        perror("send");
        return EXIT_FAILURE;
    }
    for(;;){
        char respond[1024]={0};
        ssize_t rlen=recv(sockfd,respond,sizeof(respond)-1,0);
        if(rlen==-1){
            perror("recv");
            return EXIT_FAILURE;
        }
    if(!rlen)
        break;
    printf("%s\n",respond);
    }
    printf("\n");
    close(sockfd);
    return 0;
}


