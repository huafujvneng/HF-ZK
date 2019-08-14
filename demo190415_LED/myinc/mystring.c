#include <stdio.h>  
#include <string.h>  
  

char * left(char *dst,char *src, int n)  
{  
    char *p = src;  
    char *q = dst;  
    int len = strlen(src);  
    if(n>len) n = len;  
    while(n--) *(q++) = *(p++);  
    *(q++)='\0'; 
    return dst;  
}  
  
 
char * mid(char *dst,char *src, int n,int m) 
{  
    char *p = src;  
    char *q = dst;  
    int len = strlen(src);  
    if(n>len) n = len-m;
    if(m<0) m=0; 
    if(m>len) return NULL;  
    p += m;  
    while(n--) 
		{
			*(q++) = *(p++);
		}			
    *(q++)='\0';
    return dst;  
}  
  

char * right(char *dst,char *src, int n)  
{  
    char *p = src;  
    char *q = dst;  
	int i;
    int len = strlen(src);  
    if(n>len)
		{
			n = len; 
		}			
    p += (len-n);
			for(i=0;i<n;i++)
		{
			*(q++)=*(p++);
			//q++;
			//p++;
		}
    return dst;
} 
