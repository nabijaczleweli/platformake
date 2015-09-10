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


#include "settings.hpp"
#include <tclap/CmdLine.h>
#include <iostream>
#include <iterator>
#include <sstream>


using namespace std;
using namespace TCLAP;


settings_t load_settings(int argc, const char ** argv) {
	settings_t ret;

	try {
		CmdLine command_line("platformake -- a multiplatforming make proxy", ' ', __DATE__ " " __TIME__);
		SwitchArg verbose("v", "verbose", "Turns on verbose output", command_line, ret.verbose);
		ValueArg<string> make_file("f", "file", "read FILE as Makefile to transform", false, ret.make_file, "FILE", command_line);
		ValueArg<string> temporary_directory("t", "temp", "use DIR instead of a System-wide temporary directory for storing temporary Makefiles", false,
		                                     ret.temporary_directory, "DIR", command_line);
		ValueArg<string> make_command("m", "make-command", "Specifies the command used to invoke GNU make", false, ret.make_command, "path to make exec",
		                              command_line);
		UnlabeledMultiArg<string> make_arguments("make-arguments", "arguments passed to GNU make, e.g. '-j'", false, "additional make arguments", command_line);

		command_line.parse(argc, argv);

		ret.verbose             = verbose.getValue();
		ret.make_command        = make_command.getValue();
		ret.make_file           = make_file.getValue();
		ret.temporary_directory = temporary_directory.getValue();

		ostringstream margs;
		copy(begin(make_arguments.getValue()), end(make_arguments.getValue()), ostream_iterator<string>(margs, " "));
		ret.make_arguments = margs.str();
	} catch(const ArgException & e) {
		cerr << argv[0] << ": error: parsing arguments failed (" << e.error() << ") for argument " << e.argId() << "\ntrying to continue anyway.";
	}

	return ret;
}
