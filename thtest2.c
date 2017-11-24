#include <stdio.h>
#include "thread.h"

void foo(int c) {
	while (1) {
		printf("foo : %d\n", c);
		c += 1;
		if(c >= 60000)
			th_exit();
		yield();
	}
}

void bar(int c) {
	printf("bar : %d\n", c);
	c += 2;
}

void baz(int c) {
	while (1) {
		printf("baz : %d\n", c);
		yield();
		c += 3;
		printf("baz : %d\n", c);
		yield();
		c += 3;
	}
}

int main() {
	new_thread(foo, 0);
	new_thread(bar, 0);
	new_thread(baz, 0);
	start_threads();
	return 0;
}
