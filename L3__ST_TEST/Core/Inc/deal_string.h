#ifndef __DEAL_STRING_H
#define __DEAL_STRING_H
//�ַ�����
#include "stdio.h"
#include "string.h"
#include "stdlib.h"
#include "stdarg.h"
#include "stdint.h"



/*����ض���malloc free����*/
#include "cJSON.h"
#define RE_HEAPS_FUN 0

extern cJSON_Hooks my_cJSON_Hooks;

//�ַ����ָ�
int segment(char* string,char **q, char* str);
void insert_str(char *str,char* q,char const* const_Format,...);
	
//JSON����
CJSON_PUBLIC(cJSON *) StringToObject(const char *str);
char * ObjectToString(const cJSON *item);

cJSON * Json_data_Change( cJSON *item,char const* const_Format, ...);

#endif
