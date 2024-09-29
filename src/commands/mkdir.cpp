/*
 * lux - a lightweight unix-like operating system
 * Omar Elghoul, 2024
 * 
 * User-level implementation of the lux file system
 */

#include <lxfs.h>

int mkdirHelp(char *name) {
    cerr << "Usage: " << name << " mkdir [disk image] [partition] [path]" << endl;
    return -1;
}

int mkdir(int argc, char **argv) {
    if(argc < 5) return mkdirHelp(argv[0]);

    string disk = argv[2];
    int partition = stoi(argv[3]);
    string path = argv[4];
    string parent = parentPath(path);
    string name = finalPath(path);

    // get the parent directory
    LXFSDirectoryEntry *parentEntry = new LXFSDirectoryEntry;
    if(!findEntry(disk, partition, parent, parentEntry)) {
        cerr << "directory " << parent << " does not exist";
        return -1;
    }

    if(((parentEntry->flags >> LXFS_DIR_TYPE_SHIFT) & LXFS_DIR_TYPE_MASK) != LXFS_DIR_TYPE_DIR) {
        cerr << parent << " is not a directory" << endl;
        cerr << hex << ((parentEntry->flags >> LXFS_DIR_TYPE_SHIFT) & LXFS_DIR_TYPE_MASK);
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
            newEntry->flags |= (LXFS_DIR_TYPE_DIR << LXFS_DIR_TYPE_SHIFT);

            newEntry->owner = LXFS_USER_ROOT;
            newEntry->group = LXFS_USER_ROOT;
            newEntry->permissions = LXFS_DEFAULT_PERMS;

            time_t now = time(nullptr);
            newEntry->createTime = (uint64_t)now;
            newEntry->accessTime = (uint64_t)now;
            newEntry->modTime = (uint64_t)now;

            newEntry->size = 0;

            for(int i = 0; i < name.length(); i++) {
                newEntry->name[i] = name[i];
            }

            newEntry->name[name.length()] = 0;
            newEntry->entrySize = sizeof(LXFSDirectoryEntry)-512 + name.length() + 1;

            // now we can construct the dir header
            vector<uint8_t> data(BLOCK_SIZE_BYTES);
            LXFSDirectoryHeader *header = (LXFSDirectoryHeader *)data.data();
            header->sizeEntries = 0;
            header->sizeBytes = sizeof(LXFSDirectoryHeader);
            header->accessTime = (uint64_t)now;
            header->accessTime = (uint64_t)now;
            header->modTime = (uint64_t)now;
            header->reserved = 0;

            // allocate blocks for the directory
            newEntry->block = allocateNewBlocks(disk, partition, 1);

            // update the table on disk
            uint64_t tableBlock = parentEntry->block;
            tableBlock += (offset/BLOCK_SIZE_BYTES);
            uint64_t blockOffset = (offset%BLOCK_SIZE_BYTES) / BLOCK_SIZE_BYTES;
            writeBlock(disk, partition, tableBlock, 1, parentData.data() + blockOffset);

            // and now write the directory header
            writeNextBlock(disk, partition, newEntry->block, data.data());

            return 0;
        } else {
            newEntry = (LXFSDirectoryEntry *)((char *)newEntry + newEntry->entrySize);
            offset += newEntry->entrySize;

            // TODO: reallocate these blocks as necessary when growing
        }
    }

    return 0;
}