/* system V message queue sender */
/* we can fork processes or create many threads as message provider and 
 * assign one message processor who reads all messages!
 * Note: these shoule be peer to peer and needn't to care the order.
 */

#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <string.h>
#include <sys/ipc.h>
#include <errno.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/msg.h>

typedef struct sysmess
{
	long msgtype;
	char msgdata[1024];
}message;

int main(int argc, char *argv[])
{
	int msgid;
	message mess;
	mess.msgtype = 1;
	bzero(mess.msgdata, sizeof(mess.msgdata));

	/* create a message queue */
	if(msgid = msgget(ftok("./etc", 2), IPC_CREAT | 00666) == -1)
	{
		perror("msgget");
		return -1;
	}
	printf("msgid:%d\n", msgid);
	while(1)
	{
		printf("Input msg...\n");
		
		scanf("%s", mess.msgdata);
	
		/* send msg to the message queue */
		if(msgsnd(msgid, &mess, strlen(mess.msgdata), 0) == -1)
		{
			perror("msgsnd");
			return -1;
		}
	}
	
	return 0;
}

