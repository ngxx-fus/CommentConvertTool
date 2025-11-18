## About

This tool conver comment like:
```
/*=====================================================================================================================================
Definition: void the_function(uint8 the_param1, uint16* the_param2 )
Input Parameter:  Excepteur sint occaecat cupidatat non proident, sunt in culpa qui officia deserunt mollit anim id est laborum.
Output Parameter: Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat.
Description: Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. 
=======================================================================================================================================*/
```

to the doxygen comment like:

```
/// @brief Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua.
/// @param the_param1 Excepteur sint occaecat cupidatat non proident, sunt in culpa qui officia deserunt mollit anim id est laborum.
/// @param the_param2 Excepteur sint occaecat cupidatat non proident, sunt in culpa qui officia deserunt mollit anim id est laborum.
/// @return Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat.
```
## Usage

### Directory tree

```
.                           <----- Current directory/rootdir (*)
+---.vscode
+---convert                 <----- The tool
+---input                   <----- Input file(s) (.h, .c)
|   +---Header_files
|   +---html
|   |   \---search
|   \---latex
\---output                  <----- Output file(s)
    +---Header_files
    +---html
    |   \---search
    \---latex
```

### Build and run

Which the config:

```
INPUT_ROOTDIR                       = ./input/
OUTPUT_ROOTDIR                      = ./output/
pattern_definition                  = Definition:
pattern_description                 = Description:
pattern_input_param                 = Input Parameter:
pattern_input_param_delimiter       = --
pattern_output_param                = Output Parameter:
pattern_output_param_delimiter      = --
prefix_brief                        = /// @brief 
prefix_param                        = /// @param 
prefix_return                       = /// @return 
```
The command below will run the tool:

```pwsh
./convert/run.bat
```

Note that: The comment run at Current directory (\*). And the `convert.exe` will generate in the Current directory (\*).
