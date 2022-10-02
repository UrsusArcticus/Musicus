#include "Library.spec.h"

//#include <string.h>

#include "../Source/Library.h"

using namespace musicus;
using namespace std::literals::string_literals;

int it_LoadLibrary();
int it_InitializeLibrary();

int runSpecs_Library() {
	int res;
	res = it_LoadLibrary();
	if ( res ) {
		return 1;
	}
	res = it_InitializeLibrary();
	if ( res ) {
		return 1;
	}
	return 0;
}

int it_LoadLibrary() {
	Library* lib = new Library();
	lib->loadLibrary("/home/philipp/Music"s);
	return 0;
}

int it_InitializeLibrary() {
	Library* lib = new Library();
	lib->initializeNewLibrary("/home/philipp/Music"s);
	return 0;
}
