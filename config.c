#include <string.h>
#include <stdlib.h>
#include "config.h"

static bool config_getString(config_t *config, const char *section, const char *name, char *value) {
    ini_section sec = *(config -> section);
    while (sec.next != NULL) {
        sec = *(sec.next);
        if (strcmp(section, sec.secname) == 0) {
            int i;
            for (i = 0; i < sec.keynum; ++i) {
                if (strcmp(name, sec.key[i].name) == 0) {
                    strcpy(value, sec.key[i].value);
                    return true;
                }
            }
        }
    }
    return false;
}

static bool config_getInt(config_t *config, const char *section, const char *name, int *value) {
    char str_value[VALUE_MAX];
    if (config_getString(config, section, name, str_value)) {
        *value = atoi(str_value);
        return true;
    }
    return false;
}

static int config_read(FILE *fp, config_t *config) {
    printf("loading config.ini...\n");
    char line[LINE_MAX];
    ini_section *section;
    section = malloc(sizeof(ini_section));
    config->section = section;
    char *begin, *end;
    while (fgets(line, LINE_MAX - 1, fp) != NULL) {
        /* ignore the comment */
        if (line[0] == ';') {
            continue;
        }
        // remove '\n'
        char *newline = strchr(line, '\n');
        if (newline != NULL) {
            *newline = '\0';
        }

        begin = strchr(line, '[');
        if (begin != NULL) {
            // this is section line
            section->next = malloc(sizeof(ini_section));
            section = section->next;
            section->keynum = 0;
            end = strchr(line, ']');
            *end = '\0';
            strcpy(section->secname, begin + 1);
        } else {
            // this is key line
            begin = strchr(line, '=');
            *begin = '\0';
            int i = section->keynum;
            strcpy(section->key[i].name, line);
            strcpy(section->key[i].value, begin + 1);
            ++(section->keynum);
        }
    }
    section->next = NULL;
    free(section);
    printf("config.ini loaded...\n");
    return 0;
}

void config_free(config_t *config) {
    if (config->section != NULL) {
        free(config->section);
    }
    if (config != NULL) {
        free(config);
    }
}

config_t *config_new(const char* filename) {
    config_t *config = malloc(sizeof(config_t));
    if (config == NULL) {
        perror("config: malloc");
        return NULL;
    }

    FILE *fp = fopen(filename, "r");
    if (fp == NULL) {
        perror("config: opening file");
        return NULL;
    }

    if (config_read(fp, config) == -1) {
        return NULL;
    }

    /* method initialize */
    config->getString = config_getString;
    config->getInt = config_getInt;

    return config;
}