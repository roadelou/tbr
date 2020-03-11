// Section 0 : Include
#include "tbr.h"

// Determined at runtime with the initialize function.
static char TBR_HOME[TBR_STR_SIZE];
// Checked at the beginning of all function to avoid undefined behaviors. The function should panic if INIT_OK is false (unless the routine is initialize, in which case it should panic if INIT_OF is true).
static int INIT_OK = 0;
// To avoid writing the panick message everytime.
static char *INIT_ERROR = "The initialize routine hasn't been called";
// Replaced the use of global hsearch with reentrant versions.
// Note that the struct this points t is malloc'd in initialized.
static struct hsearch_data *HTABLE;

// Keeps reference of all malloc'd strings (for the HTABLE keys).
// Always malloc'd to contain TBR_DICT_SIZE keys of in initialize and freed in finalize.
static char **BOOK;

// Used to keep count of all the valid references in BOOK that have to be freed.
static size_t BOOK_COUNT = 0;

// Section 1 : Prototypes and static values

// This implementation uses some helper functions gathered at the end of the
// file.

// Writes some text to a file create at given path.
Error write_file(const char *filename, const char *text);
// Checks whether the dir at path exists.
int dir_exists(const char *path);
// Read the .tbr file located at the given path. This does not belong in tbr.h
// since this should only be called if filename is valid.
Error read_tbr(Project *p, const char *filename);
// Copies file from path scr to path dest.
Error fcopy(const char *src, const char *dest);

// Section 2 : tbr.h implementation

// Creates a new project with given name.
Error nproj(Project *p, const char *name) {
  // Check init flag
  if (!INIT_OK) {
	return error(-1, INIT_ERROR);
  }
  // else ...

  // First thing is to get the current working directory path
  char cwd_buffer[TBR_STR_SIZE];
  char *cwd_ptr = getcwd(cwd_buffer, TBR_STR_SIZE);
  if (cwd_ptr == NULL) {
    return error(-1, "Current path too long");
  }
  strncpy(p->name, name, TBR_STR_SIZE);
  snprintf(p->path, TBR_STR_SIZE, "%s/%s", cwd_ptr, p->name);

  int exists = dir_exists(p->path);
  if (exists == 1) {
    return errorf(-1, "Project folder %s already exists", p->path);
  }

  mlog("Creating project %s\n", p->name);

  // else ...
  int created_dir = mkdir(p->path, 0777);
  if (created_dir == -1) {
    return errorf(-1, "Path %s could not be created", p->path);
  }

  int changed_dir = chdir(p->path);
  if (changed_dir == -1) {
    return errorf(-1, "Could not enter directory %s", p->path);
  }

  // Yes, I hard-coded the Makefile. Aside from the obvious, what is wrong with
  // that ?
  char *makefile_format =
      "# Makefile generated automatically by the tbr tool.\n"
      "NAME = %s\n"
      "# All the dependancies of the project. To modify those dependancies "
      "edit the %s.tbr file and let the tool do the hard work.\n"
      "DEP = $(wildcard ./.dep/*.v)\n"
      "# The file that the tbr tool uses to manage your projects. Edit it to "
      "add or remove a dependancy.\n"
      "TBR = ./$(NAME).tbr\n"
      "SRC = $(NAME).v\n"
      "# The top module, used to test your implementation with verilator.\n"
      "TOP = top.v\n"
      "# The global location where all librairies are stored, used by the tbr "
      "tool to gather the dependancies of your project.\n"
      "LIB = %s/.tbr\n"
      "\n"
      "top: $(SRC) deps $(TOP)\n"
      "\tiverilog -o top $(SRC) $(DEP) $(TOP)\n"
      "\n"
      "# Asks the tbr tool to gather the dependancies of your project.\n"
      "deps: $(TBR)\n"
      "\ttbr build\n"
      "\n"
      "# Creates a usable librairy from the current project\n"
      "install: $(SRC) $(TBR)\n"
      "\trm -rf $(LIB)/$(NAME)\n"
      "\tmkdir $(LIB)/$(NAME)\n"
      "\tcp $(SRC) $(LIB)/$(NAME)/\n"
      "\tcp $(TBR) $(LIB)/$(NAME)/\n"
      "\n"
      "clean:\n"
      "\ttbr clean\n"
      "\trm -f top\n"
      "\n";

  char makefile_buffer[1024];
  snprintf(makefile_buffer, 1024, makefile_format, p->name, p->name, TBR_HOME);
  Error makefile_written = write_file("Makefile", makefile_buffer);
  if (makefile_written.code == -1) {
    return error(-1, "Could not write Makefile");
  }
  // else ...
  llog("Added Makefile to project %s\n", p->name);

  char *top_template =
      "// top module created automatically by the tbr tool.\n"
      "\n"
      "module Top;\n"
      "// Because of how icarus verilog works this top file must not have "
      "inputs or outputs (it is a simulation file).\n"
      "\n"
      "endmodule\n";

  Error top_written = write_file("top.v", top_template);
  if (top_written.code == -1) {
    return error(-1, "Could not write top.v file");
  }
  llog("Added top module to project %s\n", p->name);

  char *dep_template = "# This files contains the dependancies that the tbr "
                       "tool will look for.\n"
                       "# Any line beginning with a '#' will be ignored.\n"
                       "\n";

  char dep_file_name[TBR_STR_SIZE];
  snprintf(dep_file_name, TBR_STR_SIZE, "%s.tbr", p->name);

  Error dep_written = write_file(dep_file_name, dep_template);
  if (dep_written.code == -1) {
    return errorf(-1, "Could not write %s file", dep_file_name);
  }
  llog("Added tbr file to project %s\n", p->name);

  char *verilog_template =
      "// This file was automatically generated by the tbr tool.\n"
      "// You can put your verilog source code inside of this file.\n"
      "\n";

  char verilog_file[TBR_STR_SIZE];
  snprintf(verilog_file, TBR_STR_SIZE, "%s.v", p->name);
  Error verilog_written = write_file(verilog_file, verilog_template);
  if (verilog_written.code == -1) {
    return errorf(-1, "Could not source file %s", verilog_file);
  }
  llog("Added verilog empty file to project %s\n", p->name);

  int created_dep_dir = mkdir(".dep", 0777);
  if (created_dep_dir == -1) {
    return error(-1, "Could not create .dep folder");
  }
  llog("Added dependancy folder to project %s\n", p->name);

  p->count = 0;
  return error(0, NULL);
}

// Reads project in path specified by path. If path is null looks in cwd.
Error rproj(Project *p, const char *path) {
  // Check init flag
  if (!INIT_OK) {
	return error(-1, INIT_ERROR);
  }
  // else ...

  if (path == NULL) {
    char cwd_buffer[TBR_STR_SIZE];
    char *cwd_ptr = getcwd(cwd_buffer, TBR_STR_SIZE);
    if (cwd_ptr == NULL) {
      return error(-1, "Current path too long");
    }
    strncpy(p->path, cwd_ptr, TBR_STR_SIZE);
  } else {
    strncpy(p->path, path, TBR_STR_SIZE);
  }
  DIR *dir_ptr = opendir(p->path);
  if (dir_ptr == NULL) {
    return errorf(-1, "Could not open directory %s", p->path);
  }
  // else ...

  // Looking for a file that ends in .tbr in the cwd. First match is considered
  // to be the roght one.
  struct dirent *ent;
  int found_tbr = 0;
  while ((ent = readdir(dir_ptr)) != NULL) {
    size_t filename_len = strlen(ent->d_name);
    int comparison = strncmp(ent->d_name + (filename_len - 4), ".tbr", 4);
    if (comparison == 0) {
      found_tbr = 1;
      size_t cpy_size = filename_len - 4;
      if (cpy_size + 1 > TBR_STR_SIZE) {
        return errorf(-1, "Project name %s too long", ent->d_name);
      }
      // else ...
      strncpy(p->name, ent->d_name, cpy_size);
      p->name[cpy_size] = '\0'; // null termination byte
      break;
    }
  }
  if (!found_tbr) {
    // Could mean that we are looking at a project with no dependancies.
    return error(-3, "Could not find project file");
  }
  closedir(dir_ptr);

  mlog("Successfully read project %s\n", p->name);

  char dep_fname[TBR_STR_SIZE];
  snprintf(dep_fname, TBR_STR_SIZE, "%s/%s.tbr", p->path, p->name);
  Error read_dep = read_tbr(p, dep_fname);
  if (read_dep.code != 0) { // Can be improved later on
    return errorf(-1, "Could not parse %s", dep_fname);
  }
  return error(0, NULL);
}

// includes the dependancy d in the .dep folder. Only works for a single
// dependancy (i.e. this is not recursive, the recursion part is handled
// elsewhere).
Error include(const Dependancy d) {
  // Check init flag
  if (!INIT_OK) {
	return error(-1, INIT_ERROR);
  }
  // else ...

  // First thing is to test whether d has already been included.
  ENTRY dep_entry;
  dep_entry.key = (char *)d.name;
  dep_entry.data = NULL;
  ENTRY *found;
  int found_code = hsearch_r(dep_entry, FIND, &found, HTABLE);
  // Must catch success, i.e. item was found in HTABLE.
  // WARNING : hsearch_r is weird and returns non-zero on success.
  if (found_code != 0 || found != NULL) {
    // Nothing to do, the dependancy is already included.
    llog("Dependancy %s has been optimized out\n", d.name); // just bragging
    return error(0, NULL);
  }
  // else ...

  char *depfolder = "./.dep";
  // First we get the directory where the source files are located.
  char libpath[TBR_STR_SIZE];
  snprintf(libpath, TBR_STR_SIZE, "%s/.tbr/%s", TBR_HOME, d.name);
  DIR *dir_ptr = opendir(libpath);
  if (dir_ptr == NULL) {
    return errorf(-1, "Dependancy %s not found in repos", d.name);
  }
  // else ...

  // We copy all verilog files from the source archive to the local .dep folder.
  struct dirent *ent;
  int found_verilog = 0;
  while ((ent = readdir(dir_ptr)) != NULL) {
    // i.e. for all files in dir_ptr.
    size_t filename_len = strlen(ent->d_name);
    // We copy all files ending with .v, assumed to be verilog files.
    int comparison = strncmp(ent->d_name + (filename_len - 2), ".v", 2);
    if (comparison == 0) {
      // i.e. found a verilog file.
      found_verilog++;
      // Getting source path.
      char verilog_src[TBR_STR_SIZE];
      snprintf(verilog_src, TBR_STR_SIZE, "%s/%s", libpath, ent->d_name);
      // Getting destination path.
      char verilog_dest[TBR_STR_SIZE];
      snprintf(verilog_dest, TBR_STR_SIZE, "%s/%s", depfolder, ent->d_name);
      // Copying the file.
      Error cpy_error = fcopy(verilog_src, verilog_dest);
      if (cpy_error.code == -1) {
        return errorf(-1, "Could not copy source file %s", ent->d_name);
      }
    }
  }
  if (found_verilog == 0) {
    return error(-1, "No source found in dep folder");
  }
  closedir(dir_ptr);

  // updating dependancy dict, but we have to keep a reference somewhere else to free the pointer later on.
  // Small check for precaution.
  if (BOOK_COUNT == TBR_DICT_SIZE) {
	return errorf(-1, "Maximum amount of dependancies %d reached", TBR_DICT_SIZE);
  }
  // else ...

  BOOK[BOOK_COUNT] = malloc(TBR_STR_SIZE);
  dep_entry.key = BOOK[BOOK_COUNT];
  BOOK_COUNT++;
  strncpy(dep_entry.key, d.name, TBR_STR_SIZE);
  //dep_entry.key = (char *)d.name;
  dep_entry.data = NULL;
  ENTRY *entered;
  int entered_code = hsearch_r(dep_entry, ENTER, &entered, HTABLE);
  if (entered_code == 0) {
	return errorf(-1, "Could not insert %s in hastable", dep_entry.key);
  }
  // else ...

  mlog("Included %s\n", d.name);
  
  return error(0, NULL);
}

// Fills the p struct with information from the .tbr file of the dependancy d
Error getdeps(Project *p, const Dependancy d) {
  // Check init flag
  if (!INIT_OK) {
	return error(-1, INIT_ERROR);
  }
  // else ...

  char dep_path[TBR_STR_SIZE];
  snprintf(dep_path, TBR_STR_SIZE, "%s/.tbr/%s", TBR_HOME, d.name);
  Error read_error = rproj(p, dep_path);
  if ((read_error.code == 0) || (read_error.code == -3)) {
    // i.e. looking at dependancy with no other dependancy
    return error(0, NULL);
  } else {
    return errorf(-1, "Could not read tbr file of %s", d.name);
  }
}

// Initial iteration of the make_recur from current working directory (reading
// the .tbr file).
Error make(void) {
  // Check init flag
  if (!INIT_OK) {
	return error(-1, INIT_ERROR);
  }
  // else ...

  Project p;
  Error read_root = rproj(&p, NULL);
  if (read_root.code == -1) {
    return error(-1, "Failed to begin recursive build");
  }

  mlog("Building project %s\n", p.name);

  for (size_t i = 0; i < p.count; i++) {
    Error make_err = make_recur(p.deps[i]);
    if (make_err.code == -1) {
      return errorf(-1, "Recursive build of %s failed", p.deps[i].name);
    }
  }
  return error(0, NULL);
}

// Recursively includes d and then all of its sub dependancies.
Error make_recur(const Dependancy d) {
  // Check init flag
  if (!INIT_OK) {
	return error(-1, INIT_ERROR);
  }
  // else ...

  // Note that this first test is usefull to solve issues with circular
  // dependancies
  ENTRY dep_entry;
  dep_entry.key = (char *)d.name;
  dep_entry.data = NULL;
  ENTRY *found;
  int found_code = hsearch_r(dep_entry, FIND, &found, HTABLE);
  if (found_code != 0 || found != NULL) {
    // Nothing to do, the dependancy is already included.
    llog("Dependancy %s has been optimized out\n", d.name); // just bragging
    return error(0, NULL);
  }
  // else ...

  Project p;
  Error gotten_deps = getdeps(&p, d);
  if (gotten_deps.code == -1) {
    return errorf(-1, "Could not get build dependancies of %s", d.name);
  }
  // else ...
  for (size_t i = 0; i < p.count; i++) {
    Error make_err = make_recur(p.deps[i]);
    if (make_err.code == -1) {
      return errorf(-1, "Recursive build of %s failed", p.deps[i].name);
    }
  }

  Error included = include(d);
  if (included.code == -1) {
    return errorf(-1, "Could not include dependancy %s", d.name);
  }
  return error(0, NULL);
}

// cleans the .dep folder
Error clean(void) {
  // Check init flag
  if (!INIT_OK) {
	return error(-1, INIT_ERROR);
  }
  // else ...

  llog("%s\n", "Cleaning dependancy folder");

  char *depfolder = "./.dep";
  DIR *dir_ptr = opendir(depfolder);
  if (dir_ptr == NULL) {
    return errorf(-1, "Dependancy folder %s could not be opened", depfolder);
  }
  // else ...

  struct dirent *ent;
  while ((ent = readdir(dir_ptr)) != NULL) {
    // i.e. for file in dir_ptr
    size_t filename_len = strlen(ent->d_name);
    int comparison = strncmp(ent->d_name + (filename_len - 2), ".v", 2);
    if (comparison == 0) {
      char fname[TBR_STR_SIZE];
      snprintf(fname, TBR_STR_SIZE, "%s/%s", depfolder, ent->d_name);
      int removed = remove(fname);
      if (removed == -1) {
        return errorf(-1, "Could not remove file %s in .dep folder",
                      ent->d_name);
      }
    }
  }
  closedir(dir_ptr);
  return error(0, NULL);
}

Error initialize(void) {
  // Check init flag
  if (INIT_OK) {
	return error(-1, "Initialize function has already been called");
  }
  // else ...

  // default log level is explicitely set to MED (it is also implicitely set to MED in log.c but that is not as obvious and depends on the implementation of log.
  loglvl(MED);

  // Step zero : malloc the hash table.
  HTABLE = malloc(sizeof(struct hsearch_data));
  // According to the doc, HTABLE must be zeroed before calling hcreate_r.
  memset(HTABLE, 0, sizeof(struct hsearch_data));

  // Step one : create the hash table required by tbr.c to handle dependancies.
  int created_dict = hcreate_r(TBR_DICT_SIZE, HTABLE);
  if (created_dict == 0) {
    return error(-1, "Could not create hash table");
  }

  // Step two : find home folder.
  strncpy(TBR_HOME, getenv("HOME"), TBR_STR_SIZE);

  // Step three : malloc the BOOK refence keeper and initialize reference counter.
  BOOK = malloc(TBR_DICT_SIZE * sizeof(char *));
  BOOK_COUNT = 0;

  INIT_OK = 1;

  return error(0, NULL);
}

Error finalize(void) {
  // Check init flag
  if (!INIT_OK) {
	return error(-1, INIT_ERROR);
  }
  // else ...

  // Free the hash table used by the program.
  hdestroy_r(HTABLE);

  // Free all the keys that were used in the hash table.
  for (size_t i = 0; i < BOOK_COUNT; i++) {
	char *ref = BOOK[i];
	free(ref);
  }
  BOOK_COUNT = 0;

  // Last but not least, free the book itself
  free(BOOK);

  INIT_OK = 0;
  return error(0, NULL);
}

// Section 3 : Helper functions implementation

int dir_exists(const char *path) {
  DIR *dir_ptr = opendir(path);
  if (dir_ptr) {
    closedir(dir_ptr);
    return 1;
  } else if (ENOENT == errno) {
    return 0;
  } else {
    // i.e. other error
    return -1;
  }
}

Error write_file(const char *filename, const char *text) {
  FILE *f_ptr = fopen(filename, "w");
  if (f_ptr == NULL) {
    return errorf(-1, "Could not create file %s", filename);
  }
  // else ...
  int written = fputs(text, f_ptr);
  if (written == EOF) {
    return errorf(-1, "Could not write to file %s", filename);
  }
  // else ...
  return error(0, NULL);
}

// Reads the .tbr file found at path filename line by line to fill the project
// struct p.
Error read_tbr(Project *p, const char *filename) {
  // The index used to iterate over deps
  p->count = 0;
  FILE *fd = fopen(filename, "r");
  if (fd == NULL) {
    return errorf(-1, "Could not open dependancy file %s", filename);
  }
  // else ...

  // using getline to read the file line by line
  char *line_ptr = NULL;
  size_t n = 0;
  ssize_t gotten;
  // Used to count the amount of dependacies that do not fit past limit, in
  // order to help the main find the right allocation size in one pass.
  size_t excess = 0;
  while ((gotten = getline(&line_ptr, &n, fd)) != -1) {
    if ((line_ptr[0] == '#') || (line_ptr[0] == '\n')) {
      // Line was commented out or is empty
      continue;
    } else if (p->count + 1 == TBR_MAX_DEP) {
      excess++;
      continue;
    }
    // else ...

    // To remove trailing newlines
    sscanf(line_ptr, "%s\n", p->deps[p->count].name);
    p->count++;
  }
  if (excess) { // i.e. excess != 0
    return errorf(-2, "%ld excessive dep for the tool", excess);
  }
  fclose(fd);
  free(line_ptr);
  return error(0, NULL);
}

// Copy file from source to destination.
Error fcopy(const char *src, const char *dest) {
  FILE *source = fopen(src, "rb");
  if (source == NULL) {
    return errorf(-1, "Could not open source file %s", src);
  }
  // else ...
  int seeked = fseek(source, 0, SEEK_END);
  if (seeked == -1) {
    return errorf(-1, "Could not seek end of file %s", src);
  }
  size_t file_size = ftell(source);
  rewind(source);

  FILE *destination = fopen(dest, "wb");
  if (destination == NULL) {
    return errorf(-1, "Could not open destination file %s", dest);
  }
  // else ...

  // allocating memory needed to transfer the file in one pass (could be improved
  // later on).
  void *buffer = malloc(file_size);
  size_t read_src = fread(buffer, 1, file_size, source);
  if (read_src != file_size) {
    return errorf(-1, "Could not read from file %s", src);
  }
  // else ...
  size_t write_dest = fwrite(buffer, 1, file_size, destination);
  fclose(source);
  if (write_dest != file_size) {
    return errorf(-1, "Could not write to file %s", dest);
  }
  // else ...
  fclose(destination);
  return error(0, NULL);
}
