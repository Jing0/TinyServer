#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include "request.h"

#define NOW_PATH "./HTTPDOC"

void server_file(int client, char *path);
void bad_notfound(int client);
void headers(int client);
void http_analysis(const char *buf, size_t length, const char *query, char *rst);



void request(int client, const char *buf)
{
	char method[255];
	char path[255];
	char query_string[510];
	char url[255];

	memset(method, 0, sizeof(method));
	memset(path, 0, sizeof(path));
	memset(query_string, 0, sizeof(query_string));
	memset(url, 0, sizeof(url));

	//get http method and some message
	http_analysis(buf, strlen(buf), "Method", method);
	http_analysis(buf, strlen(buf), "Url", url);
	//printf("%s\n", buf);
	//printf("Url: %s\n", url);

	if(url[strlen(url) - 1] == '/')
		strcat(url,"index.html");
	if(url[strlen(url) - 1] == '\0')
		strcpy(url,"/index.html");

	strcpy(path, NOW_PATH);
	strcat(path, url);
	//printf("Url: %s\n", url);
	printf("Path: %s\n", path);
	//printf("Method: %s\n", method);
	fflush(stdout);
	
	if(!strcasecmp(method, "GET"))
	{
		//GET
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
	fp = fopen(path, "rb");
	if(fp == NULL)
	{
		bad_notfound(client);
	}
	else
	{
		//headers(client);
		char buf[1024];
		
		while(!feof(fp))
		{
			memset(buf, 0, sizeof(buf));
			fgets(buf, sizeof(buf), fp);
			send(client, buf, strlen(buf), MSG_NOSIGNAL);
		}
		fclose(fp);
	}
	fp = NULL;
}

void bad_notfound(int client)
{
	char buf[1024];
	memset(buf, 0, sizeof(buf));

	// strcpy(buf, "HTTP/1.0 404 Not Found\r\n");
	// send(client, buf, strlen(buf), 0);
	// strcpy(buf, "Server: MyServer/0.1.0\r\n");
	// send(client, buf, strlen(buf), 0);
	// sprintf(buf, "Content-Type: text/html\r\n");
	// send(client, buf, strlen(buf), 0);
	// strcpy(buf, "\r\n");
	//send(client, buf, strlen(buf), 0);
	server_file(client, "./HTTPDOC/404NotFound.html");
}

void headers(int client)
{
	char buf[1024];
	memset(buf, 0, sizeof(buf));

	strcpy(buf, "HTTP/1.1 200 OK\r\n");
	send(client, buf, strlen(buf), 0);
	strcpy(buf, "Server: MyServer/0.1.0\r\n");
	send(client, buf, strlen(buf), 0);
	//sprintf(buf, "Content-Type: text/html\r\n");
	//send(client, buf, strlen(buf), 0);
	strcpy(buf, "\r\n");
	send(client, buf, strlen(buf), 0);
}
