/*
 * lux - a lightweight unix-like operating system
 * Omar Elghoul, 2024
 * 
 * User-level implementation of the lux file system
 */

#include <lxfs.h>

int help(char *name) {
    cerr << "Usage: " << name << " [command] [parameters]" << endl;
    cerr << "Available commands:" << endl;
    cerr << " boot      - Makes an lxfs volume bootable" << endl;
    cerr << " bootsec   - Copies a boot sector to the partition" << endl;
    cerr << " bootblk   - Copies a boot block to the partition" << endl;
    cerr << " cat       - Retrieves a file from the disk image" << endl;
    cerr << " cp        - Copies a file onto the disk image" << endl;
    cerr << " create    - Creates a disk image" << endl;
    cerr << " format    - Creates and formats an lxfs volume on a disk image" << endl;
    cerr << " help      - Shows this message" << endl;
    cerr << " ls        - Directory listing" << endl;
    cerr << " mkdir     - Creates a directory on the disk image" << endl;
    cerr << " part      - Shows the partition table of the disk image" << endl;

    return -1;
}

int main(int argc, char **argv) {
    if(argc <= 1 || !strcmp(argv[1], "help")) return help(argv[0]);

    if(!strcmp(argv[1], "create")) {
        return create(argc, argv);
    } else if(!strcmp(argv[1], "format")) {
        return format(argc, argv);
    } else if(!strcmp(argv[1], "part")) {
        return part(argc, argv);
    }

    cerr << "Undefined command " << argv[1] << ", use '" << argv[0] << " help' for a list of commands." << endl;
    return -1;
}
