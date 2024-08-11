/*
 * lux - a lightweight unix-like operating system
 * Omar Elghoul, 2024
 * 
 * User-level implementation of the lux file system
 */

#include <lxfs.h>

int bootblkHelp(char *name) {
    cerr << "Usage: " << name << " bootblk [disk image] [partition] [boot binary]" << endl;
    return -1;
}

int bootblk(int argc, char **argv) {
    if(argc < 5) return bootblkHelp(argv[0]);

    string disk = argv[2];
    size_t partition = stoi(argv[3]);
    string binary = argv[4];

    ifstream file(binary, ios::in | ios::binary | ios::ate);
    size_t fileSize = file.tellg();
    file.clear();
    file.seekg(0);

    // vector for the file data, rounded up to the nearest block + 1
    size_t sizeBlocks = (fileSize + BLOCK_SIZE_BYTES - 1) / BLOCK_SIZE_BYTES;
    sizeBlocks++;       // one extra block for metadata
    vector<uint8_t> data(sizeBlocks*BLOCK_SIZE_BYTES);
    file.read((char *)data.data() + BLOCK_SIZE_BYTES, fileSize);
    file.close();

    // add the time
    time_t now = time(nullptr);
    LXFSBootHeader *boot = (LXFSBootHeader *)data.data();
    boot->timestamp = (uint64_t)now;
    cout << "lxfs: installed boot program for " << boot->description << endl;

    writeBlock(disk, 0, 1, sizeBlocks, data.data());
    return 0;
}