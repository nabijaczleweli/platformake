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


#include "glob.hpp"
#include <SimpleGlob.h>


using namespace std;


vector<string> glob(const vector<string> & from, glob_include_mode mode) {
	unsigned int flags = SG_GLOB_TILDE;
	switch(mode) {
		case glob_include_mode::only_files:
			flags |= SG_GLOB_ONLYFILE;
			break;
		case glob_include_mode::only_directories:
			flags |= SG_GLOB_ONLYDIR;
			break;
		default:
			break;
	}
	CSimpleGlob globber(flags);

	for(const auto & pattern : from)
		globber.Add(pattern.c_str());

	return {globber.Files(), globber.Files() + globber.FileCount()};
}
