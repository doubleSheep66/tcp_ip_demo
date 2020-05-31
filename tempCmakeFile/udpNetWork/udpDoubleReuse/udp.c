#include <stdio.h>

#include <sys/types.h>          

#include <sys/socket.h>

#include <string.h>

#include <sys/socket.h>

#include <netinet/in.h>

#include <arpa/inet.h>

#include <unistd.h>

#include <stdlib.h>

 

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

	

	printf("you can recv and send!!!\n");

	//定义一个文件描述符集合

	fd_set fds;

	

	

	//存放地址信息

    struct sockaddr_in addr = {0};

    addr.sin_family = AF_INET;//使用IPv4协议

	int dest_port = 0;//目的端口号

	char dest_ip[32] = {0};//目的IP

	char msg[1024] = {0};

	int len = sizeof(addr);//地址信息大小

	

	//循环监视文件描述符集合 	

	while(1)

	{

		//清空文件描述符集合

		FD_ZERO(&fds);

		//把标准输入设备加入到集合中 

		FD_SET(0,&fds);

		//把网络通信文件描述符加入到集合中 

		FD_SET(udp_socket_fd, &fds);

		

		

		ret = select(udp_socket_fd+1,&fds,NULL,NULL,NULL);//阻塞等待，直到集合中有活跃的描述符

		if(ret < 0)//错误

		{

			perror("select fail:");

			close(udp_socket_fd);

			return -1;

		}else if(ret > 0) //有活跃的  ret为1

		{

			//判断是否是 标准输入设备活跃 假设是则发送数据 

			if(FD_ISSET(0, &fds))//标准输入的描述符是0

			{

				printf("ip port msg\n");

				scanf("%s %d %s", dest_ip, &dest_port, msg);//输入目的ip 与 端口号

				addr.sin_port = htons(dest_port);//设置接收方端口号

				addr.sin_addr.s_addr = inet_addr(dest_ip); //设置接收方IP 

				

				sendto(udp_socket_fd, msg, strlen(msg), 0, (struct sockaddr *)&addr,sizeof(addr)); 

				if(strcmp(msg, "exit") == 0 || strcmp(msg, "") == 0)

				{

					break;//退出循环

				}

				memset(dest_ip, 0, sizeof(dest_ip));

			}

			

			//判断是否是new_socket_fd活跃，(有消息收到)

			if(FD_ISSET(udp_socket_fd, &fds))

			{

				ret = recvfrom(udp_socket_fd, msg, sizeof(msg), 0, (struct sockaddr *)&addr, &len);

				if(ret == -1)

				{

					break;

				}

				printf("[%s:%d]",inet_ntoa(addr.sin_addr),ntohs(addr.sin_port));//打印消息发送方的ip与端口号

				printf("msg=%s\n",msg);

				if(strcmp(msg, "exit") == 0 || strcmp(msg, "") == 0)

				{

					//通知主线程。。。

					break;

				}

			}

			

			memset(msg, 0, sizeof(msg));//清空存留消息

			

		}

		

	}

	

	//4 关闭通信socket

	close(udp_socket_fd);

}

 
