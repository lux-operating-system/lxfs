/*
 * lux - a lightweight unix-like operating system
 * Omar Elghoul, 2024
 * 
 * User-level implementation of the lux file system
 */

#include <lxfs.h>

int formatHelp(char *name) {
    cerr << "Usage: " << name << " format [disk image] [size in MB]" << endl;
    return -1;
}

int format(int argc, char **argv) {
    if(argc < 4) return formatHelp(argv[0]);

    string disk = (string)argv[2];
    size_t size = stoi(argv[3]);

    vector<uint8_t> mbr(SECTOR_SIZE);
    readSector(disk, 0, 1, mbr.data());
    MBRPartition *partitions = (MBRPartition *)((uint8_t *)mbr.data() + MBR_PARTITION_OFFSET);

    // find a free starting sector
    uint32_t startSector = 0;

    for(int i = 0; i < 4; i++) {
        if(partitions[i].start > startSector) {
            startSector = partitions[i].start + partitions[i].size;
        }
    }

    if(!startSector) startSector = 63;  // default starting sector

    // TODO: validate size

    // find a free partition entry
    for(int i = 0; i < 4; i++) {
        if(!partitions[i].size) {
            // not bootable by default
            partitions[i].flags = 0x00;
            partitions[i].id = MBR_ID_LXFS;
            partitions[i].start = startSector;
            partitions[i].size = size * (1048576/SECTOR_SIZE);

            // TODO: is it still necessary to calculate CHS? does BIOS/UEFI still check for this?

            writeSector(disk, 0, 1, mbr.data());

            // now construct the file system
            uint64_t volumeSizeBlocks = (size * (1048576/SECTOR_SIZE)) / BLOCK_SIZE;
            uint64_t blockTableSizeBlocks = ((volumeSizeBlocks * 8) / SECTOR_SIZE) / BLOCK_SIZE;
            uint64_t dataBlocks = 33 + blockTableSizeBlocks;

            // identification block/boot sector
            vector<uint8_t> identifier(SECTOR_SIZE);
            LXFSIdentification *id = (LXFSIdentification *)identifier.data();
            id->identifier = LXFS_MAGIC;
            id->version = LXFS_VERSION;
            id->name[0] = 0;
            id->volumeSize = volumeSizeBlocks;
            id->rootBlock = dataBlocks;
            id->parameters = (SECTOR_SIZE >> 9) & 0x06;
            id->parameters |= ((BLOCK_SIZE-1) << 3);
            //cerr << "parameters 0x" << hex << (uint32_t)id->parameters << endl;
            writeSector(disk, startSector, 1, identifier.data());

            

            return 0;
        }
    }

    cerr << "No empty partitions found on the disk" << endl;
    return -1;
}
