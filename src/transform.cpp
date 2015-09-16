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
#include "util/error.hpp"
#include "util/file.hpp"
#include "util/glob.hpp"
#include "util/line.hpp"
#include <iostream>
#include <fstream>
#include <regex>


using namespace std;


static int include_file(const vector<string> & files, ostream & to, const string & rootdir, const settings_t & settings);


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
	static const regex include_r("include([[:space:]]*((?:[^[:space:]]+[[:space:]]?)*))?", regex_constants::optimize);

	smatch match;
	for(string line; getline(from, line);) {
		strip_line(line);

		if(regex_match(line, match, include_r)) {
			const auto & includepaths = match.str(2);
			if(includepaths.empty()) {
				if(settings.verbose)
					clog << "v: empty include directive, skipping\n";
			} else
				// GNU make doesn't permit spaces in paths, see http://git.savannah.gnu.org/cgit/make.git/tree/read.c#n3102
				include_file(glob(tokenize(includepaths)), to, relative_directory, settings);
			continue;
		}

		if(!line.empty()) {
			line = regex_replace(line, regex("%EXEC"s), ".exe");
			to << line << '\n';
		}
	}

	return 0;
}

static int include_file(const vector<string> & files, ostream & to, const string & rootdir, const settings_t & settings) {
	for(const auto & file : files) {
		if(settings.verbose)
			clog << "v: including file \"" << file << "\"\n";

		ifstream includefile(rootdir + '/' + file);
		if(!includefile)
			return error(file, 2, settings);
		if(int ret = transform_makefile(includefile, to, path_nolastnode(rootdir + '/' + file), settings))
			return ret;
	}

	return 0;
}
