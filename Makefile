CXXFLAGS=-c -I./src/include
LDFLAGS=
CXX=c++
LD=c++
SRC:=$(shell find ./src -type f -name "*.cpp")
OBJ:=$(SRC:.cpp=.o)

all: lxfs

clean:
	@rm -f $(OBJ)

%.o: %.cpp
	@echo "\x1B[0;1;32m [  CXX  ]\x1B[0m $<"
	@$(CXX) $(CXXFLAGS) -o $@ $<

lxfs: $(OBJ)
	@echo "\x1B[0;1;34m [  LD   ]\x1B[0m lxfs"
	@$(LD) $(LDFLAGS) $(OBJ) -o lxfs

test:
	./lxfs create disk.hdd 5
	./lxfs format disk.hdd 4
	./lxfs part disk.hdd
	