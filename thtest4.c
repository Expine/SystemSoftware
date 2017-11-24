/*
 * プリエンティブスケジュールのテスト
 */

#include <stdio.h>
#include "thread.h"

void foo(int c) {
	while (1) {
		printf("foo : %d\n", c);
		c += 1;
		if(isAuto())
			yield();
	}
}

void bar(int c) {
	while (1) {
		printf("bar : %d\n", c);
		c += 2;
		if(isAuto())
			yield();
	}
}

void baz(int c) {
	while (1) {
		printf("baz : %d\n", c);
		c += 3;
		printf("baz : %d\n", c);
		c += 3;
		if(isAuto())
			yield();
	}
}

int main() {
	new_thread(foo, 10);
	new_thread(bar, 0);
	new_thread(baz, 0);
	start_preemptive_threads();
	return 0;
}
