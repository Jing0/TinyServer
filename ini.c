/*
 * A simple ini file reader implementation in c
 *
 * The implementation of the string's methods
 *
 * File:   ini.h
 * Author: Jackie Kuo <j.kuo2012@gmail.com>
 *
 * @license GNU AGPLv3
 */

#include <string.h>
#include <stdlib.h>
#include "ini.h"

bool ini_getString(ini_t *config, const char *section, const char *name, char *value) {
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

bool ini_getInt(ini_t *config, const char *section, const char *name, int *value) {
    char str_value[VALUE_MAX];
    if (ini_getString(config, section, name, str_value)) {
        *value = atoi(str_value);
        return true;
    }
    return false;
}

static int ini_read(FILE *fp, ini_t *config) {
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

void ini_destroy(ini_t *config) {
    if (config->section != NULL) {
        free(config->section);
    }
    if (config != NULL) {
        free(config);
    }
}

ini_t *ini_new(const char* filename) {
    ini_t *config = malloc(sizeof(ini_t));
    if (config == NULL) {
        perror("config: malloc");
        return NULL;
    }

    FILE *fp = fopen(filename, "r");
    if (fp == NULL) {
        perror("config: opening file");
        return NULL;
    }

    if (ini_read(fp, config) == -1) {
        return NULL;
    }

    return config;
}