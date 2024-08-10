/*
 * lux - a lightweight unix-like operating system
 * Omar Elghoul, 2024
 * 
 * User-level implementation of the lux file system
 */

#include <lxfs.h>

int bootsecHelp(char *name) {
    cerr << "Usage: " << name << " bootsec [disk image] [partition] [boot binary]" << endl;
    return -1;
}

int bootsec(int argc, char **argv) {
    if(argc < 5) return bootsecHelp(argv[0]);

    string disk = argv[2];
    size_t partition = stoi(argv[3]);
    string binary = argv[4];

    // read the new file
    vector<uint8_t> newBoot(SECTOR_SIZE);
    ifstream bootFile(binary, ios::in | ios::binary);
    bootFile.read((char *)newBoot.data(), SECTOR_SIZE);
    bootFile.close();

    vector<uint8_t> mbr(SECTOR_SIZE);
    readSector(disk, 0, 1, mbr.data());

    MBRPartition *partitions = (MBRPartition *)((uint8_t *)mbr.data() + MBR_PARTITION_OFFSET);

    vector<uint8_t> oldBoot(SECTOR_SIZE);
    readSector(disk, partitions[partition].start, 1, oldBoot.data());

    // copy the boot code only
    memcpy(oldBoot.data(), newBoot.data(), 4);      // first four bytes
    memcpy(oldBoot.data() + sizeof(LXFSIdentification), newBoot.data() + sizeof(LXFSIdentification), 512 - sizeof(LXFSIdentification));
    writeSector(disk, partitions[partition].start, 1, oldBoot.data());

    return 0;
}