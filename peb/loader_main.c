

#include <windows.h>
#include <stdio.h>

#include "pe_loader.h"


int main(unsigned int argc, char *argv[], char *envp[]){

PeHeaders pe;
unsigned int i;
IMAGE_IMPORT_DESCRIPTOR *imp;

    if(argc < 1){
        printf("usage %s <filename>\n",argv[0]);
        return 1;
        }
    
    peb = GetCurrentPEB();
	//RemoveModuleFromCurrentLdrLists(L"peb.exe");
    if (!LoadPeImage (argv[1], argv[1], &pe)) {
    //if (!LoadPeHeaders ("C:\\Windows\\system32\\cmd.exe", &pe, 0)) {
        return 1;
    }


    return 0;
}
