#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdarg.h>
#include <string.h>
#include <unistd.h>
#include <pwd.h>
#include <sys/utsname.h>
#include <limits.h>
#include <ctype.h>

#include "config.h"

char* to_lower(char* str) {
  for(char *p=str; *p; p++) *p=tolower(*p);
  return str;
}

size_t print_seperator_line(size_t length) {
    char line[length + 128];
    for (size_t i = 0; i != length; ++i)
        strcat(line, "-");

    printf("%s\n", line);

    return length;
}

size_t print_user_name_at_host_name() {
    char login_name[LOGIN_NAME_MAX];
    char host_name[HOST_NAME_MAX];

    getlogin_r(login_name, LOGIN_NAME_MAX);
    gethostname(host_name, HOST_NAME_MAX);

    printf("%s@%s\n", to_lower(login_name), to_lower(host_name));

    return strlen(login_name) + strlen("@") + strlen(host_name);
}

size_t print_system_name() {
    struct utsname name;
    uname(&name);

    printf("system: %s (%s)\n", to_lower(name.sysname), to_lower(name.release));

    return strlen("system: ") + strlen(name.sysname) + strlen(" (") + strlen(name.release) + strlen(")");
}

size_t print_distro_name() {
    FILE* fp = popen("lsb_release -d", "r");
    if (fp == NULL) {
        printf("distro: [!] error: couldn't run lsb_release\n");
        return 0; 
    }

    char tmp[128];
    if (fgets(tmp, sizeof(tmp) - 1, fp) == NULL) {
        printf("distro: [!] error: could not read output from lsb_release\n");
        return 0; 
    }

    fclose(fp);

    char* token = strtok(tmp, "\t");
    token = strtok(NULL, "\t");
    token[strcspn(token, "\n")] = 0; 

    printf("distro: %s\n", to_lower(token));

    return strlen("distro: ") + strlen(token);
}

size_t print_shell_name() {
    char* shell = getpwuid(geteuid())->pw_shell;

    printf("shell: %s\n", to_lower(shell));

    return strlen("shell: ") + strlen(shell);
}

size_t print_pkgs() { 
    char pkgs[128];

    FILE* fp = popen(PKGS_CMD, "r");

    fgets(pkgs, 127, fp);
    pkgs[strcspn(pkgs, "\n")] = 0; 
    
    pclose(fp);

    printf("pkgs: %s\n", pkgs);

    return strlen("pkgs: ") + strlen(pkgs);
}

size_t print_wm_name() {
    // todo: add more wms
    char* wm_directory[] = {
        "bspwm", "dwm", "herbstluftwm"
    };

    size_t wm_directory_length = sizeof(wm_directory) / sizeof(wm_directory[0]);

    char* lookup = malloc(1024);
    FILE* fp = popen("ps aux", "r");
    char* wm;

    while (fgets(lookup, sizeof(lookup) - 1, fp) != NULL) {
        for (size_t i = 0; i != wm_directory_length; ++i){
            if (strstr(lookup, wm_directory[i]) != NULL){
                asprintf(&wm, wm_directory[i]); 
                break;
            }
        }
    }

    fclose(fp);

    printf("wm: %s\n", to_lower(wm));

    return strlen("wm: ") + strlen(wm);
}

int main() {
#ifdef USER_AND_HOST 
    print_seperator_line(print_user_name_at_host_name());
#endif

#ifdef SYSTEM
    print_system_name();
#endif

#ifdef DISTRO
    print_distro_name();
#endif

#ifdef SHELL
    print_shell_name();
#endif

#ifdef PKGS
    print_pkgs();
#endif

#ifdef WM
    print_wm_name();
#endif
}
