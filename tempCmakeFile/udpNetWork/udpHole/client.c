#include"myhead.h"  

char wbuf[50];  

  

int main()  

{  

    int sockfd;  

    int size,on = 1;  

    struct sockaddr_in saddr;  

    int ret;  

  

    size = sizeof(struct sockaddr_in);  

    bzero(&saddr,size);  

  

    //设置地址信息，ip信息  

    saddr.sin_family = AF_INET;  

    saddr.sin_port = htons(5555);  //端口要和服务端绑定的一样

    saddr.sin_addr.s_addr = inet_addr("39.106.127.0");//xxxxx为你云服务器的公网ip，例如118.89.20.60 

  

    //创建udp 的套接字  

    sockfd= socket(AF_INET,SOCK_DGRAM,0);  

    if(sockfd<0)  

    {  

        perror("failed socket");  

        return -1;  

    }  

    //设置端口复用  

    setsockopt(sockfd,SOL_SOCKET,SO_REUSEADDR,&on,sizeof(on));  

  

    //循环发送信息给服务端  

    while(1)  

    {  

        puts("please enter data:");  

        scanf("%s",wbuf);  

        ret=sendto(sockfd,wbuf,50,0,(struct sockaddr*)&saddr,  

            sizeof(struct sockaddr));  

        if(ret<0)  

        {  

            perror("sendto failed");  

        }  

  

        bzero(wbuf,50);  

    }  

    close(sockfd);  

    return 0;  

}  

 
