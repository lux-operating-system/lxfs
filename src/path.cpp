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

    if(path.length() > 1 && path[path.length()-1] != '/') {
        return count+1;
    }

    return count;
}
