#ifndef _USER_SETTING_INTERFACE_H_
#define _USER_SEETING_INTERFACE_H_

#include "iwdg.h"
#include "user_gui.h"

/* 获取失败返回0 获取成功返回1 */
unsigned char user_interface_readkeyboard(unsigned int *key_v,
                unsigned char width);

#endif
