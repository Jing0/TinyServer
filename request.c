#include <stdio.h>
#include <string.h>
#include "request.h"

//#define NOW_PATH 

void server_file(int client, char *path);
void bad_notfound(int client);
void headers(int client);


void request(int client)
{
	char method[255];
	char path[255];
	char query_string[510];
	char url[255];

	strcpy(method,getMes("method"));
	strcpy(url,getMes("url"));
	if(!strcasecmp(method, "GET"))
	{
		//GET
		printf("%s\n", url);
		sprintf(path, "%s", url);
		headers(client);
		server_file(client, path);
	}
	else if(!strcasecmp(method, "POST"))
	{
		//POST
		;
	}
	else
	{
		printf("Not Found Method\n");
		bad_notfound(client);
	}
}
/*find this file*/
void server_file(int client, char *path)
{
	FILE *fp = NULL;
	fp = fopen(path, "r");
	if(fp == NULL)
		bad_notfound(client);
	else
	{
		char buf[1024];
		
		while(!feof(fp))
		{
			fgets(buf, sizeof(buf), fp);
			printf("%s\n", buf);
			send(client, buf, strlen(buf), 0);
		}
		close(fp);
	}
	fp = NULL;
}

void bad_notfound(int client)
{
	char buf[1024];
	memset(buf, 0, sizeof(buf));

	strcpy(buf, "HTTP/1.0 404 Not Found\r\n");
	send(client, buf, strlen(buf), 0);
	strcpy(buf, "Server: MyServer/0.1.0\r\n");
	send(client, buf, strlen(buf), 0);
	sprintf(buf, "Content-Type: text/text\r\n");
	send(client, buf, strlen(buf), 0);
	strcpy(buf, "\r\n");
	send(client, buf, strlen(buf), 0);
	server_file(client, "./404NotFound.html");
}

void headers(int client)
{
	char buf[1024];
	memset(buf, 0, sizeof(buf));

	strcpy(buf, "HTTP/1.0 200 OK\r\n");
	send(client, buf, strlen(buf), 0);
	strcpy(buf, "Server: MyServer/0.1.0\r\n");
	send(client, buf, strlen(buf), 0);
	sprintf(buf, "Content-Type: text/text\r\n");
	send(client, buf, strlen(buf), 0);
	strcpy(buf, "\r\n");
	send(client, buf, strlen(buf), 0);
}
