#include "conf.h"
#include "ini.h"
#include "map.h"

#include <string.h>
#include <stdlib.h>
#include <errno.h>

#define DEFAULT_CONF_FILE "conf.ini"

struct configuration {
    char inited;
    map_str_t map;
};

static char *comb_str(const char *section, const char *name)
{
    size_t len = strlen(section) + strlen(name) + 2;
    char *k = malloc(len);
    memset(k, 0, len);
    strcat(k, section);
    strcat(k, "%");
    strcat(k, name);
    return k;
}

static int conf_handler(void* user, const char* section, const char* name,
        const char* value)
{
    configuration *conf = (configuration *)user;
    char *k;
    char *v = strdup(value);
    k = comb_str(section, name);
    map_set(&conf->map, k, v);
    free(k);
    // free(v); // 不要释放存值的内存
    return 1;
}

configuration *config_new()
{
    return malloc(sizeof(configuration));
}

bool config_init(configuration *conf)
{
    return config_load(conf, DEFAULT_CONF_FILE);
}

bool config_free(configuration *conf)
{
    if (conf) {
        map_deinit(&conf->map);
        free(conf);
        conf = NULL;
        return true;
    }
    return true;
}

bool config_load(configuration *conf, const char *filename)
{
    if (config_is_inited(conf))
        return false;
    map_init(&conf->map);
    if (ini_parse(filename, conf_handler, conf) == 0) {
        conf->inited = 1;
        return true;
    }
    return false;
}

bool config_is_inited(configuration *conf)
{
    return conf->inited;
}

bool config_get_str(configuration *conf, const char *section, const char *name, char **value)
{
    char *k = comb_str(section, name);
    char **v = map_get(&conf->map, k);
    free(k);
    if (v) {
        *value = *v;
        return true;
    }
    return false;
}

bool config_get_int(configuration *conf, const char *section, const char *name, int *value)
{
    char* v;
    if (config_get_str(conf, section, name, &v)) {
        int i = atoi(v);
        if (errno == 0) {
            *value = i;
            return true;
        }
    }
    return false;
}

