#ifndef __CHANGE_STATUS_H
#define __CHANGE_STATUS_H
#include "main.h"
#include "cmsis_os.h"

extern Device_Poweron_status_t Device_Poweron_status;
extern Change_Status_t Device_Run_Status;

void change_status_fun(void);

#endif
