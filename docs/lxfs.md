# Lightweight Unix File System Specification
This document outlines the structure that defines the Lightweight Unix File System, or _lxfs_. As the name implies, the file system was created specifically for use with _lux_.

## 1. Vision
_lxfs_ aims to achieve the best of both worlds from an OS developmental perspective. It aims to combine the simplicity of the FAT file system with the security, power, and hierarchical structure of Unix-like file systems. 

## 2. Design
In line with the vision, _lxfs_ is based on the concept of allocating blocks of a fixed size to the files and directories on the parititon. There is a block allocation table, analogous to the file allocation table of the FAT family of file systems, where the value indexed by each block contains a reference to the next block of said file or directory. 

Unlike FAT, however, which is limited to 28-bit addressing even on the so-called 32-bit FAT32 file system, _lxfs_ uses true 64-bit addressing for blocks, allowing a theoretical maximum of 2^64 blocks on a parititon. Depending on the sector size of the physical drive containing the partition and the number of sectors allocated per block, this figure can easily be multiplied by up to 16 in terms of physical sector count.

The directory structure of _lxfs_ also natively supports long file names that are not limited to 7-bit ASCII characters, working around a painfully common shortcoming of the FAT family. In line with _lux_ being a Unix-like operating system, the directory structure also natively supports the traditional Unix file permissions.

_lxfs_ uses little-endian ordering for all multibyte values.

## 3. Identification
Parititons formatted with _lxfs_ can be identified via the Master Boot Record or the GUID Partition Table depending on how the medium is formatted. In the case of Master Boot Records, the partition ID identifying _lxfs_ is `0xF3`. On a GUID Partition Table, _lxfs_ is defined by the UUID `25EB9A8C-E69D-4F3B-AE09-D8FBEA1E02A0`.

## 4. Structure
_lxfs_ assumes it is on a sector-addressable physical medium. For backwards compatibility, _lxfs_ also leaves space for a traditional boot loader running from the boot sector, for legacy x86 systems.

### 4.1. Outline
The following table outlines the overall structure of _lxfs_ on its partition. Assume that _n_ refers to the block size, in number of sectors. _m_ refers to the size of the block allocation table in sectors. The calculation regarding this is discussed further below, in the block allocation table section. _z_ refers to however many sectors are left until the end of the partition.

_Table 1: Partition structure outline_

| Sector | Count | Description |
| ------ | ----- | ----------- |
| 0 | 1 | Identification sector, as well as boot sector for legacy x86 systems |
| 1 | n-1 | Reserved sectors – may be used in future expansions |
| n | 32n | Boot loader blocks |
| 33n | m | Block allocation table |
| 33n+m | z | Blocks – i.e. file and directory data sectors |

### 4.2. Identification and boot sector
The first sector of an _lxfs_ partition is the sector identifying the partition's parameters, as well as optionally containing a boot loader for legacy x86 systems. While physical sectors may be 512 or 4096 bytes depending on the underlying medium containing them, this sector and this sector alone is assumed to be 512 bytes in size for backwards compatibility. Sector size is outside the scope of _lxfs_ but is assumed in the definition of the boot sector in the case of running a BIOS-based boot program. In case of a sector size larger than 512, the contents after byte 511 are undefined and unused by _lxfs_.

Offset and size are given in bytes for all the following tables.

_Table 2: Identification and boot sector_

| Offset | Size | Description |
| ------ | ---- | ----------- |
| 0 | 4 | (Optional) Boot code for an x86 boot loader |
| 4 | 4 | Partition identification, `'LXFS'` or `0x5346584C` |
| 8 | 8 | Timestamp of last access, Unix time, seconds |
| 16 | 8 | Size of partition in blocks |
| 24 | 8 | First block of the root directory |
| 32 | 1 | Medium and partition parameters |
| 33 | 15 | (Optional) Volume name, ASCII text |
| 48 | 462 | (Optional) Boot code for an x86 boot loader |
| 510 | 2 | (Optional) Boot signature for an x86 boot loader, `0xAA55` |

* **Size of partition in blocks:** This field contains the total size of the partition in blocks, including the identification sector, reserved sectors, boot loader blocks, and all other contents of the partition. This can also be used to calculate the size of the block allocation table in sectors. The size of the block allocation table is given as:

    ```
    BLOCK_ALLOCATION_TABLE_SIZE_SECTORS = ceil( SIZE_IN_BLOCKS * 8 / BYTES_PER_SECTOR )
    ```

* **Medium and partition parameters:** This field is one byte that identifies details about the partition and the underlying medium. It is defined as follows.

    _Table 3: Medium and partition parameters_

    | Bit Offset | Bit Count | Description |
    | ---------- | --------- | ----------- |
    | 0 | 1 | Boot flag |
    | 1 | 2 | Sector size |
    | 3 | 4 | Sectors per block |
    | 7 | 1 | Reserved – should be zero until future expansion |

    * **Boot flag:** One indicates that there is a boot program within the boot loader blocks.
    * **Sector size:** Size of the physical sector in bytes given by the following formula. This allows for sector sizes of 512, 1024, 2048, and 4096.
        ```
        BYTES_PER_SECTOR = 512 << SECTOR_SIZE
        ```
    * **Sectors per block:** Number of physical sectors per block minus 1. As such, this value ranging from 0 to 15 allows for block sizes of 1 to 16 sectors per block.

### 4.3. Boot loader blocks
Blocks 1 through 32 inclusive are reserved for an optional boot program, as indicated by the boot flag in the identification sector. If the boot flag is set to zero, the contents of the boot loader blocks are undefined and should not be used.

Due to the variability in sector size and block size, this program may be limited to as little as 16 KB (assuming one sector per block and a sector size of 512 bytes) or as much as 2 MB (assuming 16 sectors per block and a maximum sector size of 4096 bytes). Because of this disparsity that can ultimately depend on the physical device _lxfs_ is formatted on, it is highly recommended to keep boot programs below 16 KB, or better yet, to store them on files rather than the reserved boot loader blocks.

The first block contains a short identification header structure regarding the boot program present immediately after it. Assume _n_ is however many bytes are occupied by the boot program, which can be structured in any format as deemed useful by the boot program authors.

_Table 4: Boot loader header structure_

| Offset | Size | Description |
| ------ | ---- | ----------- |
| 0 | 4 | LXFS identification, `'LXFS'` or `0x5346584C` |
| 4 | 4 | Architecture family of the boot program |
| 8 | 8 | Timestamp of last modification, Unix time, seconds |
| 16 | 32 | (Optional) Description of the operating system installed, ASCII text |
| 48 | 16 | Reserved for alignment |
| 64 | n | Boot code |

* **Architecture family:** This field determines the CPU architecture that the boot program is written for. It can take on several values.

    _Table 5: Boot loader CPU architecture family_

    | Value | Description |
    | ----- | ----------- |
    | `0x00000001` | 32-bit x86 |
    | `0x00000002` | 64-bit x86 |
    | `0x00000003` | 32-bit MIPS |
    | `0x00000004` | 64-bit MIPS |

    All other values are undefined and are reserved for future expansion as deemed necessary.

### 4.4. Block allocation table
The block allocation table is the most integral component to the _lxfs_ file system and is analogous to the file allocation table of the FAT family of file systems. It is merely an array that indicates both whether or not a block is used, as well as what the next block in the chain of blocks is.

The size of the partition in blocks is given by the identification sector, and the size of the block allocation table can thus be calculated according to the formula shown in _section 4.2_.

In essense, the block allocation table is merely an array of 64-bit integers, where the number of entries of the array is equal to the number of blocks in the partition. Each value in this array indicates the next block in the chain of blocks containing a given file or directory. For example, if a file starts at block 729 which is read, and then the array entry at index 729 contains 822, then the next block to be read would be 822, and then the subsequent block would be given by index 822, and so forth until the terminal block is reached.

Note that the first 33 blocks of the partition are always reserved for the boot program, and then the number of blocks reserved for the block allocation table itself is also variable according to the partition size. For this reason, there are several special values for block numbers that are reserved to signify end-of-file and reserved blocks.

_Table 6: Block allocation table special values_

| Value | Description |
| ----- | ----------- |
| `0x0000000000000000` | Empty block that can be allocated to a new file or directory |
| `0x0000000000000001` | Unusable block – reserved for identification block (i.e. block 0) |
| `0x0000000000000002 - 0x0000000000000020` | Unusable block - reserved for future expansion |
| `0xFFFFFFFFFFFFFF00 - 0xFFFFFFFFFFFFFFFC` | Unusable block – reserved for future expansion |
| `0xFFFFFFFFFFFFFFFD` | Unusable block – reserved for boot loader blocks (i.e. blocks 1-32) |
| `0xFFFFFFFFFFFFFFFE` | Unusable block – reserved for block allocation table |
| `0xFFFFFFFFFFFFFFFF` | Terminal block in a chain (i.e. end-of-file) |

All other values not shown in the table indicate the next block in the chain of succession.

