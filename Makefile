# directories and names
name := project.exe
name_d := project_d.exe
src_dir := src
include_dir := $(src_dir)
build_dir := build
build_dir_d := build_debug
result_dir := .
deps_dir := deps
result := $(result_dir)/$(name)
result_d := $(result_dir)/$(name_d)
minisat+_exec := minisat+.exe
minisat+_root := minisat+

# compiler and complier flags
CC = g++

DEBUG = -g
OPTIMIZE = -O2

WARNINGS = -W -Wall -Wunused -Wuninitialized
WARNINGS_D = -W -Wall -Wunused

CFLAGS = -c $(OPTIMIZE) $(WARNINGS) -I$(include_dir)
CFLAGS_D = -c $(DEBUG) $(WARNINGS_D) -I$(include_dir)

# linker and linker flags
LINK = g++

LDFLAGS = $(OPTIMIZE)
LDFLAGS_D = $(DEBUG)

LIBS = -lfm -lgmpxx -lgmp

objects := $(patsubst $(src_dir)/%.cpp,$(build_dir)/%.o,$(wildcard $(src_dir)/*.cpp))
objects_d := $(subst $(build_dir)/,$(build_dir_d)/,$(objects))
depfiles := $(subst .o,.d,$(objects))
depfiles_d := $(subst .o,.d,$(objects_d))

all : release debug

release :	$(result) $(minisat+_exec)

debug : $(result_d) $(minisat+_exec)

$(result) : $(objects)
	$(LINK) $(LDFLAGS) $(objects) -o $(result) $(LIBS)

$(result_d) : $(objects_d)
	$(LINK) $(LDFLAGS) $(objects_d) -o $(result_d) $(LIBS)

$(build_dir)/%.d $(build_dir_d)/%.d : $(src_dir)/%.cpp
	$(CC) -MM -MT '$(@:.d=.o) $@'  $(CFLAGS) $< > $@

-include $(depfiles)
-include $(depfiles_d)

$(build_dir)/%.o : $(src_dir)/%.cpp
	$(CC) $(CFLAGS) $< -o $@

$(build_dir_d)/%.o : $(src_dir)/%.cpp
	$(CC) $(CFLAGS_D) $< -o $@

$(minisat+_exec) : $(minisat+_root)/*.C $(minisat+_root)/*.h Makefile
	(cd $(minisat+_root) && \
	$(MAKE) rs && \
	mv minisat+_bignum_static ../$(minisat+_exec)) || exit 1

.PHONY : all release debug clean clean_r clean_d clean_minisat

clean : clean_r clean_d clean_minisat
	-rm -rf doc/ input.smv nusmv.out

clean_r :
	-rm -f $(result) $(depfiles) $(objects)

clean_d :
	-rm -f $(result_d) $(depfiles_d) $(objects_d)

clean_minisat :
	-rm -f $(minisat+_exec)
	(cd $(minisat+_root) && $(MAKE) clean) || exit 1

TAGS : $(src_dir)/* $(include_dir)/*
	ctags -R --c++-kinds=+p --fields=+iaS --extra=+q $(src_dir) $(include_dir)

doc: $(src_dir)/* $(include_dir)/* Doxyfile
	doxygen Doxyfile
