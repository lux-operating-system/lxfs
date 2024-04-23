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

    bool isRootDirectory = (countPath(path) <= 1);

    // find the path
    LXFSDirectoryEntry *entry = new LXFSDirectoryEntry;
    if(!findEntry(disk, partition, path, entry)) {
        cerr << "cannot list for non-existent directory" << endl;
        return -1;
    }

    // make sure it is a directory
    if(((entry->flags >> LXFS_DIR_TYPE_SHIFT) & LXFS_DIR_TYPE_MASK) != LXFS_DIR_TYPE_DIR) {
        cerr << "this entry is not a directory" << endl;
        return -1;
    }

    return 0;
}