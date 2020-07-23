/*
 * chatInfo.h 
 * Hw#2 Make simple chat program
 * Lee Jae Yoon(이재윤), 201624548
 * 유닉스 응용 프로그래밍(CP33357-059)
*/

#include "chat.h"
#include "chatInfo.h"

int main(void) 
{
	int shmid;
	shmid = shmget((key_t)20200406, sizeof(CHAT_INFO), 0666|IPC_CREAT);
	
	if (shmid < 0) {
		perror("shmget failed: ");
		exit(-1);
	}

	if (shmctl(shmid, IPC_RMID, 0) < 0) {
		printf("Failed to delete shared memory\n");
		exit(-1);
	}
	
	printf("Chat Remove Success!!\n");

	return 0;
}

