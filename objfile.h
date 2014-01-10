#ifndef OBJ_FILE_H
#define OBJ_FILE_H
#include "stabs.h"
#include "a.out.h"

struct symbol_entry{
	int word1;
	int word2;
	int word3;
};

struct objfile{
	public:
	objfile(const std::string& file) {
			unsigned int args[8];		
			buffer.resize(HEADER_LENGTH);
			std::fstream filestream(file,std::ios::binary|std::ios::in);
			filestream.read(&buffer[0],HEADER_LENGTH);
			
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
			e = {args[0],args[1],args[2],args[3],args[4],args[5],args[6],args[7]};
			filestream.seekg(0,std::ios_base::end);
			size_t filesize = filestream.tellg();
			filestream.seekg(HEADER_LENGTH);
			buffer.resize(filesize);
			filestream.read(&buffer[HEADER_LENGTH],filesize);
			
			int bytesRead = 0;
			int currFile = -1;
			while (bytesRead < e.a_syms){
				if (stabs::table.count((unsigned char)buffer[N_SYMOFF(e) + bytesRead + 4])){
					legal_symbols.push_back({
						*reinterpret_cast<int*>(&buffer[0] + N_SYMOFF(e) + bytesRead),
						*reinterpret_cast<int*>(&buffer[0] + N_SYMOFF(e) + bytesRead + 4),
						*reinterpret_cast<int*>(&buffer[0] + N_SYMOFF(e) + bytesRead + 8)});
					lgl_sym_list[ (unsigned char)buffer[N_SYMOFF(e) + bytesRead + 4] ].push_back({
						*reinterpret_cast<int*>(&buffer[0] + N_SYMOFF(e) + bytesRead),
						*reinterpret_cast<int*>(&buffer[0] + N_SYMOFF(e) + bytesRead + 4),
						*reinterpret_cast<int*>(&buffer[0] + N_SYMOFF(e) + bytesRead + 8)});
						
					//construct line tables
					if ((unsigned char)buffer[N_SYMOFF(e) + bytesRead + 4] == 0x44){
						line_to_addr_table[currFile][
							*reinterpret_cast<short*>(&buffer[0] + N_SYMOFF(e) + bytesRead + 6)
						] = *reinterpret_cast<int*>(&buffer[0] + N_SYMOFF(e) + bytesRead + 8);
						addr_to_line_table[currFile][
							*reinterpret_cast<int*>(&buffer[0] + N_SYMOFF(e) + bytesRead + 8)
						] = *reinterpret_cast<short*>(&buffer[0] + N_SYMOFF(e) + bytesRead + 6);
						printf("%d\n",*reinterpret_cast<short*>(&buffer[0] + N_SYMOFF(e) + bytesRead + 6));
					}else if ((unsigned char)buffer[N_SYMOFF(e) + bytesRead + 4] == 0x64){
						currFile ++;
						addr_to_file_table[
							*reinterpret_cast<short*>(&buffer[0] + N_SYMOFF(e) + bytesRead + 8)] = 
						currFile;
						line_to_addr_table.resize(currFile + 1);
						addr_to_line_table.resize(currFile + 1);
						int line_addr = N_STROFF(e) + *reinterpret_cast<int*>(&buffer[0] + N_SYMOFF(e) + bytesRead);
						std::string source_file(&buffer[0] + line_addr);
						filenum_to_name.push_back(source_file);
					}
					
				}
				bytesRead += 12;
			}
			filestream.close();
	}//end of constructor
	
	unsigned char byteAt(int address){
		return (unsigned char) buffer[address];
	}
	
	//address to file
	int a_to_f (int addr) const{

		if (addr_to_file_table.count (addr)){
			return addr_to_file_table [addr];
		} else{
			auto itr = addr_to_file_table.begin();
			for (; (*itr).first < addr && itr != addr_to_file_table.end();
				 ++itr);
			--itr;
			return itr->second;
		}
	}
	
	//convert from line number to offset
	//from start of text section
	int l_to_a(short line, int file) const throw (std::string){
		if (file >= line_to_addr_table.size()){
			throw std::string("There aren't that many files. Note the filenum is 0-indexed");
		}
		if (line_to_addr_table[file].count (line)){
			return line_to_addr_table[file][line];
		}else{
			return -1;
		}
	}
	
	//the other way around.
	int a_to_l(int addr) const{
		int fileNum = a_to_f(addr);
		if (addr_to_line_table[fileNum].count (addr)){
			
			return addr_to_line_table[fileNum] [addr];
		} else{
			auto itr = addr_to_line_table[fileNum].begin();
			for (; (*itr).first < addr && itr != addr_to_line_table[fileNum].end();
				 ++itr);
			--itr;
			return itr->second;
		}
	
	}
	
	//file number to name
	std::string f_to_n(int filenum) const throw (std::string) {
		if (filenum >= filenum_to_name.size()){
			throw std::string("There aren't that many files. Note the filenum is 0-indexed");
		}
		return filenum_to_name[filenum];
	}
	
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
	struct exec e;
};


#endif //ifndef obj_file_h