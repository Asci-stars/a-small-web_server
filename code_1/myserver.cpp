#include <iostream>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <stdlib.h>
#include <assert.h>
#include <sys/epoll.h>
#include "threadpool.h"

using namespace std;
const int port = 8888;
int setnonblocking(int fd)
{
	int old_option = fcntl(fd,F_GETFL);
	int new_version= old_option | O_NONBLOCK;
	fcntl(fd,F_SETFL,new_option);
	return old_option;
}
void addfd(int epfd,int fd,bool flag)
{
	epoll_event ev;
	ev.data.fd = fd;
	ev.events = EPOLLIN | EPOLLET | EPOLLRDHUP;
	if(flag)
	{
		ev.events = ev.events | EPOLLONESHOT;
	}
	epoll_ctl(epfd,EPOLL_CTL_ADD,fd,&ev);
	setnonblocking(fd);
}

int main()
{
	cout<<"进入"<<endl;
	threadpool<http_coon>* pool = NULL;
	cout<<"55"<<endl;
	pool = new threadpool<http_coon>;
	cout<<"lala"<<endl;
	http_coon* users = new http_coon[100];
	assert(users);
	struct sockaddr_in address;
	bzero(&address,sizeof(address));
	address.sin_family = AF_INET;
	address.sin_port = htons(port);
	address.sin_addr.s_addr = htons(INADDR_ANY);
	
	int listenfd = socket(AF_INET,SOCK_STREAM,0);
	assert(listened >= 0);
	
	int ret;
	ret = bind(listenfd,(struct sockaddr*)&address,sizeof(address));
	assert(ret != -1);
	
	ret = listen(listenfd,5);
	assert(ret >= 0);
	
	int epfd;
	epoll_event events[1000];
	epfd = epoll_create(5);
	assert(epfd != -1);
	addfd(epfd,listenfd,false);
	
	while(true)
	{
		int number = epoll_wait(epfd,events,1000,-1);
		if((number < 0) && (errno != EINTR))
		{
			cout<<"my epoll is failure"<<endl;
			break;
		}
		for(int i = 0;i < number;i++)
		{
			int sockfd = events[i].data.fd;
			if(sockfd == listened)	//有新用户连接
			{
				struct sockaddr_in client_address;
				socklen_t client_addresslength = sizeof(client_address);
				int client_fd = accept(listenfd,(struct sockaddr*)&client_address,client_addresslength);
				if(client_fd < 0)
				{
					cout<<"errno is "<<errno<<endl;
					continue;
				}
				//初始化客户连接
				cout<<epfd <<"   "<<client_fd<<endl;
				addfd(epfd,client_fd,true);
				users[client_fd].init(epfd,client_fd);
			}
			else if(events[i].events & (EPOLLRDHUP | EPOLLHUP | EPOLLERR))
			{
				/* 出现异常则关闭客户端连接 */
				users[sockfd].close_coon();
			}
			else if(events[i].events & EPOLLIN)		//可以读取
			{
				cout<<"jinru 读取"<<endl;
				if(users[sockfd].myread())
				{
					cout<<"kedu"<<endl;
					//读取成功则添加任务队列
					pool->addjob(users + sockfd);
				}
				else{
					users[sockfd].close_coon();
				}
			}
			else if(evenet[i].events & EPOLLOUT)		//可写入
			{
				if(!users[sockfd].mywrite())
				{
					users[sockfd].close_coon();
				}
			}
		}
	}
	close(epfd);
	close(listenfd);
	delete []users;
	delete pool;
	return 0;
}






















