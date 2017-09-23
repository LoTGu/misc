#include "pthread.h"
#include "stdio.h"

int hello = 1;

void	*thread_test(void *ptr)
{ 
	while(1)
	{
		printf("i am pthread %d\n", hello++);
		if(hello > 1000)
			break;
	}
}

int main()
{
	pthread_attr_t tattr;
	int ret;
	/* set the thread detach state */	
	pthread_t pid;
	ret = pthread_attr_setdetachstate(&tattr,PTHREAD_CREATE_DETACHED);	
	pthread_create(&pid, &tattr, thread_test, NULL);
	
	while(1)
		printf("i am main pthread: %d\n", hello++);
	
	//pthread_join (pid, NULL);
	
	return 0;
}
