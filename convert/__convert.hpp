#ifndef __CONVERT_H__
#define __CONVERT_H__

#include <bits/stdc++.h>
#include <filesystem> 
#include <algorithm>

using namespace std;
namespace fs = std::filesystem;

#include "./__config.hpp"
#include "./__convertGlobalUtil.hpp"
#include "./__convertFileOperation.hpp"

enum state0_t {
    normal_code = 0,
    normal_comment,
    normal_comment_single_line,
    normal_comment_multi_line,
    doxygen_comment,
};

/// @brief Recursively scans the input directory for source files.
/// @details Filters for .h, .hpp, .c, .cpp extensions case-insensitively.
void scanFiles() {
    InputFilename.clear();
    
    if (!fs::exists(INPUT_ROOTDIR)) {
        cout << "[WARN] Directory " << INPUT_ROOTDIR << " does not exist.\n";
        return;
    }

    // Recursive directory iterator
    for (const auto& entry : fs::recursive_directory_iterator(INPUT_ROOTDIR)) {
        if (entry.is_regular_file()) {
            string ext = entry.path().extension().string();
            
            // Normalize extension to lowercase
            string ext_lower = ext;
            transform(ext_lower.begin(), ext_lower.end(), ext_lower.begin(), ::tolower);

            if (ext_lower == ".h" || ext_lower == ".hpp" || ext_lower == ".c" || ext_lower == ".cpp") {
                // Get relative path and normalize separators
                string relPath = fs::relative(entry.path(), INPUT_ROOTDIR).string();
                replace(relPath.begin(), relPath.end(), '\\', '/');
                InputFilename.push_back(relPath);
            }
        }
    }
}

void Print_Info(){
    printf("Input file paths:\n");
    for (auto s:InputFilename){
        cout << "\t -> " << INPUT_ROOTDIR << s << '\n';
    }
    printf("Output file paths:\n");
    for (auto s:InputFilename){
        cout << "\t <- " << OUTPUT_ROOTDIR << s << '\n';
    }
}

/// @brief Parses a Bosch-style comment block.
/// @details Detects File/Author/Date/Param/Return tags. 
///          Sets MASK_OTHERS if unstructured text (Copyright, Config) is found.
/// @param s Raw comment string.
/// @param brief Output for @brief/@file.
/// @param param Output for @param/@author/@date.
/// @param ret Output for @return.
/// @return Mask of found sections.
int parseBoschComent(string s, string &brief, vector<string> &param, string &ret) {
    const int maxEqualSymNum = 3;
    
    // Function Patterns
    const string pat_def    = pattern_definition;
    const string pat_desc   = pattern_description;
    const string pat_in     = pattern_input_param;
    const string pat_in_div = pattern_input_param_delimiter;
    const string pat_out    = pattern_output_param;

    // File Header Patterns
    const string pat_file1  = "File:";
    const string pat_file2  = "Filename:";
    const string pat_auth   = "Author:";
    const string pat_date   = "Date:";
    const string pat_ver    = "Version:";
    
    int currentSection = 0; // 0:Header/None, 1:Def, 2:In, 3:Out, 4:Desc
    int foundMask = MASK_NONE;

    // Store definitions found to map generic descriptions later
    vector<string> extractedParams; 
    string genericInputDesc = "";

    auto is_separator = [&](const string& line) -> bool {
        long countEq = count(line.begin(), line.end(), '=');
        long countAst = count(line.begin(), line.end(), '*');
        return (countEq > maxEqualSymNum) || (countAst > 10); // Handle ****** lines too
    };

    auto clean_line = [&](string t) -> string {
        // Remove markers inside text
        string markers[] = {"//", "/*", "*/"};
        for (const auto& m : markers) {
            size_t pos;
            while ((pos = t.find(m)) != string::npos) t.replace(pos, m.length(), " ");
        }
        // Trim
        size_t first = t.find_first_not_of(" \t\n\r*/");
        if (string::npos == first) return "";
        size_t last = t.find_last_not_of(" \t\n\r");
        return t.substr(first, (last - first + 1));
    };

    // Lambda to extract parameter names from "void func(const uint8_t* data, int len)"
    auto extract_params = [&](string defLine) {
        size_t openP = defLine.find('(');
        size_t closeP = defLine.find(')');
        if (openP != string::npos && closeP != string::npos && closeP > openP) {
            string args = defLine.substr(openP + 1, closeP - openP - 1);
            stringstream ssArg(args);
            string segment;
            while (getline(ssArg, segment, ',')) {
                string token, lastToken;
                stringstream ssToken(segment);
                // Iterate tokens to find variable name (usually the last one)
                while (ssToken >> token) {
                    // Skip keywords
                    if (token == "const" || token == "volatile" || token == "struct" || token == "enum") continue;
                    lastToken = token;
                }
                
                // Cleanup pointer/ref/array chars from name: * & [ ]
                string cleanToken;
                for (char c : lastToken) {
                    if (c != '*' && c != '&' && c != '[' && c != ']') cleanToken += c;
                }
                
                if (!cleanToken.empty() && cleanToken != "void") {
                    extractedParams.push_back(cleanToken);
                }
            }
        }
    };

    stringstream ss(s);
    string line;

    while (getline(ss, line)) {
        if (is_separator(line)) continue;

        // --- 1. HEADER TAG DETECTION ---
        if (line.find(pat_file1) != string::npos || line.find(pat_file2) != string::npos) {
            string p = (line.find(pat_file1) != string::npos) ? pat_file1 : pat_file2;
            string val = clean_line(line.substr(line.find(p) + p.length()));
            string tag = "/// @file " + val;
            brief = brief.empty() ? tag : tag + "\n" + brief;
            foundMask |= MASK_BRIEF; currentSection = 0; continue;
        }
        if (line.find(pat_auth) != string::npos) {
            string val = clean_line(line.substr(line.find(pat_auth) + pat_auth.length()));
            param.push_back("/// @author " + val);
            foundMask |= MASK_PARAM; currentSection = 0; continue;
        }
        if (line.find(pat_date) != string::npos) {
            string val = clean_line(line.substr(line.find(pat_date) + pat_date.length()));
            param.push_back("/// @date " + val);
            foundMask |= MASK_PARAM; currentSection = 0; continue;
        }
        if (line.find(pat_ver) != string::npos) {
            string val = clean_line(line.substr(line.find(pat_ver) + pat_ver.length()));
            param.push_back("/// @version " + val);
            foundMask |= MASK_PARAM; currentSection = 0; continue;
        }

        // --- 2. FUNCTION TAG DETECTION ---
        if (line.find(pat_def) != string::npos) {
            currentSection = 1;
            line = line.substr(line.find(pat_def) + pat_def.length());
        } else if (line.find(pat_in) != string::npos) {
            currentSection = 2;
            line = line.substr(line.find(pat_in) + pat_in.length());
        } else if (line.find(pat_out) != string::npos) {
            currentSection = 3;
            line = line.substr(line.find(pat_out) + pat_out.length());
        } else if (line.find(pat_desc) != string::npos) {
            currentSection = 4;
            line = line.substr(line.find(pat_desc) + pat_desc.length());
        }

        string content = clean_line(line);
        if (content.empty()) continue;

        // --- 3. CONTENT PARSING ---
        switch (currentSection) {
            case 1: // Definition
            {
                // Extract params immediately from definition line
                extract_params(content); 
                // Definition line itself is metadata, not "Others" noise
                break; 
            }
            case 2: // Input
            {
                size_t dPos = content.find(pat_in_div);
                if (dPos != string::npos) {
                    // Specific param: "Name -- Description"
                    string pName = clean_line(content.substr(0, dPos));
                    string pDesc = clean_line(content.substr(dPos + pat_in_div.length()));
                    param.push_back("/// @param " + pName + " " + pDesc);
                    foundMask |= MASK_PARAM;
                } else {
                    // No separator found. Treat as generic desc if valid text
                    if (content != "None" && content != "void") {
                         if (genericInputDesc.empty()) genericInputDesc = content;
                         else genericInputDesc += " " + content;
                    }
                }
                break;
            }
            case 3: // Output
            {
                if (content == "void" || content == "None") break;
                if (ret.empty()) ret = "/// @return " + content;
                else ret += " " + content;
                foundMask |= MASK_RET;
                break;
            }
            case 4: // Description
            {
                if (brief.empty()) brief = "/// @brief " + content;
                else brief += " " + content;
                foundMask |= MASK_BRIEF;
                break;
            }
            default:
                // Any text here that isn't a Tag is considered "Noise" 
                foundMask |= MASK_OTHERS; 
                break;
        }
    }

    // --- 4. POST PROCESSING: FILL GENERIC PARAMS ---
    // Logic: If we found params in definition but they weren't documented specifically,
    // apply the generic description found in Input section.
    if (!extractedParams.empty() && !genericInputDesc.empty()) {
        for (const string& pName : extractedParams) {
            bool alreadyExists = false;
            // Check if this param was already documented specifically
            // Use " " after pName to ensure we match whole word (e.g. 'val' vs 'value')
            string searchTag = "@param " + pName + " "; 
            for (const string& pTag : param) {
                if (pTag.find(searchTag) != string::npos) {
                    alreadyExists = true;
                    break;
                }
            }
            // If not documented, use the generic description
            if (!alreadyExists) {
                param.push_back("/// @param " + pName + " " + genericInputDesc);
                foundMask |= MASK_PARAM;
            }
        }
    }

    return foundMask;
}

/// @brief Converts file comments using state machine logic.
Status Convert(string finPath, string foutPath) {
    cout << "Convert " << finPath << " ---> " << foutPath << '\n';
    
    if (finOpen(finPath) != STATUS_OKE) return STATUS_ERR;
    if (foutOpen(foutPath) != STATUS_OKE) { finClose(); return STATUS_ERR; }

    int             state = normal_code;
    string          indent;
    char            c = 0;
    string          buffer;
    
    // Parser outputs
    string          doxygenBrief;
    string          doxygenReturn;
    vector<string>  doxygenParam;
    int             retMask = MASK_NONE;

    buffer.reserve(4096);

    do {
        c = finReadChar();
        if (c == 0 && finIsEOF()) { foutWriteBuffer(buffer); break; }
        if (c == 0) continue; 

        buffer.push_back(c);

        switch (state) {
            case normal_code:
                // Start of //
                if (c == '/' && buffer.size() >= 2 && buffer[buffer.size()-2] == '/') {
                    buffer.pop_back(); buffer.pop_back(); 
                    
                    // Capture Indent: Find the last newline to get pure indentation of current line
                    indent.clear();
                    size_t last_nl = buffer.find_last_of('\n');
                    size_t start = (last_nl == string::npos) ? 0 : last_nl + 1;
                    for (size_t i = start; i < buffer.length(); ++i) {
                        if (buffer[i] == ' ' || buffer[i] == '\t') indent += buffer[i]; else break;
                    }

                    foutWriteBuffer(buffer); 
                    buffer.clear();
                    state = normal_comment_single_line;
                }
                // Start of /*
                else if (c == '*' && buffer.size() >= 2 && buffer[buffer.size()-2] == '/') {
                    buffer.pop_back(); buffer.pop_back(); 
                    
                    // Capture Indent
                    indent.clear();
                    size_t last_nl = buffer.find_last_of('\n');
                    size_t start = (last_nl == string::npos) ? 0 : last_nl + 1;
                    for (size_t i = start; i < buffer.length(); ++i) {
                        if (buffer[i] == ' ' || buffer[i] == '\t') indent += buffer[i]; else break;
                    }

                    foutWriteBuffer(buffer);
                    buffer.clear();
                    state = normal_comment_multi_line;
                }
                break;

            case normal_comment_single_line:
                if (c == '\n') {
                    buffer.pop_back(); // Remove newline
                    retMask = parseBoschComent(buffer, doxygenBrief, doxygenParam, doxygenReturn);

                    // LOGIC: Must have valid tags AND NO "Others" (Copyright, etc)
                    // Bypass check: if OTHERS is set, it's likely a license block or unsupported format
                    if ( (retMask & (MASK_BRIEF | MASK_PARAM | MASK_RET)) && !(retMask & MASK_OTHERS) ) {
                        // Convert
                        if (retMask & MASK_BRIEF) { foutWriteBuffer(indent); foutWriteBuffer(doxygenBrief); foutWriteChar('\n'); }
                        if (retMask & MASK_PARAM) { for (const auto& p : doxygenParam) { foutWriteBuffer(indent); foutWriteBuffer(p); foutWriteChar('\n'); } }
                        if (retMask & MASK_RET)   { foutWriteBuffer(indent); foutWriteBuffer(doxygenReturn); foutWriteChar('\n'); }
                        
                        // Eat newlines after Doxygen block (Requirement: Remove trailing newlines)
                        while (!finIsEOF()) {
                            int next_c = fin.peek();
                            if (next_c == '\n' || next_c == '\r') finReadChar(); else break; 
                        }
                    } else {
                        // Bypass (keep original) - Restore //
                        foutWriteBuffer(indent); foutWriteBuffer("//"); foutWriteBuffer(buffer); foutWriteChar('\n'); 
                    }

                    doxygenBrief.clear(); doxygenParam.clear(); doxygenReturn.clear();
                    buffer.clear();
                    state = normal_code;
                }
                break;

            case normal_comment_multi_line:
                if (c == '/' && buffer.size() >= 2 && buffer[buffer.size()-2] == '*') {
                    buffer.pop_back(); buffer.pop_back();
                    retMask = parseBoschComent(buffer, doxygenBrief, doxygenParam, doxygenReturn);

                    // LOGIC: Must have valid tags AND NO "Others"
                    if ( (retMask & (MASK_BRIEF | MASK_PARAM | MASK_RET)) && !(retMask & MASK_OTHERS) ) {
                        // Convert
                        if (retMask & MASK_BRIEF) { foutWriteBuffer(indent); foutWriteBuffer(doxygenBrief); foutWriteChar('\n'); }
                        if (retMask & MASK_PARAM) { for (const auto& p : doxygenParam) { foutWriteBuffer(indent); foutWriteBuffer(p); foutWriteChar('\n'); } }
                        if (retMask & MASK_RET)   { foutWriteBuffer(indent); foutWriteBuffer(doxygenReturn); foutWriteChar('\n'); }

                        // Eat newlines after Doxygen block
                        while (!finIsEOF()) {
                            int next_c = fin.peek();
                            if (next_c == '\n' || next_c == '\r') finReadChar(); else break; 
                        }
                    } else {
                        // Bypass (keep original) - Restore /* */
                        foutWriteBuffer(indent); foutWriteBuffer("/*"); foutWriteBuffer(buffer); foutWriteBuffer("*/"); 
                    }

                    doxygenBrief.clear(); doxygenParam.clear(); doxygenReturn.clear();
                    buffer.clear();
                    state = normal_code;
                }
                break;
        }
    } while (1);

    finClose(); foutClose();
    return STATUS_OKE;
}

void convertFromInputDir(){
    scanFiles();
    for (auto filename:InputFilename){
        string inFile = concat(INPUT_ROOTDIR, filename);
        string outFile = concat(OUTPUT_ROOTDIR, filename);
        fs::create_directories(fs::path(outFile).parent_path());
        Convert(inFile, outFile);
    }
}

void convertFromFilenameList(){
    for (auto filename:InputFilename){
         string outFile = concat(OUTPUT_ROOTDIR, filename);
         fs::create_directories(fs::path(outFile).parent_path());
         Convert(concat(INPUT_ROOTDIR, filename), outFile);
    }
}

#endif