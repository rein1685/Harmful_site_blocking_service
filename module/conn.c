#include "test.h"
#include <linux/time.h>

extern char filter_all_message[100][50];
extern int filter_all_message_count;

int check_packet(unsigned long saddr , unsigned long* snet , struct msg* filter_list , int filter_list_cnt)
{
	int i;
	for(i=0;i<filter_list_cnt;i++)
	{
		*snet = inet_aton(filter_list[i].subnet) & saddr;	
		//printk("*snet = %ld , filter list = %ld\n", *snet , inet_aton(filter_list[i].net));
		if(*snet == inet_aton(filter_list[i].net))
		{
			//printk("denied\n");
			return i;
		}
	}
	return -1;
}

int conn_execute(struct conn_dst_t* conn , int* dst_cnt , int* src_cnt , unsigned long daddr , unsigned long saddr , char domain[30])
{
	int scnt=0 , dcnt=0;

	//printk("daddr = %ld , saddr = %ld\n",daddr , saddr);
	
	do{
		if((conn+dcnt)->dst == 0)
		{
			add(conn , dcnt , scnt , daddr , saddr , domain);
			//printk("[%d , %d] %ld -> %ld add\n",dcnt , scnt , daddr , saddr);
			return 0;
		}

		if((conn+dcnt)->dst == daddr)	break;
		//printk("[%d , %d destination] %ld compare %ld Loop Back\n",dcnt , scnt , (conn+dcnt)->dst , daddr);
	}while((conn+dcnt++)->dst != 0);

	do{
		if((conn+dcnt)->conn_src[scnt]->src == 0)
		{
			add(conn , dcnt , scnt , daddr , saddr , domain);
			//printk("[%d , %d] %ld -> %ld add\n",dcnt , scnt , daddr , saddr);
			return 0;
		}
	
		if(((conn+dcnt)->conn_src[scnt]->src == saddr) && !(conn+dcnt)->conn_src[scnt]->is_end)
		{
			check_timeout(conn , dcnt , scnt , daddr , saddr , domain);
			return 0;
		}

		//printk("[%d , %d source] %ld compare %ld Loop Back\n",dcnt , scnt , (conn+dcnt)->conn_src[scnt]->src , saddr);
	}while((conn+dcnt)->conn_src[scnt++]->src != 0);

	*dst_cnt = dcnt;
	*src_cnt = scnt;
	return 0;

}

void check_timeout(struct conn_dst_t* conn , int dst_cnt , int src_cnt , unsigned long daddr , unsigned long saddr , char domain[30])
{
	int i =0;
	unsigned long timeout;

	if((conn+dst_cnt)->conn_src[src_cnt]->prev.tv_sec == 0)
	{
		(conn+dst_cnt)->conn_src[src_cnt]->prev.tv_sec = (conn+dst_cnt)->conn_src[src_cnt]->time.tv_sec;
		do_gettimeofday(&(conn+dst_cnt)->conn_src[src_cnt]->time);
		return;
	}

	(conn+dst_cnt)->conn_src[src_cnt]->timeout = (conn+dst_cnt)->conn_src[src_cnt]->time.tv_sec - (conn+dst_cnt)->conn_src[src_cnt]->prev.tv_sec;
	timeout = (conn+dst_cnt)->conn_src[src_cnt]->timeout;

	if((conn+dst_cnt)->conn_src[src_cnt]->timeout > TIMELIMIT)
	{
		//printk("Timeout! [%d , %d : %ld - %ld]\n",dst_cnt , src_cnt , (conn+dst_cnt)->conn_src[src_cnt]->time.tv_sec , (conn+dst_cnt)->conn_src[src_cnt]->prev.tv_sec);
		while((conn+dst_cnt)->conn_src[i++]->is_end != 0)	continue;
	
		add(conn , dst_cnt , i , daddr , saddr , domain);
	
		(conn+dst_cnt)->conn_src[src_cnt]->is_end = 1;
	}
	else
	{
		//printk("Same Connection! [%d , %d : %ld - %ld]\n",dst_cnt , src_cnt , (conn+dst_cnt)->conn_src[src_cnt]->time.tv_sec , (conn+dst_cnt)->conn_src[src_cnt]->prev.tv_sec);
		(conn+dst_cnt)->conn_src[src_cnt]->total += timeout;
		conn[dst_cnt].conn_src[src_cnt]->timeout = 0;
		(conn+dst_cnt)->conn_src[src_cnt]->prev.tv_sec = (conn+dst_cnt)->conn_src[src_cnt]->time.tv_sec;
		do_gettimeofday(&(conn+dst_cnt)->conn_src[src_cnt]->time);
	}
}

void add(struct conn_dst_t* conn , int dst_cnt , int src_cnt , unsigned long dst , unsigned long src , char domain[30])
{
	//(conn+dst_cnt) = (struct conn_dst_t*)kmalloc(sizeof(conn_dst) ,  GFP_KERNEL);
	char dst_address[18];
	char src_address[18];

	inet_ntoa(dst_address , dst);
	inet_ntoa(src_address , src);

	//printk("before %ld -> %ld access\n",dst , src);
	//printk("before %s -> %d access\n",dst_address , src_address);
	sprintf(filter_all_message[filter_all_message_count++] , "%s -> %s(%s)" , dst_address , src_address , domain);
	//printk("%s\n",filter_all_message[filter_all_message_count-1]);
	//sprintf(filter_all_message[filter_all_message_count++] , "%ld -> %ld access",dst , src);
	if(filter_all_message_count < 99)	cus_strcpy(filter_all_message[filter_all_message_count] , "end");

	(conn+dst_cnt)->conn_src[src_cnt]->prev.tv_sec = (conn+dst_cnt)->conn_src[src_cnt]->time.tv_sec;
	do_gettimeofday(&(conn+dst_cnt)->conn_src[src_cnt]->time);
	(conn+dst_cnt)->conn_src[src_cnt]->timeout = 0;
	(conn+dst_cnt)->conn_src[src_cnt]->total = 0;
	(conn+dst_cnt)->dst = dst;
	(conn+dst_cnt)->conn_src[src_cnt]->src = src;
	(conn+dst_cnt)->conn_src[src_cnt]->is_end = 0;
	cus_strcpy((conn+dst_cnt)->conn_src[src_cnt]->domain , domain);

	//printk("add called :dst_cnt = %d , src_cnt = %d , dst = %ld\n",dst_cnt , src_cnt , (conn+dst_cnt)->dst);
}

///EXPORT_SYMBOL(check_timeout);
//EXPORT_SYMPOL(conn_execute);
//EXPORT_SYMBOL(check_packet);
