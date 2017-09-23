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
 * posix信号灯分为有名(named)和无名(unnamed)信号灯函数, 有名信号灯总是既可用于线程间的同步, 
 * 又能用于进程间的同步.
 * 注意：posix有名信号灯和posix基于内存的无名信号灯有一些区别，我们必须注意到这些：
 *
 *	1.sem_open不需要类型与shared的参数，有名信号灯总是可以在不同进程间共享的。
 *
 *  2.sem_init不使用任何类似于O_CREAT标志的东西，也就是说，sem_init总是初始化信号灯的值。
 *    因此，对于一个给定的信号灯，我们必须小心保证只调用一次sem_init。
 *
 *  3.sem_open返回一个指向某个sem_t变量的指针，该变量由函数本身分配并初始化。但sem_init
 *    的第一个参数是一个指向某个sem_t变量的指针，该变量由调用者分配，然后由sem_init函数初始化。
 *	
 *  4.posix有名信号灯是通过内核持续的，一个进程创建一个信号灯，另外的进程可以通过该信号灯的
 *    外部名（创建信号灯使用的文件名）来访问它。 posix基于内存的信号灯的持续性却是不定的，
 *    如果基于内存的信号灯是由单个进程内的各个线程共享的，那么该信号灯就是随进程持续的，
 *    当该进程终止时它也会消失。如果某个基于内存的信号灯是在不同进程间同步的，该信号灯必须
 *    存放在共享内存区中，只要该共享内存区存在，该信号灯就存在。
 *
 *  5.基于内存的信号灯应用于进程程很麻烦，而有名信号灯却很方便，它比较适合应用于一个进程的
 *    多个线程。
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
 * pshared参数标明信号量是否在进程的线程间或者是进程间被共享.
 * 
 * 如果pshared的值为0, 那么信号量就是在一个进程的线程间进行共享, 而且要被定义在所有线程都有效的地址
 * (如全局变量,或者是在栈中分配的动态地址).
 * 
 * 如果pshared的值不为0,那么信号量就是在进程间进行共享, 应该定义在共享内存区域内(参见shm_open(3),mmap(3),shmget(3)).
 *
 * 由fork(2)创建的子进程继承父进程的内存映射,那么子进程也可以访问这个信号量. 任何能够访问共享内存区域的进程都
 * 能够通过使用sem_post(3)和sem_wait(3)来操作信号量.
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
