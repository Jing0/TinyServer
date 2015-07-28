#ifndef _CONFIG_H
#define _CONFIG_H
#include <stdio.h>

struct conflink {
#define CONFLINK_KEY_MAX	256
#define CONFLINK_VAL_MAX	1024
	char cl_key[CONFLINK_KEY_MAX];
	char cl_val[CONFLINK_VAL_MAX];
	struct conflink *cl_next;
};


typedef struct config config_t;
struct config {
	FILE *fp;
	struct conflink *list;

	int (*get_port)(config_t *);
	void (*delete)(config_t *);
};

config_t *config_new(const char *path);

#endif
