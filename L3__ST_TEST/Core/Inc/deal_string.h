#ifndef __DEAL_STRING_H
#define __DEAL_STRING_H
//字符处理
#include "stdio.h"
#include "string.h"
#include "stdlib.h"
#include "stdarg.h"
#include "stdint.h"



/*如果重定义malloc free函数*/
#include "cJSON.h"
#define RE_HEAPS_FUN 0

extern cJSON_Hooks my_cJSON_Hooks;

//字符串分割
int segment(char* string,char **q, char* str);
void insert_str(char *str,char* q,char const* const_Format,...);
	
//JSON方法
CJSON_PUBLIC(cJSON *) StringToObject(const char *str);
char * ObjectToString(const cJSON *item);

cJSON * Json_data_Change( cJSON *item,char const* const_Format, ...);

#endif
