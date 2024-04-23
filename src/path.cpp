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

string splitPath(string path, int index) {
    if(!index) return path;

    //cout << "split " << path << " as part " << dec << index << endl;

    int current = 0;
    int start = 0;
    int end = path.length()-1;
    for(int i = 0; i < path.length(); i++) {
        if(path[i] == '/') {
            //cout << "found / at position " << dec << i << endl;
            current++;

            if(current == index) start = i+1;
            else if(current == index+1) end = i-start;
        }
    }

    //cout << "start " << dec << start << " end " << dec << end << endl;
    return path.substr(start, end);
}