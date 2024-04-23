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

uint64_t volumeSize(string disk, int partition) {
    vector<uint8_t> data(BLOCK_SIZE_BYTES);
    readBlock(disk, partition, 0, 1, data.data());

    LXFSIdentification *id = (LXFSIdentification *)data.data();
    return id->volumeSize;
}

uint64_t findFreeBlock(string disk, int partition, int index) {
    uint64_t block;
    uint64_t size = volumeSize(disk, partition);
    int currentIndex = 0;
    for(uint64_t i = 0; i < size; i++) {
        block = getNextBlock(disk, partition, i);
        if(block == LXFS_BLOCK_FREE) currentIndex++;

        if(currentIndex > index) return block;
    }

    return LXFS_BLOCK_EOF;
}

void setNextBlock(string disk, int partition, uint64_t block, uint64_t next) {
    vector<uint64_t> blockTable(BLOCK_SIZE_BYTES/8);

    // reference the starting block
    uint64_t tableBlock = block / (BLOCK_SIZE_BYTES/8);
    tableBlock += 33;
    uint64_t tableIndex = block % (BLOCK_SIZE_BYTES/8);

    readBlock(disk, partition, tableBlock, 1, blockTable.data());

    blockTable[tableIndex] = next;
    writeBlock(disk, partition, tableBlock, 1, blockTable.data());
}

uint64_t allocateNewBlocks(string disk, int partition, size_t blockCount) {
    vector<uint64_t> blocks(blockCount+1);
    blocks[blockCount] = LXFS_BLOCK_EOF;

    for(size_t i = 0; i < blockCount; i++) {
        blocks[i] = findFreeBlock(disk, partition, i);
    }

    // update the table on disk
    for(size_t i = 0; i < blockCount; i++) {
        setNextBlock(disk, partition, blocks[i], blocks[i+1]);
    }

    return blocks[0];
}
