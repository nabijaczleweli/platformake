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
#include "util/scope.hpp"
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


#define PCRE2_DELETE_WRAPPER(tpe)              \
	struct pcre2_##tpe##_deleter {               \
		void operator()(pcre2_##tpe * arg) const { \
			pcre2_##tpe##_free(arg);                 \
		}                                          \
	}  // no ;


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


PCRE2_DELETE_WRAPPER(code);
PCRE2_DELETE_WRAPPER(match_data);

using regex_data_t = tuple<basic_string<PCRE2_UCHAR8>, int /*errorcode*/, PCRE2_SIZE /*erroffset*/>;
using regex_t      = unique_ptr<pcre2_code, pcre2_code_deleter>;

static regex_data_t & regex_data(char macrochar) {
	static map<char, regex_data_t> data;

	auto itr = data.find(macrochar);
	if(itr == data.end())
		itr = data.emplace(macrochar, regex_data_t(reinterpret_cast<PCRE2_SPTR8>(("("s + macrochar + "({)?([A-Za-z0-9_]+)(?(2)}))").c_str()), 0, size_t(0))).first;
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

int process_macros(string & line, const settings_t & settings) {
	const auto & expr = macro_regex(settings.macro_substitution_character);

	unique_ptr<pcre2_match_data, pcre2_match_data_deleter> match_data(pcre2_match_data_create(20, nullptr));
	for(;;) {
		const int rc = pcre2_match(expr.get(), reinterpret_cast<PCRE2_SPTR8>(line.c_str()), line.size(), 0, 0, match_data.get(), nullptr);
		if(rc <= 0)
			break;

		const auto ovector   = pcre2_get_ovector_pointer(match_data.get());
		const auto mbeg      = ovector[0];
		const auto mlen /**/ = ovector[1] - mbeg;

		PCRE2_UCHAR * submatch{};
		PCRE2_SIZE submatch_len{};
		quickscope_wrapper submatch_scope{bind(pcre2_substring_free, ref(submatch))};
		pcre2_substring_get_bynumber(match_data.get(), 3, &submatch, &submatch_len);
		const string submatch_s(reinterpret_cast<char *>(submatch), submatch_len);

		if(settings.verbose)
			clog << "Found macro " << submatch_s << " with";

		const auto macroitr = macros().find(submatch_s);
		if(macroitr == macros().end()) {
			if(settings.verbose)
				clog << "out a value\n";

			cerr << settings.invocation_command << ": macro not found: \"" << submatch_s << "\"\n";
			return 3;
		} else
			line.replace(mbeg, mlen, macroitr->second);
	}

	return 0;
}
