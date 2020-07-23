/*
 * chat.c 
 * Hw#2 Make simple chat program
 * Lee Jae Yoon(이재윤), 201624548
 * 유닉스 응용 프로그래밍(CP33357-059)
*/

#include "chat.h"
#include "chatInfo.h"

int main(int argc, char* argv[]) 
{
	//define variables
	int shmid;
	char userID[id_length];
	CHAT_INFO * chatInfo = NULL;
	void * shmaddr = (void*) 0;
	
	if (argc < 2) {
		fprintf(stderr, "[Useage]: ./chat UserID \n");
		exit(-1);
	}
	
	strcpy(userID, (const char*)argv[1]);
	
	shmid = shmget((key_t)20200406, sizeof(CHAT_INFO), 0666|IPC_CREAT);
	
	if (shmid < 0) {
		perror("shmget failed: ");
		exit(-1);
	}
	// Attach Share memory
	shmaddr = shmat(shmid, (void *)0, 0666);
	chatInfo = (CHAT_INFO *) shmaddr;
	chatInfo->messageTime = 0;
	
	printf("Chat Start#\n");
	
	while (true){
		char inputstr[message_length];
		fgets(inputstr,message_length, stdin);
		// replace escape letter
		rstrip(inputstr, message_length);
		// loading Chatting
		if (strcmp(inputstr, "..") == 0) {
			printf("[%s] #%ld : %s\n",chatInfo->userID, chatInfo->messageTime, chatInfo->message);
		}
		// Exit chat
		else if (strcmp(inputstr, "./bye") == 0) {
			printf("Chat Program Exit !!\n");
			printf("GoodBye %s\n", userID);
			break;
		}
		// Save chat to share memory
		else {
			strcpy(chatInfo->userID, userID);
			chatInfo->messageTime++;
			strcpy(chatInfo->message, inputstr);
		}
	}
	// detach Share memory
	if (shmdt(shmaddr) == -1)
	{
		printf("Failed to detach Share Memory !!");
		exit(-1);
	}
	
	return 0;
}
