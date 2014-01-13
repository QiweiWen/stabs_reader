#include "objfile.h"


std::fstream& GotoLine(std::fstream& file, unsigned int num){
    file.seekg(std::ios::beg);
    for(int i=0; i < num - 1; ++i){
        file.ignore(std::numeric_limits<std::streamsize>::max(),'\n');
    }
    return file;
}

//返回文件中某行数的内容
std::string getLineFromFile(const std::string& fn, int lineNum){
	std::fstream file (fn);
	GotoLine(file, lineNum);
	std::string line;
	std::getline(file, line);
	file.close();
	return line;
}

//命令行循环
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
		if (opr == "la"){
			const char* opr1_chp = strtok(NULL," ");
			const char* fn_chp = strtok(NULL, " ");
			if (!(opr1_chp && fn_chp)){
				printf("format: la [line] [file]\n");
				continue;
			}
			std::string operand1(opr1_chp);
			std::string filename(fn_chp);
			short line = (short)atoi(operand1.c_str());
			int filenum = file.f_name_to_num(filename);
			if (filenum == -1){
				printf("no file named %s found\n",filename.c_str());
				continue;
			}			
			try{
				int addr = file.l_to_a(line,filenum);
				printf("%d\n",addr);
			}catch (std::string s){
				std::cout<<s<<std::endl;
			}			
		}else if (opr == "al"){
			const char* addr_chp = strtok(NULL," ");
			if (!addr_chp){
				printf("format: al [addr]\n");
				continue;
			}
			std::string operand (addr_chp);
			int addr = (short)atoi(operand.c_str());
			int filenum = file.a_to_f(addr);
			int line = file.a_to_l(addr,filenum);
			std::string filename(file.f_to_n(filenum));
			printf("line %d of %s\n",line,filename.c_str());
			std::cout<<	getLineFromFile(filename, line)<<std::endl;
		}else if (opr == "q") break;
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
	const std::string filename("hello_new");
	objfile o(filename);
	cmd_ln_loop(o);
	return 0;
}