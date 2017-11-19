#include <stdio.h>
#include <stdlib.h>
#include "thread.h"
#include "swtch.h"

#define push(s, v) (*--(s)=(v))

mythread_t running;
mythread_t dummy;
mythread_t thread_queue[MAX_THREAD_SIZE];
uint queue_head = 0;
uint queue_tail = 0;

/*
 * スレッドをキューに入れる
 */
void enqueue(mythread_t thread) {
	thread_queue[queue_tail] = thread;
	queue_tail = (queue_tail + 1) % MAX_THREAD_SIZE;
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
	sp += STACK_DEPTH;
	// 引数を置く
    push(sp, arg);
    push(sp, 0);
	// スレッド構造体を置き、関数を設置する
    sp -= sizeof(struct mythread) / sizeof(uint);
    mythread_t thread = (mythread_t)sp;
    thread->eip = (uint)fun;
    // スレッドを格納
    enqueue(thread);
    return thread;
}

/*
 * スレッドを開始する
 */
void start_threads() {
	yield();
}

/*
 * スレッドを切り替える
 */
void yield() {
	// 旧スレッドを取得して、キューに突っ込む
	mythread_t old = running;
	enqueue(old);
	// 新スレッドを取得して、実行スレッドにする
	mythread_t new = dequeue();
	running = new;
	swtch(&old, new);
}

/*
 * スレッドを終了させる
 */
void th_exit() {
	free((uint *)running);
	running = dummy;
	yield();
}