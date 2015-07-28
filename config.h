#ifndef _CONFIG_H
#define _CONFIG_H
#include <stdio.h>
#include <stdbool.h>

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

	// 不要用get_port这样的接口，不可能每次增加参数都增加类似的接口
	int (*get_port)(config_t *);
	// delete这个名字务必改掉
	void (*delete)(config_t *);
};

config_t *config_new(const char *path);

//建议的接口方式

/*
 * @ret 成功返回true, 失败或者section, key不存在返回false
*/

bool config_get_int(struct config* c, const char* sec, const char* key, int* result);
bool config_get_string(struct config* c, const char* sec, const char* key, char** result);


#endif
