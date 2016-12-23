out=./ic

# compilers
cc=gcc $(c-etc-flags) $(cflags)
cxx=g++ $(c-etc-flagss) $(cxxflags)
ld=g++

# compiler flags
c-etc-flags=-Wall -g -O2 -m64 $(include)
cflags=
cxxflags=-std=c++11

# inlude / link
include=
linkage=

# automated build stuff
src_dirs=$(shell find src -type d)
obj_dirs=$(src_dirs:src/%=obj/%)
srcs=$(wildcard $(src_dirs:%=%/*.cpp))
objs=$(srcs:src/%=obj/%.o)
deps=$(srcs:src/%=obj/%.dep)

.PHONY: all
all: $(out)

# make dirs
obj:
	@mkdir -p obj $(obj_dirs)

# compile objects
obj/%.cpp.o: src/%.cpp
	@echo CXX $@
	@$(cxx) $< -c -o $@
	@$(cxx) $< -c -MM -MT $@ -o $(@:%.o=%.dep)

obj/%.c.o: src/%.c
	@echo CC $@
	@$(cc) $< -c -o $@
	@$(cc) $< -c -MM -MT $@ -o $(@:%.o=%.dep)

-include $(wildcard obj/*.dep)

# link executable
$(out): obj $(objs)
	@echo LINK $@
	@$(ld) $(objs) -o $@

# clean
.PHONY: clean
clean:
	@echo CLEAN
	@rm -rf obj $(out)

.PHONY: rebuild
rebuild: clean all
