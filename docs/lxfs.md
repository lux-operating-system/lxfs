# Lightweight Unix File System Specification
This document outlines the structure that defines the Lightweight Unix File System, or _lxfs_. As the name implies, the file system was created specifically for use with _lux_.

## 1. Vision
_lxfs_ aims to achieve the best of both worlds from an OS developmental perspective. It aims to combine the simplicity of the FAT file system with the security, power, and hierarchical structure of Unix-like file systems. 

## 2. Design
In line with the vision, _lxfs_ is based on the concept of allocating blocks of a fixed size to the files and directories on the parititon. There is a block allocation table, analogous to the file allocation table of the FAT family of file systems, where the value indexed by each block contains a reference to the next block of said file or directory. 

Unlike FAT, however, which is limited to 28-bit addressing even on the so-called 32-bit FAT32 file system, _lxfs_ uses true 64-bit addressing for blocks, allowing a theoretical maximum of 2^64 blocks on a parititon. Depending on the sector size of the physical drive containing the partition and the number of sectors allocated per block, this figure can easily be multiplied by up to 16 in terms of physical sector count.

The directory structure of _lxfs_ also natively supports long file names that are not limited to 7-bit ASCII characters, working around a painfully common shortcoming of the FAT family. In line with _lux_ being a Unix-like operating system, the directory structure also natively supports the traditional Unix file permissions.

## 3. Identification
Parititons formatted with _lxfs_ can be identified via the Master Boot Record or the GUID Partition Table depending on how the medium is formatted. In the case of Master Boot Records, the partition ID identifying _lxfs_ is `0xF3`. On a GUID Partition Table, _lxfs_ is defined by the UUID `25EB9A8C-E69D-4F3B-AE09-D8FBEA1E02A0`.

## 4. Partition
