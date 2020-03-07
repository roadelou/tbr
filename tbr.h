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

// Define the home folder (should be done at runtime, but I am alone and I don't
// care).
#ifndef TBR_HOME
#define TBR_HOME "/home/roadelou"
#endif

#include "error.h"  // used to make erroe handling less painfull
#include "log.h"    // used for nice log info
#include <dirent.h> // used to check if directories exist
#include <errno.h>  // used for errno manipulations
#include <search.h> // used for dict manipulations (hsearch etc...)
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>  // used for mkdir
#include <sys/types.h> // used for mkdir
#include <unistd.h>

/*
IMPORTANT : at runtime, in order to avoid including twice the same dependancy we
use a global hash table created with hcreate.
*/
typedef struct Dependancy Dependancy;
typedef struct Project Project;

struct Dependancy {
  char name[TBR_STR_SIZE];
};

struct Project {
  char name[TBR_STR_SIZE];
  char path[TBR_STR_SIZE];
  size_t count; // The number of elements in deps
  Dependancy deps[TBR_MAX_DEP];
};

// Create a non existent project
extern Error nproj(Project *p, const char *name);
// Reads project in current directory specified by path
extern Error rproj(Project *p, const char *path);
// Builds the project in the current working directory
extern Error make(void);
// dep is a single dependancy, whose tbr file will be read and used to complete
// p in order to make the necessay inclusions.
extern Error getdeps(Project *p, const Dependancy d);
// Includes a single dependancy into the project
extern Error include(const Dependancy d);
// Cleans the hidden folders used by tbr in current working directory
extern Error clean(void);
// Build current project
extern Error make(void);
// End of once include header guard
#endif
