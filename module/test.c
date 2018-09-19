#include "test.h"

struct nf_hook_ops netfilter_ops;
struct sk_buff *sock_buff;

static int Device_Write = 0;
static int Device_Read = 0;
static int Netfilter_Execute = 0;

//static int signal_type = -1;
static msg_t imsi;

static int Major;
static int counter = 0;
static filter_msg filter_list[FILTERLIST];
unsigned long filtering_by_app[FILTERLIST];
char filter_all_message[100][50];
int filter_all_message_count;


static struct semaphore sema;
//alarm processing
//static pid_t pid;
//static int ret;
//static struct siginfo info;
//alarm processing end

static char connect_ip_list[100][50];
static conn_dst conn[100];

static struct file_operations fops = {
        .read = device_read,
        .write = device_write,
        .open = device_open,
        .release = device_release
};

int init_module(void)
{
	int i, j;
        sema_init(&sema , 1);
	filter_all_message_count = 0;	
	for(i=0;i<100;i++)
	{
		conn[i].dst = 0;
		for(j=0;j<100;j++)
		{
			conn[i].conn_src[j] = (conn_src*)kmalloc(sizeof(conn_src) , GFP_KERNEL);
			memset(conn[i].conn_src[j] , 0 , sizeof(conn_src));
		}
	}

        //memset(&info , 0 , sizeof(struct siginfo));
        //info.si_signo = SIGALRM;
        //info.si_code = SI_QUEUE;
        //info.si_int = 1234;

        netfilter_ops.hook = (nf_hookfn*)main_hook;
        netfilter_ops.pf = PF_INET;
        netfilter_ops.hooknum = NF_INET_PRE_ROUTING;
        netfilter_ops.priority = 1;

        nf_register_hook(&netfilter_ops);
        Major = register_chrdev(0 , DEVICE_NAME , &fops);

        printk("Major = %d\n",Major);
        printk("'mknod /dev/%s c %d 0'.\n", DEVICE_NAME, Major);

        return 0;
}

void cleanup_module(void)
{
	int i , j;
        printk("module clean\n");

	
	//for(i=0;i<100;i++)	printk("[%d] dst = %ld , src=%ld\n",i, conn[0].dst , conn[0].conn_src[i]->src);

	printk("conn[0].dst = %ld\n",conn[0].dst);	

        for(i=0;i<100;i++)
	{
        	for(j=0;j<100;j++)
        		kfree(conn[i].conn_src[j]);
	}
	
        printk("filter_list.net = %s\n",filter_list[0].net);
        printk("filter_list.subnet = %s\n",filter_list[0].subnet);
        printk("filter_list.domain = %s\n",filter_list[0].domain);


	nf_unregister_hook(&netfilter_ops);
        unregister_chrdev(Major, DEVICE_NAME);

}

static int device_open(struct inode* inosde , struct file *file)
{
        printk("device open\n");
        return 0;
}

static int device_release(struct inode* inode , struct file* file)
{
        printk("device close\n");
        return 0;
}

static ssize_t device_read(struct file *flip , char *buffer, size_t length, loff_t * offset)
{
	int return_value;

	/*if(Device_Write)	return WR_BUSY;
	if(Device_Read)		return RD_BUSY;
	if(Netfilter_Execute)	return NF_BUSY;*/

	Device_Read++;
        //char msg[512];
        printk("device read\n");

	printk("in read imsi.type = %d , imsi.message = %s\n",imsi.type , imsi.message);

		switch(imsi.type)
		{
			case 3:
			{
				int i=0 , j=0;
				char send_message[100][50];

				//if(inet_aton(imsi.message) == conn[0].dst)	printk("%lu %lu compare\n",inet_aton(imsi.message) , conn[0].dst);
				while(inet_aton(imsi.message) != conn[i++].dst)
				{
					//printk("%lu %lu compare\n",inet_aton(imsi.message) , conn[i-1].dst);
					continue;
				}
				
				while(conn[i-1].conn_src[j]->src != 0)
				{
					char buffer[20];
					inet_ntoa(buffer , conn[i-1].conn_src[j]->src);
					sprintf(send_message[j] , "%s(%s) access" , buffer , conn[i-1].conn_src[j]->domain);
					j++;
				}
				if(j<99)	cus_strcpy(send_message[j] , "end");

				copy_to_user(buffer , (char**)send_message , sizeof(char)*100*50);
				break;
			}
			case 2:
			{
				//int i=0;
				
				//while(filter_all_message[i] != NULL)	i++;
				//if(i<99)	cus_strcpy(filter_all_message[i] , "end");	
				copy_to_user(buffer , (char**)filter_all_message , sizeof(char)*100*50);	
				break;
			}
			case 4:
			{
				//char message[10] = "success";
				//copy_to_user(buffer , message , sizeof(char)*10); 
				break;
			}
			case 5:
			{
				int i=0;
				printk("conn[0].dst = %ld\n",conn[0].dst);
				while(conn[i].dst != 0)
				{
					inet_ntoa(connect_ip_list[i] , conn[i].dst);
					//cus_strcpy(connect_ip_list[i] , inet_ntoa(conn[i].dst));
					//sprintf(connect_ip_list[i] , "list %ld access", conn[i].dst);
					printk("list = %ld , connect_ip_list = %s\n",conn[i].dst , connect_ip_list[i]);
					i++;
				}
				cus_strcpy(connect_ip_list[i] , "end");
				copy_to_user(buffer , (char**)connect_ip_list , sizeof(char)*100*50);
			}
			case 6:
			{
				break;
			}
		}
		
		Device_Read--;
		return length;
}

static ssize_t device_write(struct file *filp, const char *buff, size_t len, loff_t * off)
{
	if(Device_Write)	return WR_BUSY;

	Device_Write++;
        printk("device write\n");
        copy_from_user(&imsi , (msg_t*)buff , sizeof(msg_t));

	printk("imsi.type = %d , imsi.message = %s\n",imsi.type , imsi.message);

        switch(imsi.type)
        {
                case 1:
                {
                        printk("data from user space : %s\n",imsi.message);
                        phassinglist(imsi.message , ";");
                        counter++;
                        break;
                }
                case 2:
                        break;
                case 3:
                        break;
		case 4:
		{
			int i=0;
			while(filtering_by_app[i] != 0)
			{
				if(filtering_by_app[i++] == inet_aton(imsi.message))	break;
			}
			filtering_by_app[i] = inet_aton(imsi.message);
			break;
		}
		case 5:
			break;
		case 6:
		{
			int is_ip = 1;
			int i=0;
			const char *domainHint[] = {"co" , "kr" , "net" , "ac" , "gg" , NULL};
			int domain_count=0;
			while((domainHint+domain_count) != NULL)
			{
				if(strstr(imsi.message , *(domainHint+domain_count)))
				{
					int j=0;
					int filter_list_cnt = counter;
					is_ip=0;
					printk("type 6 domain executed\n");
					while(j<filter_list_cnt)
					{
						if(!strcmp(filter_list[j].domain , imsi.message))
						{
							//swap
							int k = j;
							while(k<filter_list_cnt)
							{
								cus_strcpy(filter_list[k].net , filter_list[k+1].net);
								cus_strcpy(filter_list[k].subnet , filter_list[k+1].subnet);
								cus_strcpy(filter_list[k].domain , filter_list[k+1].domain);
								k++;
							}
						}
						j++;
					}
					break;
				}
				domain_count++;
			}
			if(is_ip)
			{
				printk("type 6 ip executed\n");
				int i=0;
				while(inet_aton(imsi.message) != filtering_by_app[i++])	continue;
				i--;
				while(filtering_by_app[i++] !=0)
				{
					filtering_by_app[i] = filtering_by_app[i+1];
				}	
			}
			break;
		}
		default:
		{
			printk("message type error : %d\n",imsi.type);
			printk("message : %s\n",imsi.message);
			break;
		}
				
        }
	Device_Write--;
        return len;
}

int phassinglist(char* message , char* token)
{
        char* result;

        result = cus_strtok(message , token);
        cus_strcpy(filter_list[counter].net , result);
        result = cus_strtok(NULL , token);
        cus_strcpy(filter_list[counter].subnet , result);
        result = cus_strtok(NULL , token);
        cus_strcpy(filter_list[counter].domain , result);
        printk("count = %d\n",counter);
        printk("filter_list.net = %s\n",filter_list[counter].net);
        printk("filter_list.subnet = %s\n",filter_list[counter].subnet);
        printk("filter_list.domain = %s\n",filter_list[counter].domain);
        //printk("[%d]%s;%s;%s\n",counter+1 , filter_list[counter].net , filter_list[counter].subnet , filter_list[counter].domain);

        return 0;
}

static unsigned int main_hook(unsigned int hooknum , struct sk_buff *skb , const struct net_device *in , const struct net_device *out, int (*okfn)(struct sk_buff*))
{

        int i=0;
        struct iphdr *iph = ip_hdr(skb);
        unsigned long saddr = 0 , daddr = 0;
        unsigned long snet = 0; //dnet =0;
	int dst_cnt=0 , src_cnt=0;
	char buffer[20];
	char domain[30];
	//hexDump("skb" , &skb , sizeof(struct sk_buff));

	//printk("\n%x\n",iph->saddr);	
	//printk("netfilter called\n");

	//down(&sema);
	Netfilter_Execute++;

        saddr = iph->saddr;
        daddr = iph->daddr;

	while(filtering_by_app[i] != 0)
		if(filtering_by_app[i++] == daddr)	return NF_DROP;
	
	inet_ntoa(buffer , saddr);
	//printk("%s access\n",buffer);		

	if((i = check_packet(saddr , &snet , filter_list , counter)) == -1)	return NF_ACCEPT;
	cus_strcpy(domain , filter_list[i].domain); 
	//printk("in main hook: daddr = %ld , saddr = %ld\n", daddr , snet);
	conn_execute(conn , &dst_cnt , &src_cnt , daddr , snet , domain);

	//printk("dst_cnt = %d , src_cnt = %d\n",dst_cnt , src_cnt);
	//printk("conn[dst_cnt]->dst = %ld\n",conn[dst_cnt].dst);
		
	Netfilter_Execute--;
	//up(&sema);*/

	return NF_DROP;
}

MODULE_LICENSE("GPL");

