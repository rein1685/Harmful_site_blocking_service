#include "test.h"

char* cus_strcpy(char *dst, const char *src)
{
    char *cp = dst;
    while(*cp++ = *src++);
    return dst;
}

char *cus_strtok(char *str, char *delim)
{
        static char *org = 0;
        static char *tracer = 0;
        char *pdelim;

        if (str != 0) tracer = str;

        if (delim == 0 || *delim == 0) return NULL;

        if (tracer == NULL || *tracer == '\0') return NULL;

        while (*tracer) {
                for (pdelim = delim; *pdelim; pdelim++) {
                        if (*tracer == *pdelim) break;
                }

                if (*pdelim == '\0') break;
                tracer++;
        }

        for (org = tracer; *tracer; tracer++) {
                for (pdelim = delim; *pdelim; pdelim++) {
                        if (*tracer == *pdelim) break;
                }
                if (*pdelim) {
                        *tracer++ = '\0';
                        break;
                }
        }
        return(org);
}

char *cus_strstr(const char *str1, const char *str2)

{
    char *cp = (char *) str1;
    char *s1, *s2;

    if (!*str2) return (char *) str1;

    while (*cp)
    {
        s1 = cp;
        s2 = (char *) str2;

        while (*s1 && *s2 && !(*s1 - *s2)) s1++, s2++;
        if (!*s2) return cp;
        cp++;
    }

    return NULL;
}

int cus_strcmp(const char * first, const char *two)
{
	int i = 0, j = 0;

	while( first[i] != '\0' )
	{
		if( first[i++] != two[j++] )
			break;
	}

	return (first[i] - two[j]);
}


//netfilter function
unsigned long inet_aton(const char* str)
{
        unsigned long result = 0;
        unsigned int iaddr[4] = {0,};
        unsigned char addr[4] = {0,};

        int i;
        sscanf(str,"%d.%d.%d.%d ",iaddr,iaddr+1,iaddr+2,iaddr+3);
        for(i=0;i<4;i++)
        {
                addr[i] = (char)iaddr[i];
        }

        for(i=3;i>0;i--)
        {
                result |= addr[i];
                result <<= 8;
        }

        result |= addr[0];
        return result;
}

int inet_ntoa(char buffer[] , unsigned long in)
{
	//char buffer[18];
	unsigned char bytes[4];
	bytes[0] = (in & 0xFF000000) >> 24;
	bytes[1] = (in & 0x00FF0000) >> 16;
	bytes[2] = (in & 0x0000FF00) >> 8;
	bytes[3] = (in & 0x000000FF);
	//printk("byte[0] = %d , byte[1] = %d , byte[2] = %d , byte[3] = %d\n",bytes[0] , bytes[1] , bytes[2] , bytes[3]);
	sprintf(buffer, "%d.%d.%d.%d", bytes[3] , bytes[2], bytes[1], bytes[0]);
	//sprintf(buffer , "Hello World");
	//printk("buffer = %s\n",buffer);
	return 1;
}

EXPORT_SYMBOL(cus_strtok);
EXPORT_SYMBOL(inet_aton);
EXPORT_SYMBOL(cus_strcpy);
EXPORT_SYMBOL(inet_ntoa);
