// 함수 정의
#include "HW1_header.h"

/*
* 첫번째 subwindow에 날짜를 출력 (subwindow는 파라미터로 받아옴)
* 생성된 윈도우를 받아 2,2 지점부터 날짜를 출력
* 쓰레드 동기화를 위해 뮤텍스 락, 언락
*/
void* showDate(void * arg)
{
	WINDOW * TargetWnd = (WINDOW *) arg;
	time_t now;
	struct tm ts;
	char buf[80];
	while(TRUE){
		pthread_mutex_lock(&mutex_lock);
		time(&now);
		ts = *localtime(&now);
		strftime(buf, sizeof(buf), "%Y-%m-%d-%a",&ts);
		mvwprintw(TargetWnd, 2,2,"%s",buf);
		box(TargetWnd, ACS_VLINE, ACS_HLINE);
		wrefresh(TargetWnd);
		pthread_mutex_unlock(&mutex_lock);
	}
}
/*
* 두번째 subwindow에 시간을 출력 (subwindow는 파라미터로 받아옴)
* 생성된 윈도우를 받아 2,2 지점부터 시간을 출력
* 쓰레드 동기화를 위해 뮤텍스 락, 언락
*/
void* showTime(void * arg)
{
	WINDOW * TargetWnd = (WINDOW *) arg;
	time_t now;
	time(&now);
	struct tm ts;
	char buf[80];
	while(TRUE){
		pthread_mutex_lock(&mutex_lock);
		time(&now);
		ts = *localtime(&now);
		strftime(buf, sizeof(buf), "%H-%M-%S",&ts);
		mvwprintw(TargetWnd, 2,2,"%s",buf);
		box(TargetWnd, ACS_VLINE, ACS_HLINE);
		wrefresh(TargetWnd);
		pthread_mutex_unlock(&mutex_lock);
	}
}
/*
* 세번째 subwindow에 시작 후 경과한 시간을 출력 (subwindow는 파라미터로 받아옴)
* 생성된 윈도우를 받아 2,2 지점부터 타이머를 출력
* 쓰레드 동기화를 위해 뮤텍스 락, 언락
*/
void* showTimer(void * arg)
{
	WINDOW * TargetWnd = (WINDOW *) arg;
	time_t start,now;
	start = time(0);
	struct tm ts;
	char buf[80];
	while(TRUE){
		pthread_mutex_lock(&mutex_lock);
		now = time(0);
		time_t tmp = now-start;
		ts = *localtime(&tmp);
		// Match with our time
		ts.tm_hour -= 9;
		strftime(buf, sizeof(buf), "%H-%M-%S",&ts);
		mvwprintw(TargetWnd, 2,2,"%s",buf);
		box(TargetWnd, ACS_VLINE, ACS_HLINE);
		wrefresh(TargetWnd);
		pthread_mutex_unlock(&mutex_lock);
	}
}

