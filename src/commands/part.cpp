/*
 * lux - a lightweight unix-like operating system
 * Omar Elghoul, 2024
 * 
 * User-level implementation of the lux file system
 */

#include <lxfs.h>

int partHelp(char *name) {
    cerr << "Usage: " << name << " part [disk image]" << endl;
    return -1;
}

int part(int argc, char **argv) {
    if(argc < 3) return partHelp(argv[0]);

    string disk = (string)argv[2];
    vector<uint8_t> mbr(SECTOR_SIZE);
    readSector(disk, 0, 1, mbr.data());   // first sector
    MBRPartition *partitions = (MBRPartition *)((uint8_t *)mbr.data() + MBR_PARTITION_OFFSET);

    for(int i = 0; i < 4; i++) {
        cout << "Partition " << dec << i << ": ";
        cout << (partitions[i].flags & 0x80 ? "bootable " : "");
        cout << "type 0x" << hex << uppercase << setw(2) << setfill('0') << (uint32_t)partitions[i].id << " ";
        cout << (partitions[i].id == MBR_ID_LXFS ? "LXFS " : "");
        cout << "start " << dec << setw(0) << partitions[i].start << " ";
        cout << "size " << dec << partitions[i].size / (1048576/SECTOR_SIZE) << " MB" << endl;
    }

    return 0;
}