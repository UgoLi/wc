#include <netdb.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
int main (int argc, char* argv[]) {
    if (argc < 2) {
        printf ("用法：%s <主机域名>\n", argv[0]);
        return EXIT_FAILURE;
    }
    struct hostent* host = gethostbyname (argv[1]);
    if (! host) {
        perror ("gethostbyname");
        return EXIT_FAILURE;
    }
    if (host->h_addrtype != AF_INET) {
        printf ("目前处理不了非IPv4的地址！\n");
        return EXIT_FAILURE;
    }
    printf ("主机官方名：\n");
    printf ("\t%s\n", host->h_name);
    printf ("主机别名表：\n");
    char** ppc = host->h_aliases;
    while (*ppc)
        printf ("\t%s\n", *ppc++);
    printf ("主机地址表：\n");
    struct in_addr** ppa =
        (struct in_addr**)host->h_addr_list;
    while (*ppa)
        printf ("\t%s\n", inet_ntoa (**ppa++));
    return EXIT_SUCCESS;
}
