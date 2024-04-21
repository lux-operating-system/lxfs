/*
 * lux - a lightweight unix-like operating system
 * Omar Elghoul, 2024
 * 
 * User-level implementation of the lux file system
 */

#include <lxfs.h>

void *readBytes(string filename, size_t start, size_t count) {
    vector<uint8_t> buffer(count);
    ifstream file(filename, ios::in | ios::binary);
    file.seekg(start);
    file.read((char *)buffer.data(), count);
    file.close();

    return buffer.data();
}

void writeBytes(string filename, size_t start, size_t count, void *buffer) {
    fstream file(filename, ios::out | ios::binary | ios::in);
    file.seekg(start);
    file.write((const char *)buffer, count);
    file.close();
}

void *readSector(string filename, size_t start, size_t count) {
    return readBytes(filename, start*SECTOR_SIZE, count*SECTOR_SIZE);
}

void writeSector(string filename, size_t start, size_t count, void *buffer) {
    writeBytes(filename, start*SECTOR_SIZE, count*SECTOR_SIZE, buffer);
}
