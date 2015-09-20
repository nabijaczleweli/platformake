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
#include <vector>


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
