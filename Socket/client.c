#include <sys/socket.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <string.h>
#include <pthread.h>
#include <time.h>
#define PORT 1280
void *Do_pthread(void *var);
int main()
{
    int clientSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (clientSock < 0)
    {
        perror("clientSock error");
        exit(1);
    }

    struct sockaddr_in clientSockaddr;
    memset(&clientSockaddr, 0, sizeof(clientSockaddr));
    clientSockaddr.sin_family = AF_INET;
    clientSockaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    clientSockaddr.sin_port = htons(PORT);
    char clientId[1000];
    printf("Please input your ID:\n");
    fgets(clientId, sizeof(clientId), stdin);
    if (clientId[strlen(clientId) - 1] == '\n') //去掉clientId末尾的换行
    {
        clientId[strlen(clientId) - 1] = '\0';
    }
    if (connect(clientSock, (struct sockaddr *)&clientSockaddr, sizeof(clientSockaddr)) < 0)
    {
        perror("connect error");
        exit(1);
    }

    pthread_t pt;
    pthread_create(&pt, NULL, Do_pthread, (void *)&clientSock);
    pthread_detach(pt);
    char sendbuffer[1000];
    char tempbuffer[1000];
    while (fgets(sendbuffer, sizeof(sendbuffer), stdin) != NULL)
    {
        time_t rawtime;
        struct tm *t;
        time(&rawtime);
        t = localtime(&rawtime);
        sprintf(tempbuffer, "%s %d月%d日 %02d:%02d:%02d\n", clientId, t->tm_mon + 1, t->tm_mday, t->tm_hour, t->tm_min, t->tm_sec);
        strcat(tempbuffer, sendbuffer); //将ID、时间和消息内容写入tempbuffer
        send(clientSock, tempbuffer, sizeof(tempbuffer), 0);
        memset(sendbuffer, 0, sizeof(sendbuffer));
        memset(tempbuffer, 0, sizeof(tempbuffer));
    }

    close(clientSock);
    return 0;
}
void *Do_pthread(void *var) //从服务端接收消息并打印
{
    int *clientSock = (int *)var;
    char recvbuffer[1000];
    while (1)
    {
        recv(*clientSock, recvbuffer, sizeof(recvbuffer), 0);
        printf("%s", recvbuffer);
        memset(recvbuffer, 0, sizeof(recvbuffer));
    }
}