/*
 * lux - a lightweight unix-like operating system
 * Omar Elghoul, 2024
 * 
 * User-level implementation of the lux file system
 */

#include <lxfs.h>

int mbrHelp(char *name) {
    cerr << "Usage: " << name << " mbr [disk image] [mbr binary]" << endl;
    return -1;
}

int mbr(int argc, char **argv) {
    if(argc < 4) return mbrHelp(argv[0]);

    string disk = argv[2];
    ifstream mbrFile(argv[3], ios::in | ios::binary);
    vector<uint8_t> newMbr(SECTOR_SIZE);
    mbrFile.read((char *)newMbr.data(), SECTOR_SIZE);
    mbrFile.close();

    vector<uint8_t> oldMbr(SECTOR_SIZE);
    readSector(disk, 0, 1, oldMbr.data());

    // copy the partition table from the old MBR to the new one so we don't overwrite it
    memcpy(newMbr.data() + 446, oldMbr.data() + 446, sizeof(MBRPartition) * 4);

    // and update
    writeSector(disk, 0, 1, newMbr.data());
    return 0;
}