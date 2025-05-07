#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <ctype.h>
#include "base64.h"

/* Gets the username by the USER environment variable */
char* getUser(void) {
  char* environmentVariable = getenv("USER");
  if (environmentVariable == NULL) {
    return NULL;
  }
  
  const size_t size = strnlen(environmentVariable, 128);
  char* username = (char*)calloc(size + 1, sizeof(char));
  if (username == NULL) {
    return NULL;
  }

  strncpy(username, environmentVariable, size);
  return username;
}

/* Gets the Operating System (Linux Distribution) in use by reading the /etc/os-release file */
char* getOS(void) {
  FILE *fp;
  char buffer[256] = { 0 };

  int length = 0;

  const int operatingSystemSize = 128;
  char* operatingSystem = (char*)calloc(operatingSystemSize + 1, 1);

  if (operatingSystem == NULL) {
    return NULL;
  }

  const char path[] = "/etc/os-release";
  fp = fopen(path, "r");

  if (fp == NULL) {
    free(operatingSystem);
    return NULL;
  }
  
  while (fgets(buffer, sizeof(buffer), fp) != NULL) {
    sscanf(buffer, "PRETTY_NAME=\"%[^\"]\"", operatingSystem);
  }
  
  fclose(fp);

  return operatingSystem; 
}

/* Gets the current shell that was used by reading from /proc/$$/cmdline */
char* getShell(void) {
  FILE *fp;
  char path[128] = { 0 };
  char buffer[128] = { 0 };

  snprintf(path, sizeof(path), "/proc/%d/cmdline", getppid());

  fp = fopen(path, "r");

  if (fp == NULL) {
    return NULL;
  }
  
  if (fgets(buffer, sizeof(buffer), fp) == NULL) {
    fclose(fp);
    return NULL; 
  }

  fclose(fp);
  
  char* shell = (char*)calloc(128, 1);
  
  if (shell == NULL) {
    return NULL;
  }
  
  if (strrchr(buffer, '/') != NULL) {
    strncpy(shell, (strrchr(buffer, '/') + 1), 127);
  }
  return shell;
}

/* Gets the Linux Desktop Environment by the XDG_CURRENT_DESKTOP environment variable */
char* getDesktop(void) {
  char* environmentVariable = getenv("XDG_CURRENT_DESKTOP");
  if (environmentVariable == NULL) {
    return NULL;
  }
  
  const size_t size = strnlen(environmentVariable, 128);
  char* desktop = (char*)calloc(size + 1, sizeof(char));
  if (desktop == NULL) {
    return NULL;
  }

  strncpy(desktop, environmentVariable, size);
  return desktop;
}

/* Gets the Terminal Emulator by the TERM environment variable */
char* getTerminal(void) {
  char* environmentVariable = getenv("TERM");
  if (environmentVariable == NULL) {
    return NULL;
  }
  
  const size_t size = strnlen(environmentVariable, 128);
  char* terminal = (char*)calloc(size + 1, sizeof(char));
  if (terminal == NULL) {
    return NULL;
  }

  strncpy(terminal, environmentVariable, size);
  return terminal;
}

/* Gets the total memory available on the system by reading the first line from /proc/meminfo and parsing it accordingly */
/* TODO: Return to this to make it more efficient */
double getTotalMemory(void) {
  FILE* fp;
  const char* path = "/proc/meminfo";
  fp = fopen(path, "r");
  if (fp == NULL) {
    return -1;
  }
  
  const size_t BUFFER_SIZE = 128;
  char buffer[128] = { 0 };
  if (fgets(buffer, sizeof(buffer), fp) == NULL) {
    return -1;
  }

  fclose(fp);

  size_t firstIndex = 0;
  size_t lastIndex = 0;

  for (int i = 0; i < strnlen(buffer, BUFFER_SIZE) - 1; i++) {
    if (isdigit(buffer[i])) {
      firstIndex = i;
      break;
    }
  }

  for (int i = strnlen(buffer, BUFFER_SIZE) - 1; i > 0; i--) {
    if (isdigit(buffer[i])) {
      lastIndex = i;
      break;
    }
  }

  int length = lastIndex - firstIndex + 1;

  char* stringAsMemory = (char*)calloc(length + 1, 1);
  if (stringAsMemory == NULL) {
    return -1;
  }
  
  strncpy(stringAsMemory, &buffer[firstIndex], length);
      
  double totalMemory = (atof(stringAsMemory) / 1024);

  free(stringAsMemory);
  
  return totalMemory;
}

/* Gets the available memory on the system by reading from /proc/meminfo and searching for "MemAvailable" */
double getAvailableMemory(void) {
  FILE* fp;
  const char path[] = "/proc/meminfo";
  fp = fopen(path, "r");
  if (fp == NULL) {
    return -1;
  }

  char buffer[1024];
  unsigned long operand = 0;
  
  while (fgets(buffer, sizeof(buffer), fp) != NULL) {
    sscanf(buffer, "MemAvailable: %lu kB", &operand);
  }

  fclose(fp);
  
  double usedMemory = ((double)operand  / 1024); 

  return usedMemory;
}

// void displayImage() {
//   char path[] = "~/Pictures/picture.jpg";
//   FILE *fp = fopen(path, "rb");
  
//   base64_encode(path, , size_t *out_len)
// }

int main(void) {
  char* username = getUser();
  char* os = getOS();
  char* shell = getShell();
  char* desktop = getDesktop();
  char* terminal = getTerminal();
  double totalMemory = getTotalMemory();
  double usedMemory = totalMemory - getAvailableMemory();

  printf("@%s\n\n", username);
  printf("os: %s\n", os);
  free(os);
  printf("shell: %s\n", shell);
  free(shell);
  printf("desktop: %s\n", desktop);
  free(desktop);
  printf("terminal: %s\n", terminal);
  free(terminal);
  printf("memory: %2.lf MiB / %2.lf MiB\n", usedMemory, totalMemory);
  
  return 0;
}
