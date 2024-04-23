/*
 * lux - a lightweight unix-like operating system
 * Omar Elghoul, 2024
 * 
 * User-level implementation of the lux file system
 */

#include <lxfs.h>

bool findEntry(string disk, int partition, string path, LXFSDirectoryEntry *dest) {
    int pathSize = countPath(path);

    if(pathSize == 1) {
        // root directory
        uint64_t start = rootDirectoryBlock(disk, partition);

        vector<uint8_t> rootBlock(BLOCK_SIZE_BYTES);
        readBlock(disk, partition, start, 1, rootBlock.data());
        LXFSDirectoryHeader *rootHeader = (LXFSDirectoryHeader *)rootBlock.data();

        dest->createTime = rootHeader->createTime;
        dest->accessTime = rootHeader->accessTime;
        dest->modTime = rootHeader->modTime;

        dest->flags = LXFS_DIR_VALID;
        dest->flags |= (LXFS_DIR_TYPE_DIR << LXFS_DIR_TYPE_SHIFT);
        dest->owner = LXFS_USER_ROOT;
        dest->group = LXFS_USER_ROOT;
        dest->permissions = LXFS_DEFAULT_PERMS;

        dest->size = rootHeader->sizeEntries;
        dest->entrySize = sizeof(LXFSDirectoryEntry);
        dest->block = start;

        return true;
    } else {
        // non-root directories, we're gonna have to follow the hierarchy
        LXFSDirectoryEntry *entry = new LXFSDirectoryEntry;
        findEntry(disk, partition, "/", entry);

start:
        // from here on it's generic
        if(((entry->flags >> LXFS_DIR_TYPE_SHIFT) & LXFS_DIR_TYPE_MASK) != LXFS_DIR_TYPE_DIR) {
            cerr << "a non-directory cannot have children" << endl;
            return -1;
        }

        uint64_t current = entry->block;;
        size_t size = 0, newSize = 0;
        vector<uint8_t> data(0);

        while(current != LXFS_BLOCK_EOF) {
            newSize += BLOCK_SIZE_BYTES;
            data.resize(newSize);
            current = readNextBlock(disk, partition, current, data.data() + size);
            size = newSize;
        }

        // and start reading from it
        entry = (LXFSDirectoryEntry *)((char *)data.data() + sizeof(LXFSDirectoryHeader));

        for(int i = 1; i < pathSize; i++) {
            bool found = false;
            string dir = splitPath(path, i);
            
            // now search this directory for the entry
            while(entry->flags) {
                if(entry->flags & LXFS_DIR_VALID) {
                    if(!strcmp(dir.c_str(), (char *)entry->name)) {
                        found = true;
                        break;
                    }
                } else {
                    entry = (LXFSDirectoryEntry *)((char *)entry + entry->entrySize);
                }
            }

            if(found) {
                if(i == pathSize-1) {
                    memcpy(dest, entry, entry->entrySize);
                    return true;
                } else {
                    goto start;
                }
            }
        }

        // file doesn't exist here
        return false;
    }
}