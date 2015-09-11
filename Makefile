# The MIT License (MIT)

# Copyright (c) 2015 nabijaczleweli

#  Permission is hereby granted, free of charge, to any person obtaining a
#  copy of this software and associated documentation files (the "Software"),
#  to deal in the Software without restriction, including without limitation
#  the rights to use, copy, modify, merge, publish, distribute, sublicense,
#  and/or sell copies of the Software, and to permit persons to whom the
#  Software is furnished to do so, subject to the following conditions:
#
#  The above copyright notice and this permission notice shall be included in
#  all copies or substantial portions of the Software.
#
#  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
#  OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
#  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
#  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
#  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
#  FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
#  DEALINGS IN THE SOFTWARE.

include configMakefile

SOURCES = $(foreach src,$(shell busybox find $(SOURCE) -name *.cpp),$(subst $(SOURCE),,$(subst .cpp,,$(src))))

.PHONY : clean all clean-all deps exe

all : exe

clean :
	rm -rf $(BUILD)

clean-all : clean
	rm -rf $(INCLUDE)

deps :
	git submodule update --recursive --init
	@rm -rf $(INCLUDE) 1>$(nul) 2>&1 || :
	@mkdir $(INCLUDE) 1>$(nul) 2>&1 || :
	@mkdir $(INCLUDE)tclap
	cp external/tclap/include/tclap/*.h $(INCLUDE)tclap

exe : $(foreach src,$(SOURCES),$(BUILD)$(src)$(OBJ))
	$(CXX) $(CXXAR) -o$(BUILD)platformake$(EXE) $^
	$(STRIP) $(STRIPAR) $(BUILD)platformake$(EXE)


$(BUILD)%$(OBJ) : src/%.cpp
	@busybox mkdir -p $(dir $@) 1>$(nul) 2>&1
	$(CXX) $(CXXAR) -c -o$@ $^
