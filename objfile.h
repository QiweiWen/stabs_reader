#ifndef OBJ_FILE_H
#define OBJ_FILE_H
#include "stabs.h"
#include "a.out.h"

struct symbol_entry{
	int word1;
	int word2;
	int word3;
};

//�����ִ���ļ�����
struct objfile{
	public:
	objfile(const std::string& file) {
			unsigned int args[8];		
			buffer.resize(HEADER_LENGTH);
			std::fstream filestream(file,std::ios::binary|std::ios::in);
			filestream.read(&buffer[0],HEADER_LENGTH);
			
			//��ȡ�ļ�ͷ,�ĸ��ֽ�Ϊ��λ
			unsigned int* int_ptr = reinterpret_cast<unsigned int*> (&buffer[0]);
			for (int i = 0; i < 8; ++i,++int_ptr){
				args[i] = *int_ptr;
			}
			//ͷ�ĸ��ֽ���"ħ��",���Ƕ������ļ�������
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
			
			//��ʼ��struct exec, �����������ļ����Ҷ���
			e = {args[0],args[1],args[2],args[3],args[4],args[5],args[6],args[7]};
			
			//�������ļ�����һ������,�Ժ�����������
			filestream.seekg(0,std::ios_base::end);
			size_t filesize = filestream.tellg();
			filestream.seekg(HEADER_LENGTH);
			buffer.resize(filesize);
			filestream.read(&buffer[HEADER_LENGTH],filesize);
			
			//�ӸղŹ������������,�����ű�λ�ö�ȡ������Ϣ
			//������Ϣ��N_SYMOFF(e)��ʼ, 12�ֽ�һ��
			int bytesRead = 0;
			int currFile = -1;
			while (bytesRead < e.a_syms){
				//����5���ֽ�(����������Ϣ������)�ǲ�����ʶ��
				if (stabs::table.count((unsigned char)buffer[N_SYMOFF(e) + bytesRead + 4])){
					//�ǵĻ��ͼ�¼����
					legal_symbols.push_back({
						*reinterpret_cast<int*>(&buffer[0] + N_SYMOFF(e) + bytesRead),
						*reinterpret_cast<int*>(&buffer[0] + N_SYMOFF(e) + bytesRead + 4),
						*reinterpret_cast<int*>(&buffer[0] + N_SYMOFF(e) + bytesRead + 8)});
					lgl_sym_list[ (unsigned char)buffer[N_SYMOFF(e) + bytesRead + 4] ].push_back({
						*reinterpret_cast<int*>(&buffer[0] + N_SYMOFF(e) + bytesRead),
						*reinterpret_cast<int*>(&buffer[0] + N_SYMOFF(e) + bytesRead + 4),
						*reinterpret_cast<int*>(&buffer[0] + N_SYMOFF(e) + bytesRead + 8)});
						
					//��5���ֽ���0x44�Ļ�,�ʹ���
					//���������кŶ�Ӧ����ε�ַ����Ϣ
					//�������������¼����
					if ((unsigned char)buffer[N_SYMOFF(e) + bytesRead + 4] == 0x44){
						line_to_addr_table[currFile][
							*reinterpret_cast<short*>(&buffer[0] + N_SYMOFF(e) + bytesRead + 6)
						] = *reinterpret_cast<int*>(&buffer[0] + N_SYMOFF(e) + bytesRead + 8);
						addr_to_line_table[currFile][
							*reinterpret_cast<int*>(&buffer[0] + N_SYMOFF(e) + bytesRead + 8)
						] = *reinterpret_cast<short*>(&buffer[0] + N_SYMOFF(e) + bytesRead + 6);
					//��5���ֽ���0x64�Ļ�,�ʹ���
					//�������ļ�����Ӧ����ε�ַ����Ϣ
					}else if ((unsigned char)buffer[N_SYMOFF(e) + bytesRead + 4] == 0x64){		
						currFile ++;
						addr_to_file_table[
							*reinterpret_cast<short*>(&buffer[0] + N_SYMOFF(e) + bytesRead + 8)] = 
						currFile;
						line_to_addr_table.resize(currFile + 1);
						addr_to_line_table.resize(currFile + 1);
						
						//�ļ��������ַ�������
						//�ļ���������Ϣ��ͷ�ĸ��ֽ���
						//�ļ�������ַ���ͷ�ĵ�ַ
						//�ַ���ͷ�ĵ�ַƫ����N_STROFF(e)����
						int line_addr = N_STROFF(e) + *reinterpret_cast<int*>(&buffer[0] + N_SYMOFF(e) + bytesRead);
						std::string source_file(&buffer[0] + line_addr);
						filenum_to_name.push_back(source_file);
						filename_to_num[source_file] = currFile;
					} 
					//�������Ҫ���书��,���ټ�"else if"����
					//�����������192(0xC0 ��Բ����), 224(0xE0 ��Բ����)
					//128(0x80 typedef,���ر���)�ȵ�
					
				}
				//����12���ֽ�
				bytesRead += 12;
			}
			filestream.close();
	}//end of constructor
	
	unsigned char byteAt(int address){
		return (unsigned char) buffer[address];
	}
	
	//����ε�ַ���ļ���ת��
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
	
	//�ļ������ļ���ת��
	int f_name_to_num(const std::string& name) const{
		if (filename_to_num.count(name)){
			return filename_to_num[name];
		}else{
			return -1;
		}
	} 
	
	//(�ļ���+�к�)������ε�ַת��
	int l_to_a(short line, int file) const throw (std::string){
		if (file >= line_to_addr_table.size()){
			throw std::string("�ļ��ų�����Χ.ע���һ���ļ���Ϊ0");
		}
		if (line_to_addr_table[file].count (line)){
			return line_to_addr_table[file][line];
		}else{
			return -1;
		}
	}
	
	//����ε�ַ���к�ת��
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
	
	//�ļ��ŵ��ļ���ת��
	std::string f_to_n(int filenum) const throw (std::string) {
		if (filenum >= filenum_to_name.size()){
			throw std::string("�ļ��ų�����Χ.ע���һ���ļ���Ϊ0");
		}
		return filenum_to_name[filenum];
	}
	
	//�������е�����Ϣ
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