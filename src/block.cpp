/*
 * lux - a lightweight unix-like operating system
 * Omar Elghoul, 2024
 * 
 * User-level implementation of the lux file system
 */

#include <lxfs.h>

uint64_t getNextBlock(string disk, int partition, uint64_t block) {
    vector<uint64_t> blockTable(BLOCK_SIZE_BYTES/8);

    // reference the starting block
    uint64_t tableBlock = block / (BLOCK_SIZE_BYTES/8);
    tableBlock += 33;
    uint64_t tableIndex = block % (BLOCK_SIZE_BYTES/8);

    //cerr << "table block " << dec << tableBlock << ", index " << dec << tableIndex << endl;

    readBlock(disk, partition, tableBlock, 1, blockTable.data());

    uint64_t next = blockTable[tableIndex];
    //cerr << "block 0x" << hex << uppercase << block << "; next 0x" << hex << uppercase << next << endl;
    return next;
}

uint64_t readNextBlock(string disk, int partition, uint64_t block, void *buffer) {
    readBlock(disk, partition, block, 1, buffer);
    return getNextBlock(disk, partition, block);
}

uint64_t rootDirectoryBlock(string disk, int partition) {
    vector<uint8_t> data(BLOCK_SIZE_BYTES);
    readBlock(disk, partition, 0, 1, data.data());

    LXFSIdentification *id = (LXFSIdentification *)data.data();
    return id->rootBlock;
}
