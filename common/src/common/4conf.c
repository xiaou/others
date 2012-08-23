#include "common.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


//从配置文件读取字符串类型数据  
char *GetConfKeyString(const char *title, const char *key, const char *filename, char * buf, size_t bufSize)   
{   
    FILE *fp;   
    char szLine[1024];  
    char tmpstr[1024];  
    int rtnval;  
    int i = 0;   
    int flag = 0;   
    char *tmp;  

	char path[1024];
	if(filename[0] == '~')
	{
		snprintf(path, 1024, "%s/%s", getenv("HOME"), &filename[1]);
		filename = path;
  	}

    if((fp = fopen(filename, "r")) == NULL)   
    {   
        printf("have   no   such   file \n");  
        return NULL;   
    }  
    while(!feof(fp))   
    {   
        rtnval = fgetc(fp);   
        if(rtnval == EOF && sizeof(szLine) != 0)   
        {   
            break;   
        }   
        else   
        {   
            szLine[i++] = rtnval;   
        }   
        if(rtnval == '\n')   
        {   
			if(szLine[i-1] == '\r')
            	i--;

            szLine[--i] = '\0';  
            i = 0;   
            tmp = strchr(szLine, '=');   
  
            if(( tmp != NULL )&&(flag == 1))   
            {   
                if(strstr(szLine,key)!=NULL)   
                {   
                    //注释行  
                    if ('#' == szLine[0])  
                    {  
                    }  
                    else if ( '\/' == szLine[0] && '\/' == szLine[1] )  
                    {  
                          
                    }  
                    else  
                    {  
                        //找打key对应变量  
						strncpy(buf, tmp+1, bufSize);
						buf[bufSize-1] = '\0';
                        fclose(fp);  
                        return buf;   
                    }  
                }   
				else
				{
					memset(szLine, 0, 1024);
				}
            }  
            else
            {   
                strcpy(tmpstr,"[");   
                strcat(tmpstr,title);   
                strcat(tmpstr,"]");  
                if( strncmp(tmpstr,szLine,strlen(tmpstr)) == 0 )   
                {  
                    //找到title  
                    flag = 1;   
                }  
            }  
        }  
    }  
    fclose(fp);   
    return NULL;   
}  
  
//从配置文件读取整类型数据  
int GetConfKeyInt(const char *title, const char *key, const char *filename, char * buf, size_t bufSize)   
{  
	char * s = GetConfKeyString(title,key,filename, buf, bufSize);
    return s ? atoi(s) : 0;  
}  
  
