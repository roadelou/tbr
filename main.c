#include "tbr.h"

int tbr_usage(const char *reason);
int tbr_new(const char *name);
int tbr_build(void);
int tbr_unrecognized(const char *word);
int tbr_clean(void);

int main(int argc, char *argv[]) {
  int err_code = 0;
  loglvl(MED);
  int created_dict = hcreate(TBR_DICT_SIZE);
  if (created_dict == 0) {
    puts(strerror(errno));
    err_code = -1;
  }
  int cursor = 0;
  while ((err_code == 0) && (argc > 1 + cursor++)) {
    const char *cmd = argv[cursor];
    if ((strlen(cmd) == 3) && (strncmp(cmd, "new", 3) == 0)) {
      if (argc > cursor++) {
        const char *name = argv[cursor];
        err_code = tbr_new(name);
        break;
      } else {
        tbr_usage("Needed argument <name>");
        err_code = -1;
        break;
      }
    } else if (strlen(cmd) == 5) {
      if (strncmp(cmd, "build", 5) == 0) {
        err_code = tbr_build();
        break;
      } else if (strncmp(cmd, "clean", 5) == 0) {
        err_code = tbr_clean();
        break;
      } else {
        err_code = tbr_unrecognized(cmd);
        break;
      }
    } else if ((strlen(cmd) == 6) && (strncmp(cmd, "silent", 6) == 0)) {
      loglvl(HIGH);
      continue;
    } else if ((strlen(cmd) == 7) && (strncmp(cmd, "verbose", 7) == 0)) {
      loglvl(LOW);
      continue;
    } else {
      err_code = tbr_unrecognized(cmd);
      break;
    }
  }
  if (argc == 1) {
    tbr_usage("Not enough arguments");
    err_code = -1;
  }
  hdestroy();
  return err_code;
}

int tbr_usage(const char *reason) {
  char *usage_msgf =
      "(!) %s\n"
      "\n"
      "Usage of tool tbr :\n"
      "\n"
      ">> tbr new <name>\n"
      "\tCreates a new project with the name <name> in a new directory.\n"
      "\n"
      ">> tbr build\n"
      "\tGather dependancies for the project in the current working "
      "directory.\n"
      "\n"
      ">> tbr clean\n"
      "\tCleans the dependancy folder.\n"
      "\n"
      "Optionnal keywords :\n"
      "\tsilent - reduces verbosity.\n"
      "\tverbose - increases verbosity.\n"
      "\n";
  return printf(usage_msgf, reason);
}

int tbr_new(const char *name) {
  Project p;
  Error new_error = nproj(&p, name);
  if (new_error.code == -1) {
    return -1;
  } else {
    return 0;
  }
}

int tbr_build(void) {
  clean();
  Error make_error = make();
  if (make_error.code == -1) {
    return -1;
  } else {
    return 0;
  }
}

int tbr_unrecognized(const char *word) {
  size_t cmd_len = strlen(word);
  char msg_buffer[cmd_len + 21];
  snprintf(msg_buffer, cmd_len + 21, "Unrecognized option %s", word);
  tbr_usage(msg_buffer);
  return -1;
}

int tbr_clean(void) {
  clean();
  return 0;
}
