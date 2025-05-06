#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <ctype.h>
#include <sixel.h>


char* getUser() {
  char* username = getenv("USER");
  if (username == NULL) {
    return NULL;
  }

  return username;
}


char* getOS() {
  FILE *fp;
  char operatingSystem[256] = { 0 };

  int length = 0;

  const int strSize = 128;
  char* str = (char*)calloc(strSize + 1, 1);

  if (str == NULL) {
    return NULL;
  }
  
  fp = popen("cat /etc/os-release | grep -i \"PRETTY_NAME\"", "r");

  if (fgets(operatingSystem, sizeof(operatingSystem), fp) == NULL) {
    return NULL;
  };

  pclose(fp);

  bool isQuoted = false;
  
  for (int i = 0; operatingSystem[i] != 0; i++) {
    if (length > strSize) {
      break;
    }

    if (operatingSystem[i] == '"') {
      isQuoted = !isQuoted;
    } else if (isQuoted == true) {
      str[length++] = operatingSystem[i];
    }
  };

  return str; 
}

char* getShell() {
  FILE *fp;
  char path[128];
  char buffer[128] = { 0 };

  snprintf(path, sizeof(path), "/proc/%d/cmdline", getppid());

  fp = fopen(path, "r");

  if (fp == NULL) {
    return NULL;
  }
  
  if (fgets(buffer, sizeof(buffer), fp) == NULL) {
   return NULL; 
  }

  fclose(fp);
  
  char* shell = (char*)calloc(128, 1);
  
  if (shell == NULL) {
    return NULL;
  }

  strncpy(shell, (strrchr(buffer, '/') + 1), 127);
  
  return shell;
}

char* getDesktop() {
  char* desktop = (char*)calloc(128, 1);
  if (desktop == NULL) {
    return NULL;
  }
  
  strcpy(desktop, getenv("XDG_CURRENT_DESKTOP"));

  return desktop;
}

char* getTerminal() {
  char* terminal = (char*)calloc(128, 1);
  if (terminal ==  NULL) {
    return NULL;
  }

  if (getenv("TERM") != NULL) {
    strncpy(terminal, getenv("TERM"), 128);
  } else {
    return NULL;
  }
  
  return terminal;
}

double getTotalMemory() {
  FILE* fp;
  const char* path = "/proc/meminfo";
  fp = fopen(path, "r");

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

  char* totalMemory = (char*)calloc(length + 1, 1);
  if (totalMemory == NULL) {
    return -1;
  } 
  
  strncpy(totalMemory, &buffer[firstIndex], length);
      
  return (atof(totalMemory) * 1024 / (1024 * 1024)) ;
}

double getAvailableMemory() {
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
  
  double usedMemory = ((double)operand * 1024) / (1024 * 1024); 

  return usedMemory;
}

void displayImage() {
  /* convert pixels into sixel format and write it to output context */
SIXELAPI SIXELSTATUS
sixel_encode(
    unsigned char  /* in */ *pixels,     /* pixel bytes */
    int            /* in */  width,      /* image width */
    int            /* in */  height,     /* image height */
    int            /* in */  depth,      /* color depth: now unused */
    sixel_dither_t /* in */ *dither,     /* dither context */
    sixel_output_t /* in */ *context);   /* output context */
}

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
};
