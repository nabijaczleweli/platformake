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
#include <vector>
#include <regex>
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


static const regex & macro_regex(char macrochar) {
	static map<char, regex> regices;

	auto itr = regices.find(macrochar);
	if(itr == regices.end())
		itr = regices.emplace(macrochar, regex("("s + macrochar + "([A-Za-z0-9]+))", regex_constants::optimize)).first;
	return itr->second;
}


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

int process_macros(string & line, const settings_t & settings) {
	smatch match;
	while(regex_search(line, match, macro_regex(settings.macro_substitution_character))) {
		const auto & wholemacro_sub = match[1];
		const auto & macro_text     = match.str(2);
		const auto macro            = macros().find(macro_text);

		if(settings.verbose)
			clog << "Found macro " << macro_text << " with";

		if(macro == macros().end()) {
			if(settings.verbose)
				clog << "out a value\n";
			cerr << settings.invocation_command << ": macro not found: \"" << macro_text << "\"\n";
			return 3;
		} else {
			if(settings.verbose)
				clog << " a value of " << macro->second << '\n';
			line.replace(wholemacro_sub.first, wholemacro_sub.second, macro->second);
		}
	}

	return 0;
}
