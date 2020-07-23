#include "HW1_header.h"

int main(void) 
{
	//declare variable 
	pthread_t date_thread,time_thread,timer_thread;
	WINDOW *dateWnd, *timeWnd,*timerWnd;
	//ncurses default setting end
	initscr();
	noecho();
	dateWnd = subwin(stdscr, 5, 50, 0, 0);
	timeWnd = subwin(stdscr, 5, 50, 5, 0);
	timerWnd = subwin(stdscr, 5, 50, 10, 0);
	curs_set(0);

	wrefresh(dateWnd);
	wrefresh(timeWnd);
	wrefresh(timerWnd);
	//ncurses default setting end
	
	//setting mutex
	pthread_mutex_init(&mutex_lock, NULL);
	
	// create date_thread
	if ( pthread_create(&date_thread,NULL,showDate,(void*)dateWnd) < 0){
		printw("cannot create t1");
		sleep(1);
		return 1;
	}
	// create time_thread
	if ( pthread_create(&time_thread,NULL,showTime,(void*)timeWnd) < 0){
		printw("cannot create time_thread");
		sleep(1);
		return 1;
	}
	// create timer_thread
	if ( pthread_create(&timer_thread,NULL,showTimer,(void*)timerWnd) < 0){
		printw("cannot create timer_thread");
		sleep(1);
		return 1;
	}
	// exit whie "q"
	while (TRUE) {
		int input = getch();
		if ( input == 'q' ) break;
	}
	
	// 쓰레드 종료
	pthread_cancel(date_thread);
	pthread_cancel(time_thread);
	pthread_cancel(timer_thread);

	// ncurses 종료
	endwin();
	return 0;
}
