/*
** 时间：2018、04、20
** 实现一个简单的http重定向功能
**
*/

#include <sys/socket.h>
#include <sys/un.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>

//监听8081端口
#define PORT 8081   

//初始化tcp连接，返回一个套接字
int tcp_init(void)    
{  
    int server_socket = socket(AF_INET, SOCK_STREAM, 0);     // 创建套接字  
    if(server_socket == -1)  
    {  
        perror("server_socket");  
        return -1;  
    }  
      
    struct sockaddr_in server_addr;

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(PORT);   
    //setsockopt 表示允许端口reuse，可以在关闭socket后允许端口被监听，而不会出现端口被占用情况
    //SO_REUSEPORT要在bing之前使用
    int opt = 1;  
    setsockopt( server_socket, SOL_SOCKET,SO_REUSEADDR,   
                    (const void *)&opt, sizeof(opt) );
    int ret;
    ret = bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr));  
    if(ret == -1)  
    {  
        perror("bind");  
        return -1;  
    } 
    ret = listen(server_socket,10);           // 监听它，并设置允许最大的连接数为 10 个  
    if(ret == -1)  
    {  
        perror("listen");  
        close(server_socket);  
        return 0;  
    }  
    return server_socket;  
}   

int main(void)
{
    int ret;
    do
    {
       int server_socket = tcp_init();
       
       int client_socket = accept(server_socket, NULL, NULL);
       if(client_socket == -1)  
       {  
           perror("accept");  
           close(server_socket);  
           return 0;  
       }  
        char buf[1024];
       ret = read(client_socket, buf, 1024);
       if(ret == -1)
       {
    	   perror("read");
           close(server_socket);
           close(client_socket);
	   return 0;
       } 
    
       printf("%s",buf);
    
       char status[] = "HTTP/1.0 302 Redirect\r\n";  // http首部返回302状态码重定向
       char body[] = "Location: https://www.baidu.com\r\n";  //利用http头部locationd

       if(write(client_socket, status, sizeof(status)) == -1)
       {
	   perror("wirite");
	   exit(1);
       } 
       if(write(client_socket, body, sizeof(body)) == -1)
       {
    	   perror("wirite");
	   exit(1);
       } 
       
          close(client_socket);
          close(server_socket);   
       } while(ret != 0);
       //这里通过判断read返回的值，防止不断的read之前的连接，当有新的连接来之后才会继续执行后续

       return 0;
}
