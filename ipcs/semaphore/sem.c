/*
 * POSIX semaphores allow processes and threads to synchronize their actions.

 * A semaphore is an integer whose value is never allowed to fall below zero.
 * Two operations can be performed on semaphores: increment the semaphore value
 * by one (sem_post(3)); and decrement the semaphore value by one (sem_wait(3)).
 * If the value of a semaphore is currently zero, then a sem_wait(3) operation
 * will block until the value becomes greater than zero.
 *
 * POSIX semaphores come in two forms: named semaphores and unnamed semaphores
 * (memory-based semaphores).
 *
 * POSIX named semaphores have kernel persistence: if not removed by
 * sem_unlink(3), a semaphore will exist until the system is shut down.
 */

/* 
 * posix�źŵƷ�Ϊ����(named)������(unnamed)�źŵƺ���, �����źŵ����Ǽȿ������̼߳��ͬ��, 
 * �������ڽ��̼��ͬ��.
 * ע�⣺posix�����źŵƺ�posix�����ڴ�������źŵ���һЩ�������Ǳ���ע�⵽��Щ��
 *
 *	1.sem_open����Ҫ������shared�Ĳ����������źŵ����ǿ����ڲ�ͬ���̼乲��ġ�
 *
 *  2.sem_init��ʹ���κ�������O_CREAT��־�Ķ�����Ҳ����˵��sem_init���ǳ�ʼ���źŵƵ�ֵ��
 *    ��ˣ�����һ���������źŵƣ����Ǳ���С�ı�ֻ֤����һ��sem_init��
 *
 *  3.sem_open����һ��ָ��ĳ��sem_t������ָ�룬�ñ����ɺ���������䲢��ʼ������sem_init
 *    �ĵ�һ��������һ��ָ��ĳ��sem_t������ָ�룬�ñ����ɵ����߷��䣬Ȼ����sem_init������ʼ����
 *	
 *  4.posix�����źŵ���ͨ���ں˳����ģ�һ�����̴���һ���źŵƣ�����Ľ��̿���ͨ�����źŵƵ�
 *    �ⲿ���������źŵ�ʹ�õ��ļ��������������� posix�����ڴ���źŵƵĳ�����ȴ�ǲ����ģ�
 *    ��������ڴ���źŵ����ɵ��������ڵĸ����̹߳���ģ���ô���źŵƾ�������̳����ģ�
 *    ���ý�����ֹʱ��Ҳ����ʧ�����ĳ�������ڴ���źŵ����ڲ�ͬ���̼�ͬ���ģ����źŵƱ���
 *    ����ڹ����ڴ����У�ֻҪ�ù����ڴ������ڣ����źŵƾʹ��ڡ�
 *
 *  5.�����ڴ���źŵ�Ӧ���ڽ��̳̺��鷳���������źŵ�ȴ�ܷ��㣬���Ƚ��ʺ�Ӧ����һ�����̵�
 *    ����̡߳�
 */

#include <stdio.h>
#include <string.h>
#include <semaphore.h>
#include <errno.h>
#include <sys/types.h>
#include <stdlib.h>

#include <sys/mman.h>
#include <sys/stat.h>        /* For mode constants */
#include <fcntl.h>           /* For O_* constants */

/* gcc -o sem-test sem.c -lrt */
int old_main()
{
	sem_t *mutex;
	int val = 0;
	
	/* if this mutex need to be created then try to delete firstly */
	if(access("/dev/shm/sem.mutex", F_OK) == 0)
	{
		printf("the mutex is exised\n");
		sem_unlink("mutex");
	}

	/* On Linux, named semaphores are created in a virtual file system, normally
   * mounted under /dev/shm, with names of the form sem.somename.  (This is the
   * reason that semaphore names are limited to NAME_MAX-4 rather than NAME_MAX
   * characters.)
	 *	
	 * so will create /dev/shm/sem.mutex here
	*/
	mutex = sem_open("mutex", O_RDWR | O_CREAT, 0644, 1);
	if(mutex == SEM_FAILED)
	{
    printf("sem_open failed! (%s)\n", strerror(errno));
    return -1;
	}

	sem_getvalue(mutex, &val);
	printf("sem_open success, and get val as %d\n", val);	

	//sem_trywait(mutex);
	pid_t pid = fork();
	if(pid == 0)
	{
		int i = 0;
		sem_wait(mutex);
		for(; i < 3; i++)
		{
			printf("child hello\n");
			sleep(1);
		}
		sem_post(mutex);
		
		/* wait for father remove mutex */
		while(1)
			sleep(1);
	}
	else if(pid > 0)
	{
		int j = 0;

		/* let child get the semphore */
		sleep(1);
		
		sem_wait(mutex);
		for(; j < 3; j++)
		{
			printf("father hello\n");
			sleep(1);
		}

		sem_post(mutex);	
	}
	
	sem_close(mutex);
	sem_unlink("mutex");
	
	return 0;
}

//------------------------------------------------------------------------------------------------------------------

/* int sem_init(sem_t *sem, int pshared, unsigned int value);*/
/*
 * pshared���������ź����Ƿ��ڽ��̵��̼߳�����ǽ��̼䱻����.
 * 
 * ���pshared��ֵΪ0, ��ô�ź���������һ�����̵��̼߳���й���, ����Ҫ�������������̶߳���Ч�ĵ�ַ
 * (��ȫ�ֱ���,��������ջ�з���Ķ�̬��ַ).
 * 
 * ���pshared��ֵ��Ϊ0,��ô�ź��������ڽ��̼���й���, Ӧ�ö����ڹ����ڴ�������(�μ�shm_open(3),mmap(3),shmget(3)).
 *
 * ��fork(2)�������ӽ��̼̳и����̵��ڴ�ӳ��,��ô�ӽ���Ҳ���Է�������ź���. �κ��ܹ����ʹ����ڴ�����Ľ��̶�
 * �ܹ�ͨ��ʹ��sem_post(3)��sem_wait(3)�������ź���.
 * 
*/

/* memory semphore no effect between process */
int omain()
{
	sem_t mutex;
	int val = 0;
	
	sem_init(&mutex, 1, 1);
	sem_getvalue(&mutex, &val);
	printf("sem_open success, and get val as %d\n", val);	

	//sem_trywait(mutex);
	pid_t pid = fork();
	if(pid == 0)
	{
		int i = 0;
		sem_wait(&mutex);
		for(; i < 3; i++)
		{
			printf("child hello\n");
			sleep(1);
		}
		sem_post(&mutex);
		
		/* wait for father remove mutex */
		while(1)
			sleep(1);
	}
	else if(pid > 0)
	{
		int j = 0;

		/* let child get the semphore */
		sleep(1);	
		
		sem_wait(&mutex);
		for(; j < 3; j++)
		{
			printf("father hello\n");
			sleep(1);
		}

		sem_post(&mutex);	
	}
	sem_destroy(&mutex);
	
	return 0;
}

/*
 * Prior to kernel 2.6, Linux only supported unnamed, thread-shared semaphores.
 * On a system with Linux 2.6 and a glibc that provides the NPTL threading
 * implementation, a complete implementation of POSIX semaphores is provided.
*/

/* now memory semphore have effect between process  */
int nmain() 
{ 
	sem_t *mutex;
	void *ptr;
	int fd;

#define MMAP_FILE "mmap_test"
	fd = shm_open(MMAP_FILE, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
	if(fd < 0)
	{ 
	  printf("error open region\n"); 
	  return 0; 
	}

	ftruncate(fd, sizeof(sem_t));	
	ptr = mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0); 
	if(ptr == MAP_FAILED)
	{
	  perror("mmap\n"); 
	  return 0; 
	}
	mutex = (sem_t *)ptr;
	
	/* second parameter must be 1 if use between processes */
	sem_init(mutex, 1, 1);

	pid_t pid = fork();
	if(pid == 0)
	{
		sem_t *mutex;
		void *ptr;
    int fd;
    
    fd = shm_open(MMAP_FILE, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR); 
    if(fd < 0) 
    { 
      perror("shm_open\n"); 
      return 0; 
    }

    ftruncate(fd, sizeof(sem_t)); 
    ptr = mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0); 
    if(ptr  == MAP_FAILED)
    { 
      perror("mmap\n"); 
      return 0; 
    }
    mutex = (sem_t *)ptr;
    
		int i = 0;
		sem_wait(mutex);
		for(; i < 3; i++)
		{
			printf("child hello\n");
			sleep(1);
		}
		sem_post(mutex);		
		close(fd);
		munmap(ptr, sizeof(sem_t));
		
		/* wait for father remove mutex */
		while(1)
			sleep(1);
	}
	else if(pid > 0)
	{
		int j = 0;

		/* let child get the semphore */
		sleep(1);	
		
		sem_wait(mutex);
		for(; j < 3; j++)
		{
			printf("father hello\n");
			sleep(1);
		}

		sem_post(mutex);	
	}
	
	close(fd);
	munmap(ptr, sizeof(sem_t));
	
	shm_unlink("region");
	sem_destroy(mutex);

	return 0; 
}

int main()
{
	/* testing for named semphore */
	old_main();
	
	/* testing for unnamed(memory based) semphore */
	omain();
	nmain();
	
	return 0;
}
