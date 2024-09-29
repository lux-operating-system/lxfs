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

    //cout << name << endl;

    // first load the source file
    ifstream file(src, ios::in | ios::binary | ios::ate);
    size_t fileSize = file.tellg();
    file.clear();
    file.seekg(0);

    // vector for the file data, rounded up to the nearest block + 1
    size_t sizeBlocks = (fileSize + BLOCK_SIZE_BYTES - 1) / BLOCK_SIZE_BYTES;
    sizeBlocks++;       // one extra block for metadata
    vector<uint8_t> rawData(sizeBlocks*BLOCK_SIZE_BYTES);
    file.read((char *)rawData.data() + BLOCK_SIZE_BYTES, fileSize);
    file.close();

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
            newEntry->flags |= (LXFS_DIR_TYPE_FILE << LXFS_DIR_TYPE_SHIFT);
            newEntry->flags |= (name.length() - 1) << 3;

            newEntry->owner = LXFS_USER_ROOT;
            newEntry->group = LXFS_USER_ROOT;
            newEntry->permissions = LXFS_DEFAULT_PERMS;

            time_t now = time(nullptr);
            newEntry->createTime = (uint64_t)now;
            newEntry->accessTime = (uint64_t)now;
            newEntry->modTime = (uint64_t)now;

            newEntry->size = fileSize;

            for(int i = 0; i < name.length(); i++) {
                newEntry->name[i] = name[i];
            }

            newEntry->name[name.length()] = 0;
            newEntry->entrySize = sizeof(LXFSDirectoryEntry)-512 + name.length() + 1;

            // now we can construct the file header
            LXFSFileHeader *header = (LXFSFileHeader *)rawData.data();
            header->size = fileSize;
            header->refCount = 1;   // TODO: fix this after implementing hard links

            // allocate blocks for the file
            newEntry->block = allocateNewBlocks(disk, partition, sizeBlocks);

            //cout << "file blocks start at " << newEntry->block << endl;

            // update the table on disk
            uint64_t tableBlock = parentEntry->block;
            tableBlock += (offset/BLOCK_SIZE_BYTES);
            uint64_t blockOffset = (offset%BLOCK_SIZE_BYTES) / BLOCK_SIZE_BYTES;
            writeBlock(disk, partition, tableBlock, 1, parentData.data() + blockOffset);

            //cout << "updating block " << dec << tableBlock << endl;
            //cout << "parent dir started at " << dec << parentEntry->block << endl;

            // and now write the actual file contents
            uint64_t block = newEntry->block;
            for(size_t i = 0; i < sizeBlocks; i++) {
                //cout << "writing block " << dec << block << endl;
                block = writeNextBlock(disk, partition, block, rawData.data() + (i * BLOCK_SIZE_BYTES));
            }

            return 0;
        } else {
            newEntry = (LXFSDirectoryEntry *)((char *)newEntry + newEntry->entrySize);
            offset += newEntry->entrySize;

            // TODO: reallocate these blocks as necessary when growing
        }
    }

    return -1;
}