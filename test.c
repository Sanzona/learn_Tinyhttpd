#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

int main() {

	// printf("size_t: %d\nint: %d\n", sizeof(size_t), sizeof(int));
	// printf("---\n---\n");
	// printf("---\r---\n");
	// printf("---\r\n---\n");
	// printf("r %d\n", isspace('\r'));
	// printf("n %d\n", isspace('\n'));
	// printf("t %d\n", isspace('\t'));
	// printf("  %d\n", isspace(' '));
	// printf("0 %d\n", isspace('\0'));
	// printf("EOF %d\n", isspace(EOF));

	FILE *fp = fopen("doc.txt", "r");
	char buf[1024];

	if (fp == NULL) {
		printf("fopen error\n");
		return 0;
	}

	fgets(buf, sizeof(buf), fp);
	printf("feof: %d\n", feof(fp));
	while (!feof(fp)) {	
		printf("%s", buf);
		fgets(buf, sizeof(buf), fp);
	}

	fclose(fp);
	return 0;




	// // printf("%d\n", feof(res));
	// // printf("pos:%ld\n", ftell(res));
	// char buf[1024];
	// fgets(buf, sizeof(buf), res);
	// printf("%d\n", feof(res));
	// printf("pos:%ld\n", ftell(res));
	// // printf("%s\n11", buf);
	// while (!feof(res)) {
	// 	printf("%s", buf);
	// 	fgets(buf, sizeof(buf), res);
	// //	printf("pos:%ld\n", ftell(res));
		
	// }

	// puts("------------------------");




	return 0;
}