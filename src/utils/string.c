#include "../../include/utils/string.h"
#include <ctype.h>
#include <stddef.h>

void str_trim(char *str) {
  // Remove leading whitespace
  while (str && isspace((unsigned char)str[0]))
    str++;
  // Remove trailing whitespace
  size_t len = strlen(str);
  while (len > 0 && isspace((unsigned char)str[len - 1]))
    len--;
  str[len] = '\0';
}
