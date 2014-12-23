#include <stdlib.h> 
#include <stdio.h> 
#include <errno.h> 
#include <string.h> 
#include <netdb.h> 
#include <sys/types.h> 
#include <netinet/in.h> 
#include <sys/socket.h> 

#define portnumber 3333

struct message
{
    char name[20];
    char passwd[20];
    char toname[20];
    char message[1024];

    int action;
};

void read_message(void *arg)
{
     int fd = *((int *)arg);
     
     int count;
     char buffer[1024];
      
     struct message msg;
     while(1)
     {
	 if((count = read(fd,&msg,sizeof(msg))) != 0)
         {
	     if(msg.action == 1)
	     {
	         printf("登录成功！\n");
	     }

	     if(msg.action == -1)
	     {
	         printf("%s 未登录！\n",msg.toname);    
	     }

             if(msg.action == 2)
	     {
	         printf("%s 悄悄对你说：%s\n",msg.name,msg.message);
	     }
         }
    }

}

int main(int argc, char *argv[]) 
{ 
	int sockfd; 

	char name[20];
	char cmd[20]; 
	struct sockaddr_in server_addr; 
	struct hostent *host; 
	int nbytes; 
        
	struct message msg;
    
	if(argc!=2) 
	{ 
		fprintf(stderr,"Usage:%s hostname \a\n",argv[0]); 
		exit(1); 
	} 

	if((host=gethostbyname(argv[1]))==NULL) 
	{ 
		fprintf(stderr,"Gethostname error\n"); 
		exit(1); 
	} 

 
	if((sockfd=socket(AF_INET,SOCK_STREAM,0))==-1) // AF_INET:Internet;SOCK_STREAM:TCP
	{ 
		fprintf(stderr,"Socket Error:%s\a\n",strerror(errno)); 
		exit(1); 
	} 


	bzero(&server_addr,sizeof(server_addr)); 
	server_addr.sin_family=AF_INET;          // IPV4
	server_addr.sin_port=htons(portnumber); 
	//server_addr.sin_addr=*((struct in_addr *)host->h_addr); 
	server_addr.sin_addr.s_addr=inet_addr(argv[1]); 
	
	if(connect(sockfd,(struct sockaddr *)(&server_addr),sizeof(struct sockaddr))==-1) 
	{ 
		fprintf(stderr,"Connect Error:%s\a\n",strerror(errno)); 
		exit(1); 
	} 
        pthread_t id;
	pthread_create(&id,NULL,(void *)read_message,(void *)&sockfd);

        while(1)
	{
	   printf("Please input cmd:\n");
	   scanf("%s",cmd);
           if(strcmp(cmd,"log") == 0)
	   {
	       msg.action = 1;
	       printf("请输入你的昵称：\n");
	       scanf("%s",msg.name);
	       strcpy(name,msg.name);
	       printf("请输入密码：\n");
	       scanf("%s",msg.passwd);    
	       write(sockfd,&msg,sizeof(msg));
	   }
           
	   if(strcmp(cmd,"chat") == 0)
	   {
	       printf("请输入你所发对象名：\n");
	       scanf("%s",msg.toname);
               printf("请输入所发内容：\n");
	       scanf("%s",msg.message);
	       strcpy(msg.name,name);
               
	       msg.action = 2;
	       write(sockfd,&msg,sizeof(msg));
	   }
	}

	close(sockfd); 
	exit(0); 
} 
