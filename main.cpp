#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/epoll.h>

#define MAX 1024

int main()
{
    int lfd = socket(AF_INET, SOCK_STREAM, 0);
    
    struct sockaddr_in saddr;
    saddr.sin_family = AF_INET;
    saddr.sin_port = htons(8989);
    saddr.sin_addr.s_addr = INADDR_ANY;
    int ret = bind(lfd, (struct sockaddr*)&saddr, sizeof(saddr));

    ret = listen(lfd, 128);
    
    int epfd = epoll_create(1);

    struct epoll_event ev;
    ev.events = EPOLLIN;
    ev.data.fd = lfd;
    ret = epoll_ctl(epfd, EPOLL_CTL_ADD, lfd, &ev);

    struct epoll_event evs[MAX];
    int addrlen = sizeof(struct sockaddr_in);
    while (1)
    {
        int num = epoll_wait(epfd, evs, MAX, -1);
        for(int i = 0; i < num; i++)
        {
            int fd = evs[i].data.fd;
            if(fd == lfd)
            {
                struct sockaddr_in caddr;
                int cfd = accept(fd, (struct sockaddr*)&caddr, (socklen_t*)&addrlen);
                char ip[32];
                printf("client ip:%s,port:%d\n",
                        inet_ntop(AF_INET, &caddr.sin_addr.s_addr, ip, sizeof(ip)),
                        ntohs(caddr.sin_port));
                ev.events = EPOLLIN;
                ev.data.fd = cfd;
                ret = epoll_ctl(epfd, EPOLL_CTL_ADD, cfd, &ev);
            }
            else
            {
                char buff[1024];
                int len = recv(fd, buff, sizeof(buff), 0);
                if (len > 0)
                {
                    printf("client say %s\n", buff);
                    send(fd, buff, len, 0);
                }
                else if (len == 0)
                {
                    printf("断开连接\n");
                    ret = epoll_ctl(epfd, EPOLL_CTL_DEL, fd, NULL);
                    close(fd);
                }
            } 
        }
    }
    ret = epoll_ctl(epfd, EPOLL_CTL_DEL, lfd, NULL);
    close(lfd);
    return 0;
}