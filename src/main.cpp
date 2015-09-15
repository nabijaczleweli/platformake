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


#include "util/error.hpp"
#include "util/scope.hpp"
#include "util/file.hpp"
#include "transform.hpp"
#include "settings.hpp"
#include <iostream>
#include <fstream>


using namespace std;


//  Cannot use "--eval=" because older versions (<4) don't support it
int main(int argc, const char ** argv) {
	settings_t settings = load_settings(argc, argv);

	if(settings.verbose)
		clog << "v: GNU make command:            \"" << settings.make_command << "\"\n"
		     << "v: GNU make explicit arguments: \"" << settings.make_arguments << "\"\n"
		     << "v: temporary directory:         \"" << settings.temporary_directory << "\"\n"
		     << "v: Makefile to be preprocessed: \"" << settings.make_file << "\"\n";


	string tempfile_path = settings.temporary_directory + "/platformake_" + tempname();
	quickscope_wrapper tempfile_delete{[&]() {
		if(settings.delete_tempfile)
			if(remove(tempfile_path.c_str()))
				error("Deleting temporary file \"" + tempfile_path + "\" failed", 0, settings);
	}};


	if(settings.verbose)
		clog << "v: temporary Makefile path:     \"" << tempfile_path << "\"\n\n";


	ofstream outfile(tempfile_path);

	if(int errc =
	       ((settings.make_file == "-" || settings.make_file == "--") ? transform_makefile(cin, outfile, ".", settings) : transform_makefile(outfile, settings)))
		return errc;

	if(settings.verbose)
		clog << '\n';

	outfile.flush();
	int retval = system((settings.make_command + " -f \"" + tempfile_path + "\" " + settings.make_arguments).c_str());

	return retval;
}
