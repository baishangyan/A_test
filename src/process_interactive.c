#include "../include/process_interactive.h"

/*****************************************************************
 *                     共享内存通信                              *
 *                                                               *
 * **************************************************************/

int producer()
{
   int running = 1; //程序运行标志位
   void *shared_memory = (void *)0;
   struct shared_use_st *shared_stuff;
   char buffer[BUFSIZ];
   int shmid; //共享内存标识符

   /*创建共享内存*/
   shmid = shmget((key_t)1234, sizeof(struct shared_use_st), 0666 | IPC_CREAT);
   if (shmid == -1)
   {
      fprintf(stderr, "shmget failed\n");
      exit(EXIT_FAILURE);
   }

   /*将共享内存连接到一个进程的地址空间中*/
   shared_memory = shmat(shmid, (void *)0, 0); //指向共享内存第一个字节的指针
   if (shared_memory == (void *)-1)
   {
      fprintf(stderr, "shmat failed\n");
      exit(EXIT_FAILURE);
   }

   printf("Memory attached at %p\n", shared_memory);
   shared_stuff = (struct shared_use_st *)shared_memory;

   /*生产者写入数据*/
   while (running)
   {
      while (shared_stuff->written_by_you == 1)
      {
         sleep(1);
         printf("waiting for client...\n");
      }
      printf("Enter some text: ");
      fgets(buffer, BUFSIZ, stdin);
      strncpy(shared_stuff->some_text, buffer, TEXT_SZ);
      shared_stuff->written_by_you = 1;
      if (strncmp(buffer, "end", 3) == 0)
      {
         running = 0;
      }
   }

   /*该函数用来将共享内存从当前进程中分离,仅使得当前进程不再能使用该共享内存*/
   if (shmdt(shared_memory) == -1)
   {
      fprintf(stderr, "shmdt failed\n");
      exit(EXIT_FAILURE);
   }

   printf("producer exit.\n");
   exit(EXIT_SUCCESS);
}

int customer()
{
   int running = 1; //程序运行标志位
   void *shared_memory = (void *)0;
   struct shared_use_st *shared_stuff;
   int shmid; //共享内存标识符
   srand((unsigned int)getpid());

   /*创建共享内存*/
   shmid = shmget((key_t)1234, sizeof(struct shared_use_st), 0666 | IPC_CREAT);
   if (shmid == -1)
   {
      fprintf(stderr, "shmget failed\n");
      exit(EXIT_FAILURE);
   }

   /*将共享内存连接到一个进程的地址空间中*/
   shared_memory = shmat(shmid, (void *)0, 0); //指向共享内存第一个字节的指针
   if (shared_memory == (void *)-1)
   {
      fprintf(stderr, "shmat failed\n");
      exit(EXIT_FAILURE);
   }

   printf("Memory attached at %p\n", shared_memory);
   shared_stuff = (struct shared_use_st *)shared_memory;
   shared_stuff->written_by_you = 0;

   /*消费者读取数据*/
   while (running)
   {
      if (shared_stuff->written_by_you)
      {
         printf("You wrote: %s", shared_stuff->some_text);
         sleep(rand() % 4);
         shared_stuff->written_by_you = 0;
         if (strncmp(shared_stuff->some_text, "end", 3) == 0)
         {
            running = 0;
         }
      }
   }

   /*该函数用来将共享内存从当前进程中分离,仅使得当前进程不再能使用该共享内存*/
   if (shmdt(shared_memory) == -1)
   {
      fprintf(stderr, "shmdt failed\n");
      exit(EXIT_FAILURE);
   }

   /*将共享内存删除,所有进程均不能再访问该共享内存*/
   if (shmctl(shmid, IPC_RMID, 0) == -1)
   {
      fprintf(stderr, "shmctl(IPC_RMID) failed\n");
      exit(EXIT_FAILURE);
   }

   printf("customer exit.\n");
   exit(EXIT_SUCCESS);
}

/*****************************************************************
 *                       内存映射mmap                            *
 *                                                               *
 * **************************************************************/

void mmap_write()
{
   printf("%s start.\n", __FUNCTION__);
   int fd;
   struct stat st;
   stshared_use_st* shmptr = NULL;
   int base = 1;
   fd = open("mmap_mem.txt", O_RDWR | O_CREAT, 0666);
   if (fd == -1)
   {
      printf("mmap_write open fail.\n");
      return;
   }
   if (fstat(fd, &st) == -1)
   {
      perror("fstat ");
      return;
   }
   if (st.st_size == 0)
   {
      write(fd, &base, sizeof(base));    //mmap不能映射到空文件中，先写入基础值
   }
   printf("file size:%d\n", (int)st.st_size);
   
   shmptr = mmap((void *)NULL, sizeof(stshared_use_st), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
   if (shmptr == NULL || shmptr == (void*)-1)
   {
      printf("mmap_write mmap fail.\n");
      close(fd);
      return;
   }
   close(fd);
   printf("ptr:%p,[%d,%d,%s]\n", shmptr, shmptr->written_by_you, shmptr->data, shmptr->some_text);
   shmptr->written_by_you = 1;
   shmptr->data = 2;
   strcpy(shmptr->some_text, "hello yx.\n");
   
   //msync(shmptr, sizeof(stshared_use_st), MS_SYNC);
   //sleep(1);
   munmap(shmptr, sizeof(stshared_use_st));         //同一个进程这里不能munmap，会导致read不出来

   
   printf("%s end.\n", __FUNCTION__);
}

void mmap_read()
{
   printf("%s start.\n", __FUNCTION__);
   int fd;
   stshared_use_st* shmptr;
   fd = open("mmap_mem.txt", O_RDONLY, 0);
   if (fd == -1)
   {
      printf("mmap_read open fail.\n");
      return;
   }
   shmptr = mmap((void*)NULL, sizeof(stshared_use_st), PROT_READ, MAP_SHARED, fd, 0);
   if (shmptr == NULL || shmptr == (void*)-1)
   {
      printf("mmap_read mmap fail.\n");
      close(fd);
      return;
   }
   close(fd);
   printf("ptr:%p\n", shmptr);
   while (TRUE)
   {
      usleep(10000);       //不同进程需要延迟等到写入数据后读取，因为打开文件数据会清空
      if (shmptr->written_by_you == 1)
      {
         printf("read:%s", shmptr->some_text);
         break;
      }
   }
   munmap(shmptr, sizeof(stshared_use_st));
   
   printf("%s end.\n", __FUNCTION__);
}

/*****************************************************************
 *                      进程间通信——IPC                          *
 *                        消息队列通信                           *
 * **************************************************************/
int msgqueue_send()
{
   int running = 1;//程序运行标识符
   struct my_msg_st some_data;
   int msgid;//消息队列标识符
   char buffer[BUFSIZ];

   /*创建与接受者相同的消息队列*/
   msgid = msgget((key_t)1234, 0666 | IPC_CREAT);
   if (msgid == -1)
   {
      fprintf(stderr, "msgget failed with error: %d/n", errno);
      return -1;
   }

   /*向消息队列中发送消息*/
   while(running)
   {
      printf("Enter some text: ");
      fgets(buffer, BUFSIZ, stdin);
      some_data.my_msg_type = 1;
      strcpy(some_data.some_text, buffer);
      if (msgsnd(msgid, (void *)&some_data, MAX_TEXT, 0) == -1)
      {
         fprintf(stderr, "msgsnd failed/n");
         exit(EXIT_FAILURE);
      }
      if (strncmp(buffer, "end", 3) == 0)
      {
         running = 0;
      }
   }
   exit(EXIT_SUCCESS);
}

int msgqueue_recv()
{
   int running = 1;//程序运行标识符
   int msgid; //消息队列标识符
   struct my_msg_st some_data;
   long int msg_to_receive = 0;//接收消息的类型--0表示msgid队列上的第一个消息

   /*创建消息队列*/
   msgid = msgget((key_t)1234, 0666 | IPC_CREAT);
   if (msgid == -1)
   {
      fprintf(stderr, "msgget failed with error: %d/n", errno);
      exit(EXIT_FAILURE);
   }

   /*接收消息*/
   while(running)
   {
      if (msgrcv(msgid, (void *)&some_data, BUFSIZ,msg_to_receive, 0) == -1)
      {
         fprintf(stderr, "msgrcv failed with error: %d/n", errno);
         exit(EXIT_FAILURE);
      }
      printf("You wrote: %s", some_data.some_text);
      if (strncmp(some_data.some_text, "end", 3) == 0)
      {
         running = 0;
      }
   }

   /*删除消息队列*/
   if (msgctl(msgid, IPC_RMID, 0) == -1)
   {
      fprintf(stderr, "msgctl(IPC_RMID) failed/n");
      exit(EXIT_FAILURE);
   }
   exit(EXIT_SUCCESS);
}


/*****************************************************************
 *                        管道通信                               *
 *                                                               *
 * **************************************************************/
void channel()
{
   int pid;
	int fd[2];
	int ret;
	ret = pipe(fd);
	if (ret == -1)
	{
		printf("pipe fail.\n");
		return;
	}
	pid = fork();//实测创建进程必须放到pipe之后，否则读不出来
   
   //管道通信是半双工，同一时间只能一端发，一端收，发的时候将收关闭，收的时候将发关闭
	if (pid == 0)//子进程
	{
		char buf[BUFFER_SIZE];
		close(fd[0]);
      sprintf(buf,"This is an example of pipe!");
      write(fd[1],buf,BUFFER_SIZE);
	}
	else if (pid > 0)//父进程
	{
		char buff[BUFFER_SIZE];
		int len;
		close(fd[1]);
   	len = read(fd[0],buff,BUFFER_SIZE);//读出字符串,read是阻塞的
		if (len > 0)
		{
			printf("read:%s\n", buff);
		}
	}
	else
	{
		printf("fork fail.\n");
	}
}

/*****************************************************************
 *                 低级通信--信号通信                            *
 *                                                               *
 * **************************************************************/
/*捕捉到信号sig之后，执行预先预定的动作函数*/
void sig_alarm(int sig)
{
   printf("---the signal received is %d--- /n", sig);
   //signal(SIGINT, SIG_DFL); //SIGINT终端中断信号，SIG_DFL：恢复默认行为，SIN_IGN：忽略信号
}
void sig_handler()
{
   signal(SIGINT, sig_alarm);//捕捉终端中断信号

	while(1)
	{
		sleep(1);
	}
}



/*****************************************************************
 *                  进程间通信——IPC                              *
 *                          信号量通信                           *
 * **************************************************************/

int createSemSet(int nums)
{
   //key_t key = ftok
   int semid = semget((key_t)123, nums, IPC_CREAT | 0666);  //nums表示创建信号量个数，当不是创建时该值无效
   if (semid < 0)
   {
      perror("semget, error.\n");
      return -1;
   }
   return semid;
}

int initSem(int semid, int nums, int initVal)
{
   union semun _un; 
   _un.val = initVal;
   if (semctl(semid, nums, SETVAL, _un) < 0)       //nums为信号量集的下标，0表示第一个
   {
      perror("semctl error.\n");
      return -1;
   }
   return 0;
}

static int commPV(int semid, int who, int op)
{
   struct sembuf _sf;
   _sf.sem_num = who;   //信号量编号，下标，同semctl的nums
   _sf.sem_op = op;
   _sf.sem_flg = 0;
   if (semop(semid, &_sf, 1) < 0)      //
   {
      perror("semop error.\n");
      return -1;
   }
   return 0;
}

int P(int semid, int who)
{
   return commPV(semid, who, -1);
}

int V(int semid, int who)
{
   return commPV(semid, who, 1);
}

int destroySemSet(int semid)
{
   if (semctl(semid, 0, IPC_RMID) < 0)
   {
      perror("semctl error.\n");
      return -1;
   }
   return 0;
}

int sem_PV_handler()
{
   int semid;
   int pid;
   semid = createSemSet(1);
   initSem(semid, 0, 1);
   pid = fork();
   int val = 1;
   if (pid < 0)
   {
      perror("fork error.\n");
      return -1;
   }
   else if (pid == 0)
   {
      int semid_child = createSemSet(0);
      while (1)
      {
         P(semid_child, 0);

         printf("A");
         fflush(stdout);
         usleep(10000);
         printf("A");
         fflush(stdout);
         usleep(30000);

         V(semid_child, 0);
      }
      
   }
   else
   {
      while (1)
      {
         P(semid, 0);

         printf("B");
         fflush(stdout);
         usleep(20000);
         printf("B");
         fflush(stdout);
         usleep(40000);

         V(semid, 0);
      }
      
   }
   return 0;
}

//////////////////////////////////////////////////////////////////////////

st_sub_proc s_subproclist[MAX_SUB_PROC_NUM] = {0};  //多进程间是数据的拷贝，从fork后数据拷贝不共享

//向子进程注册处理函数
int sub_proc_func_register(pid_t pid, void* func, void* data) 
{
    

    
}

//
void timer_test(unsigned long data)
{
    sleep(data);
    printf("pid:%d, %s, exit.\n", getpid(), __FUNCTION__);
    exit(0);
}


//进程处理执行函数
void sub_proc_exec_func(int i)
{
    //子进程状态注册，并执行
    //后续需要加锁，否则有冲突
    s_subproclist[i].valid = TRUE;
    s_subproclist[i].pid = getpid();
    s_subproclist[i].func = NULL;
    s_subproclist[i].data = 0;

    switch(i)
    {
        case SUB_PROC_0:
        {
            s_subproclist[i].func = timer_test;
            s_subproclist[i].data = 5;
        }
        break;
        case SUB_PROC_1:
        {
            s_subproclist[i].func = timer_test;
            s_subproclist[i].data = 10;
        }
        break;
        case SUB_PROC_2:
        {
            s_subproclist[i].func = timer_test;
            s_subproclist[i].data = 15;
        }
        break;
        case SUB_PROC_3:
        {
            s_subproclist[i].func = timer_test;
            s_subproclist[i].data = 20;
        }
        break;
        case SUB_PROC_4:
        {
            s_subproclist[i].func = timer_test;
            s_subproclist[i].data = 25;
        }
        break;
        default:
        {

        }
    }
    printf("%d, sub process pid:%d, ppid:%d.\n", i, s_subproclist[i].pid, getppid());
    while (1)
    {
        if (s_subproclist[i].func == NULL)
        {
            sleep(1);
        }
        else
        {
            s_subproclist[i].func(s_subproclist[i].data);
        }
    }
}

//初始化启动5个进程
void proc_register_init()
{
    int i;
    pid_t pid;


    for (i = 0; i < SUB_PROC_NUM; i++)
    {
        pid = fork();
        //子进程break，避免子进程创建新的子进程
        if (pid == 0)
        {
            break;
        }
        else if (pid < 0)
        {
            printf("parent process create pid fail.\n");
        }
        else if (pid > 0)
        {
//            printf("parent process create new pid:%d.\n", pid);
        }
    }

    if (i == SUB_PROC_NUM)
    {
        printf("parent process pid:%d.\n", getpid());
        return;
    }

    
    sub_proc_exec_func(i);
}



void proc_wait_handle()
{
    int status;
    

    
//    kill(s_subproclist[0].pid,SIGKILL);

    while(-1 != wait(NULL))
    {

    }
    printf("parent exit.\n");
    exit(0);
//    waitpid(s_subproclist[0].pid,&status,WNOHANG);
}
















//////////////////////////////////////////////////////////////////////////



