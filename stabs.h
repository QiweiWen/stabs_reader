#ifndef STAB_H
#define STAB_H
#include "console.h"
#define STAB_ENTR_LEN 12
#define BUF_SIZE 1024
#define HEADER_LENGTH 32

namespace stabs{
	//debug symbol types we're interested in
	std::unordered_map<int,bool> table (
	{{100,1},{128,1},{196,1},{68,1},{36,1},{224,1},{32,1}});
};

#endif //ifndef stab_h