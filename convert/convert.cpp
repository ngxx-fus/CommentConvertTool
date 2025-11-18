#include "__convert.hpp"



int main(){
    printf("Hello from foo!\n");
    loadConfig("config.txt");
    Print_Info();
    convertFromInputDir();
    // convertFromFilenameList();
    printf("Done!");
    return 0;
}