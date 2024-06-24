#include "deal_string.h"
#include "cmsis_os.h"

/*重定向内存分配函数*/
#if RE_HEAPS_FUN == 1
void *my_malloc(unsigned int uSize, const char *pszFunc, unsigned int uLine)
{
	void *malloc_heaps = pvPortMalloc(uSize);   //调用标准C库的malloc.
	printf("MALLOC:addr:%p; size:%d; func:%s; line:%d\r\n",malloc_heaps, uSize, pszFunc, uLine);
	return malloc_heaps;
}

void my_free(void *pPtr, const char *pszFunc, unsigned int uLine)
{
	printf("FREE: addr:%p; func:%s; line:%d\r\n", pPtr, pszFunc, uLine);
	return vPortFree(pPtr);   //调用标准C库的free.
}
#else
void *my_malloc(unsigned int uSize, const char *pszFunc, unsigned int uLine)
{
return pvPortMalloc(uSize);   //调用FreeRTOS库的malloc.
}

void my_free(void *pPtr, const char *pszFunc, unsigned int uLine)
{
return vPortFree(pPtr);   //调用FreeRTOS库库的free.
}
#endif

cJSON_Hooks my_cJSON_Hooks=
{
	my_malloc,
	my_free,
	NULL
};



/**
 *用于进行字符串分割 
 * @param  string 分割的字符串
 *
 * @param  q      存储分割后字符串的指针数组
 *
 * @param  str    按照哪种字符进行分割
 *
 * @return  返回分割后字符串个数
 */
int segment(char* string,char **q, char* str)
{
	int i = 0;
			char* temp = strtok(string, str);
			while (temp)
			{   
//					printf("%s\r\n", temp);
					q[i++] = temp;			
					if (i >= 20)  break;
					temp = strtok(NULL, str);   
			}
			return i;
}




/**
 *用于进行字符串拼接 
 * @param  string               要插入的源字符串值 例如
 *
 * @param  q                    存储拼接后字符串
 *
 * @param  const_Format         按照哪种字符进行分割
 *
 * @return  返回分割后字符串个数
 */
void insert_str(char *str,char* q,char const* const_Format, ...)
{
		va_list ap;
	uint8_t len = 0;
	char Format[50]; 
	for(int i=0;*(const_Format+i) != '\0'; i++)
	{
		if(*(const_Format+i)=='%')
		{
			Format[len] = *(const_Format+i+1);
			i++;
			len++;
		}
	}
	if(len != 0)
	{
		va_start(ap, const_Format);
		int num = 0;
		char* addr;
		int number=0;
		strcpy(q,str);
		for(int j=0;j<len;j++)
		{
			num += strlen(str+num)+1;
			if(Format[j] == 's')
			{
				addr = va_arg(ap,char *);
				sprintf(q,"%s%s%s",q,addr,str+num);
			}
			else if(Format[j] == 'd')
			{
				number = va_arg(ap,int);
				sprintf(q,"%s%d%s",q,number,str+num);
			}
		}
	}
	else printf("输入格式不正确错误\r\n");
}

///**
//	
//	还未完成

// *用于进行JSON类型数据更改
// * @param  string               要插入的源字符串值 例如
// *
// * @param  q                    存储拼接后字符串
// *
// * @param  const_Format         按照哪种字符进行分割
// *
// * @return  返回分割后字符串个数
// */
//void dict_assign(const char* str, uint8_t num, ...)
//{
//	char* addr;
//	char* parameter[10] = {0};
//	char  q[50] = {0};
//	va_list ap;
//	va_start(ap, num);
//	for(int i=0; i <= num; i++)
//	{
//		parameter[i] = va_arg(ap,char *);
//		if(i==0)                                           //初次查找
//		{
//			addr = strstr(str,parameter[i]);           
//			if(addr == NULL)                                 //未找到
//			{
//				printf("未发现dict[%s]\r\n",parameter[i]);
//				return ;
//			}
//		}
//		else    
//		{
//			addr =  strstr(addr,parameter[i]);
//			if(addr[i] == NULL)
//			{
//				for(int j=0;j <= i;j++)
//				{
//					sprintf(q,"%s[%s]",q,parameter[i]);
//				}
//				printf("未发现dict%s\r\n",q);
//				return ;
//			}
//		}
//		
//	}
//}

/**

 *将字符串变为对象
 * @param  string               要插入的源字符串值 例如
 *
 * @param  q                    存储拼接后字符串
 *
 * @param  const_Format         按照哪种字符进行分割
 *
 * @return  返回分割后字符串个数
*/
CJSON_PUBLIC(cJSON *) StringToObject(const char *str)
{
	cJSON *object = cJSON_Parse(str);
	if (object == NULL)
    {
			const char *error_ptr = cJSON_GetErrorPtr();
			if (error_ptr != NULL)
			{
					printf("Error before: %s\n", error_ptr);
			}
			return NULL;
    }
	return object;
}



/**

 *将对象变为字符串
 * @param  string               要插入的源字符串值 例如
 *
 * @param  q                    存储拼接后字符串
 *
 * @param  const_Format         按照哪种字符进行分割
 *
 * @return  返回分割后字符串个数
*/
char * ObjectToString(const cJSON *item)
{
	 char *test = cJSON_Print(item);
	if(test == NULL) return NULL;
	return test;
}


/**

 *json数值修改
 * @param  item                 要修改的json结构体 
 *
 * @param  const_Format         “%s%c%d%f” 第1位：修改为数据的格式    第2~n位：json每一层的名称的格式 如果有3层就写4个%
 *
 * @param  ...                  第一位为要修改的值，后面的为json每一层的名称
 *
 * @return  返回分割后字符串个数
*/
/*json数值修改*/
cJSON * Json_data_Change( cJSON *item,char const* const_Format, ...)
{
	va_list ap;
	uint8_t len = 0;
	char Format[50]; 
	cJSON *new_item = item;
	for(int i=0;*(const_Format+i) != '\0'; i++)
	{
		if(*(const_Format+i)=='%')
		{
			Format[len] = *(const_Format+i+1);
			i++;
			len++;
		}
	}
	if(len != 0)
	{
		va_start(ap, const_Format);
		char* name[len-1];
		char* addr;
		int number=0;
		double f_number=0;
		for(int j=0;j<len;j++)
		{
			if(j == 0) //需要更改为的值,解析
			{
				if(Format[j] == 's')
				{
					addr = va_arg(ap,char *);
				}
				else if(Format[j] == 'd')
				{
					number = va_arg(ap,int);
				}
				else if(Format[j] == 'f')
				{
					f_number = va_arg(ap,double);
				}
				else 
				{
					printf("输入格式不正确错误\r\n");
					return 0;
				}
			}
			else  //查找json位置
			{
				name[j-1] = va_arg(ap,char *);
				new_item  = cJSON_GetObjectItemCaseSensitive(new_item,name[j-1]);
				if(new_item == NULL) //出现错误，报错
				{
					printf("cjson中不包含%s",name[j-1]);
					return 0;
				}
			}
		}
		if(Format[0] == 's')
		{
			memset(new_item->valuestring,0,strlen(new_item->valuestring));
			strcpy(new_item->valuestring,addr);
		}
		else if(Format[0] == 'd')
		{
			new_item->valueint = number;
			new_item->valuedouble = number;
		}
		else if(Format[0] == 'f')
		{
			new_item->valuedouble = f_number;
		}
	}
	else printf("输入格式不正确错误\r\n");

	return new_item;
}
/*json数值获取*/
void Json_data_Receive(cJSON *item,char const* const_Format, ...)
{
	
}

