#include <iostream>
#include <cstring>
#include <unistd.h>
#include "vm_app.h"

using std::cout;

int main()
{
    printf("test start");
    /* Allocate swap-backed page from the arena */
    char* filename = static_cast<char *>(vm_map(nullptr, 0));

    /* Write the name of the file that will be mapped */
    strcpy(filename, "lampson83.txt");

    /* Map a page from the specified file */
    char* p0 = (char*) (vm_map(filename, 0));
    char* p1 = (char*) (vm_map(filename, 0));
    char* p2 = (char*) (vm_map(filename, 0));
    char* p3 = (char*) (vm_map(filename, 0));
    char* p4 = (char*) (vm_map(filename, 0));
    char* p5 = (char*) (vm_map(filename, 0));

    /* Print the first part of the paper */
    for (unsigned int i=0; i<1930; i++) {
	    cout << p0[i];
    }
    printf("\n\n\n\n");
    for (unsigned int i=0; i<1930; i++) {
	    cout << p1[i];
    }
    printf("\n\n\n\n");
    for (unsigned int i=0; i<1930; i++) {
	    cout << p2[i];
    }
    printf("\n\n\n\n");
    for (unsigned int i=0; i<1930; i++) {
	    cout << p3[i];
    }
    printf("\n\n\n\n");
    for (unsigned int i=0; i<1930; i++) {
	    cout << p4[i];
    }
    printf("\n\n\n\n");
    for (unsigned int i=0; i<1930; i++) {
	    cout << p5[i];
    }
    printf("\n\n\n\n");
    for (unsigned int i=0; i<1930; i++) {
	    cout << p0[i];
    }
}