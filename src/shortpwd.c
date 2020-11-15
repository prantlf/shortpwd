#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#ifdef _WIN32
#  include <direct.h>
#else
#  include <unistd.h>
#endif
#include "shortpwd.h"

void usage() {
  printf(
    "shortpwd %d.%d.%d\n"
    "  Prints the current working directory trimmed by an ellipsis in the middle.\n\n"
    "Usage: shortpwd [options] <leading> <trailing> [ellipsis]\n\n"
    "  Specify the maximum count of leading and trailing characters.\n"
    "  The overall maximum length will be their sum plus the length\n"
    "  of an ellipsis (3 dots by default). Zero for either leading\n"
    "  or trailing count means no leading or trailing parts.\n"
    "  The home part directory will be replaced by \"~\" if present.\n\n"
    "Options:\n"
    "  -V|--version  prints the version of the executable and exits\n\n"
    "  -h|--help     prints the usage information and exits\n\n"
    "Examples:\n"
    "  $ cd ~/Sources/private/shortpwd\n"
    "  $ shortpwd 5 10\n"
    "  ~/Sou...e/shortpwd\n"
    "  $ shortpwd 0 12 ..\n"
    "  ..ate/shortpwd\n", shortpwd_VERMAJ, shortpwd_VERMIN, shortpwd_VERPAT
  );
}

char const * const hint = "use -h to get help";
char const * ellipsis = "...";
size_t elliplen = 3;

int main (int argc, char const * const * const argv) {
  // print usage information if no arguments were provided
  if (argc == 1) {
    usage();
    return 0;
  }
  // check if printing the version number or usage information was requested
  if (argc == 2) {
    char const * const arg = argv[1];
    if (strcmp(arg, "-V") == 0 || strcmp(arg, "--version") == 0) {
      printf("%d.%d.%d\n", shortpwd_VERMAJ, shortpwd_VERMIN, shortpwd_VERPAT);
      return 0;
    }
    if (strcmp(arg, "-h") == 0 || strcmp(arg, "--help") == 0) {
      usage();
      return 0;
    }
    // unexpected one argument or fail if unexpected arguments were not provided
    printf("invalid argument: %s (%s)\n", arg, hint);
    return 1;
  }
  // check if exactly two arguments are available
  if (argc > 4) {
    printf("too many arguments (%s)\n", hint);
    return 1;
  }

  // make sure that leading and trailing character count are numeric
  int lead = strtoul(argv[1], NULL, 10);
  if (errno != 0) {
    printf("invalid leading character count: \"%s\" (%s - %s)\n",
      argv[1], strerror(errno), hint);
    return 1;
  }
  int trail = strtoul(argv[2], NULL, 10);
  if (errno != 0) {
    printf("invalid trailing character count: \"%s\" (%s - %s)\n", argv[2], strerror(errno), hint);
    return 1;
  }
  // ellipsis cannot be put to the middle unless the middle is specified
  if (lead == 0 && trail == 0) {
    printf("both leading and trailing counts cannot be zero (%s)\n", hint);
    return 1;
  }
  // check if a custom ellipsis was specified
  if (argc == 4) {
    ellipsis = argv[3];
    elliplen = strlen(ellipsis);
  }

  // get the current working directory
#ifdef _WIN32
  char * const cwd = _getcwd(NULL, 0);
#else
  char * const cwd = getcwd(NULL, 0);
#endif
  // if it is not possible, just print the error message to make it obvious
  if (cwd == NULL) {
    puts(cwd);
    return 2;
  }
  size_t cwdlen = strlen(cwd);

  // if the home directory is available and the current working directory
  // starts with it, replace it by "~" to shorten the length at first
  char const * const home = getenv("HOME");
  if (home != NULL) {
    size_t homelen = strlen(home);
    // $HOME should be always longer than one character, but...
    if (homelen > 1 && strncmp(cwd, home, homelen) == 0) {
      cwdlen -= homelen - 1;
      cwd[0] = '~';
      memmove(cwd + 1, cwd + homelen, cwdlen);
      cwd[cwdlen] = 0;
    }
  }

  // if the current directory is shorter than the ledting and trailing
  // character count plus the ellipsis length, leave it intact
  if (cwdlen > lead + trail + elliplen) {
    if (lead == 0) {
      // if only the trailing part should be displayed, put the ellipsis
      // to the beginning and move the trailing part behind it
      memcpy(cwd, ellipsis, elliplen);
      memmove(cwd + elliplen, cwd + cwdlen - trail, trail);
      cwd[trail + elliplen] = 0;
    } else if (trail == 0) {
      // if only the leading part should be displayed, put the ellipsis
      // right behind it
      strcpy(cwd + lead, ellipsis);
    } else {
      // if both leading and trailing parts should be displayed, put the
      // ellipsis behind the leading part and move the trailing part behind it
      memcpy(cwd + lead, ellipsis, elliplen);
      memmove(cwd + lead + elliplen, cwd + cwdlen - trail, trail);
      cwd[lead + trail + elliplen] = 0;
    }
  }

  // print the final current working directory, free its buffer and exit
  puts(cwd);
  free(cwd);
  return 0;
}
