#ifndef OBJ_FILE_H
#define OBJ_FILE_H
#include "stabs.h"
#include "a.out.h"

struct symbol_entry{
	int word1;
	int word2;
	int word3;
};

//代表可执行文件的类
struct objfile{
	public:
	objfile(const std::string& file) {
			unsigned int args[8];		
			buffer.resize(HEADER_LENGTH);
			std::fstream filestream(file,std::ios::binary|std::ios::in);
			filestream.read(&buffer[0],HEADER_LENGTH);
			
			//读取文件头,四个字节为单位
			unsigned int* int_ptr = reinterpret_cast<unsigned int*> (&buffer[0]);
			for (int i = 0; i < 8; ++i,++int_ptr){
				args[i] = *int_ptr;
			}
			//头四个字节是"魔数",就是二进制文件的类型
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
			
			//初始化struct exec, 过后用来在文件里找东西
			e = {args[0],args[1],args[2],args[3],args[4],args[5],args[6],args[7]};
			
			//把整个文件读到一个数组,以后用它就行了
			filestream.seekg(0,std::ios_base::end);
			size_t filesize = filestream.tellg();
			filestream.seekg(HEADER_LENGTH);
			buffer.resize(filesize);
			filestream.read(&buffer[HEADER_LENGTH],filesize);
			
			//从刚才构造的数组里面,到符号表位置读取调试信息
			//调试信息从N_SYMOFF(e)开始, 12字节一个
			int bytesRead = 0;
			int currFile = -1;
			while (bytesRead < e.a_syms){
				//检查第5个字节(该条调制信息的类型)是不是认识的
				if (stabs::table.count((unsigned char)buffer[N_SYMOFF(e) + bytesRead + 4])){
					//是的话就记录下来
					legal_symbols.push_back({
						*reinterpret_cast<int*>(&buffer[0] + N_SYMOFF(e) + bytesRead),
						*reinterpret_cast<int*>(&buffer[0] + N_SYMOFF(e) + bytesRead + 4),
						*reinterpret_cast<int*>(&buffer[0] + N_SYMOFF(e) + bytesRead + 8)});
					lgl_sym_list[ (unsigned char)buffer[N_SYMOFF(e) + bytesRead + 4] ].push_back({
						*reinterpret_cast<int*>(&buffer[0] + N_SYMOFF(e) + bytesRead),
						*reinterpret_cast<int*>(&buffer[0] + N_SYMOFF(e) + bytesRead + 4),
						*reinterpret_cast<int*>(&buffer[0] + N_SYMOFF(e) + bytesRead + 8)});
						
					//第5个字节是0x44的话,就代表
					//包含的是行号对应代码段地址的信息
					//在两个表里面记录下来
					if ((unsigned char)buffer[N_SYMOFF(e) + bytesRead + 4] == 0x44){
						line_to_addr_table[currFile][
							*reinterpret_cast<short*>(&buffer[0] + N_SYMOFF(e) + bytesRead + 6)
						] = *reinterpret_cast<int*>(&buffer[0] + N_SYMOFF(e) + bytesRead + 8);
						addr_to_line_table[currFile][
							*reinterpret_cast<int*>(&buffer[0] + N_SYMOFF(e) + bytesRead + 8)
						] = *reinterpret_cast<short*>(&buffer[0] + N_SYMOFF(e) + bytesRead + 6);
					//第5个字节是0x64的话,就代表
					//包含的文件名对应代码段地址的信息
					}else if ((unsigned char)buffer[N_SYMOFF(e) + bytesRead + 4] == 0x64){		
						currFile ++;
						addr_to_file_table[
							*reinterpret_cast<short*>(&buffer[0] + N_SYMOFF(e) + bytesRead + 8)] = 
						currFile;
						line_to_addr_table.resize(currFile + 1);
						addr_to_line_table.resize(currFile + 1);
						
						//文件名称在字符表里面
						//文件名调试信息的头四个字节是
						//文件名相对字符表开头的地址
						//字符表开头的地址偏移用N_STROFF(e)计算
						int line_addr = N_STROFF(e) + *reinterpret_cast<int*>(&buffer[0] + N_SYMOFF(e) + bytesRead);
						std::string source_file(&buffer[0] + line_addr);
						filenum_to_name.push_back(source_file);
						filename_to_num[source_file] = currFile;
					} 
					//如果将来要扩充功能,就再加"else if"段落
					//比如如果等于192(0xC0 左圆括号), 224(0xE0 右圆括号)
					//128(0x80 typedef,本地变量)等等
					
				}
				//读下12个字节
				bytesRead += 12;
			}
			filestream.close();
	}//end of constructor
	
	unsigned char byteAt(int address){
		return (unsigned char) buffer[address];
	}
	
	//代码段地址到文件号转换
	int a_to_f (int addr) const{

		if (addr_to_file_table.count (addr)){
			return addr_to_file_table [addr];
		} else{
			auto itr = addr_to_file_table.begin();
			for (; (*itr).first < addr && itr != addr_to_file_table.end();
				 ++itr)
				 ;
		 	if (itr != addr_to_file_table.begin())--itr;
			return itr->second;
		}
	}
	
	//文件名到文件号转换
	int f_name_to_num(const std::string& name) const{
		if (filename_to_num.count(name)){
			return filename_to_num[name];
		}else{
			return -1;
		}
	} 
	
	//(文件号+行号)到代码段地址转换
	int l_to_a(short line, int file) const throw (std::string){
		if (file >= line_to_addr_table.size()){
			throw std::string("文件号超出范围.注意第一个文件号为0");
		}
		if (line_to_addr_table[file].count (line)){
			return line_to_addr_table[file][line];
		}else{
			return -1;
		}
	}
	
	//代码段地址到行号转换
	int a_to_l(int addr,int fileNum) const{
		if (addr_to_line_table[fileNum].count (addr)){
			
			return addr_to_line_table[fileNum] [addr];
		} else{
			auto itr = addr_to_line_table[fileNum].begin();
			while((*itr).first < addr && itr != addr_to_line_table[fileNum].end()){
				 ++itr;
			}
			if (itr != addr_to_line_table[fileNum].begin()) --itr;
			return itr->second;
		}
	
	}
	
	//文件号到文件名转换
	std::string f_to_n(int filenum) const throw (std::string) {
		if (filenum >= filenum_to_name.size()){
			throw std::string("文件号超出范围.注意第一个文件号为0");
		}
		return filenum_to_name[filenum];
	}
	
	//返回所有调试信息
	std::vector<symbol_entry>& list_symbols(unsigned char type) const throw (std::string){
		if (lgl_sym_list.count(type)){
			return lgl_sym_list[type];
		}else{
			throw std::string("No symbol found");
		}
	}
	

	private:
	std::vector<symbol_entry> legal_symbols;
	mutable std::unordered_map<unsigned char,std::vector<symbol_entry>> lgl_sym_list;
	mutable std::vector<std::unordered_map<short,int>> line_to_addr_table;
	mutable std::vector<std::map<int,short>> addr_to_line_table;
	mutable std::map<int,int> addr_to_file_table;
	std::vector<char> buffer;
	std::vector<std::string> filenum_to_name;
	mutable std::unordered_map<std::string,int> filename_to_num;
	struct exec e;
};


#endif //ifndef obj_file_h