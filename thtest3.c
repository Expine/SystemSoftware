/*
 * wait, notify, notify_allの検査
 */

#include <stdio.h>
#include "thread.h"
int th_tmp1 = 0;
mythread_t th_baz;

void foo(int c) {
	while (1) {
		wait_thread(&th_tmp1);
		th_tmp1 += c;
		printf("foo : %d\n", th_tmp1);
	}
}

void bar(int c) {
	while(1) {
		th_tmp1 += c;
		printf("bar : %d\n", th_tmp1);
		yield();
		th_tmp1 += c;
		printf("bar : %d\n", th_tmp1);
		notify(th_baz, &th_tmp1);
	}
}

void baz(int c) {
	while (1) {
		wait_thread(&th_tmp1);
		th_tmp1 += c;
		printf("baz : %d\n", th_tmp1);
		notify_all(&th_tmp1);
	}
}

int main() {
	new_thread(foo, 1);
	new_thread(bar, 10);
	th_baz = new_thread(baz, 100);
	start_threads();
	return 0;
}
