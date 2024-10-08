CXXFLAGS=-Wall -c -I./src/include
LDFLAGS=
CXX=c++
LD=c++
SRC:=$(shell find ./src -type f -name "*.cpp")
OBJ:=$(SRC:.cpp=.o)

all: lxfs

clean:
	@rm -f $(OBJ)

%.o: %.cpp
	@echo "\x1B[0;1;32m cxx \x1B[0m $<"
	@$(CXX) $(CXXFLAGS) -o $@ $<

lxfs: $(OBJ)
	@echo "\x1B[0;1;93m ld  \x1B[0m lxfs"
	@$(LD) $(LDFLAGS) $(OBJ) -o lxfs

test:
	./lxfs create disk.hdd 5
	./lxfs format disk.hdd 4
	./lxfs part disk.hdd
	./lxfs ls disk.hdd 0 /
	./lxfs cp disk.hdd 0 ./testfiles/hello.txt /hello.txt
	./lxfs cp disk.hdd 0 ./testfiles/hello2.txt /hello2.txt
	./lxfs ls disk.hdd 0 /
	./lxfs cat disk.hdd 0 /hello.txt
	./lxfs cat disk.hdd 0 /hello2.txt
	./lxfs mkdir disk.hdd 0 /directory
	./lxfs ls disk.hdd 0 /
	./lxfs mkdir disk.hdd 0 /directory/dir2
	./lxfs ls disk.hdd 0 /
	./lxfs ls disk.hdd 0 /directory