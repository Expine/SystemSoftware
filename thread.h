
typedef unsigned int uint;

typedef struct mythread
{
	uint edi;
    uint esi;
    uint ebx;
    uint ebp;
    uint eip;

} *mythread_t;

#define STACK_SIZE 4096
#define STACK_DEPTH (STACK_SIZE / sizeof(uint))
#define MAX_THREAD_SIZE 100

#define THREAD_STATE_ALIVE 0
#define THREAD_STATE_SLEEP 1
#define THREAD_STATE_DEAD  2

mythread_t new_thread(void (*fun)(int), int arg);
void start_threads();
void yield();
void th_exit();
