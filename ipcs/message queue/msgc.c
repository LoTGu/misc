/* system V message queue receiver */

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

int main()
{
	int msgid;
	message mess;
	mess.msgtype = 1;

	if(msgid = msgget(ftok("./etc", 2), IPC_CREAT | 00666) == -1)
	{
		perror("msgget");
		return -1;
	}
	printf("msgid:%d\n", msgid);
	while(1)
	{
		bzero(mess.msgdata, sizeof(mess.msgdata));
		
		if(msgrcv(msgid, &mess, sizeof(mess.msgdata), 0, 0) == -1)
		{
			perror("msgrcv");
			return -1;
		}
		
		printf("recv:%s\n", mess.msgdata);
	}
	
	return 0;
}
