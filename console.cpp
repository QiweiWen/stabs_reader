#include "console.h"
#define BUF_SIZE 1024
#define HEADER_LENGTH 32
#define MAX(x,y)(x>y? x: y)
void test_endianness(void){
	short test = 0x1234;
	char* test_char = (char*)&test;
	if (*test_char != 0x34){
		fprintf(stderr,"Doesn't support big-endian systems.\n");
		exit(0);
	}
}

void cmd_ln_loop(void){
	char cmd[51] = {0};
	while (1){
		std::cout<<"<-";
		std::cin.getline(cmd,50);
		std::cout<<"typed in "<<cmd<<std::endl;
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
	unsigned int args[8];		
	std::vector <char> buffer(HEADER_LENGTH);
	const std::string filename("hello");
	std::fstream filestream(filename,std::ios::binary|std::ios::in);
	filestream.read(&buffer[0],32);
	
	//read the header word-wise
	unsigned int* int_ptr = reinterpret_cast<unsigned int*> (&buffer[0]);
	for (int i = 0; i < 8; ++i,++int_ptr){
		args[i] = *int_ptr;
	}
	switch (args[0]){
		case (0x10b):{
			args[0] = ZMAGIC;
			break;
		}
		case (0x107):{
			args[0] = OMAGIC;
			break;
		}
		case (0x108):{
			args[0] = NMAGIC;
			break;
		}
		default:break;	
	};
	struct exec e {args[0],args[1],args[2],args[3],args[4],args[5],args[6],args[7]};
	filestream.seekg(0,std::ios_base::end);
	size_t filesize = filestream.tellg();
	filestream.seekg(HEADER_LENGTH);
	buffer.resize(filesize);
	filestream.read(&buffer[HEADER_LENGTH],filesize);
	printf("%x\n",buffer[N_SYMOFF(e)]);
	return 0;
}