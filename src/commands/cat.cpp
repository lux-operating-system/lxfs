/*
 * lux - a lightweight unix-like operating system
 * Omar Elghoul, 2024
 * 
 * User-level implementation of the lux file system
 */

#include <lxfs.h>

int catHelp(char *name) {
    cerr << "Usage: " << name << " cp [disk image] [partition] [path]" << endl;
    return -1;
}

int cat(int argc, char **argv) {
    if(argc < 5) return catHelp(argv[0]);

    string disk = argv[2];
    int partition = stoi(argv[3]);
    string path = argv[4];

    // find the directory
    LXFSDirectoryEntry *entry = new LXFSDirectoryEntry;
    if(!findEntry(disk, partition, path, entry)) {
        cerr << "path not found" << endl;
        return -1;
    }
    
    return 0;
}