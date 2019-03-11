#include <unistd.h>
#include <stdio.h>

extern int hi_rust(int);

void testprint(int code) {
	unsigned int sleep_time = 1;
	printf("testprint: Someone called me with code %d\n\n", code);
    sleep(sleep_time);
}

void main(void)
{
	int i = 0;
	int return_value = 0; 
	unsigned int sleep_time = 1;

	while (++i) {
		printf("\nHello world (%d)\nLet's call rust!\n", i);
		sleep(sleep_time);
		return_value = hi_rust(i);
	    sleep(sleep_time);
		printf("hi_rust returned (%d)!\n", return_value);
	}
}
