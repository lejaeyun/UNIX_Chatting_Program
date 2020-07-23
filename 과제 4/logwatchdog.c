/*
 * logwatchdog.h 
 * Hw#4 Make logwatchdog
 * Lee Jae Yoon(이재윤), 201624548
 * 유닉스 응용 프로그래밍(CP33357-059)
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <wait.h>

#include <sys/types.h>
#include <errno.h>
#include <utmp.h>
#include <syslog.h>

void SystemInfoIO();
void make_files(char *, char *);
void showLog(char *);
void appendLog(char * , struct utmp );

int main(int argc, char* argv[])
{
	pid_t 	pid, waitPID;
	if ((pid = fork()) < 0) 
		perror("fork error");
	
	if (pid == 0) { //child
		if ((pid = fork()) < 0)
			perror("fork error");
		else if (pid > 0) {
			printf("1st child's parent process id : %d\n", getppid());
			printf("1st child process id : %d\n", getpid());
			exit(0);
		}
		sleep(1);
		printf("\n2st child's parent process id : %d\n", getppid());
		printf("2st child process id : %d\n", getpid());
		
		// Start
		SystemInfoIO();

		
		exit(0);
	}

	waitPID = waitpid(pid, NULL, 0);

	if (waitPID != pid)
		perror("Wait error");

	printf("\n waitPid : %d \n", waitPID);
	return 0;
}

// make directory and log tmp file
void make_files(char * strFolderPath, char * strFileName) {
	// make directory
	FILE * file;
	int result = mkdir(strFolderPath,0777);
	if (result == 0)
		printf("\nmkdir Success !!\n");
	else
		printf("\nDirectory already exist\n");
	// tmp file not exist
	if (access(strFileName, F_OK) == -1) {
		file = fopen(strFileName, "w");
		fclose(file);
		printf("make log file success !!\n");
	}
}

void appendLog(char * strFileName, struct utmp buff) {
	// append log (all)
	FILE * file;
	file = fopen(strFileName, "a");
	if (file){
		fwrite(&buff, 1, sizeof(struct utmp), file);
		printf("Append log file success !!\n");
	}
	else
		printf("Append Log File Failed!!\n");

	fclose(file);
}

void showLog(char * strFileName) {
	// tmp File size Check
	FILE * file = fopen(strFileName, "r");
	fseek(file, 0, SEEK_END);
	int size = ftell(file);
	fclose(file);
	// Log Count
	int count = size / sizeof(struct utmp);
	// Read Log
	struct utmp log[count];
	file = fopen(strFileName, "r");
	int x = 0;
	if (file)
		x = fread(&log, sizeof(struct utmp), count, file);
	// Send Syslog
	for (int i = 0; i < x; i++) {
		syslog(LOG_INFO, "[Status : %d] id : %s, time : %d\n", 
				log[i].ut_type, log[i].ut_user, log[i].ut_time );
		printf("[Status : %d] id : %s, time : %d\n", 
				log[i].ut_type, log[i].ut_user, log[i].ut_time );
	}	
	printf("syslog Send Success !!\n");
}

void SystemInfoIO() {
	// variables
	char * strFolderPath = "/tmp/201624548";
	char * strFileName = "/tmp/201624548/cse";
	char * strwtmpPath = "/var/log/wtmp";
	FILE * file;
	time_t log_time = 0;
	// buffer
	struct utmp buff_log;
	struct utmp log;
	// to
	buff_log.ut_time = 0;
	make_files(strFolderPath, strFileName);
	while (1) {
		file = fopen(strwtmpPath, "rb");
		if (file) {
			int x = fread(&log, sizeof(struct utmp), 1, file);
			if (x) {
				if ( (log.ut_time != buff_log.ut_time) ||
				     (strcmp(log.ut_user, buff_log.ut_user) != 0)  ) {
					buff_log.ut_time = log.ut_time;
					strcpy(buff_log.ut_user, log.ut_user);
					if (log.ut_type <= 10 && log.ut_type >= 0)
						appendLog(strFileName, log);
				}
			}
		}
		else {
			perror("Failed to open wtmp");
			break;
		}
		// send log with 30 sec
		if ( (time(NULL) - log_time) >= 30)  {
			log_time = time(NULL);
			showLog(strFileName);
		}
		fclose(file);
	}
}
