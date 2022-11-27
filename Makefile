CC			=g++
CPPFLAGS	=
CXXFLAGS	=
LDFLAGS		=

COMPILER_SOURCE = GarlicCompiler.cpp $(wildcard building/*.cpp)
COMPILER_OBJ = $(patsubst %.cpp,obj/%.o,$(COMPILER_SOURCE))

all: gcompiler

gcompiler: $(COMPILER_OBJ)
	@$(CC) $(LDFLAGS) -o $@ $^
	@echo "Built: $@"

obj/%.o: %.cpp
	@mkdir -p obj
	@mkdir -p obj/src
	@$(CC) $(CPPFLAGS) $(CXXFLAGS) -c -o $@ $<
	@echo "Compiled: $@"

clean:
	@rm -rf obj
	@echo "Cleaned build files."