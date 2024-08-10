/*
 * lux - a lightweight unix-like operating system
 * Omar Elghoul, 2024
 * 
 * User-level implementation of the lux file system
 */

#include <lxfs.h>

int bootHelp(char *name) {
    cerr << "Usage: " << name << " boot [disk image] [partition]" << endl;
    return -1;
}

int boot(int argc, char **argv) {
    if(argc < 4) return bootHelp(argv[0]);

    string disk = argv[2];
    size_t partition = stoi(argv[3]);

    vector<uint8_t> mbr(SECTOR_SIZE);
    readSector(disk, 0, 1, mbr.data());

    MBRPartition *partitions = (MBRPartition *)((uint8_t *)mbr.data() + MBR_PARTITION_OFFSET);
    partitions[partition].flags |= MBR_FLAG_BOOTABLE;
    writeSector(disk, 0, 1, mbr.data());

    return 0;
}