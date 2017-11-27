
typedef unsigned int uint;

typedef struct mythread
{
	uint edi;
    uint esi;
    uint ebx;
    uint ebp;
    uint eip;

} *mythread_t;

typedef struct condition
{
	uint cond;
	mythread_t thread;
} condition;

#define STACK_SIZE 4096
#define STACK_DEPTH (STACK_SIZE / sizeof(uint))
#define MAX_THREAD_SIZE 100

#define THREAD_STATE_ALIVE 0
#define THREAD_STATE_SLEEP 1
#define THREAD_STATE_DEAD  2

mythread_t new_thread(void (*fun)(int), int arg);
void start_threads();
void start_preemptive_threads();
void yield();
void th_exit();

void wait_thread(void *a);
void notify(mythread_t th, void *a);
void notify_all(void *a);
