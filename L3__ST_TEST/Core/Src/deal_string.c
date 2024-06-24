#include "deal_string.h"
#include "cmsis_os.h"

/*�ض����ڴ���亯��*/
#if RE_HEAPS_FUN == 1
void *my_malloc(unsigned int uSize, const char *pszFunc, unsigned int uLine)
{
	void *malloc_heaps = pvPortMalloc(uSize);   //���ñ�׼C���malloc.
	printf("MALLOC:addr:%p; size:%d; func:%s; line:%d\r\n",malloc_heaps, uSize, pszFunc, uLine);
	return malloc_heaps;
}

void my_free(void *pPtr, const char *pszFunc, unsigned int uLine)
{
	printf("FREE: addr:%p; func:%s; line:%d\r\n", pPtr, pszFunc, uLine);
	return vPortFree(pPtr);   //���ñ�׼C���free.
}
#else
void *my_malloc(unsigned int uSize, const char *pszFunc, unsigned int uLine)
{
return pvPortMalloc(uSize);   //����FreeRTOS���malloc.
}

void my_free(void *pPtr, const char *pszFunc, unsigned int uLine)
{
return vPortFree(pPtr);   //����FreeRTOS����free.
}
#endif

cJSON_Hooks my_cJSON_Hooks=
{
	my_malloc,
	my_free,
	NULL
};



/**
 *���ڽ����ַ����ָ� 
 * @param  string �ָ���ַ���
 *
 * @param  q      �洢�ָ���ַ�����ָ������
 *
 * @param  str    ���������ַ����зָ�
 *
 * @return  ���طָ���ַ�������
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
 *���ڽ����ַ���ƴ�� 
 * @param  string               Ҫ�����Դ�ַ���ֵ ����
 *
 * @param  q                    �洢ƴ�Ӻ��ַ���
 *
 * @param  const_Format         ���������ַ����зָ�
 *
 * @return  ���طָ���ַ�������
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
	else printf("�����ʽ����ȷ����\r\n");
}

///**
//	
//	��δ���

// *���ڽ���JSON�������ݸ���
// * @param  string               Ҫ�����Դ�ַ���ֵ ����
// *
// * @param  q                    �洢ƴ�Ӻ��ַ���
// *
// * @param  const_Format         ���������ַ����зָ�
// *
// * @return  ���طָ���ַ�������
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
//		if(i==0)                                           //���β���
//		{
//			addr = strstr(str,parameter[i]);           
//			if(addr == NULL)                                 //δ�ҵ�
//			{
//				printf("δ����dict[%s]\r\n",parameter[i]);
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
//				printf("δ����dict%s\r\n",q);
//				return ;
//			}
//		}
//		
//	}
//}

/**

 *���ַ�����Ϊ����
 * @param  string               Ҫ�����Դ�ַ���ֵ ����
 *
 * @param  q                    �洢ƴ�Ӻ��ַ���
 *
 * @param  const_Format         ���������ַ����зָ�
 *
 * @return  ���طָ���ַ�������
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

 *�������Ϊ�ַ���
 * @param  string               Ҫ�����Դ�ַ���ֵ ����
 *
 * @param  q                    �洢ƴ�Ӻ��ַ���
 *
 * @param  const_Format         ���������ַ����зָ�
 *
 * @return  ���طָ���ַ�������
*/
char * ObjectToString(const cJSON *item)
{
	 char *test = cJSON_Print(item);
	if(test == NULL) return NULL;
	return test;
}


/**

 *json��ֵ�޸�
 * @param  item                 Ҫ�޸ĵ�json�ṹ�� 
 *
 * @param  const_Format         ��%s%c%d%f�� ��1λ���޸�Ϊ���ݵĸ�ʽ    ��2~nλ��jsonÿһ������Ƶĸ�ʽ �����3���д4��%
 *
 * @param  ...                  ��һλΪҪ�޸ĵ�ֵ�������Ϊjsonÿһ�������
 *
 * @return  ���طָ���ַ�������
*/
/*json��ֵ�޸�*/
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
			if(j == 0) //��Ҫ����Ϊ��ֵ,����
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
					printf("�����ʽ����ȷ����\r\n");
					return 0;
				}
			}
			else  //����jsonλ��
			{
				name[j-1] = va_arg(ap,char *);
				new_item  = cJSON_GetObjectItemCaseSensitive(new_item,name[j-1]);
				if(new_item == NULL) //���ִ��󣬱���
				{
					printf("cjson�в�����%s",name[j-1]);
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
	else printf("�����ʽ����ȷ����\r\n");

	return new_item;
}
/*json��ֵ��ȡ*/
void Json_data_Receive(cJSON *item,char const* const_Format, ...)
{
	
}

