#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/time.h>
#include <string.h>
#include "thread.h"
#include "swtch.h"

#define push(s, v) (*--(s)=(v))

// start_threadsで回すためのスレッド
mythread_t 	switcher;
// メモリ解放のための先頭アドレス
uint*		thread_addr[MAX_THREAD_SIZE];
// ユーザに渡したスレッド構造体に対応したキュー(変換コストを避ける)
mythread_t	user_thread_queue[MAX_THREAD_SIZE];
// スレッドのキュー
mythread_t 	thread_queue[MAX_THREAD_SIZE];
// スレッドの状態
uint		thread_state[MAX_THREAD_SIZE];
// SLEEP状態のスレッドのキュー
condition	sleep_thread_queue[MAX_THREAD_SIZE];

// 現在処理中のスレッド
uint queue_head = 0;
// スレッドの最後尾
uint queue_tail = 0;

uint sleep_queue_tail = 0;


void printWaitQueue() {
	uint i = 0;
	for(; i < sleep_queue_tail; ++i) {
		printf("%p,", (void*)(sleep_thread_queue[i].thread));
	}
	printf("\n");
}

/*
 * スレッドを待機列に入れる
 */
void enqueue_wait(mythread_t thread, void* a) {
	condition cond;
	cond.cond = (uint)a;
	cond.thread = thread;
	sleep_thread_queue[sleep_queue_tail++] = cond;
}

/*
 * 待機列のスレッドを取り出し、そのスレッド番号を返す
 * 見つからなかった場合は-1を返す
 */
int dequeue_wait(mythread_t thread, void* a) {
	mythread_t ret = NULL;
	uint i = 0;
	uint cond = (uint)a;
	for(i = 0; i < sleep_queue_tail; ++i) {
		condition c = sleep_thread_queue[i];
		// 該当スレッドであるか、スレッド指定がない場合
		if(thread == NULL || c.thread == thread) {
			// 条件変数が一致する場合
			if(c.cond == cond) {
				ret = c.thread;
			}
		}
	}
	if(ret == NULL)
		return -1;

	// 減じる
	--sleep_queue_tail;
	for(; i < sleep_queue_tail; ++i)
		sleep_thread_queue[i] = sleep_thread_queue[i + 1];

	// retが何番目のスレッドかを確認する
	for(i = 0; i < queue_tail; ++i) {
		if(user_thread_queue[i] == ret) {
			if(thread_state[i] == THREAD_STATE_SLEEP)
				return i;
		}
	}
	return -1;
}


/*
 * スレッドをキューに入れる
 */
void enqueue(mythread_t thread, uint state) {
	thread_queue[queue_tail] = thread;
	user_thread_queue[queue_tail] = thread;
	thread_state[queue_tail] = state;
	queue_tail = (queue_tail + 1) % MAX_THREAD_SIZE;
}

/*
 * 確保したアドレスを記録する
 */
void register_addr(uint* addr) {
	thread_addr[queue_tail] = addr;
}

/*
 * 現在のスレッドを解放する
 */
void release() {
	free(thread_addr[queue_head]);
}

/*
 * 現在のスレッドの状態を取得する
 */
uint seek_state() {
	return thread_state[queue_head];
}

/*
 * 現在のスレッドの状態を変更する
 */
void set_state(uint state) {
	thread_state[queue_head] = state;
}

/*
 * ユーザーに渡したthread構造体に対応したスレッドを返す
 */
mythread_t seek_user_thread() {
	return user_thread_queue[queue_head];
}

/*
 * 現在のスレッドを取得する
 */
mythread_t *seek_thread() {
	return &thread_queue[queue_head];
}

/*
 * 次のスレッドに進める
 */
void next() {
	queue_head = (queue_head + 1) % queue_tail;
}

/*
 * スレッドをキューから出す
 */
mythread_t dequeue() {
	if(queue_head == queue_tail)
		return NULL;
	mythread_t ret = thread_queue[queue_head];
	queue_head = (queue_head + 1) % MAX_THREAD_SIZE;
	return ret;
}

/*
 * スレッドを新規作成する関数
 */
mythread_t new_thread(void (*fun)(int), int arg) {
	// メモリアロケーション
	uint *sp = valloc(STACK_SIZE);
	// 割り当てが失敗したらNULLを返す
	if(sp == NULL)
		return NULL;
	register_addr(sp);
	sp += STACK_DEPTH;
	// 引数を置く
    push(sp, arg);
    // リターンアドレスを置く(スレッドの終了を示す)
    push(sp, (uint)th_exit);
	// スレッド構造体を置き、関数を設置する
    sp -= sizeof(struct mythread) / sizeof(uint);
    mythread_t thread = (mythread_t)sp;
    thread->eip = (uint)fun;
    // スレッドを格納
    enqueue(thread, THREAD_STATE_ALIVE);
    return thread;
}

void printQueue()
{
	int i;
	for(i = 0; i < MAX_THREAD_SIZE; ++i) {
		if(thread_queue[i] == NULL)
			return;
		printf("%p(%p - %d),", (void *)(thread_queue[i]), (void *)(thread_queue[i]->eip), (uint)(thread_state[i]));		
	}
	printf("\n");

}

/*
 * スレッドを開始する
 */
void start_threads() {
	while(1) {
//		printQueue();
		// 生きていないスレッドなら次に回す
		while(seek_state() != THREAD_STATE_ALIVE)
			next();
		swtch(&switcher, *(seek_thread()));
	}
}

void handler() {
	// 旧スレッドを取得
	mythread_t *old = &thread_queue[queue_head];
	// 新スレッドに移行する
	queue_head = (queue_head + 1) % queue_tail;
	swtch(old, switcher);
}

/*
 * プリエンティブスケジュールでスレッドを開始する
 */
void start_preemptive_threads() {
	// シグナル登録
	struct sigaction *act = malloc(sizeof(struct sigaction));
	act->sa_sigaction = handler;
	act->sa_flags = SA_RESTART;	
	if(sigaction(SIGALRM, act, NULL) < 0) {
		// 失敗時はエラー
		perror("sigaction error");
		exit(1);
	}

	// タイマー登録
	struct itimerval timer;
	timer.it_value.tv_sec = 0;
	timer.it_value.tv_usec = 10000;
	timer.it_interval.tv_sec = 0;
	timer.it_interval.tv_usec = 10000;
	if(setitimer(ITIMER_REAL, &timer, NULL) < 0) {
		// 失敗時はエラー
		perror("setitmer error");
		exit(1);
	}

	start_threads();
}

/*
 * スレッドを切り替える
 */
void yield() {
	// 旧スレッドを取得
	mythread_t *old = seek_thread();
	// 新スレッドに移行する
	next();
	swtch(old, switcher);
}

/*
 * スレッドを停止させる
 */
void th_exit() {
	set_state(THREAD_STATE_DEAD);
	release();
	yield();
}

void wait_thread(void *a) {
	set_state(THREAD_STATE_SLEEP);
	enqueue_wait(seek_user_thread(), a);
	yield();

}

void notify(mythread_t th, void *a) {
	int th_num = dequeue_wait(th, a);
	if(th_num == -1)
		exit(1);
	// 存在するなら切り替える
	if(th_num >= 0) {
		mythread_t *old = seek_thread();
		queue_head = th_num;
		set_state(THREAD_STATE_ALIVE);
		swtch(old, switcher);
	}
}

void notify_all(void *a) {
	notify(NULL, a);
}

