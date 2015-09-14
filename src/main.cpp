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


#include "util/scope.hpp"
#include "util/file.hpp"
#include "util/line.hpp"
#include "settings.hpp"
#include "sysprops.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <cstdio>
#include <regex>


using namespace std;


int transform_makefile(istream & from, ostream & to, const string & relative_directory, const settings_t & settings);
int error(const string & msg, int retc, const settings_t & settings);


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
				cerr << "\nDeleting temporary file \"" << tempfile_path << "\" failed.\n";
	}};


	if(settings.verbose)
		clog << "v: temporary Makefile path:     \"" << tempfile_path << "\"\n\n";


	ofstream outfile(tempfile_path);

	if(settings.make_file == "-")
		transform_makefile(cin, outfile, ".", settings);
	else {
		ifstream infile(settings.make_file);
		if(!infile)
			return error(settings.make_file, 2, settings);

		if(int errc = transform_makefile(infile, outfile, path_nolastnode(settings.make_file), settings))
			return errc;
	}

	if(settings.verbose)
		clog << '\n';

	outfile.flush();
	int retval = system((settings.make_command + " -f \"" + tempfile_path + "\" " + settings.make_arguments).c_str());

	return retval;
}


int transform_makefile(istream & from, ostream & to, const string & relative_directory, const settings_t & settings) {
	static const regex include_r("include([[:space:]]*((?:[^[:space:]]+[[:space:]]?)*))?", regex_constants::optimize);

	smatch match;
	for(string line; getline(from, line);) {
		strip_line(line);

		if(regex_match(line, match, include_r)) {
			const auto & includepaths = match.str(2);
			if(includepaths.empty()) {
				if(settings.verbose)
					clog << "v: empty include directive, skipping\n";
			} else {
				istringstream files(includepaths);
				string file;
				while(getline(files, file, ' ')) {  // GNU make doesn't permit spaces in paths, see http://git.savannah.gnu.org/cgit/make.git/tree/read.c#n3102
					if(settings.verbose)
						clog << "v: including file \"" << file << "\"\n";

					ifstream includefile(file);
					if(!includefile)
						return error(file, 2, settings);
					transform_makefile(includefile, to, path_nolastnode(relative_directory + '/' + file), settings);
				}
			}
			continue;
		}

		if(!line.empty()) {
			line = regex_replace(line, regex("%EXEC"s), ".exe");
			to << line << '\n';
		}
	}

	return 0;
}

int error(const string & msg, int retc, const settings_t & settings) {
	cerr << settings.invocation_command << ": ";
	perror(msg.c_str());
	return retc;
}
