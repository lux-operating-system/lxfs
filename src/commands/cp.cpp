/*
 * lux - a lightweight unix-like operating system
 * Omar Elghoul, 2024
 * 
 * User-level implementation of the lux file system
 */

#include <lxfs.h>

int cpHelp(char *name) {
    cerr << "Usage: " << name << " cp [disk image] [partition] [src file] [dst path]" << endl;
    return -1;
}

int cp(int argc, char **argv) {
    if(argc < 6) return cpHelp(argv[0]);

    string disk = argv[2];
    int partition = stoi(argv[3]);
    string src = argv[4];
    string dst = argv[5];
    string parent = parentPath(dst);
    string name = finalPath(dst);

    cout << name << endl;

    // get the parent directory
    LXFSDirectoryEntry *parentEntry = new LXFSDirectoryEntry;
    if(!findEntry(disk, partition, parent, parentEntry)) {
        cerr << "directory " << parent << " does not exist";
        return -1;
    }

    if(((parentEntry->flags >> LXFS_DIR_TYPE_SHIFT) & LXFS_DIR_TYPE_MASK) != LXFS_DIR_TYPE_DIR) {
        cerr << parent << " is not a directory";
        return -1;
    }

    // now load the parent directory, block by block
    uint64_t current = parentEntry->block;
    size_t size = 0, newSize = 0;
    vector<uint8_t> parentData(0);

    while(current != LXFS_BLOCK_EOF) {
        newSize += BLOCK_SIZE_BYTES;
        parentData.resize(newSize);
        current = readNextBlock(disk, partition, current, parentData.data() + size);
        size = newSize;
    }

    // look for a free entry
    LXFSDirectoryEntry *newEntry = (LXFSDirectoryEntry *)((char *)parentData.data() + sizeof(LXFSDirectoryHeader));
    size_t offset = sizeof(LXFSDirectoryHeader);
    while(offset < size) {
        //cout << "offset " << dec << offset << endl;
        if(!(newEntry->flags & LXFS_DIR_VALID)) {
            // found a free entry
            newEntry->flags = LXFS_DIR_VALID;
            newEntry->flags |= (LXFS_DIR_TYPE_FILE << LXFS_DIR_TYPE_MASK);

            newEntry->owner = LXFS_USER_ROOT;
            newEntry->group = LXFS_USER_ROOT;
            newEntry->permissions = LXFS_DEFAULT_PERMS;

            // temporary test
            for(int i = 0; i < name.length(); i++) {
                newEntry->name[i] = name[i];
            }

            newEntry->name[name.length()] = 0;
            newEntry->entrySize = sizeof(LXFSDirectoryEntry)-512 + name.length() + 1;

            // and update the table on disk
            uint64_t tableBlock = parentEntry->block;
            tableBlock += (offset/BLOCK_SIZE_BYTES);
            uint64_t blockOffset = (offset%BLOCK_SIZE_BYTES) / BLOCK_SIZE_BYTES;
            writeBlock(disk, partition, tableBlock, 1, parentData.data() + blockOffset);

            //cout << "updating block " << dec << tableBlock << endl;
            //cout << "parent dir started at " << dec << parentEntry->block << endl;

            return 0;
        } else {
            newEntry = (LXFSDirectoryEntry *)((char *)newEntry + newEntry->entrySize);
            offset += newEntry->entrySize;

            // TODO: reallocate these blocks as necessary when growing
        }
    }

    return -1;
}