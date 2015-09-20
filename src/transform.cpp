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


#include "transform.hpp"
#include "directives.hpp"
#include "util/error.hpp"
#include "util/file.hpp"
#include "util/line.hpp"
#include "macros.hpp"
#include <fstream>
#include <iostream>
#include <regex>


using namespace std;


char macro_substitution_character = '%';


int transform_makefile(ostream & to, const settings_t & settings) {
	return transform_makefile(settings.make_file, to, settings);
}

int transform_makefile(const string & path, ostream & to, const settings_t & settings) {
	ifstream infile(path);
	if(!infile)
		return error(path, 2, settings);

	return transform_makefile(infile, to, path_nolastnode(path), settings);
}

int transform_makefile(istream & from, ostream & to, const string & relative_directory, const settings_t & settings) {
	for(string line; getline(from, line);) {
		strip_line(line);

		string::size_type pos{};
		while((pos = line.find(macro_substitution_character, pos)) != string::npos) {
			auto after = line.find_first_not_of("ABCDEFGHIJKLMOPQRSTUVWXYZabcdefghijklmopqrstuvwxyz0123456789", pos + 1);

			if(after == pos + 1) {
				++pos;
				continue;
			}

			cout << line << '\n';
			if(after == string::npos)  // Last thing on a line
				after = line.size() - 1;

			const auto macrokey = line.substr(pos + 1, after - pos);
			const auto macro = macros().find(macrokey);
			if(macro == macros().end()) {
				cerr << settings.invocation_command << ": macro not found: \"" << macrokey << "\"\n";
				++pos;  // Ignore this particular macro
			} else
				line.replace(pos, after - pos, macro->second);
		}

		process_directives(line, to, relative_directory, settings);

		if(!line.empty())
			to << line << '\n';
	}

	return 0;
}
