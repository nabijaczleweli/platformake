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
#include "settings.hpp"
#include "sysprops.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <cstdio>
#include <regex>


using namespace std;


void transform_makefile(istream & from, ostream & to, const string & relative_directory, const settings_t & settings);
void strip_line(string & line);


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
		const string & makepath = path_nolastnode(settings.make_file);
		transform_makefile(infile, outfile, makepath.empty() ? "." : makepath, settings);
	}

	if(settings.verbose)
		clog << '\n';

	outfile.flush();
	int retval = system((settings.make_command + ' ' + settings.make_arguments + " -f \"" + tempfile_path + '"').c_str());

	return retval;
}


void transform_makefile(istream & from, ostream & to, const string & relative_directory, const settings_t & settings) {
	static const regex include_r("include[[:space:]](.+)", regex_constants::optimize);

	smatch match;
	for(string line; getline(from, line);) {
		strip_line(line);

		if(regex_match(line, match, include_r)) {
			const auto & includepath = match.str(1);
			if(settings.verbose)
				clog << "v: including file \"" << includepath << "\"\n";

			ifstream includefile(includepath);
			transform_makefile(includefile, to, path_nolastnode(relative_directory + '/' + includepath), settings);
			continue;
		}

		if(!line.empty()) {
			line = regex_replace(line, regex("%EXEC"s), ".exe");
			to << line << '\n';
		}
	}
}

void strip_line(string & line) {
	static const vector<regex> regices = [&]() {
		vector<regex> temp(3);
		for(const auto reg : {"#.*", " +$", "^ +"}) // comment, start-of-line space, end-of-line space
			temp.emplace_back(reg, regex_constants::optimize);
		return temp;
	}();

	for(const auto & rgx : regices)
		line = regex_replace(line, rgx, "");
}
