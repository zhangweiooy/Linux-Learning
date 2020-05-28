#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>

typedef struct s
{
    int argc;
    char **argv;
} MyCommand;
MyCommand historyCmd[2000];

char **Split(char *source); //拆分输入参数
void MyCd(int argc, char **argv);
void MyLs(int argc, char **argv);
void MyPwd(int argc, char **argv);
void MyRm(int argc, char **argv);
void MyMkdir(int argc, char **argv);
void MyMv(int argc, char **argv);
void MyCp(int argc, char **argv);
void MyHistory(int count);
void RemoveDirectory(char *path);
void CopyDirectory(char *srcpath, char *destpath);
void CopyFile(char *srcfile, char *destfile);
int main()
{
    char temp[200];
    int count = 0;
    memset(historyCmd, 0, sizeof(historyCmd));
    while (1)
    {
        printf("MyShell$$ ");
        MyCommand command;
        memset(&command, 0, sizeof(command));
        memset(temp, 0, sizeof(temp));
        //指令预处理
        fgets(temp, sizeof(temp), stdin);
        if (temp[strlen(temp) - 1] == '\n')
        {
            temp[strlen(temp) - 1] = '\0';
        }
        command.argv = Split(temp);
        for (int i = 0; command.argv[i] != NULL; i++)
        {
            command.argc = i + 1;
        }
        historyCmd[count] = command;
        count++;
        //指令执行
        if (strcmp(command.argv[0], "cd") == 0)
        {
            MyCd(command.argc, command.argv);
        }
        else if (strcmp(command.argv[0], "ls") == 0)
        {
            MyLs(command.argc, command.argv);
        }
        else if (strcmp(command.argv[0], "pwd") == 0)
        {
            MyPwd(command.argc, command.argv);
        }
        else if (strcmp(command.argv[0], "rm") == 0)
        {
            MyRm(command.argc, command.argv);
        }
        else if (strcmp(command.argv[0], "mkdir") == 0)
        {
            MyMkdir(command.argc, command.argv);
        }
        else if (strcmp(command.argv[0], "mv") == 0)
        {
            MyMv(command.argc, command.argv);
        }
        else if (strcmp(command.argv[0], "cp") == 0)
        {
            MyCp(command.argc, command.argv);
        }
        else if (strcmp(command.argv[0], "exit") == 0)
        {
            return 0;
        }
        else if (strcmp(command.argv[0], "history") == 0)
        {
            MyHistory(count);
        }
        else
        {
            printf("unsupported instructions\n");
        }
    }
    return 0;
}
char **Split(char *source)
{
    char **argv;
    int len = strlen(source);
    int count = 1, j = 0, n = 0;
    char temp[len + 1];
    memset(temp, 0, sizeof(temp));
    for (int i = 0; i < len; i++)
    {
        if (source[i] == ' ' && source[i + 1] != ' ')
        {
            count++;
        }
    }
    argv = (char **)malloc((count + 1) * sizeof(char *));
    count = 0;
    for (int i = 0; i < len; i++)
    {
        if (i == len - 1 && source[i] != ' ')
        {
            temp[n++] = source[i];
            j = strlen(temp) + 1;
            argv[count] = (char *)malloc(j * sizeof(char));
            strcpy(argv[count++], temp);
        }
        else if (source[i] == ' ')
        {
            j = strlen(temp);
            if (j != 0)
            {
                argv[count] = (char *)malloc((j + 1) * sizeof(char));
                strcpy(argv[count++], temp);
                n = 0;
                memset(temp, 0, sizeof(temp));
            }
        }
        else
        {
            temp[n++] = source[i];
        }
    }
    argv[count] = NULL;
    return argv;
}
void MyCd(int argc, char **argv)
{
    char path[100];
    memset(path, 0, sizeof(path));
    if (argv[1] == NULL)
    {
        printf("input error\n");
        return;
    }
    else
    {
        strcpy(path, argv[1]);
    }
    if (chdir(path) < 0)
    {
        perror("change directory error");
    }
    return;
}
void MyLs(int argc, char **argv)
{
    DIR *dir;
    char path[100];
    memset(path, 0, sizeof(path));
    if (argc < 2)
    {
        strcpy(path, ".");
    }
    else
    {
        strcpy(path, argv[1]);
    }
    if ((dir = opendir(path)) == NULL) //查找目录
    {
        perror("opendir error");
        return;
    }
    else
    {
        struct dirent *entry;
        while ((entry = readdir(dir)) != NULL)
        {
            printf("%s  ", entry->d_name);
        }
        printf("\n");
        closedir(dir);
    }
    return;
}
void MyPwd(int argc, char **argv)
{
    char path[100];
    memset(path, 0, sizeof(path));
    if (getcwd(path, 100) == NULL)
    {
        perror("get current directory error");
    }
    else
    {
        printf("%s\n", path);
    }
    return;
}
void MyRm(int argc, char **argv)
{
    char path[100];
    memset(path, 0, sizeof(path));

    if (argv[1] == NULL)
    {
        printf("input error\n");
    }
    else
    {
        strcpy(path, argv[1]);
    }
    if (access(path, 0) < 0)
    {
        perror("file no exist");
    }
    else
    {
        struct stat statbuf;
        stat(path, &statbuf);
        if (S_ISREG(statbuf.st_mode))
        {
            remove(path);
        }
        else if (S_ISDIR(statbuf.st_mode)) //若rm的为目录，则须判断其是否空
        {
            DIR *dir = opendir(path);
            struct dirent *entry;
            int flag = 0;
            while ((entry = readdir(dir)) != NULL)
            {
                if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0)
                {
                    flag = 1;
                }
            }
            if (flag)
            {
                printf("can't remove a directory\n");
                return;
            }
            else
            {
                rmdir(path);
            }
        }
    }
    return;
}
void MyMkdir(int argc, char **argv)
{
    char path[100];
    memset(path, 0, sizeof(path));

    if (argv[1] == NULL)
    {
        printf("input error\n");
    }
    else
    {
        strcpy(path, argv[1]);
    }
    if (access(path, 0) == 0)
    {
        perror("file already exist");
    }
    else
    {
        mkdir(path, 0777);
    }
    return;
}
void MyMv(int argc, char **argv)
{
    char srcpath[100], destpath[100];
    memset(srcpath, 0, sizeof(srcpath));
    memset(destpath, 0, sizeof(destpath));

    if (argv[1] == NULL || argv[2] == NULL)
    {
        printf("input error\n");
    }
    else
    {
        strcpy(srcpath, argv[1]);
        strcpy(destpath, argv[2]);
    }
    DIR *dir;
    struct stat statbuf; //存储文件属性
    if (access(srcpath, 0) < 0)
    {
        perror("file no exist");
        return;
    }
    stat(srcpath, &statbuf);
    if (S_ISREG(statbuf.st_mode)) //普通文件
    {
        CopyFile(srcpath, destpath);
        remove(srcpath);
    }
    else if (S_ISDIR(statbuf.st_mode)) //目录文件
    {
        if ((dir = opendir(destpath)) == NULL)
        {
            mkdir(destpath, statbuf.st_mode);
        }
        closedir(dir);
        CopyDirectory(srcpath, destpath);
        RemoveDirectory(srcpath);
    }
    else //其他文件
    {
        perror("unknown file type");
    }
    return;
}
void MyCp(int argc, char **argv)
{
    char srcpath[100], destpath[100];
    memset(srcpath, 0, sizeof(srcpath));
    memset(destpath, 0, sizeof(destpath));

    if (argv[1] == NULL || argv[2] == NULL)
    {
        printf("input error\n");
    }
    else
    {
        strcpy(srcpath, argv[1]);
        strcpy(destpath, argv[2]);
    }
    DIR *dir;
    struct stat statbuf; //存储文件属性
    if (access(srcpath, 0) < 0)
    {
        perror("file no exist");
        return;
    }
    stat(srcpath, &statbuf);
    if (S_ISREG(statbuf.st_mode)) //普通文件
    {
        CopyFile(srcpath, destpath);
    }
    else if (S_ISDIR(statbuf.st_mode)) //目录文件
    {
        if ((dir = opendir(destpath)) == NULL)
        {
            mkdir(destpath, statbuf.st_mode);
        }
        closedir(dir);
        CopyDirectory(srcpath, destpath);
    }
    else //其他文件
    {
        perror("unknown file type");
    }

    return;
}
void RemoveDirectory(char *path)
{
    DIR *dir;
    struct stat statbuf;
    char pathname[1000];
    memset(pathname, 0, sizeof(pathname));
    dir = opendir(path);
    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL)
    {
        if (entry->d_type == 4) //若为目录
        {
            if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0)
            { //若既不是当前目录也不是父目录
                memset(pathname, 0, sizeof(pathname));
                strcpy(pathname, path);

                strcat(pathname, "/");
                strcat(pathname, entry->d_name);
                RemoveDirectory(pathname); //递归删除
                rmdir(pathname);
            }
        }
        else //若为文件
        {
            memset(pathname, 0, sizeof(pathname));
            strcpy(pathname, path);
            strcat(pathname, "/");
            strcat(pathname, entry->d_name);
            remove(pathname);
        }
    }
    rmdir(path);
    return;
}
void CopyDirectory(char *srcpath, char *destpath)
{
    DIR *dir;
    struct stat statbuf;
    char source[1000], object[1000];
    strcpy(object, destpath);
    strcat(object, "/");
    if ((dir = opendir(srcpath)) == NULL)
    {
        printf("open dir error!\n");
        return;
    }
    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL)
    {
        if (entry->d_type == 4) //若为目录
        {
            if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0)
            { //若既不是当前目录也不是父目录,则创建新目录
                memset(source, 0, sizeof(source));
                strcpy(source, srcpath);
                strcat(source, "/");
                strcat(source, entry->d_name);
                strcat(object, entry->d_name);
                stat(source, &statbuf);
                mkdir(object, statbuf.st_mode);
                CopyDirectory(source, object); //递归调用，进入子目录继续复制

                strcpy(object, destpath);
                strcat(object, "/");
            }
        }
        else //若为文件
        {
            memset(source, 0, sizeof(source));
            strcpy(source, srcpath);
            strcat(source, "/");
            strcat(source, entry->d_name);
            strcat(object, entry->d_name);
            CopyFile(source, object);
            strcpy(object, destpath);
            strcat(object, "/");
        }
    }
    closedir(dir);
    return;
}
void CopyFile(char *srcfile, char *destfile)
{
    int source = open(srcfile, 0);
    struct stat statbuf;
    stat(srcfile, &statbuf);
    int object = creat(destfile, statbuf.st_mode); //创建一个文件，若该文件已存在，则会将其长度截为零
    //复制文件
    char buffer[2000];
    int temp;
    while ((temp = read(source, buffer, 2000)) > 0)
    {
        if (write(object, buffer, temp) != temp) //判断是否写入完全
        {
            perror("write error!\n");
            exit(0);
        }
    }
    close(source);
    close(object);
    return;
}
void MyHistory(int count)
{
    for (int i = 0; i < count; i++)
    {
        printf(" %d\t", i);
        for (int j = 0; historyCmd[i].argv[j] != NULL; j++)
        {
            printf(" %s", historyCmd[i].argv[j]);
        }
        printf("\n");
    }
    return;
}