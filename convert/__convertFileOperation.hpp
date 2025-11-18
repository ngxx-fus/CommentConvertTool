#ifndef __CONVERT_FILEOP_H__
#define __CONVERT_FILEOP_H__

#include <bits/stdc++.h>
#include <filesystem>
#include <fstream>

using namespace std;
namespace fs = std::filesystem;

#define END_OF_FILE -1

fstream fin;
fstream fout;

Status finOpen(const string& path) {
    fin.open(path, ios::in);
    if (!fin.is_open()) {
        cerr << "[ERR] Cannot open Input: " << path << endl;
        return STATUS_ERR;
    }
    return STATUS_OKE;
}

char finReadChar() {
    char c;
    if (!fin.get(c)) {
        return 0;
    }
    return c;
}

int finIsEOF() {
    return fin.eof() ? 1 : 0;
}

Status finClose() {
    if (fin.is_open()) {
        fin.close();
    }
    return STATUS_OKE;
}

Status foutOpen(const string& path) {
    fs::path p = path;
    if (p.has_parent_path()) {
        if (!fs::exists(p.parent_path())) {
            fs::create_directories(p.parent_path());
        }
    }

    fout.open(path, ios::out | ios::trunc);
    if (!fout.is_open()) {
        cerr << "[ERR] Cannot open Output: " << path << endl;
        return STATUS_ERR;
    }
    return STATUS_OKE;
}

Status foutWriteChar(char c) {
    if (!fout.is_open()) return STATUS_ERR;
    
    fout.put(c);
    return STATUS_OKE;
}

Status foutWriteBuffer(const string& buffer) {
    if (!fout.is_open()) return STATUS_ERR;
    
    if (buffer.empty()) return STATUS_OKE;

    // write(const char* s, streamsize n)
    fout.write(buffer.data(), buffer.size());
    
    return STATUS_OKE;
}

Status foutClose() {
    if (fout.is_open()) {
        fout.close();
    }
    return STATUS_OKE;
}


#endif