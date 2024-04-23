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

string parentPath(string path) {
    int count = countPath(path);
    if(count <= 1) return path;     // root has no parent
    count -= 1;

    int currentCount = 0;
    string parent = "";

    for(int i = 0; i < path.length(); i++) {
        parent += path[i];

        if(path[i] == '/') {
            currentCount++;
            if(currentCount >= count) return parent;
        }
    }

    return parent;
}

string finalPath(string path) {
    int count = countPath(path);
    if(count <= 1) return path;     // again for root
    
    string parent = parentPath(path);
    return path.substr(parent.length(), path.length()-parent.length());
}