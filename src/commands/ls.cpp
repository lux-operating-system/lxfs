/*
 * lux - a lightweight unix-like operating system
 * Omar Elghoul, 2024
 * 
 * User-level implementation of the lux file system
 */

#include <lxfs.h>

int lsHelp(char *name) {
    cerr << "Usage: " << name << " ls [disk image] [partition] [path]" << endl;
    return -1;
}

void printEntry(LXFSDirectoryEntry *entry, char *override) {
    if(entry->flags & LXFS_DIR_VALID) {
        switch((entry->flags >> LXFS_DIR_TYPE_SHIFT) & LXFS_DIR_TYPE_MASK) {
        case LXFS_DIR_TYPE_DIR:
            cout << "d";
            break;
        case LXFS_DIR_TYPE_HARD_LINK:
            cout << "l";
            break;
        default:
            cout << "-";
        }
    }

    // owner perms
    cout << (entry->permissions & LXFS_PERMS_OWNER_R ? "r" : "-");
    cout << (entry->permissions & LXFS_PERMS_OWNER_W ? "w" : "-");
    cout << (entry->permissions & LXFS_PERMS_OWNER_X ? "x" : "-");
    
    // group perms
    cout << (entry->permissions & LXFS_PERMS_GROUP_R ? "r" : "-");
    cout << (entry->permissions & LXFS_PERMS_GROUP_W ? "w" : "-");
    cout << (entry->permissions & LXFS_PERMS_GROUP_X ? "x" : "-");

    // Other perms
    cout << (entry->permissions & LXFS_PERMS_OTHER_R ? "r" : "-");
    cout << (entry->permissions & LXFS_PERMS_OTHER_W ? "w" : "-");
    cout << (entry->permissions & LXFS_PERMS_OTHER_X ? "x" : "-");

    cout << " ";

    // owner ID and group ID
    // for now just print the actual numbers
    cout << hex << setw(4) << setfill('0') << entry->owner << " ";
    cout << hex << setw(4) << setfill('0') << entry->group << " ";

    // size
    cout << dec << setw(4) << setfill(' ') << entry->size << " ";

    // TODO: human-readable date and time go here

    // name
    if(override) {
        cout << override;
    } else {
        cout << entry->name;
    }

    cout << endl;
}

int ls(int argc, char **argv) {
    if(argc < 5) return lsHelp(argv[0]);

    string disk = argv[2];
    int partition = stoi(argv[3]);
    string path = argv[4];

    bool isRootDirectory = (countPath(path) <= 1);

    // find the path
    LXFSDirectoryEntry *entry = new LXFSDirectoryEntry;
    if(!findEntry(disk, partition, path, entry)) {
        cerr << "cannot list for non-existent directory" << endl;
        return -1;
    }

    // make sure it is a directory
    if(((entry->flags >> LXFS_DIR_TYPE_SHIFT) & LXFS_DIR_TYPE_MASK) != LXFS_DIR_TYPE_DIR) {
        cerr << "this entry is not a directory" << endl;
        return -1;
    }

    // now list the directory
    cout << "directory listing for " << path << ":" << endl;

    // first with the directory itself
    printEntry(entry, ".");

    // and then its children by loading the directory
    uint64_t current = entry->block;
    size_t size = 0, newSize = 0;
    vector<uint8_t> data(0);

    while(current != LXFS_BLOCK_EOF) {
        newSize += BLOCK_SIZE_BYTES;
        data.resize(newSize);
        current = readNextBlock(disk, partition, current, data.data() + size);
        size = newSize;
    }

    LXFSDirectoryEntry *child = (LXFSDirectoryEntry *)((char *)data.data() + sizeof(LXFSDirectoryHeader));
    size_t offset = sizeof(LXFSDirectoryHeader);
    while(offset < size) {
        if(!child->flags) return 0;
        if(child->flags & LXFS_DIR_VALID) {
            printEntry(child, nullptr);
            child = (LXFSDirectoryEntry *)((char *)child + child->entrySize);
        }
    }

    return 0;
}