CC			=g++
CPPFLAGS	=
CXXFLAGS	=
LDFLAGS		=

LIB_SOURCE = $(wildcard src/*.cpp)
LIB_OBJ = $(patsubst %.cpp,obj/%.o,$(COMPILER_SOURCE))

COMPILER_SOURCE = GarlicCompiler.cpp $(LIB_SOURCE)
COMPILER_OBJ = $(patsubst %.cpp,obj/%.o,$(COMPILER_SOURCE))

all: gcompiler

gcompiler: $(COMPILER_OBJ)
	@$(CC) $(LDFLAGS) -o $@ $^
	@echo "Built: $@"

glib.o: $(LIB_SOURCE)
	@echo $(LIB_OBJ)
	$(CC) $(LDFLAGS) -c -o $@ $^
	@echo "Built: $@"

obj/%.o: %.cpp
	@mkdir -p obj
	@mkdir -p obj/src
	@$(CC) $(CPPFLAGS) $(CXXFLAGS) -c -o $@ $<
	@echo "Compiled: $@"

clean:
	@rm -rf obj
	@echo "Cleaned build files."