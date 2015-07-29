#ifndef _CONF_H_
#define _CONF_H_

#include <stdbool.h>

typedef struct configuration configuration;

configuration *config_new();
bool config_init(configuration *conf);
bool config_load(configuration *conf, const char *filename);
bool config_free(configuration *conf);

bool config_is_inited(configuration *conf);

bool config_get_str(configuration *conf, const char *section, const char *name, char **value);
bool config_get_int(configuration *conf, const char *section, const char *name, int *value);

#endif
