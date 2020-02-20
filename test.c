#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

int main() {

	char s[100] = "asg/img/asd.jp";
	char *now = s;
	char *end = s + strlen(s);
	while (now + 3 < end) {
		if (now[0] == '/' && now[1] == 'i' &&
		    now[2] == 'm' && now[3] == 'g') {
				printf("pos: %d", (int)(now-s));
				break;
			}
		now++;
	}

	return 0;
}
