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


#include "line.hpp"
#include <vector>
#include <regex>


using namespace std;


void strip_line(string & line) {
	static const vector<regex> regices = [&]() {
		vector<regex> temp(4);
		for(const auto reg : {"#.*", " +$", "^ +", "\r$"})  // comment, start-of-line space, end-of-line space. CRLF -> LF
			temp.emplace_back(reg, regex_constants::optimize);
		return temp;
	}();

	for(const auto & rgx : regices)
		line = regex_replace(line, rgx, "");
}

//blatantly ripped off http://stackoverflow.com/a/53878/2851815
vector<string> & tokenize(const string & what, vector<string> & where, char sep) {
	const char * str = what.c_str();
	do {
		const char * begin = str;

		while(*str != sep && *str)
			str++;

		where.emplace_back(begin, str);
	} while(*str++);

	return where;
}

vector<string> tokenize(const string & what, char sep) {
	vector<string> tokens;
	tokenize(what, tokens, sep);
	return tokens;
}
