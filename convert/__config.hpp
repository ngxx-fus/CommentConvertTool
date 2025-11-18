#include <bits/stdc++.h>
using namespace std;

#define INPUT_ROOTDIR   "./input/"
#define OUTPUT_ROOTDIR   "./output/"

vector<string> InputFilename ={
     "rbd_AnalogIpHndlr.h",
     "rbd_AnalogIpHndlr.c",
};

static const string pattern_definition                = "Definition:";
static const string pattern_description               = "Description:";
static const string pattern_input_param               = "Input Parameter:";
static const string pattern_input_param_delimiter     = "--";
static const string pattern_output_param              = "Output Parameter:";
static const string pattern_output_param_delimiter    = "--";

static const string prefix_brief                      = "/// @brief ";
static const string prefix_param                      = "/// @param ";
static const string prefix_return                     = "/// @return ";

enum CommentTypeMask {
    MASK_NONE  = 0x00,
    MASK_BRIEF = 0x01, // Bit 0
    MASK_PARAM = 0x02, // Bit 1
    MASK_RET   = 0x04,  // Bit 2
    MASK_OTHERS = 0x08  // bit 3
};