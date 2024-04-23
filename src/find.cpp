/*
 * lux - a lightweight unix-like operating system
 * Omar Elghoul, 2024
 * 
 * User-level implementation of the lux file system
 */

#include <lxfs.h>

bool findEntry(string disk, int partition, string path, LXFSDirectoryEntry *dest) {
    int pathSize = countPath(path);

    uint64_t start = 0;

    if(pathSize == 1) {
        // root directory
        start = rootDirectoryBlock(disk, partition);

        vector<uint8_t> rootBlock(BLOCK_SIZE_BYTES);
        readBlock(disk, partition, start, 1, rootBlock.data());
        LXFSDirectoryHeader *rootHeader = (LXFSDirectoryHeader *)rootBlock.data();

        dest->createTime = rootHeader->createTime;
        dest->accessTime = rootHeader->accessTime;
        dest->modTime = rootHeader->modTime;

        dest->flags = LXFS_DIR_VALID;
        dest->flags |= (LXFS_DIR_TYPE_DIR << LXFS_DIR_TYPE_SHIFT);
        dest->owner = LXFS_USER_ROOT;
        dest->group = LXFS_USER_ROOT;
        dest->permissions = LXFS_DEFAULT_PERMS;

        dest->size = rootHeader->sizeEntries;
        dest->entrySize = sizeof(LXFSDirectoryEntry);
        dest->block = start;

        return true;
    } else {
        cerr << "TODO: find non-root directories" << endl;
        dest->flags = 0;
        return false;
    }

    //cout << "directory starts at block " << dec << setw(0) << start << "; ";

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

    //cout << "size " << dec << setw(0) << size/BLOCK_SIZE_BYTES << endl;
}