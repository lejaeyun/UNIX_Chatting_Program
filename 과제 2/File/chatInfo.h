/*
 * chatInfo.h 
 * Hw#2 Make simple chat program
 * Lee Jae Yoon(이재윤), 201624548
 * 유닉스 응용 프로그래밍(CP33357-059)
 * Struct About chatInfo
*/

#ifndef __CHAT_SHM_H__
#define __CHAT_SHM_H__

#include "chat.h"

typedef struct chatInfo {
	char userID[id_length];
	long messageTime;
	char message[message_length];
} CHAT_INFO;

#endif //__CHAT_SHM_H__
