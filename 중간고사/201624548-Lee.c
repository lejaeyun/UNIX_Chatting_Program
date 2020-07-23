/*
 * chat.c
 * Mid-Term Exam Make Simple Chat Ncurses 
 * Using semaphore, Mutex
 * Lee Jae Yoon(이재윤), 201624548
 * 유닉스 응용 프로그래밍(CP33357-059)
 * Revise HW #3
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <ncurses.h>
#include <semaphore.h>
#include <fcntl.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#define BUFFSIZE 1024
#define MAXACCOUNT 20
#define NAMESIZE 20

typedef struct message_buffer {
	char name[NAMESIZE];
	char msg[BUFFSIZE];
	char account[MAXACCOUNT][NAMESIZE];
	int id;
} MSG_BUFF;

// Save the Message By using buffer (In this Program I dont use Queue)
// Because I thought it could be implemented without using queue
MSG_BUFF buf_msg;
// Share the Message By using shm
MSG_BUFF * msg_buff;

int is_running;
int account_cnt;

// INPUT (Using semaphore, mutex)
void *get_input();
// Automatically Send message (Using semaphore, mutex)
void *autoSendMessage();
// Display Message From buf_msg (Using )
// Using pthread_cond_wait, pthread_mutex_unlock
void *DisplayMessageThread();
// Check new message from shm ( Using semaphore )
// Using pthread_cond_signal To DisplayMessageThread
void *FetchMessageFromShmThread();
// show current time, elapse time
void *show_time();
// show accounts who join this chat
void *show_account();

// ncurses setting and start chat threads
void chat();
// cleanup memory and other things
void cleanup();
void die(char * msg);

void setbox();

void refreshing();

int shmid;
char userID[NAMESIZE];
void * shmaddr = (void*) 0;

WINDOW *terminal_scr;
WINDOW *output_scr;
WINDOW *input_scr;
WINDOW *account_scr;
WINDOW *time_scr;

// Mutex (For Every Thread)
pthread_mutex_t thread_lock;
// Conditional Mutex (For Chat-Fetch Thread)
pthread_cond_t chat_cond;

// Semaphore Object
sem_t *sem;
int main(int argc, char* argv[])
{

	if (argc < 2) {
		fprintf(stderr, "[Useage]: ./chat UserID \n");
		exit(-1);
	}

	memset(userID , 0, NAMESIZE);
	strcpy(userID, (const char*)argv[1]);
	// attach shm
	key_t key;	
	key = ftok("namedSemShm.c",'R');
	shmid = shmget(key, sizeof(MSG_BUFF), 0644|IPC_CREAT|IPC_EXCL);
	if (shmid < 0) {
		shmid = shmget(key, sizeof(MSG_BUFF), 0644);
		shmaddr = shmat(shmid, (void*)0, 0666);
		if (shmaddr < (void*) 0) {
			perror("shmat attach is failed : ");
			exit(-1);
		}
	}
	else {
		shmaddr = shmat(shmid, (void*)0, 0666);
	}
	
	if (shmaddr == (int*) (-1))
		perror("shmat: ");
	
	msg_buff = (MSG_BUFF *) shmaddr;
	
	// Mutex Initialize
	pthread_mutex_init(&thread_lock, NULL);
	pthread_cond_init(&chat_cond, NULL);

	// Clear Shared Memory (For Testing)
//	for (int i = 0; i < MAXACCOUNT; i++) {memset(msg_buff->account[i] , 0, NAMESIZE);}
//	return 0;

	for (int i = 0; i < MAXACCOUNT; i++) {
		if (msg_buff->account[i][0] == '\0') {
			strcpy(msg_buff->account[i], (const char*)argv[1]);
			account_cnt = i;
			break;
		}

	}

	// Attach named-semaphore
	sem = (sem_t*) malloc(sizeof(sem_t));
	// if not exist (= first person join chat)
	if (account_cnt == 0) {
		sem_unlink("201624548_Sem");
		sem = sem_open("201624548_Sem",O_CREAT|O_EXCL,0666,1);
	}
	// if exist
	else {
		sem = sem_open("201624548_Sem",0,0666,1);
	}

	if (sem == NULL) {
		perror("sem_open Failed : ");
		cleanup();
		exit(-1);
	}
	initscr();
	chat();
	endwin();
	return 0;
}

void chat()
{
	curs_set(0);
	scrollok(input_scr, TRUE);
	terminal_scr = newwin(24, 80, 0, 0);
	output_scr = subwin(terminal_scr, 20, 60, 0, 0);
	input_scr = subwin(terminal_scr, 4, 60, 20, 0);
	account_scr = subwin(terminal_scr, 20, 20, 0, 60);
	time_scr = subwin(terminal_scr, 4, 20, 20, 60);
	mvwhline(output_scr, 1, 1, 68, 0);
	wprintw(output_scr, "    ***** Type /bye to quit !! ***** \n\n");
	setbox();
	refreshing();
	
	is_running = 1;

	//thread Start
	pthread_t thread[6];
	pthread_create(&thread[0], NULL, get_input, NULL);
	pthread_create(&thread[1], NULL, DisplayMessageThread, NULL);
	pthread_create(&thread[2], NULL, show_account, NULL);
	pthread_create(&thread[3], NULL, show_time, NULL);
	pthread_create(&thread[4], NULL, FetchMessageFromShmThread, NULL);
	pthread_create(&thread[5], NULL, autoSendMessage, NULL);
	
	for (int i = 0; i < 6; i++)
	{
		pthread_join(thread[i], NULL);
	}


	delwin(terminal_scr);
	delwin(output_scr);
	delwin(input_scr);
	delwin(account_scr);
	delwin(time_scr);

}

void *autoSendMessage() {
	int account_num = account_cnt;
	// This Thread Only For 0, 1 (Jico, Izzy) Person
	if (account_num > 1) {
		return NULL;
	}
	// Make Chat Message
	char tmp[BUFFSIZE] = "";
	if (account_num == 0) {
		strcat(tmp,"Hello! My name is ");
		strcat(tmp, userID);
		strcat(tmp, " I love to sing any song");
	}	
	else if (account_num == 1) {
		strcat(tmp,"Hi!! i am ");
		strcat(tmp, userID);
		strcat(tmp, " I like to play on the stage. Ho");
	}
	
	// Send Message (same with get_input())
	srand(0);
	int send_id = 1;
	while (is_running) {
		pthread_mutex_lock(&thread_lock);
		sem_wait(sem);
		sprintf(msg_buff->msg, " %s-%d\n", tmp,send_id);
		strcpy(msg_buff->name, userID);
		msg_buff->id = send_id;
		sem_post(sem);
		wprintw(output_scr, " [Send : %d] > %s", send_id++, msg_buff->msg);
		setbox();
		refreshing();
		pthread_mutex_unlock(&thread_lock);
		sleep((rand()%2+1));
	}
}

void *show_account()
{
	int cnt;
	while (is_running) {
		pthread_mutex_lock(&thread_lock);
		werase(account_scr);
		cnt = 1;
		for (int i = 0; i < MAXACCOUNT; i++) {
			if (msg_buff->account[i][0] != '\0') {
				mvwprintw(account_scr, cnt++, 2,"%s", msg_buff->account[i]);
			}
		}
		box(account_scr, ACS_VLINE, ACS_HLINE);
		wrefresh(account_scr);
		pthread_mutex_unlock(&thread_lock);
		usleep(500);

	}
}

void *show_time()
{
	time_t start,now;
	start = time(0);
	struct tm ts;
	char buf[80];
	while(is_running){
		//시간 출력
		pthread_mutex_lock(&thread_lock);
		werase(time_scr);
		time(&now);
		ts = *localtime(&now);
		strftime(buf, sizeof(buf), "%H-%M-%S",&ts);
		mvwprintw(time_scr, 1,2,"%s",buf);
		
		now = time(0);
		time_t tmp = now-start;
		ts = *localtime(&tmp);
		// Match with our time
		ts.tm_hour -= 9;
		strftime(buf, sizeof(buf), "%H-%M-%S",&ts);
		mvwprintw(time_scr, 2,2,"%s",buf);
		box(time_scr, ACS_VLINE, ACS_HLINE);
		wrefresh(time_scr);
		pthread_mutex_unlock(&thread_lock);
		usleep(500000);
	}
}

void *get_input()
{
	char tmp[BUFFSIZE];
	int send_id = 1;
	while (is_running) {
		mvwgetstr(input_scr, 1, 1, tmp);
		
		pthread_mutex_lock(&thread_lock);
		sem_wait(sem);
		sprintf(msg_buff->msg, " %s\n", tmp);
		strcpy(msg_buff->name, userID);
		msg_buff->id = send_id;
		sem_post(sem);
		if (strcmp (msg_buff->msg, " /bye\n") == 0 ) {
			die("exit");
		}
		wprintw(output_scr, " [Send : %d] > %s", send_id++, msg_buff->msg);
		werase(input_scr);
		setbox();
		refreshing();
		pthread_mutex_unlock(&thread_lock);
		usleep(300);
	}
}

// shm에서 데이터를 메세지 버퍼에 저장
void * FetchMessageFromShmThread() {
	strcpy(buf_msg.msg, msg_buff->msg);
	strcpy(buf_msg.name, msg_buff->name);
	buf_msg.id = msg_buff->id;
	int i = 0;
	while (is_running) {
		sem_wait(sem);
		if (strcmp(msg_buff->msg, " .exit\n") == 0) {
			fprintf(stderr, "Chat is closed\n");
			is_running = 0;
		}
		else {
			if ( (  strcmp(msg_buff->msg, buf_msg.msg) != 0 || 
				strcmp(msg_buff->name, buf_msg.name) != 0 || 
				buf_msg.id != msg_buff->id ) && 
				strcmp(msg_buff->name,userID) != 0 ) {
					
				buf_msg.id = msg_buff->id;
				strcpy(buf_msg.msg, msg_buff->msg);
				strcpy(buf_msg.name, msg_buff->name);
				pthread_cond_signal(&chat_cond);
			}
		}
		sem_post(sem);
		usleep(500);
	}
}

// FetchMessageFromShmThread 에서 시그널을 보내면 메세지 버퍼에 저장된 값을 출력
void *DisplayMessageThread()
{
	int myrecv = 1;
	while (is_running) {
		pthread_cond_wait(&chat_cond,&thread_lock);
		if (strcmp(buf_msg.msg, " .exit\n") == 0) {
			fprintf(stderr, "Chat is closed\n");
			is_running = 0;
			cleanup();
		}
		else {
			if (strcmp(buf_msg.msg, " /bye\n") == 0) {
				wprintw(output_scr, " [Exit %s] Goodbye %s !\n", 
					buf_msg.name, buf_msg.name);
			}
			else {
				wprintw(output_scr, " [Recv #%d By %s] > %s", 
					myrecv++, buf_msg.name, buf_msg.msg);
			}
			box(output_scr, ACS_VLINE, ACS_HLINE);
			wrefresh(output_scr);
		}
		pthread_mutex_unlock(&thread_lock);
	}
}

void setbox() {
	box(output_scr, ACS_VLINE, ACS_HLINE);
	box(input_scr, ACS_VLINE, ACS_HLINE);
	box(account_scr, ACS_VLINE, ACS_HLINE);
	box(time_scr, ACS_VLINE, ACS_HLINE);
	wrefresh(input_scr);
}

void refreshing() {
	wrefresh(output_scr);
	wrefresh(input_scr);
	wrefresh(account_scr);
	wrefresh(time_scr);
}

void cleanup()
{
	memset(msg_buff->account[account_cnt] , 0, NAMESIZE);
	delwin(terminal_scr);
	delwin(output_scr);
	delwin(input_scr);
	delwin(account_scr);
	delwin(time_scr);
	endwin();
	sem_close(sem);
	shmdt(shmaddr);
}

void die(char * msg)
{
	cleanup();
	perror(msg);
	exit(-1);
}
