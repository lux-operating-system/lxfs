/*
 * lux - a lightweight unix-like operating system
 * Omar Elghoul, 2024
 * 
 * User-level implementation of the lux file system
 */

#include <lxfs.h>

int countPath(string path) {
    int count = 0;

    for(int i = 0; i < path.length(); i++) {
        if(path[i] == '/') {
            count++;
        }
    }

    return count;
}
