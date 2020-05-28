#include <sys/socket.h>
#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <time.h>
#include <pthread.h>
#define PORT 1280
struct clientInfo
{
    int used;
    int clientSockFd;
    int port;
    struct in_addr ip;
};
struct clientInfo clientTable[20] = {0};
void *Do_server(void *var);
int main()
{
    int serverSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (serverSock < 0)
    {
        perror("serverSock error");
        exit(1);
    }

    struct sockaddr_in serverSockaddr;
    memset(&serverSockaddr, 0, sizeof(serverSockaddr));
    serverSockaddr.sin_family = AF_INET;
    serverSockaddr.sin_addr.s_addr = INADDR_ANY;
    serverSockaddr.sin_port = htons(PORT);
    int on = 1; //设置允许地址重用
    setsockopt(serverSock, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
    if (bind(serverSock, (struct sockaddr *)&serverSockaddr, sizeof(serverSockaddr)) < 0)
    {
        perror("bind error");
        exit(1);
    }
    if (listen(serverSock, 20) < 0)
    {
        perror("listen error");
        exit(1);
    }

    struct clientInfo temp;
    while (1)
    {
        struct sockaddr_in clientSockaddr;
        socklen_t clientSockaddr_size = sizeof(clientSockaddr);
        int clientSock = accept(serverSock, (struct sockaddr *)&clientSockaddr, &clientSockaddr_size);
        if (clientSock < 0)
        {
            perror("clientSock error");
            exit(1);
        }
        else //尝试这部分在子进程中
        {
            temp.clientSockFd = clientSock;
            temp.ip = clientSockaddr.sin_addr;
            temp.port = ntohs(clientSockaddr.sin_port);
            temp.used = 1;
        }
        int clientNum;
        for (clientNum = 0; clientNum < 20; clientNum++)
        {
            if (clientTable[clientNum].used == 0)
            {
                clientTable[clientNum] = temp;
                break;
            }
        }

        pthread_t pt;
        pthread_create(&pt, NULL, Do_server, (void *)&clientNum);
        pthread_detach(pt);
    }

    close(serverSock);
    return 0;
}
void *Do_server(void *var) //接收消息并转发
{
    int clientNum = *((int *)var);
    char buffer[1000];
    while (1)
    {
        memset(buffer, 0, sizeof(buffer));
        int size = recv(clientTable[clientNum].clientSockFd, buffer, sizeof(buffer), 0);
        if (size == 0)
        {
            break;
        }

        if (strlen(buffer) != 0) //空消息不发送
        {
            for (int i = 0; i < 20; i++)
            {
                if (clientTable[i].used == 1 && i != clientNum) //把消息转发给其他客户端
                {
                    send(clientTable[i].clientSockFd, buffer, sizeof(buffer), 0);
                    printf("Received and send: %s\n", buffer);
                }
            }
        }
    }
    close(clientTable[clientNum].clientSockFd);
    clientTable[clientNum].used = 0;
}