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

SOURCES := $(foreach src,$(shell $(FIND) $(SOURCE) -name *.cpp),$(subst $(SOURCE),,$(subst .cpp,,$(src))))

.PHONY : clean all clean-all deps exe

all : exe
	echo asdf%{EXEC}asdf %EXEC $(NESTEDEXE) %OS

clean :
	rm -rf $(BUILD)

clean-all : clean
	rm -rf $(INCLUDE) $(EXTOBJ) external/pcre2/build

deps :
	git submodule update --recursive --init
	@rm -rf $(EXTOBJ) $(INCLUDE) external/pcre2/build 1>$(nul) 2>&1 || :
	@mkdir $(EXTOBJ) $(INCLUDE) $(INCLUDE)tclap $(EXTOBJ) 1>$(nul) 2>&1 || :
	cd external/pcre2 && mkdir build && cd build && cmake .. -G"$(MAKEFILE_VARIANT) Makefiles" -DCMAKE_C_COMPILER="$(CC)" -DPCRE2_NEWLINE=ANY -DPCRE2_SUPPORT_JIT=ON -DPCRE2_BUILD_PCRE2GREP=OFF -DPCRE2_BUILD_TESTS=OFF -DCMAKE_BUILD_TYPE=Release && $(MAKE) -j 2
	cp external/tclap/include/tclap/*.h $(INCLUDE)tclap
	cp external/pcre2/build/pcre2.h external/simpleopt/SimpleGlob.h $(INCLUDE)
	cp external/pcre2/build/CMakeFiles/pcre2-8.dir/pcre2_*.* external/pcre2/build/CMakeFiles/pcre2-8.dir/src/pcre2_* $(EXTOBJ)

exe : $(foreach src,$(SOURCES),$(OBJDIR)$(src)$(OBJ))
	$(CXX) $(CXXAR) -o$(BUILD)platformake$(EXE) $(EXTOBJ)* $^
	$(STRIP) $(STRIPAR) $(BUILD)platformake$(EXE)


$(OBJDIR)%$(OBJ) : $(SOURCE)%.cpp
	@mkdir -p $(dir $@) 1>$(nul) 2>&1
	$(CXX) $(CXXAR) -c -o$@ $<

$(foreach src,$(subst main,,$(SOURCES)),$(eval $(OBJDIR)$(src)$(OBJ) : $(SOURCE)$(src).hpp))
