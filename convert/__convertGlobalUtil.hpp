#include <bits/stdc++.h>
using namespace std;

typedef int Status;
#define STATUS_OKE 0
#define STATUS_ERR 1

#ifndef REPT
    #define REPT(type, i, a, b) for(type i = (a); (i) < (b); ++(i))
#endif

#ifndef REVT
    #define REVT(type, i, a, b) for(type i = (a); (i) > (b); --(i))
#endif

#ifndef REP
    #define REP(i, a, b) for(int i = (a); (i) < (b); ++(i))
#endif

#ifndef REV
    #define REV(i, a, b) for(int i = (a); (i) > (b); --(i))
#endif

template<typename... Args>
string concat(Args const&... args) {
    stringstream ss;
    (ss << ... << args); 
    return ss.str();
}

// Status parseBoschComent(string s, string &brief, vector<string> &param, string &ret) {
//     // 1. Define Constants & Patterns
//     const int maxEqualSymNum = 3;
//     const string pattern_definition        = "Definition:";
//     const string pattern_description       = "Description:";
//     const string pattern_input_param       = "Input Parameter:";
//     const string pattern_input_param_delim = "--";
//     const string pattern_output_param      = "Output Parameter:";
    
//     // 2. Static State (Memory of the parser)
//     // 0: None, 1: Definition, 2: Input, 3: Output, 4: Description
//     static int currentSection = 0; 

//     // 3. Helper Lambdas
//     // Check for separator lines (e.g., "=======")
//     auto is_separator = [&](const string& line) -> bool {
//         long countEq = count(line.begin(), line.end(), '=');
//         return countEq > maxEqualSymNum;
//     };

//     // Clean raw artifacts: remove "//", "/*", "*/" and leading/trailing spaces/asterisks
//     auto clean_line = [&](string t) -> string {
//         // Remove specific comment markers
//         string markers[] = {"//", "/*", "*/"};
//         for (const auto& m : markers) {
//             size_t pos;
//             while ((pos = t.find(m)) != string::npos) {
//                 t.replace(pos, m.length(), " ");
//             }
//         }
        
//         // Trim leading whitespace & '*'
//         size_t first = t.find_first_not_of(" \t\n\r*");
//         if (string::npos == first) return "";
        
//         // Trim trailing whitespace
//         size_t last = t.find_last_not_of(" \t\n\r");
//         return t.substr(first, (last - first + 1));
//     };

//     // 4. Process the input string (handle multi-line string if passed as a block)
//     stringstream ss(s);
//     string line;

//     while (getline(ss, line)) {
//         // Skip separator lines immediately
//         if (is_separator(line)) continue;

//         // 5. Detect Context Switch & Strip Header
//         // Only check for headers if the line actually contains them
//         bool headerFound = false;
//         if (line.find(pattern_definition) != string::npos) {
//             currentSection = 1;
//             line = line.substr(line.find(pattern_definition) + pattern_definition.length());
//             headerFound = true;
//         } else if (line.find(pattern_input_param) != string::npos) {
//             currentSection = 2;
//             line = line.substr(line.find(pattern_input_param) + pattern_input_param.length());
//             headerFound = true;
//         } else if (line.find(pattern_output_param) != string::npos) {
//             currentSection = 3;
//             line = line.substr(line.find(pattern_output_param) + pattern_output_param.length());
//             headerFound = true;
//         } else if (line.find(pattern_description) != string::npos) {
//             currentSection = 4;
//             line = line.substr(line.find(pattern_description) + pattern_description.length());
//             headerFound = true;
//         }

//         // 6. Clean the content
//         string content = clean_line(line);
//         if (content.empty()) continue;

//         // 7. Process Content based on Memory (currentSection)
//         switch (currentSection) {
//             case 2: // Input Parameter
//             {
//                 size_t delimPos = content.find(pattern_input_param_delim);
//                 string finalStr;
//                 if (delimPos != string::npos) {
//                     string pName = clean_line(content.substr(0, delimPos));
//                     string pDesc = clean_line(content.substr(delimPos + pattern_input_param_delim.length()));
//                     finalStr = "/// @param " + pName + " " + pDesc;
//                 } else {
//                     // Fallback if delimiter missing, treat whole line as part of param
//                     finalStr = "/// @param " + content;
//                 }
//                 param.push_back(finalStr);
//                 break;
//             }
//             case 3: // Output Parameter
//             {
//                 // Ignore "void" or "None"
//                 if (content == "void" || content == "None") break;
                
//                 if (ret.empty()) {
//                     ret = "/// @return " + content;
//                 } else {
//                     ret += " " + content;
//                 }
//                 break;
//             }
//             case 4: // Description
//             {
//                 if (brief.empty()) {
//                     brief = "/// @brief " + content;
//                 } else {
//                     brief += " " + content;
//                 }
//                 break;
//             }
//             default:
//                 break;
//         }
//     }

//     return STATUS_OKE;
// }