/*
 * lux - a lightweight unix-like operating system
 * Omar Elghoul, 2024
 * 
 * User-level implementation of the lux file system
 */

#include <lxfs.h>

int lsHelp(char *name) {
    cerr << "Usage: " << name << " ls [disk image] [partition] [path]" << endl;
    return -1;
}

int ls(int argc, char **argv) {
    if(argc < 5) return lsHelp(argv[0]);

    string disk = argv[2];
    int partition = stoi(argv[3]);
    string path = argv[4];

    
    return 0;
}