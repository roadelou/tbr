#ifndef __TBR_INCLUDED__
#define __TBR_INCLUDED__

// Define size of buffer used to contain strings, can be overriden at compile
// time.
#ifndef TBR_STR_SIZE
#define TBR_STR_SIZE 128
#endif

// Define maximum amount of direct dependancies to a project, can be overriden
// at compile time.
#ifndef TBR_MAX_DEP
#define TBR_MAX_DEP 16
#endif

// Define maximum total amount of dependancies to a project, can be overriden at
// compile time.
#ifndef TBR_DICT_SIZE
#define TBR_DICT_SIZE 128
#endif

// Used to get access to reentrant versions of the hsearch functions
#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include "error.h"  // used to make erroe handling less painfull
#include "log.h"    // used for nice log info
#include <dirent.h> // used to check if directories exist
#include <errno.h>  // used for errno manipulations
#include <search.h> // used for dict manipulations (hsearch etc...)
#include <stdio.h>
#include <stdlib.h> // used for various file manipulations
#include <string.h>
#include <sys/stat.h>  // used for mkdir
#include <sys/types.h> // used for mkdir
#include <unistd.h>    // used for various file manipulations

/*
IMPORTANT : at runtime, in order to avoid including twice the same dependancy we
use a global hash table created with hcreate.

!!! BEFORE USING ANY FUNCTION IN THIS LIBRARY USE HCREAT TO CREATE A GLOBAL HASH
TABLE !!!

And don't forget to call hdestroy once you are done.
*/
typedef struct Dependancy Dependancy;
typedef struct Project Project;

// A Dependancy is only defined by its name, when you wants to find the
// dependancies of a dependancy you have to fill a Project struct with the
// corresplonding informations using the routine getdeps.
struct Dependancy {
  char name[TBR_STR_SIZE];
};

struct Project {
  char name[TBR_STR_SIZE]; // The name of the project
  char path[TBR_STR_SIZE]; // The path where the project is located
  size_t count;            // The number of elements in deps
  Dependancy deps[TBR_MAX_DEP];
};

// Create a non existent project in a subdirectory of the current working
// directory
extern Error nproj(Project *p, const char *name);
// Reads project (through .tbr file) in the directory specified by path
extern Error rproj(Project *p, const char *path);
// Builds the project in the current working directory (high level wrapper)
extern Error make(void);
// Includes the dependancy d then recursively includes all its sub depend
// ncies.
extern Error make_recur(const Dependancy d);
// dep is a single dependancy, whose tbr file will be read and used to complete
// p by making the necessary inclusions.
extern Error getdeps(Project *p, const Dependancy d);
// Includes a single dependancy into the project (copies it to the .dep folder).
extern Error include(const Dependancy d);
// Cleans the hidden folders used by tbr in current working directory.
extern Error clean(void);
// Function that must be called before calling any other function in this library.
extern Error initialize(void);
// Funtion to call once you are done using the library to wrap thing up.
extern Error finalize(void);

// End of once include header guard
#endif
