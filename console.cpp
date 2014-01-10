#include "objfile.h"

void test_endianness(void){
	short test = 0x1234;
	char* test_char = (char*)&test;
	if (*test_char != 0x34){
		fprintf(stderr,"Doesn't support big-endian systems.\n");
		exit(0);
	}
}

std::fstream& GotoLine(std::fstream& file, unsigned int num){
    file.seekg(std::ios::beg);
    for(int i=0; i < num - 1; ++i){
        file.ignore(std::numeric_limits<std::streamsize>::max(),'\n');
    }
    return file;
}

std::string getLineFromFile(const std::string& fn, int lineNum){
	std::fstream file (fn);
	GotoLine(file, lineNum);
	std::string line;
	std::getline(file, line);
	file.close();
	return line;
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
			int addr = (short)atoi(operand.c_str());
			int line = file.a_to_l(addr);
			int filenum = file.a_to_f(addr);
			std::string filename(file.f_to_n(filenum));
			printf("line %d of %s\n",line,filename.c_str());
			std::cout<<	getLineFromFile(filename, line)<<std::endl;
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