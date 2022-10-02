#include <iostream>
#include <string.h>

#include "Library.spec.h"

#include "../Source/Utils.h"

using namespace std;
using namespace std::literals::string_literals;

void printline() {
	for (int i = 0; i < 50; i++) {
		cout << "-";
	}
	cout << endl;
}

void eval_result(int result, string name) {
	if (result == 1) {
		cerr << "Test '" << name << "' failed" << endl;
	} else if (result == 0) {
		cout << "Test '" << name << "' passed" << endl;
	}
}

int main() {
	cout << endl << endl << "Execute Test for Musicus functionality" << endl << endl;
	printline();
	cout << "Run Library specs" << endl;
	eval_result( runSpecs_Library(), "Library");
	cout << endl;
}
