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


#include "directives.hpp"
#include "transform.hpp"
#include "util/error.hpp"
#include "util/file.hpp"
#include "util/glob.hpp"
#include "util/line.hpp"
#include <iostream>
#include <fstream>


using namespace std;


static inline regex operator"" _r(const char * str, size_t) {
	return regex(str, regex_constants::optimize);
}

static int include_file(const smatch & match, ostream & to, const string & rootdir, const settings_t & settings);


decltype(directives) directives({make_tuple("^[s-]?include([[:space:]]*((?:[^[:space:]]+[[:space:]]?)*))?$"_r, include_file, false)});


static int do_include_file(const vector<string> & files, ostream & to, const string & rootdir, const settings_t & settings);


static int include_file(const smatch & match, ostream & to, const string & rootdir, const settings_t & settings) {
	const auto & includepaths = match.str(2);
	if(includepaths.empty()) {
		if(settings.verbose)
			clog << "v: empty include directive, skipping\n";
		return 0;
	} else
		// GNU make doesn't permit spaces in paths, see http://git.savannah.gnu.org/cgit/make.git/tree/read.c#n3102
		return do_include_file(glob(tokenize(includepaths)), to, rootdir, settings);
}


static int do_include_file(const vector<string> & files, ostream & to, const string & rootdir, const settings_t & settings) {
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
