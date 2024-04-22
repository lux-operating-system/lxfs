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
    uint64_t tableIndex = block % (BLOCK_SIZE_BYTES/8);

    readBlock(disk, partition, tableBlock, 1, blockTable.data());
    return blockTable[tableIndex];
}

uint64_t readNextBlock(string disk, int partition, uint64_t block, void *buffer) {
    readBlock(disk, partition, block, 1, buffer);
    return getNextBlock(disk, partition, block);
}
