// The MIT License (MIT)

// Copyright (c) 2015 nabijaczleweli

//  Permission is hereby granted, free of charge, to any person obtaining a
//  copy of this software and associated documentation files (the "Software"),
//  to deal in the Software without restriction, including without limitation
//  the rights to use, copy, modify, merge, publish, distribute, sublicense,
//  and/or sell copies of the Software, and to permit persons to whom the
//  Software is furnished to do so, subject to the following conditions:
//
//  The above copyright notice and this permission notice shall be included in
//  all copies or substantial portions of the Software.
//
//  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
//  OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
//  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
//  FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
//  DEALINGS IN THE SOFTWARE.


#include "util/string.hpp"
#include "util/scope.hpp"
#include "util/file.hpp"
#include "settings.hpp"
#include "sysprops.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <cstdio>


using namespace std;


int main(int argc, const char ** argv) {
	settings_t settings = load_settings(argc, argv);

	if(settings.verbose) {
		clog << "v: GNU make command:            \"" << settings.make_command << "\"\n";
		clog << "v: GNU make explicit arguments: \"" << settings.make_arguments << "\"\n";
		clog << "v: temporary directory:         \"" << settings.temporary_directory << "\"\n";
		clog << "v: preprocessed Makefile:       \"" << settings.make_file << "\"\n";
	}


	string tempfile_path = settings.temporary_directory + "/platformake_" + tempname();
	quickscope_wrapper tempfile_delete{[&]() {
		if(settings.delete_tempfile)
			if(remove(tempfile_path.c_str()))
				cerr << "\nDeleting temporary file \"" << tempfile_path << "\" failed.\n";
	}};


	if(settings.verbose)
		clog << "v: temporary Makefile path:     \"" << tempfile_path << "\"\n\n";

	/*istringstream iss("all:\n"
	                  "\techo git gud %EXEC");*/
	ifstream iss(settings.make_file);

	ofstream file(tempfile_path);
	for(string line; getline(iss, line);) {
		if(!line.empty()) {
			replaceAll(line, "%EXEC"s, ".exe"s);
			file << line << '\n';
		}
	}

	file.flush();
	int retval = system((settings.make_command + ' ' + settings.make_arguments + " -f \"" + tempfile_path + '"').c_str());

	return retval;
}
