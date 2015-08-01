#ifndef _CONFIG_H_
#define _CONFIG_H_

#include <stdbool.h>
#include <stdio.h>

#define NAME_MAX    50
#define VALUE_MAX   50
#define SECNAME_MAX 50
#define KEY_MAX     50
#define LINE_MAX    300

typedef struct ini_key {
    char name[NAME_MAX];
    char value[VALUE_MAX];
} ini_key;

typedef struct ini_section {
    char secname[SECNAME_MAX];  /* section name */
    int keynum;                 /* key number in the section */
    ini_key key[KEY_MAX];       /* an array to store the keys in the section */
    struct ini_section *next;   /* next section */
} ini_section;

typedef struct config_t {
    ini_section *section;
} config_t;

config_t *config_new(const char *);
void config_free(config_t *);
bool config_getString(config_t *config, const char *section, const char *name, char *value);
bool config_getInt(config_t *config, const char *section, const char *name, int *value);

#endif