/*
 * lux - a lightweight unix-like operating system
 * Omar Elghoul, 2024
 * 
 * User-level implementation of the lux file system
 */

#include <lxfs.h>

int createHelp(char *name) {
    cerr << "Usage: " << name << " create [disk image] [size in MB]" << endl;
    return -1;
}

int create(int argc, char **argv) {
    if(argc < 4) return createHelp(argv[0]);

    char *disk = argv[2];
    size_t size = stoi(argv[3]) * 1048576;

    ofstream output(disk);

    for(int i = 0; i < size; i++) {
        output << (uint8_t)0;
    }

    output.close();

    return 0;
}

