#include <stdlib.h>
int gint = 5;
int gint2 = 6;
int foo(void){
	int i = 5;
	if (i == 6){
		i = 7;
	}
	return i;
}

int main(){
	char* arbit = "trivial string";
	foo();
	return 0;
}