#include "__convertGlobalUtil.hpp"

// --- Global Configuration Variables (Mutable) ---

/// @brief Root directory for input files
string INPUT_ROOTDIR = "./input/";
/// @brief Root directory for output files
string OUTPUT_ROOTDIR = "./output/";

/// @brief Patterns for Bosch-style comments
string pattern_definition             = "Definition:";
string pattern_description            = "Description:";
string pattern_input_param            = "Input Parameter:";
string pattern_input_param_delimiter  = "--";
string pattern_output_param           = "Output Parameter:";
string pattern_output_param_delimiter = "--";

/// @brief Doxygen tag prefixes
string prefix_brief                   = "/// @brief ";
string prefix_param                   = "/// @param ";
string prefix_return                  = "/// @return ";

/// @brief List of files to process (initially empty or default)
vector<string> InputFilename = {
     "rbd_AnalogIpHndlr.h",
     "rbd_AnalogIpHndlr.c",
};

enum CommentTypeMask {
    MASK_NONE   = 0x00,
    MASK_BRIEF  = 0x01,
    MASK_PARAM  = 0x02,
    MASK_RET    = 0x04,
    MASK_OTHERS = 0x08
};

// --- External dependencies from __convertFileOperation.hpp ---
// Ensure these are linked correctly in your main file
extern fstream fin; 
Status finOpen(const string& path);
Status finClose();
int finIsEOF();

/// @brief Trims whitespace from beginning and end of string.
/// @param s Input string.
/// @return Trimmed string.
static string trim_cfg(const string& s) {
    size_t first = s.find_first_not_of(" \t\n\r");
    if (string::npos == first) return "";
    size_t last = s.find_last_not_of(" \t\n\r");
    return s.substr(first, (last - first + 1));
}

/// @brief Reads config.txt and updates global variables.
/// @param path Path to config file.
void loadConfig(const string& path) {
    if (finOpen(path) != STATUS_OKE) {
        cout << "[WARN] Config file not found, using defaults.\n";
        return;
    }

    string line;
    while (getline(fin, line)) {
        // Skip comments or empty lines
        if (line.empty() || line[0] == '#' || line.find('=') == string::npos) continue;

        stringstream ss(line);
        string key, val;
        
        // Split by '='
        if (getline(ss, key, '=')) {
            getline(ss, val);
            key = trim_cfg(key);
            val = trim_cfg(val);

            // Map keys to variables
            if (key == "INPUT_ROOTDIR")                  INPUT_ROOTDIR = val;
            else if (key == "OUTPUT_ROOTDIR")            OUTPUT_ROOTDIR = val;
            else if (key == "pattern_definition")        pattern_definition = val;
            else if (key == "pattern_description")       pattern_description = val;
            else if (key == "pattern_input_param")       pattern_input_param = val;
            else if (key == "pattern_input_param_delimiter") pattern_input_param_delimiter = val;
            else if (key == "pattern_output_param")      pattern_output_param = val;
            else if (key == "pattern_output_param_delimiter") pattern_output_param_delimiter = val;
            else if (key == "prefix_brief")              prefix_brief = val;
            else if (key == "prefix_param")              prefix_param = val;
            else if (key == "prefix_return")             prefix_return = val;
        }
    }
    finClose();
    cout << "[INFO] Configuration loaded from " << path << "\n";
}
