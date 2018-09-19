#ifndef __TEST__
#define __TSET__

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/string.h>
#include <linux/fs.h>
#include <asm/uaccess.h>
#include <linux/fcntl.h>
#include <linux/types.h>
#include <linux/signal.h>
#include <asm/siginfo.h>
#include <linux/rcupdate.h>
#include <linux/sched.h>
#include <linux/semaphore.h>
#include <linux/mutex.h>
#include <linux/slab.h>

#include <linux/skbuff.h>
#include <linux/netdevice.h>
#include <linux/netfilter.h>
#include <linux/netfilter_ipv4.h>
#include <linux/in.h>
#include <linux/tcp.h>
#include <linux/udp.h>
#include <linux/ip.h>

#include <linux/inet.h>

#define DEVICE_NAME "test"
#define FILTERLIST 100
#define WR_BUSY -100
#define RD_BUSY -101
#define NF_BUSY -102
#define TIMELIMIT 5

typedef struct msg{
        char net[30];
        char subnet[30];
        char domain[30];
}filter_msg;

typedef struct message{
        int type;
        char message[512];
}msg_t;

typedef struct conn_src_t
{
        unsigned long src;
        struct timeval time;
        struct timeval prev;
        unsigned int timeout;
        int is_end;
        unsigned long total;
	char domain[30];
}conn_src;

typedef struct conn_dst_t{
        unsigned long dst;
        struct conn_src_t* conn_src[100];
}conn_dst;

char *cus_strtok(char *str, char *delim);
char *cus_strstr(const char *str1, const char *str2);
int cus_strcmp(const char * first, const char *two);
unsigned long inet_aton(const char* str);
int conn_execute(struct conn_dst_t* , int* , int* , unsigned long , unsigned long , char[]);
int check_packet(unsigned long , unsigned long* , struct msg* , int);
void check_timeout(struct conn_dst_t* , int , int , unsigned long , unsigned long , char[]);
void add(struct conn_dst_t* , int , int , unsigned long , unsigned long , char[]);
char* cus_strcpy(char* src , const char* dst);
int inet_ntoa(char buffer[] , unsigned long in);

static int device_open(struct inode *, struct file *);
static int device_release(struct inode *, struct file *);
static ssize_t device_read(struct file *, char *, size_t, loff_t *);
static ssize_t device_write(struct file *, const char *, size_t, loff_t *);
int phassinglist(char* , char*); 
//netfilter proccessing
static unsigned int main_hook(unsigned int hooknum , struct sk_buff *skb ,
                        const struct net_device *in , const struct net_device *out,
                        int (*okfn)(struct sk_buff*));
//netfilter proceessing end

#endif
