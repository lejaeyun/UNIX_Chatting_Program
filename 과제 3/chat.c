/*
 * chat.c
 * Hw#3 Make Simple Chat Ncurses
 * Lee Jae Yoon(이재윤), 201624548
 * 유닉스 응용 프로그래밍(CP33357-059)
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <ncurses.h>
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



MSG_BUFF * msg_buff;

int is_running;
int account_cnt;

void *print_chat();
void *get_input();
void *recv_send();
void chat();
void cleanup();
void die(char * msg);
void setbox();
void refreshing();
void *show_time();
void *show_account();

int shmid;
char userID[NAMESIZE];
void * shmaddr = (void*) 0;

WINDOW *terminal_scr;
WINDOW *output_scr;
WINDOW *input_scr;
WINDOW *account_scr;
WINDOW *time_scr;

int main(int argc, char* argv[])
{


	if (argc < 2) {
		fprintf(stderr, "[Useage]: ./chat UserID \n");
		exit(-1);
	}

	memset(userID , 0, NAMESIZE);
	strcpy(userID, (const char*)argv[1]);
		
	shmid = shmget((key_t)20200406, sizeof(MSG_BUFF), 0666|IPC_CREAT);
	shmaddr = shmat(shmid, (void *)0, 0666);
	msg_buff = (MSG_BUFF *) shmaddr;
	memset(msg_buff->msg , 0, BUFFSIZE);



	for (int i = 0; i < MAXACCOUNT; i++) {
		if (msg_buff->account[i][0] == '\0') {
			strcpy(msg_buff->account[i], (const char*)argv[1]);
			account_cnt = i;
			break;
		}
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
	wprintw(output_scr, "    ***** Type /exit to quit !! ***** \n\n");
	setbox();
	refreshing();
	

	is_running = 1;

	pthread_t thread[4];
	pthread_create(&thread[0], NULL, get_input, NULL);
	pthread_create(&thread[1], NULL, recv_send, NULL);
	pthread_create(&thread[2], NULL, show_account, NULL);
	pthread_create(&thread[3], NULL, show_time, NULL);
	
	pthread_join(thread[0], NULL);
	pthread_join(thread[1], NULL);
	pthread_join(thread[2], NULL);
	pthread_join(thread[3], NULL);
	
	delwin(terminal_scr);
	delwin(output_scr);
	delwin(input_scr);
	delwin(account_scr);
	delwin(time_scr);
	
}

void *show_account()
{
	int cnt;
	while (is_running) {
		cnt = 1;
		werase(account_scr);
		for (int i = 0; i < MAXACCOUNT; i++) {
			if (msg_buff->account[i][0] != '\0') {
				mvwprintw(account_scr, cnt++, 2,"%s", msg_buff->account[i]);
			}
		}
		box(account_scr, ACS_VLINE, ACS_HLINE);
		wrefresh(account_scr);
		usleep(300);

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
		usleep(500);
	}
}

void *get_input()
{
	char tmp[BUFFSIZE];
	int send_id = 1;
	while (is_running) {
		mvwgetstr(input_scr, 1, 1, tmp);
		sprintf(msg_buff->msg, " %s\n", tmp);
		strcpy(msg_buff->name, userID);
		msg_buff->id = send_id;

		if (strcmp (msg_buff->msg, " /exit\n") == 0 ) {
			die("exit");
		}
		wprintw(output_scr, " [Send : %d] > %s", send_id++, msg_buff->msg);
		werase(input_scr);
		setbox();
		refreshing();
		usleep(100);
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

void *recv_send()
{
	MSG_BUFF bef_msg;
	strcpy(bef_msg.msg, msg_buff->msg);
	strcpy(bef_msg.name, msg_buff->name);
	bef_msg.id = msg_buff->id;
	int myrecv = 1;
	while (is_running) {
		if (strcmp(msg_buff->msg, " .exit\n") == 0) {
			fprintf(stderr, "Chat is closed\n");
			is_running = 0;
		}
		else {
			if ( (  strcmp(msg_buff->msg, bef_msg.msg) != 0 || 
				strcmp(msg_buff->name, bef_msg.name) != 0 || 
				bef_msg.id != msg_buff->id ) && 
				strcmp(msg_buff->name,userID) != 0 ) {
				if (strcmp(msg_buff->msg, " /exit\n") == 0) {
					wprintw(output_scr, " [Exit %s] Goodbye %s !\n", 
						msg_buff->name, msg_buff->name);
					strcpy(bef_msg.msg, msg_buff->msg);
				}
				else {
					wprintw(output_scr, " [Recv #%d By %s] > %s", 
						myrecv++, msg_buff->name, msg_buff->msg);
				}
				bef_msg.id = msg_buff->id;
				strcpy(bef_msg.msg, msg_buff->msg);
				strcpy(bef_msg.name, msg_buff->name);
				box(output_scr, ACS_VLINE, ACS_HLINE);
				wrefresh(output_scr);
				wrefresh(input_scr);
			}
		}
	}
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
	shmdt(shmaddr);
}

void die(char * msg)
{
	cleanup();
	perror(msg);
	exit(-1);
}
