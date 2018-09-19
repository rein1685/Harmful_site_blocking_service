#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <netdb.h>
#include <netinet/in.h>

#define BUF_SIZE 50
#define FILTER_NUMBER 100
#define PORT_NUM 8080

typedef struct msg
{
	int type;
	char message[BUF_SIZE];
}msg_t;

int dev_write , dev_read;
FILE* fd_write , *fd_read;
char message_from_module[FILTER_NUMBER][BUF_SIZE];
int i;

msg_t imsi;

void phassing(char* , char*);
void read_childproc(int sig);
void error_handling(char* message);
void list();

int main()
{
	int serv_sock , clnt_sock;
	struct sockaddr_in serv_adr , clnt_adr;

	pid_t pid;
	struct sigaction act;
	socklen_t adr_sz;
	int str_len , state;

	char buf[BUF_SIZE];

	int pos;

	dev_write = open("/dev/test" , O_WRONLY | O_NDELAY);
	dev_read = open("/dev/test" , O_RDONLY);
	//fd_write = fopen("/etc/logging" , "w");
	fd_read = fopen("/etc/list" , "r");

	if(dev_write < 0 || dev_read < 0 || fd_write < 0 || fd_read < 0)	printf("error file open\n");

	imsi.type = 1;
	list();

	printf("list end\n");

	act.sa_handler= read_childproc;
	sigemptyset(&act.sa_mask);
	act.sa_flags=0;
	state = sigaction(SIGCHLD , &act , 0);
	
	serv_sock = socket(PF_INET , SOCK_STREAM , 0);
	memset(&serv_adr , 0 , sizeof(serv_adr));
	serv_adr.sin_family = AF_INET;
	serv_adr.sin_addr.s_addr = htonl(INADDR_ANY);
	serv_adr.sin_port = htons(PORT_NUM);

	if(bind(serv_sock , (struct sockaddr*)&serv_adr , sizeof(serv_adr)) == -1)	error_handling("bind error");

	if(listen(serv_sock , 5)==-1)	error_handling("listen error");

	while(1)
	{
		//printf("Server Start...\n");

		adr_sz = sizeof(clnt_adr);
		clnt_sock = accept(serv_sock , (struct sockaddr*)&clnt_adr , &adr_sz);
		
		if(clnt_sock==-1)	continue;
		else	puts("new client connected...");

		pid = fork();
		if(pid==-1)
		{
			close(clnt_sock);
			continue;
		}

		if(pid==0)
		{
			printf("execute comunication!\n");
			close(serv_sock);
			while((str_len = read(clnt_sock , buf , BUF_SIZE)) != 0)
			{
				printf("execute read!\n");
				phassing(buf , ";");
				int i=0;
				int pos = strlen(imsi.message);
				imsi.message[strlen(imsi.message)-1] = '\0';	
				printf("imsi.type = %d , imsi.message = %s\n",imsi.type , imsi.message);

				switch(imsi.type)
				{
					case 1:
					{
						printf("1 execute , imsi.message = %s\n",imsi.message);
						
						struct hostent *he;
						struct in_addr **addr_list;
						struct in_addr myen;
						long *add;
						int i;
							
						char ip[20];

						if((he = gethostbyname(imsi.message))==NULL)
						{
							printf("gethostbyname error\n");
							break;
						}

						//addr_list = (struct in_addr**)he->h_addr_list;

						/*for(i=0; he->h_addr_list[i] != NULL;i++)
						{
							strcpy(ip , he->h_addr_list[i]);
							break;
						}*/

						while(*he->h_addr_list != NULL)
						{
							char domain[30];
							add = (long int*)*he->h_addr_list;
							myen.s_addr = *add;
							strcpy(ip , inet_ntoa(myen));
							he->h_addr_list++;
							strcpy(domain , imsi.message);
							sprintf(imsi.message , "%s;%s;%s" , ip , "255.255.255.255" , domain);
							write(dev_write , &imsi , sizeof(imsi));
						}
							
						printf("ip = %s , imsi.message = %s\n",ip , imsi.message); 
						break;
					}
        			                //list();
					
					case 2:			//packet
					{
						int return_value;
						fd_write = fopen("/etc/logging" , "w");
						printf("if type2, execute\n");	
                        			if(write(dev_write , &imsi , sizeof(imsi)) >0)	printf("write success\n");
						//sleep(5);
						if((return_value = read(dev_read , message_from_module , FILTER_NUMBER*BUF_SIZE)) > 0)
						{
							printf("read success\n");
							pos = strlen(message_from_module[i]);
							do
							{
								int j;
								pos = strlen(message_from_module[i]);	
								message_from_module[i][pos] = '\r';
								message_from_module[i][pos+1] = '\n';
								message_from_module[i][pos+2] = '\0';
								//write(fd_write , message_from_module[i] , BUF_SIZE);	//logging
								fputs(message_from_module[i] , fd_write);
								j = write(clnt_sock , message_from_module[i] , strlen(message_from_module[i])-1);	//android
								printf("message_from_module[%d] = %s",i , message_from_module[i]);
								printf("size = %d\n",j);
							}		
							while(strcmp(message_from_module[i++] , "end\r\n"));
							printf("write end\n");
						}
						else	printf("not received data %d\n",return_value);
						fclose(fd_write);
                      				break;
					}
					case 3:			//monitoring result
					{
						//pos = strlen(imsi.message);
						//imsi.message[pos-2] = '\0';
						//printf("imsi.type = %d , imsi.message = %s\n",imsi.type , imsi.message);
						write(dev_write , &imsi , sizeof(imsi));
                                                if(read(dev_read , message_from_module , FILTER_NUMBER*BUF_SIZE) > 0)
                                                {
							do
                                                        {
                                                                int j;
                                                                pos = strlen(message_from_module[i]);
                                                                message_from_module[i][pos] = '\r';
                                                                message_from_module[i][pos+1] = '\n';
                                                                message_from_module[i][pos+2] = '\0';
                                                                //write(fd_write , message_from_module[i] , BUF_SIZE);  //logging
                                                                j = write(clnt_sock , message_from_module[i] , strlen(message_from_module[i])-1);       //android
                                                                printf("message_from_module[%d] = %s",i , message_from_module[i]);
                                                                printf("size = %d\n",j);
                                                        }
                                                        while(strcmp(message_from_module[i++] , "end\r\n"));

                                                }
						else	printf("not received data\n");
                                                //logging(2);
                                                break;
					}
					case 4:			//filtering
					{
                        			write(dev_write , &imsi , sizeof(imsi));
						break;
						/*if(read(dev_read , message_from_module , FILTER_NUMBER*BUF_SIZE) > 0)
                                                {
                                                        while(strcmp(message_from_module[i] , "end"))
                                                        {
                                                                write(clnt_sock , message_from_module[i] , BUF_SIZE);      //android
								i++;
                                                        }
                                                }*/
						//write(clnt_sock , success , sizeof(success));
					}
					case 5:
					{
						printf("list start\n");
						write(dev_write , &imsi , sizeof(imsi));
						printf("imsi.type = %d , imsi.message = %s\n",imsi.type , imsi.message);
						if(read(dev_read , message_from_module , FILTER_NUMBER*BUF_SIZE) >0)
						{
							do
							{
								pos = strlen(message_from_module[i]);
								message_from_module[i][pos] = '\r';
                                                                message_from_module[i][pos+1] = '\n';
                                                                message_from_module[i][pos+2] = '\0';
								printf("list = %s\n",message_from_module[i]);
								write(clnt_sock , message_from_module[i] , strlen(message_from_module[i]) -1);
							}
							while(strcmp(message_from_module[i++] , "end\r\n"));
						}
						printf("list send end\n");
						break;
					}
					case 6:
					{
						write(dev_write , &imsi , sizeof(imsi));
						break;
                                                /*write(dev_write , &imsi , sizeof(imsi));
                                                if(read(dev_read , message_from_module , FILTER_NUMBER*BUF_SIZE) > 0)
                                                {
                                                        do
                                                        {
                                                                int j;
                                                                pos = strlen(message_from_module[i]);
                                                                message_from_module[i][pos] = '\r';
                                                                message_from_module[i][pos+1] = '\n';
                                                                message_from_module[i][pos+2] = '\0';
                                                                //write(fd_write , message_from_module[i] , BUF_SIZE);  //logging
                                                                j = write(clnt_sock , message_from_module[i] , strlen(message_from_module[i])-1);       //android
                                                                printf("message_from_module[%d] = %s",i , message_from_module[i]);
                                                                printf("size = %d\n",j);
                                                        }
                                                        while(strcmp(message_from_module[i++] , "end\r\n"));

                                                }
                                                else    printf("not received data\n");
                                                //logging(2);
                                                break;*/

					}	
					default:
					{
						printf("incorerect number\n");
						break;
					}
				}
			}

			close(clnt_sock);
			puts("client disconnected...");
			return 0;
		}
		else
			close(clnt_sock);
	}
	close(serv_sock);
	return 0;
}

void read_childproc(int sig)
{
	pid_t pid;
	int status;
	pid = waitpid(-1 , &status , WNOHANG);
	printf("removed proc id : %d\n",pid);
}

void error_handling(char* message)
{
	fputs(message , stderr);
	fputc('\n' , stderr);
	exit(1);
}

void list()
{
        int nread;
        int pos;

        if(fd_read < 0 || dev_write < 0)   printf("file open error!\n");


        while(fgets(imsi.message , sizeof(imsi.message) , fd_read) != NULL)
        {
                pos = strlen(imsi.message);
                imsi.message[pos-1] = '\0';

                printf("block : %s\n",imsi.message);
                write(dev_write , &imsi , sizeof(imsi));
        }
}

void phassing(char* message , char* token)
{
        char* result;

        result = strtok(message , token);
        //strcpy(imsi.type , result);
	imsi.type = atoi(result);
        result = strtok(NULL , token);
        strcpy(imsi.message , result);
        
        /*printk("count = %d\n",counter);
        printk("filter_list.net = %s\n",filter_list[counter].net);
        printk("filter_list.subnet = %s\n",filter_list[counter].subnet);
        printk("filter_list.domain = %s\n",filter_list[counter].domain);*/
}
