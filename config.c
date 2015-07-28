#include <stdlib.h>
#include <string.h>
#include "config.h"


#define OPEN_FLAGS	"r"
#define LINE_MAX	1024

/*
 * return the number of port on success, or -1 on error.
 */
static int config_get_port(config_t *conf)
{
	int port;
	char *ep;
	struct conflink *next = conf->list;

	while (NULL != next) {
		if ((strcmp(next->cl_key, "port")) == 0) {
			port = strtol(next->cl_val, &ep, 10);
			if (ep && *ep)
				return -1;
			return port;
		}
		next = next->cl_next;
	}
	return -1;
}

static void config_stripnl(char *src)
{
	int len = strlen(src);
	if (src[len - 1] == '\n')
		src[len - 1] = 0;
}

static int config_read(config_t *conf)
{
	char line[LINE_MAX], *p;
	struct conflink **next = &conf->list;

	while (NULL != fgets(line, LINE_MAX - 1, conf->fp)) {
		if (NULL != (p = strchr(line, '='))) {
			*p++ = 0;
			
			*next = malloc(sizeof(**next));
			if (NULL == *next)
				return -1;
			
			strcpy((*next)->cl_key, line);
			config_stripnl(p);	/* strip '\n' */
			strcpy((*next)->cl_val, p);
			
			(*next)->cl_next = NULL;
			next = &(*next)->cl_next;
		}
	}
	return 0;
}

static void config_delete(config_t *conf)
{
	struct conflink *next, *t;

	if (NULL != conf) {
		next = conf->list;

		/* free the key=val pairs list */
		while (NULL != next) {
			t = next;
			next = next->cl_next;
			free(t);
		}

		free(conf);
	}
}

/*
 * return a pointer to config_t, which is allocated by malloc().
 */
config_t *config_new(const char *path)
{
	config_t *conf;

	conf = malloc(sizeof(*conf));
	if (NULL != conf) {
		memset(conf, 0, sizeof(*conf));
		/* config's APIs initial... */
		conf->get_port = config_get_port;
		conf->delete = config_delete;

		conf->fp = fopen(path, OPEN_FLAGS);
		if (NULL != conf->fp) {
			/* read configure file, and establish the linked list */
			if (-1 == config_read(conf))
				return NULL;

			return conf;
		}
	}

	return NULL;
}
