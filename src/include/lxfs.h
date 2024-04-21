/*
 * lux - a lightweight unix-like operating system
 * Omar Elghoul, 2024
 * 
 * User-level implementation of the lux file system
 */

#pragma once

#include <iostream>
#include <iomanip>
#include <fstream>
#include <cstddef>      // size_t
#include <cstdint>      // integers
#include <string>
#include <vector>

using namespace std;

#define SECTOR_SIZE         512     // default
#define BLOCK_SIZE          4       // 4 sectors per block, just a default
#define BLOCK_SIZE_BYTES    (SECTOR_SIZE*BLOCK_SIZE)

int create(int, char **);
int format(int, char **);
int part(int, char **);

void *readBytes(string, size_t, size_t, void *);
void writeBytes(string, size_t, size_t, void *);
void *readSector(string, size_t, size_t, void *);
void writeSector(string, size_t, size_t, void *);
void *readBlock(string, int, size_t, size_t, void *);
void writeBlock(string, int, size_t, size_t, void *);

typedef struct {
    uint8_t flags;
    uint8_t chsStart[3];
    uint8_t id;
    uint8_t chsEnd[3];
    uint32_t start;
    uint32_t size;
} __attribute__((packed)) MBRPartition;

#define MBR_PARTITION_OFFSET        446
#define MBR_FLAG_BOOTABLE           0x80
#define MBR_ID_LXFS                 0xF3

typedef struct {
    uint8_t bootCode1[4];
    uint32_t identifier;
    uint64_t volumeSize;
    uint64_t rootBlock;
    uint8_t parameters;
    uint8_t version;
    uint8_t name[16];
    uint8_t reserved[6];

    // more boot code follows
} __attribute__((packed)) LXFSIdentification;

#define LXFS_MAGIC                  0x5346584C  // 'LXFS', little endian
#define LXFS_VERSION                0x01

#define LXFS_ID_BOOTABLE            0x01
#define LXFS_ID_SECTOR_SIZE_SHIFT   1
#define LXFS_ID_SECTOR_SIZE_MASK    0x03
#define LXFS_ID_BLOCK_SIZE_SHIFT    3
#define LXFS_ID_BLOCK_SIZE_MASK     0x0F

typedef struct {
    uint32_t identifier;
    uint32_t cpuArch;
    uint64_t timestamp;
    uint8_t description[32];
    uint8_t reserved[16];
} __attribute__((packed)) LXFSBootHeader;

#define LXFS_CPU_X86                0x00000001
#define LXFS_CPU_X86_64             0x00000002
#define LXFS_CPU_MIPS32             0x00000003
#define LXFS_CPU_MIPS64             0x00000004

#define LXFS_BLOCK_FREE             0x0000000000000000
#define LXFS_BLOCK_ID               0xFFFFFFFFFFFFFFFC
#define LXFS_BLOCK_BOOT             0xFFFFFFFFFFFFFFFD
#define LXFS_BLOCK_TABLE            0xFFFFFFFFFFFFFFFE
#define LXFS_BLOCK_EOF              0xFFFFFFFFFFFFFFFF

typedef struct {
    uint64_t createTime;
    uint64_t modTime;
    uint64_t accessTime;
    uint64_t sizeEntries;
    uint64_t sizeBytes;
    uint64_t reserved;
} __attribute__((packed)) LXFSDirectoryHeader;

typedef struct {
    uint16_t flags;

    uint16_t owner;
    uint16_t group;
    uint16_t permissions;
    uint64_t size;

    uint64_t createTime;
    uint64_t modTime;
    uint64_t accessTime;

    uint64_t block;
    uint8_t reserved[16];
    uint8_t name[];
} __attribute__((packed)) LXFSDirectoryEntry;

#define LXFS_DIR_VALID              0x0001
#define LXFS_DIR_TYPE_SHIFT         1
#define LXFS_DIR_TYPE_MASK          0x03

#define LXFS_DIR_TYPE_FILE          0x00
#define LXFS_DIR_TYPE_DIR           0x01
#define LXFS_DIR_TYPE_SOFT_LINK     0x02
#define LXFS_DIR_TYPE_HARD_LINK     0x03

#define LXFS_PERMS_OWNER_R          0x0001
#define LXFS_PERMS_OWNER_W          0x0002
#define LXFS_PERMS_OWNER_X          0x0004
#define LXFS_PERMS_GROUP_R          0x0008
#define LXFS_PERMS_GROUP_W          0x0010
#define LXFS_PERMS_GROUP_X          0x0020
#define LXFS_PERMS_OTHER_R          0x0040
#define LXFS_PERMS_OTHER_W          0x0080
#define LXFS_PERMS_OTHER_X          0x0100

#define LXFS_USER_ROOT              0x0000

typedef struct {
    uint64_t size;
    uint64_t refCount;
} __attribute__((packed)) LXFSFileHeader;

