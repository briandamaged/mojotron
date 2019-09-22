#include <stdio.h>

int main(int argc, char** argv) {
	FILE* f = NULL;
	char buffer[80];

	f = fopen("demo", "r");
	printf(fgets(buffer, 80, f));

	if (f)	fclose(f);
	f = NULL;
}
