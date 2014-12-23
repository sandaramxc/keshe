#include <stdlib.h> 
#include <stdio.h> 
#include <errno.h> 
#include <string.h> 
#include <netdb.h> 
#include <sys/types.h> 
#include <netinet/in.h> 
#include <sys/socket.h> 
#include <unistd.h>
#include <pthread.h>

#define portnumber 3333

struct message
{
    char name[20];
    char passwd[20];
    char toname[20];
    char message[1024];

    int action;
};

struct online
{
    int fd;
    char name[20];

    struct online *next;    
};

struct online *head = NULL;

int find_fd(char *toname)
{
    if(head == NULL)
    {
        return -1;  
    }

    struct online *temp = head;

    while(temp != NULL)
    {
        if(strcmp(toname,temp->name) == 0)
	{
	    return temp->fd; 
	}

	temp = temp->next;
    }

    return -1;
}

int insert_online(struct online *p)
{
     if(head == NULL)
     {
	 p->next = head;

         head = p;
	 return 1;
     }
    
     p->next = head;
     head = p;

     return 1;
}

void read_message(void *arg)
{
     int fd = *((int *)arg);
     
     int count;
     
     int to_fd;

     struct message msg;
     
     struct online *p;

     while(1)
     {
	 if((count = read(fd,&msg,sizeof(msg))) != 0)
         {
	     if(msg.action == 1)
	     {
	         p = (struct online *)malloc(sizeof(struct online));
		 p->fd = fd;
		 strcpy(p->name,msg.name);

		 insert_online(p);

		 write(fd,&msg,sizeof(msg));
	     }

	     if(msg.action == 2)
	     {
	         to_fd = find_fd(msg.toname);

		 if(to_fd == -1)
		 {
		     msg.action = to_fd;
                     
		     write(fd,&msg,sizeof(msg));
		 }
		 else
		 {
		     write(to_fd,&msg,sizeof(msg));
		 }

	     }
         }


    }

}



int main(int argc, char *argv[]) 
{    
	int sockfd,new_fd; 
	struct sockaddr_in server_addr; 
	struct sockaddr_in client_addr; 
	int sin_size; 
	char msg[1024]; 
	

	/* 服务器端开始建立sockfd描述符 */ 
	if((sockfd=socket(AF_INET,SOCK_STREAM,0))==-1) // AF_INET:IPV4;SOCK_STREAM:TCP
	{ 
		fprintf(stderr,"Socket error:%s\n\a",strerror(errno)); 
		exit(1); 
	} 
        
	printf("socket!\n");
	/* 服务器端填充 sockaddr结构 */ 
	bzero(&server_addr,sizeof(struct sockaddr_in)); // 初始化,置0
	server_addr.sin_family=AF_INET;                 // Internet
	//server_addr.sin_addr.s_addr=htonl(INADDR_ANY); //INADDR_ANY 
	server_addr.sin_addr.s_addr=inet_addr("192.168.1.1");
	server_addr.sin_port=htons(portnumber); 
	
	if(bind(sockfd,(struct sockaddr *)(&server_addr),sizeof(struct sockaddr))==-1) 
	{ 
		fprintf(stderr,"Bind error:%s\n\a",strerror(errno)); 
		exit(1); 
	} 

	printf("bind!\n");

	if(listen(sockfd,5)==-1) 
	{ 
		fprintf(stderr,"Listen error:%s\n\a",strerror(errno)); 
		exit(1); 
	}

	printf("listen!\n");

	int count;
	pthread_t id;

	while(1) 
	{ 
		sin_size=sizeof(struct sockaddr_in); 
		if((new_fd=accept(sockfd,(struct sockaddr *)(&client_addr),&sin_size))==-1) 
		{ 
			fprintf(stderr,"Accept error:%s\n\a",strerror(errno)); 
			exit(1); 
		} 
		printf("accpet!\n");
		fprintf(stderr,"Server get connection from %s\n",inet_ntoa(client_addr.sin_addr));
		
		pthread_create(&id,NULL,(void *)read_message,(void *)&new_fd);
//		pthread_create(&id,NULL,(void *)write_message,(void *)&new_fd);
		
	} 

	/* 结束通讯 */ 
	close(sockfd); 
	exit(0); 
} 
