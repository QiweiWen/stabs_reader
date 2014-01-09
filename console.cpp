#include "objfile.h"

void test_endianness(void){
	short test = 0x1234;
	char* test_char = (char*)&test;
	if (*test_char != 0x34){
		fprintf(stderr,"Doesn't support big-endian systems.\n");
		exit(0);
	}
}

void cmd_ln_loop(const objfile& file){
	char cmd[51] = {0};
	int quit = 0;
	int length;
	while (1){
		std::cout<<"<-";
		std::cin.getline(cmd,50);
		length = strlen(cmd);
		if (!cmd[0]){
			continue;
		}
		std::string opr = strtok(cmd," ");
		if (opr.length() == length) {
			continue;
		}
		std::string operand;
		if (opr == "la"){
			std::string operand1 = strtok(NULL," ");
			if (operand1.length() == length - opr.length()) continue;
			std::string operand2 = strtok(NULL," ");
			short line = (short)atoi(operand1.c_str());
			int filename = atoi(operand2.c_str());
			try{
				int addr = file.l_to_a(line,filename);
				printf("%d\n",addr);
			}catch (std::string s){
				std::cout<<s<<std::endl;
			}			
		}else if (opr == "al"){
			operand = strtok(NULL," ");
			int line = (short)atoi(operand.c_str());
			printf("line %d of %s\n",file.a_to_l(line),(file.f_to_n(file.a_to_f(line))).c_str());	
		}
		if (quit) break;
		Sleep(50);
	}
}

int main (int argc, char** argv){
/*
	if (argc < 2){
		fprintf(stderr,"look, I came here for an argument!\n");
		exit(0);
	}
	*/
	test_endianness();
	const std::string filename("hello_new");
	objfile o(filename);
	cmd_ln_loop(o);
	return 0;
}