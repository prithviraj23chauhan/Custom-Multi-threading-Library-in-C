

#include "Mythreads.h"


///////////////////////////////// ADDRESS MANIPULATION  ////////////////////////////////////////////////



#ifdef __x86_64__
/* code for 64 bit Intel arch */

typedef unsigned long address_t;
#define JB_SP 6
#define JB_PC 7

/* A translation is required when using an address of a variable.
    Use this as a black box in your code. */
address_t translate_address(address_t addr)
{
    address_t ret;
    asm volatile("xor    %%fs:0x30,%0\n"
        "rol    $0x11,%0\n"
                 : "=g" (ret)
                 : "0" (addr));
    return ret;
}

#else
/* code for 32 bit Intel arch */

typedef unsigned int address_t;
#define JB_SP 4
#define JB_PC 5

/* A translation is required when using an address of a variable.
    Use this as a black box in your code. */
address_t translate_address(address_t addr)
{
    address_t ret;
    asm volatile("xor    %%gs:0x18,%0\n"
        "rol    $0x9,%0\n"
                 : "=g" (ret)
                 : "0" (addr));
    return ret;
}

#endif


///////////////////////////////// THREAD MANAGEMENT  ////////////////////////////////////////////////


jmp_buf start_env;
int currentThread = -1;
int nThreads = 0;
TCB threadList[MAXTHREADS];
int ran = 0;


int createWithArgs(void *(*f)(void *), void *args) 
{
    // printf("CREATE2\n");
    int id=-1;
    if(ran==0)    // checking if running for first time
    {
        for(int i = 0; i < MAXTHREADS; i++){  // for all threads 
            threadList[i].ID    = i;          // initializing ID 
	        threadList[i].state = DED;        // setting intial state as dead 
        }
        ran = 1;
    }
	for(int i = 0; i < MAXTHREADS; i++)
 	{
 	    if(threadList[i].state==DED)           // searching for empty thread slots
		{
            id = i;
            break;
        }
	}
    if(id == -1)              // implies all thread slots are full
	{
		fprintf(stderr, "Error Cannot allocate id\n");
		return id;

	}
    assert(id >= 0 && id < MAXTHREADS); // gives error for out of range IDs

    threadList[id].f2       = f;
    threadList[id].args     = args;
	threadList[id].state    = NEW;
	threadList[id].retVal   = NULL;
	threadList[id].complete = FALSE;
	threadList[id].lock     = FALSE;
	nThreads++;
	return  id;
}


int create(void (*f)(void))
{
    // printf("CREATE\n");
	int id=-1;
	if(ran == 0)      // checking if running for first time
	{
	    for(int i = 0; i < MAXTHREADS; i++){  // for all threads 
            threadList[i].ID    = i;          // initializing ID 
	        threadList[i].state = DED;        // setting intial state as dead
        }
        ran = 1;
	}
	for(int i = 0; i < MAXTHREADS; i++)
 	{
 	    if(threadList[i].state==DED)           // searching for empty thread slots
			{
			    id = i;
			    break;
			}
	}

	if(id==-1){              // implies all thread slots are full
		fprintf(stderr, "Error Cannot allocate id");
		return id;
	}
	assert(id >= 0 && id < MAXTHREADS);  // gives error for out of range IDs

	threadList[id].tType    = NOARG;
	threadList[id].f1       = f;
	threadList[id].state    = NEW;
	threadList[id].retVal   = NULL;
	threadList[id].complete = FALSE;
	threadList[id].lock     = FALSE;

	nThreads++;
	return  id;
}


static void dispatch(int sig);
static void alarm_handler(int sig)
{
    // recives signal from alarm and inturupts the curernt procees and call thread schedular 

    // printf("ALARM HANDLER\n");
	signal(SIGALRM, alarm_handler);   
	dispatch(SIGALRM);
}


static void dispatch(int sig)    
{
    // searches for next thread routine to perform
    // and return to main thread if all thread routines are complete

    // printf("DISPATCH %d\n",currentThread);

    int count = 0;
    signal(SIGALRM,SIG_IGN);  // ignores alarm signal while dispact is performing

    if(currentThread != -1){

        if(threadList[currentThread].lock) return;   // implementation of lock()

        if( threadList[currentThread].state != DED){

            int ret_val = sigsetjmp(threadList[currentThread].env,1); // setting env variable
            //ret_val == 0 when setting
            //ret_val == 0 when returning from siglongjmp
            if (ret_val == 1) {
                return;
            }

            threadList[currentThread].state = RED;
        }
    }
    for(int i = (currentThread+1)%MAXTHREADS; count <= MAXTHREADS; i = (i+1)%MAXTHREADS){

        if(threadList[i].state == RED ){    // looking for next executable thread
            currentThread = i;              // setting next executable thread as current thread 
            break;
        }
        else if(threadList[i].state == DED) // avoiding suspended threads
            count++;
    }
    if(count >= MAXTHREADS){   // all threads are dead
        printf("ALL THREADS TERMINATED\n");
        siglongjmp(start_env,1);
        //exit(0);
    }

    threadList[currentThread].state = RUN;
    signal(SIGALRM, alarm_handler);
    //  ualarm(INTERVAL_uSEC,INTERVAL_uSEC);
    siglongjmp(threadList[currentThread].env,1);  // executing new current thread
}

static void wrapper()     // function handeler
{
    // printf("WRAPPER\n");

    if(threadList[currentThread].tType == ARG)
    {
        threadList[currentThread].retVal = threadList[currentThread].f2(threadList[currentThread].args);
    }
    else
    {
        threadList[currentThread].f1();
    }
    threadList[currentThread].complete = TRUE;
    // printf("Thread %d exited\n",getID());
    deleteThread(currentThread);
    signal(SIGALRM,alarm_handler);
    // ualarm(INTERVAL_uSEC,INTERVAL_uSEC);
    dispatch(SIGALRM);
}


void join(int threadID)//bonus
{
    if( (threadID < 0 && threadID > MAXTHREADS) || threadID ==  currentThread ) 
        return ;

    int waitingThread = currentThread;

    suspend( waitingThread );    // suspending current thread 
    // and waitng until thread we want to join is finshed 
    while( threadList[ threadID].state!=DED && threadList[ threadID].complete == FALSE)
        usleep(INTERVAL_uSEC);
        
    resume( waitingThread );      // resuming waiting thread
}

void* GetThreadResult(int threadID)
{
    
    if( (threadID < 0 && threadID > MAXTHREADS) || threadID ==  currentThread )
    return NULL;

    void * result = NULL;
    int waitingThread = currentThread;

    suspend( waitingThread );   // suspending current thread 
    // and waitng until result we want is being processed 
    while( threadList[ threadID].state!=DED && threadList[ threadID].complete == FALSE)
        usleep(INTERVAL_uSEC);

    result = threadList[ threadID].retVal;
    resume( waitingThread );      // resuming waiting thread
    return result;
}

void yield(){
  dispatch(SIGALRM);    // giving up processor and executing next routine
}

void lock(void) {
    threadList[currentThread].lock = TRUE;  // locking current thread
}

void release(void) {
    threadList[currentThread].lock = FALSE; // releasing lock 
    dispatch(1);
}


void deleteThread(int threadID)
{
    nThreads --;

    threadList[threadID].state = DED;
    // printf("Thread with %d id is deleted\n",threadID);
}

int getID()
{
	return currentThread;
}

void run(int tid)
{
	threadList[tid].state = RED;
}

void suspend(int tid)
{
	threadList[tid].state = SUS;
}

void resume(int tid)
{
	threadList[tid].state = RED;
}


void clean()
{
    exit(0);
}

void start(void)
{
    // printf("START\n");
    if(sigsetjmp(start_env,1) == 0){
        address_t sp, pc;
    for(int i = 0; i < MAXTHREADS; i++)
    {
            // printf("%d ",currentThread);

        sp = (address_t)threadList[i].stack + STACK_SIZE - sizeof(address_t);
        pc = (address_t)wrapper;
        sigsetjmp(threadList[i].env, 1);
        (threadList[i].env->__jmpbuf)[JB_SP] = translate_address(sp);
        (threadList[i].env->__jmpbuf)[JB_PC] = translate_address(pc);
        sigemptyset(&(threadList[i].env)->__saved_mask);
        if(threadList[i].state == NEW)   threadList[i].state = RED;
    }
    signal(SIGALRM,alarm_handler);
    ualarm(INTERVAL_uSEC,INTERVAL_uSEC);
    
    while(1) usleep(INTERVAL_uSEC);
        
    }

}
