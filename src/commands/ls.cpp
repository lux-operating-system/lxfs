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
    int pathSize = countPath(path);

    uint64_t start = 0;

    if(pathSize == 1) {
        // root directory
        start = rootDirectoryBlock(disk, partition);
    } else {
        cerr << "TODO: listing non-root directories" << endl;
        return -1;
    }

    cout << "directory starts at block " << dec << setw(0) << start << endl;

    // now read the series of blocks
    uint64_t current = start;
    size_t size = 0, newSize = 0;
    vector<uint8_t> data(10000);

    while(current != LXFS_BLOCK_EOF) {
        //cerr << "read block 0x" << hex << uppercase << current;

        newSize += BLOCK_SIZE_BYTES;
        data.resize(newSize);
        current = readNextBlock(disk, partition, current, data.data() + size);
        size = newSize;

        //cerr << "; next block 0x" << hex << uppercase << current << endl;
    }

    // now we can list the directory itself

    return 0;
}