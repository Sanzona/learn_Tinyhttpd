#include <sys/stat.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
int main() {

	int fd = open("txt", O_RDWR | O_TRUNC);
	dup2(fd, 1);

	char buf[1024*1024] = {0};

	FILE *fp = fopen("ccc.png", "rb");
	fseek(fp, 0, SEEK_END);
	int len = ftell(fp);
	fseek(fp, 0, SEEK_SET);

	len = fread(buf, 1, len, fp);
	printf("len = %d\n", len);
	int i;
	for (i = 0; i < len; ++i) {
		printf("i: %d, %02X\n", i, (unsigned char)buf[i]);
		continue;
		/*
		unsigned char num = buf[i];
		printf("i = %d, ", (int)num);
		printf("%X", num >> 4);
		printf("%X",(num & 0x0F));
		puts("");
		*/
	}
	fclose(fp);

	return 0;
}
