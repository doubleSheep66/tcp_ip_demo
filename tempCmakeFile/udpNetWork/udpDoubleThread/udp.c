#include <stdio.h>

#include <sys/types.h>          

#include <sys/socket.h>

#include <string.h>

#include <sys/socket.h>

#include <netinet/in.h>

#include <arpa/inet.h>

#include <unistd.h>

#include <stdlib.h>

#include <pthread.h>

 

void * recv_msg(void *arg);//接收消息

 

int main(int argc,char *argv[])

{

	//判断命令行参数是否满足

	if(argc != 2)

	{

		printf("请传递一个端口号\n");

		return -1;

	}

 

	//将接收端口号并转换为int

	int port = atoi(argv[1]);

	if( port<1025 || port>65535 )//0~1024一般给系统使用，一共可以分配到65535

	{

		printf("端口号范围应为1025~65535");

		return -1;

	}

	

	// 1.创建udp通信socket  

	int udp_socket_fd = socket(AF_INET,SOCK_DGRAM,0);

	if(udp_socket_fd < 0 )

	{

		perror("creat socket fail\n");

		return -1;

	}

 

	//2.设置UDP的地址并绑定 

	struct sockaddr_in  local_addr = {0};

	local_addr.sin_family  = AF_INET; //使用IPv4协议

	local_addr.sin_port	= htons(port);   //网络通信都使用大端格式

	local_addr.sin_addr.s_addr = INADDR_ANY;//让系统检测本地网卡，自动绑定本地IP

 

	int ret = bind(udp_socket_fd,(struct sockaddr*)&local_addr,sizeof(local_addr));

	if(ret < 0)

	{

		perror("bind fail:");

		close(udp_socket_fd);

		return -1;

	}

	

	//开启接收线程

	pthread_t recv_thread;

	pthread_create(&recv_thread, NULL, recv_msg, (void*)&udp_socket_fd);

	

	

	//设置目的IP地址

    struct sockaddr_in dest_addr = {0};

    dest_addr.sin_family = AF_INET;//使用IPv4协议

    

	int dest_port = 0;//目的端口号

	char dest_ip[32] = {0};//目的IP

	char msg[1024] = {0};

	

	//循环发送消息 

	while(1)

	{

		printf("ip port msg\n");

 

		scanf("%s %d %s", dest_ip, &dest_port, msg);//输入目的ip 与 端口号

		dest_addr.sin_port = htons(dest_port);//设置接收方端口号

		dest_addr.sin_addr.s_addr = inet_addr(dest_ip); //设置接收方IP 

		

		sendto(udp_socket_fd, msg, strlen(msg), 0, (struct sockaddr *)&dest_addr,sizeof(dest_addr)); 

        if(strcmp(msg, "exit") == 0 || strcmp(msg, "") == 0)

		{

			pthread_cancel(recv_thread);//取消子线程

			break;//退出循环

		}

		memset(msg,0,sizeof(msg));//清空存留消息

		memset(dest_ip,0,sizeof(dest_ip));

	}

	

	//4 关闭通信socket

	close(udp_socket_fd);

}

 

//接收线程所要执行的函数 接收消息

void * recv_msg(void *arg)

{

	int ret = 0;

	int *socket_fd = (int *)arg;//通信的socket

	struct sockaddr_in  src_addr = {0};  //用来存放对方(信息的发送方)的IP地址信息

	int len = sizeof(src_addr);	//地址信息的大小

	char msg[1024] = {0};//消息缓冲区

	

	//循环接收客户发送过来的数据  

	while(1)

	{

		ret = recvfrom(*socket_fd, msg, sizeof(msg), 0, (struct sockaddr *)&src_addr, &len);

		if(ret == -1)

		{

			break;

		}

		printf("[%s:%d]",inet_ntoa(src_addr.sin_addr),ntohs(src_addr.sin_port));//打印消息发送方的ip与端口号

		printf("msg=%s\n",msg);

		if(strcmp(msg, "exit") == 0 || strcmp(msg, "") == 0)

		{

			//通知主线程。。。

			break;

		}

		memset(msg, 0, sizeof(msg));//清空存留消息

 

	}

	//关闭通信socket

	close(*socket_fd);

	return NULL;

}
