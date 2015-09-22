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


#include "macros.hpp"
#include "sysprops.hpp"
#include <functional>
#include <algorithm>
#include <iostream>
#define PCRE2_STATIC
#define PCRE2_CODE_UNIT_WIDTH 8
#include <pcre2.h>
#include <memory>
#include <vector>
#include <map>


using namespace std;

using macros_t          = map<string, string>;
using system_variants_t = vector<system_variant_t>;


static const vector<pair<const system_variants_t, const macros_t>> defaults({
    {{system_variant_t::linux}, {{"system", "linux"}}},      //
    {{system_variant_t::windows}, {{"system", "windows"}}},  //
    {{system_variant_t::macosx}, {{"system", "macosx"}}},    //
    {{}, {{"system", "system"}}},                            //

    {{system_variant_t::windows}, {{"EXEC", ".exe"}}},  //
    {{}, {{"EXEC", ".out"}}}                            //
});


macros_t & macros() {
	static macros_t mcrs = []() {
		macros_t toret;

		for(const auto & definitions : defaults)
			if(definitions.first.empty() || any_of(begin(definitions.first), end(definitions.first), [](auto sys) { return sys == system_variant(); }))
				toret.insert(begin(definitions.second), end(definitions.second));

		return toret;
	}();

	return mcrs;
}


using stringrange       = pair<string::const_iterator, string::const_iterator>;
using doublestringrange = pair<stringrange, stringrange>;


static const doublestringrange emptydoublestringrange(const string & str) {
	return {{str.end(), str.end()}, {str.end(), str.end()}};
}


struct pcre2_code_deleter {
	void operator()(pcre2_code * rgx) const {
		pcre2_code_free(rgx);
	}
};

using regex_data_t = tuple<basic_string<PCRE2_UCHAR8>, int /*errorcode*/, PCRE2_SIZE /*erroffset*/>;
using regex_t      = unique_ptr<pcre2_code, pcre2_code_deleter>;

static regex_data_t & regex_data(char macrochar) {
	static map<char, regex_data_t> data;

	auto itr = data.find(macrochar);
	if(itr == data.end())
		itr = data.emplace(macrochar, regex_data_t(reinterpret_cast<PCRE2_SPTR8>(("("s + macrochar + "({)?([A-Za-z0-9]+)(?(2)}))").c_str()), 0, size_t(0))).first;
	return itr->second;
}

static const regex_t & macro_regex(char macrochar) {
	static map<char, regex_t> regices;

	auto itr = regices.find(macrochar);
	if(itr == regices.end()) {
		auto & data          = regex_data(macrochar);
		const auto & pattern = get<0>(data);
		auto & errorcode     = get<1>(data);

		itr = regices.emplace(macrochar, regex_t(pcre2_compile(pattern.c_str(), pattern.size(), 0, &errorcode, &get<2>(data), nullptr))).first;
		if(!itr->second.get()) {
			string buffer;
			for(size_t i = 256;; i += 256) {
				buffer.resize(i);
				if(pcre2_get_error_message(errorcode, reinterpret_cast<PCRE2_UCHAR8 *>(&buffer[0]), buffer.size()) > 0)
					break;
			}
			cerr << 'A' << ((static_cast<int>(errorcode) < 0) ? "n UTF formatting or matching error"s : " regex compilation error") << " (" << errorcode
			     << ") occured\n";
			throw runtime_error(buffer);
		}
	}
	return itr->second;
}

struct pcre2_match_data_deleter {
	void operator()(pcre2_match_data * rgx) const {
		pcre2_match_data_free(rgx);
	}
};

int process_macros(string & line, const settings_t & settings) {
	const auto & expr = macro_regex(settings.macro_substitution_character);

	unique_ptr<pcre2_match_data, pcre2_match_data_deleter> match_data(pcre2_match_data_create(20, nullptr));
	const int rc = pcre2_match(expr.get(), (PCRE2_SPTR8)line.c_str(), line.size(), 0, 0, match_data.get(), nullptr);
	if(rc > 0) {
		auto ovector = pcre2_get_ovector_pointer(match_data.get());
		cout << "Match succeeded at offset " << ovector[0] << '\n';
		/* Use ovector to get matched strings */
		for(int i = 0; i < rc; i++) {
			PCRE2_SPTR start = (PCRE2_SPTR8)line.c_str() + ovector[2 * i];
			PCRE2_SIZE slen = ovector[2 * i + 1] - ovector[2 * i];
			cout << i << ": " << string((char *)start, (int)slen) << '\n';
			break;
		}
	}

	return 0;
}
doublestringrange next_macro(const string & str, char macrochar) {
	const auto begpos = str.find_first_of(macrochar);

	if(begpos == string::npos)
		return emptydoublestringrange(str);

	const auto braces     = (str[begpos + 1] == '{');
	const auto textbegpos = begpos + braces + 1;
	auto endpos           = str.find_first_not_of("ABCDEFGHIJKLMOPQRSTUVWXYZ"
	                                    "abcdefghijklmopqrstuvwxyz"
	                                    "0123456789",
	                                    textbegpos);

	if(endpos == string::npos)
		endpos = str.size() - 1;

	const auto textendpos = endpos + braces;
	endpos += braces;

	if(textbegpos == textendpos && textendpos == endpos)  // nonmacro character (such as in "out/%.o : src/%.cpp")
		return emptydoublestringrange(str);

	cout << str[begpos] << ' ' << str[textbegpos] << ' ' << str[textendpos] << ' ' << str[endpos] << ' ' << boolalpha << braces << ' ' << str << '\n';

	return {{str.begin() + begpos, str.begin() + endpos}, {str.begin() + textbegpos, str.begin() + textendpos}};
}
