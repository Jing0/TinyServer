/*
 * A simple ini file reader implementation in c
 *
 * The definition of the reader and it's methods
 *
 * File:   ini.h
 * Author: Jackie Kuo <j.kuo2012@gmail.com>
 *
 * @license GNU AGPLv3
 */

#ifndef _INI_H_
#define _INI_H_

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

typedef struct ini_t {
    ini_section *section;
} ini_t;

ini_t *ini_new(const char *);
void ini_destroy(ini_t *);
bool ini_getString(ini_t *config, const char *section, const char *name, char *value);
bool ini_getInt(ini_t *config, const char *section, const char *name, int *value);

#endif