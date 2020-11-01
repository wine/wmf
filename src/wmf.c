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

size_t print_kernel_name() {
    struct utsname name;
    uname(&name);

    printf("kernel: %s %s\n", to_lower(name.sysname), to_lower(name.release));

    return strlen("kernel: ") + strlen(name.sysname) + strlen(name.release);
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

size_t print_gpu() {
    char* gpu;
 
    FILE* fp = popen("lspci -mm", "r"); 

    char tmp[128], manufacturer[128];
 
    while (fgets(tmp, 127, fp) != NULL) {
        if(strstr(tmp, "VGA compatible controller")){
            strtok(tmp, "\"");  
            strtok(NULL, "\""); 
            strtok(NULL, "\"");
 
            char *token = strtok(NULL, "\"");

            if (strstr(token, "Intel"))
                strcpy(manufacturer, "intel");
            else if (strstr(token, "AMD") || strstr(token, "ATI")
                     || strstr(token, "ATi"))
                strcpy(manufacturer, "amd");
            else if (strstr(token, "NVIDIA")){
                strcpy(manufacturer, "nvidia");
            }
            
            strtok(NULL, "\"");
 
            token = strtok(NULL, "\"");

            if (strstr(token, "[")){
                while(*token != '['){
                    token++;
                }
                token++;
            }

            token[strcspn(token, "]")] = 0;

            asprintf(&gpu, "%s %s", manufacturer, token);
            
            break;
        }
    }

    gpu[strcspn(gpu, "\n")] = 0;
    
    pclose(fp);

    printf("gpu: %s\n", gpu);

    return strlen("gpu: ") + strlen(gpu);
}

size_t print_ram() {
    char* ram;

    FILE* fp = fopen("/proc/meminfo", "r");

    char tmp[128];
    char* token, *ptr;
    size_t total_mem = 0, free_mem = 0, buffers = 0, cached = 0;

    while (fgets(tmp, 127, fp) != NULL && !(total_mem && free_mem && buffers && cached)) {
        token = strtok(tmp, " ");

        if (!strcmp(token, "MemTotal:")){
            token = strtok(NULL, " ");
            total_mem = strtol(token, &ptr, 10);
        } else if (!strcmp(token, "MemAvailable:")){
            token = strtok(NULL, " ");
            free_mem = strtol(token, &ptr, 10);
        } else if (!strcmp(token, "Buffers:")){
            token = strtok(NULL, " ");
            buffers = strtol(token, &ptr, 10);
        } else if (!strcmp(token, "Cached:")){
            token = strtok(NULL, " ");
            cached = strtol(token, &ptr, 10);
        }
    }

    asprintf(&ram, "%zi/%zi mib",
            (total_mem - free_mem - buffers) / 1024, total_mem / 1024);
    
    fclose(fp);

    printf("ram: %s\n", ram);

    return strlen("ram: ") + strlen(ram);
}

int main() {
#ifdef USER_AND_HOST 
    print_seperator_line(print_user_name_at_host_name());
#endif

#ifdef KERNEL
    print_kernel_name();
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

#ifdef GPU
    print_gpu();
#endif

#ifdef RAM
    print_ram();
#endif

#ifdef WM
    print_wm_name();
#endif
}
