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

## 4. Partition
_lxfs_ assumes it is on a sector-addressable physical medium. For backwards compatibility, _lxfs_ also leaves space for a traditional boot loader running from the boot sector, for legacy x86 systems.

### 4.1. Outline
The following table outlines the overall structure of _lxfs_ on its partition. Assume that _n_ refers to the block size, in number of sectors. _m_ refers to the size of the block allocation table in sectors. The calculation regarding this is discussed further below, in the block allocation table section. _z_ refers to however many sectors are left until the end of the partition.

| Sector | Sector Count | Description |
| ------ | ----- | ----------- |
| 0 | 1 | Identification sector, as well as boot sector for legacy x86 systems |
| 1 | n-1 | Reserved sectors – may be used in future expansions |
| n | 32n | Boot loader blocks |
| 33n | m | Block allocation table |
| 33n+m | z | Blocks – i.e. file and directory data sectors |

### 4.2. Identification and boot sector
The first sector of an _lxfs_ partition is the sector identifying the partition's parameters, as well as optionally containing a boot loader for legacy x86 systems. While physical sectors may be 512 or 4096 bytes depending on the underlying medium containing them, this sector and this sector alone is assumed to be 512 bytes in size for backwards compatibility. Sector size is outside the scope of _lxfs_ but is assumed in the definition of the boot sector in the case of running a BIOS-based boot program. In case of a sector size larger than 512, the contents after byte 511 are undefined and unused by _lxfs_.

Offset and size are given in bytes for all the following tables.

| Offset | Size | Description |
| ------ | ---- | ----------- |
| 0 | 4 | (Optional) Boot code for an x86 boot loader |
| 4 | 4 | Partition identification, `'LXFS'` or `0x5346584C` |
| 8 | 8 | Last mount time, Unix time in seconds |
| 16 | 8 | Size of partition in blocks |
| 24 | 8 | First block of the root directory |
| 32 | 1 | Medium and partition parameters |
| 33 | 15 | (Optional) Volume name, ASCII text |
| 48 | 462 | (Optional) Boot code for an x86 boot loader |
| 510 | 2 | (Optional) Boot signature for an x86 boot loader, `0xAA55` |

* **Size of partition in blocks:** This field contains the total size of the partition in blocks, including the identification sector, reserved sectors, boot loader blocks, and all other contents of the partition. This can also be used to calculate the size of the block allocation table in sectors. The size of the block allocation table is given as:

    ```
    ceil( SIZE_IN_BLOCKS * 8 / BYTES_PER_SECTOR )
    ```

* **Medium and partition parameters:** This field is one byte that identifies details about the partition and the underlying medium. It is defined as follows.

    | Bit Offset | Bit Count | Description |
    | ---------- | --------- | ----------- |
    | 0 | 1 | Bootable bit |
    | 1 | 2 | Sector size |
    | 3 | 4 | Sectors per block |
    | 7 | 1 | Reserved – should be zero until future expansion |

    * **Bootable bit:** 