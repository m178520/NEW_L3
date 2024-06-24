#include "rtk_L1.h"

extern void EC600U_send_msg(char* Name,char* fun,char *Source,uint16_t len);

/*RTK上电*/
void RTK_L1_power()
{
	
}

//{"Name":"OPEN","fun":"Moving","Source":"dif_base"}
//{"Name":"OPEN","fun":"Moving","Source":"qxwz"}
//{"Name":"OPEN","fun":"Base","Source":"qxwz"}

/*RTK开机作业*/
void RTK_L1_open(char * Name,char *fun,char * source)
{
	EC600U_send_msg(Name,fun,source,100);
}

//{"Name":"Mode","fun":"change_Mode","Source":"qxwz"}
//{"Name":"Mode","fun":"change_Mode","Source":"dif_base"}
/*改变Mode*/
void RTK_L1_Mode(char * Name,char *fun,char * source)
{
	EC600U_send_msg(Name,fun,source,100);
}

//{"Name":"Mode","fun":"change_source","Source":"qxwz"}
//{"Name":"Mode","fun":"change_source","Source":"dif_base"}
/*变源source*/
void RTK_L1_Source(char * Name,char *fun,char * source)
{
	EC600U_send_msg(Name,fun,source,100);
}



