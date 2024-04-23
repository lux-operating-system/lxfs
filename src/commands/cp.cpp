/*
 * lux - a lightweight unix-like operating system
 * Omar Elghoul, 2024
 * 
 * User-level implementation of the lux file system
 */

#include <lxfs.h>

int cpHelp(char *name) {
    cerr << "Usage: " << name << " cp [disk image] [partition] [src file] [dst path]" << endl;
    return -1;
}

int cp(int argc, char **argv) {
    if(argc < 6) return cpHelp(argv[0]);

    string disk = argv[2];
    int partition = stoi(argv[3]);
    string src = argv[4];
    string dst = argv[5];

    return 0;
}