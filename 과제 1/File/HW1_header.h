/*
* HwHeader.h 
* Hw#1 Make Clock With Threads and Ncurses
* Lee Jae Yoon(이재윤), 201624548
* 유닉스 응용 프로그래밍(CP33357-059)
*/
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <unistd.h>

#include <pthread.h>
#include <ncurses.h>

// 쓰레드 동기화를 위한 뮤텍스 전역변수 선언
pthread_mutex_t mutex_lock;
// 첫번째 subwindow에 날짜를 출력
void* showDate(void * arg);
// 두번째 subwindow에 시간을 출력
void* showTime(void * arg);
// 세번째 subwindow에 시작 후 경과한 시간을 출력
void* showTimer(void * arg);
