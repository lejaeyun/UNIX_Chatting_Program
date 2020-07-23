/*
 * chat.h 
 * Hw#2 Make simple chat program
 * Lee Jae Yoon(이재윤), 201624548
 * 유닉스 응용 프로그래밍(CP33357-059)
 * include files, define lengths
*/

#ifndef __CHAT_H__
#define __CHAT_H__

#include <stdio.h>

// To use strcmp, strcpy
#include <string.h>

// To use exit
#include <stdlib.h> 

// To use Share Memory
#include <sys/ipc.h>
#include <sys/shm.h>

// define lengths
#define id_length 20
#define message_length 40

//define true
#define true 1
#define false 0

// \n 개행문자
void rstrip(char * arr, int length) {
	for (int i = 0; i < length; i++)
	{
		if (arr[i] == '\n') {
			arr[i] = '\0';
			break;
		}
	}
}

#endif // CHAT_H
