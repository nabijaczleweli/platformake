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


#include "sysprops.hpp"
#include <initializer_list>
#include <algorithm>
#include <iostream>
#include <cstdlib>
#include <cctype>


using namespace std;


static const char * envvar_or(initializer_list<const char *> from, const char * ifnotfound) {
	for(const auto envvar : from)
		if(const char * val = getenv(envvar))
			return val;

	return ifnotfound;
}


const char * const system_temporary_directory = envvar_or({"TEMP", "TMP"}, "/tmp");  // TEMP always set on Windows systems, might not be on Linuxish
const char * const system_name                = envvar_or({"OS", "OSTYPE"}, "linux");


system_variant_t system_variant() {
	static const system_variant_t variant = []() {
		string osname = system_name;
		transform(begin(osname), end(osname), begin(osname), [](char c) { return tolower(c); });

		if(osname.find("windows") != string::npos)
			return system_variant_t::windows;
		else if(osname.find("darwin") != string::npos)
			return system_variant_t::osx;
		else if(osname.find("linux") != string::npos)
			return system_variant_t::linux;
		else {
			cerr << "w: unable to determine host OS type, set to \"" << system_name << "\" using \"linux\" defaults\n";
			return system_variant_t::linux;
		}
	}();

	return variant;
}
