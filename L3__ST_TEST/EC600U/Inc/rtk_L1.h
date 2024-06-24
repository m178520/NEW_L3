#ifndef _RTK_L1_H
#define _RTK_L1_H

#include "deal_string.h"

#define ST_EC600(NAME,FUN,SOURCE) "{\"Name\":\""NAME"\",\"fun\":\""FUN"\",\"Source\":"SOURCE"}"

void RTK_L1_open(char * Name,char *fun,char * source);
void RTK_L1_Mode(char * Name,char *fun,char * source);
void RTK_L1_Source(char * Name,char *fun,char * source);

#endif


