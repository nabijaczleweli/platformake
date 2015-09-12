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


#include "file.hpp"
#include <algorithm>
#include <chrono>
#include <random>


using namespace std;
using namespace std::chrono;


string tempname(size_t length) {
	static const char filename_characters[]              = "aBcDeFgHiJkLmNoPqQsTuVwXyZ01234567890-_.";
	static const constexpr auto filename_characters_size = sizeof filename_characters - 1;
	static mt19937 engine(high_resolution_clock::now().time_since_epoch().count());  // random_device is deterministic on Windows
	static uniform_int_distribution<size_t> distribution(0, filename_characters_size - 1);
	static const auto generator = [&]() { return filename_characters[distribution(engine)]; };

	string retval(length, '\0');
	generate(begin(retval), end(retval), generator);
	while(retval[0] == '-' || retval[0] == '.')
		retval[0] = generator();
	return retval;
}

string path_nolastnode(const string & curpath) {
	const auto last = curpath.find_last_of("/");
	return (last == string::npos) ? "." : curpath.substr(0, last);
}
