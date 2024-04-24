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

    // find the directory entry
    LXFSDirectoryEntry *entry = new LXFSDirectoryEntry;
    if(!findEntry(disk, partition, path, entry)) {
        cerr << "path not found" << endl;
        return -1;
    }

    // read the file into memory - round up size according to the blocks
    size_t blockSize = (entry->size + BLOCK_SIZE_BYTES - 1) / BLOCK_SIZE_BYTES;
    blockSize++;
    vector<uint8_t> data(blockSize*BLOCK_SIZE_BYTES);

    // and follow the chain
    uint64_t current = entry->block;
    size_t index = 0;
    while(current != LXFS_BLOCK_EOF) {
        //cout << "read block " << dec << current << endl;
        current = readNextBlock(disk, partition, current, data.data() + (index*BLOCK_SIZE_BYTES));
        index++;
    }

    uint8_t *ptr = data.data() + BLOCK_SIZE_BYTES;

    for(uint64_t i = 0; i < entry->size; i++) {
        cout << ptr[i];
    }

    //cout << endl;
    return 0;
}