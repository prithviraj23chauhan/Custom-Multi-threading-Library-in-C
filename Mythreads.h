#ifndef MYTHREAD_H
#define MYTHREAD_H 

#include<stdio.h>
#include<stdlib.h>
#include<signal.h>
#include<string.h>
#include<assert.h>
#include<setjmp.h>
#include<ctype.h>
#include<time.h>
#include<unistd.h>


#define INTERVAL_uSEC 200000   // time slot for each thread in millisecond

#define MAXTHREADS 10          // max no of threads

#define STACK_SIZE 4096        // size for local stack of a thread in bytes

typedef void *(*f_WithArgs)(void *);
enum STATES{DED=0,RUN,RED,FIN,SUS, NEW};
enum BOOLEAN {FALSE=0,TRUE};
enum TYPE {ARG = 0, NOARG};

typedef struct tcb
{
	jmp_buf env;             // environment variable
    int ID;                  // ID no from 0 to MAXTHREADS
	enum STATES state;       // current state of thread
	enum BOOLEAN lock;       // indication if a thread is locked or not 
	enum BOOLEAN complete;   // to check if a thread routine is finshed or not
	enum TYPE tType;         // thread type determining arg/noarg

	char stack[STACK_SIZE];  // the thread stack

	void (*f1)(void);        // thread routine no arg type
	f_WithArgs f2;           // thread routine arg type
	void* args;              // thread args
	void* retVal;            // to temporarily store return value as a void pointer
	
}TCB; //the thread control block


extern jmp_buf start_env; // healps in returning from start function after all thread routines are complelted



extern int currentThread;
extern int nThreads;
extern TCB threadList[MAXTHREADS];
extern int ran;



//////////////////   USABLE FUNCTIONS   //////////////////////////
/* Create a new thread to run a function with no arguments.
*  @param f Pointer to the function to run in the new thread.
*  @return Thread ID on success, -1 on failure.           */
int    create(void (*f)(void));// create a thread without arguments

/* Create a new thread to run a function with a single argument.
 * @param f Pointer to the function to run in the new thread (takes a void* argument).
 * @param arg Argument to pass to the thread function.
 * @return Thread ID on success, -1 on failure.    */
int    createWithArgs(void *(*f)(void *), void *arg);// create a thread with arguments


void   start(void);           //start executing the threads - possibly a master thread???
  
void   suspend(int threadID); // halts the execution of thread until resumed
void   resume(int threadID);  // resumes the execution of thread

void   lock(void);            // locks processor to current thread
void   release(void);         // releases the lock 

void   join(int threadID);    // waits till the exuction of given thread
void*  GetThreadResult(int threadID);// return the retVal from a thread
int    getID(void);           // return RThreadID
void   yield(void);           // giving up processor voluntarily
void   deleteThread(int threadID);// deletes a thread
void   clean(void);           // stops the master thread?


/////////////   INTERNAL WORKING FUNCTIONS   /////////////////////
// void   wrapper();        // function handeler
// void   alarm_handler(int sig);// handels the signal inturupts
// void   dispatch(int);         // the scheduler func


#endif
