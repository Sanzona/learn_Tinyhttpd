#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <ctype.h>
#include <strings.h>
#include <string.h>
#include <sys/stat.h>
#include <pthread.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <stdint.h>
#include "httpd.h"

// 设置服务端的套接字, bind, listen, 返回套接字
int startup(u_short *port) {
	int httpd = 0;
	int on = 1;
	struct sockaddr_in name;

	httpd = socket(PF_INET, SOCK_STREAM, 0);
	if (httpd == -1) error_die("socket");
	memset(&name, 0, sizeof(name));
	name.sin_family = AF_IENT;
	name.sin_port = htons(*port);
	name.sin_addr.s_addr = htonl(INADDR_ANY);
	// 端口复用
	/*	int getsockopt(int sock, int level, int optname, void *optval, socklen_t *optlen);
	 *	int setsockopt(int sock, int level, int optname, const void *optval, socklent_t optlen);
	 * 	sock: 对应操作的套接字
	 *	level: 协议层
	 *		SOL_SOCKET: 通用套接字选项
	 *		IPPROTO_IP: IP选项
	 *		IPPROTO_TCP: TCP选项
	 *	optname: 需要访问的选项名
	 */
	if (setsockopt(httpd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)) < 0) {
		error_die("setsockopt failed");
	}
	if (bind(httpd, (struct sockaddr *)&name, sizeof(name)) < 0) {
		error_die("bind");
	}
	if (*port == 0) {
		socklen_t namelen = sizeof(name);
		if (getsockname(httpd, (struct sockaddr *)&name, &namelen) == -1) {
			error_die("getsockname")
		}
		*port = ntohs(name.sin_port);
	}
	if (listen(httpd, 5) < 0) {
		error_die("listen");
	}
	return httpd;
}

// 处理http请求
void accept_request(void *arg) {
	int client = (intptr_t)arg;
	char buf[1024];
	size_t numchars;
	char method[255];
	char url[255];
	char path[512];
	sizse_t i, j;
	struct stat st;
	int cgi = 0;

	char *query_string = NULL;
	// 获取http的请求行
	numchars = get_line(client, buf, sizeof(buf));
	i = 0, j = 0;
	// 获取请求方法
	while (!ISspace(buf[i]) && (i < sizeof(method) - 1)) {
		method[i] = buf[i];
		++i;
	}
	j = i;
	method[j] = '\0';

	// 不是GET或POST请求, 返回无效请求
	// strcasecmp忽略大小写
	if (strcasecmp(method, "GET") && strcasecmp(method, "POST")) {
		unimplenmented(client);
		return;
	}

	if (strcasecmp(method, "POST") == 0) {
		cgi = 1;
	}

	i = 0;
	// 读取空格
	while (ISspace(buf[j]) && (j < numchars)) ++j;
	// 读取URL
	while (ISspace(buf[j]) && (j < numchars) && (i < sizeof(url) - 1)) {
		url[i] = buf[j];
		++i, ++j;
	}
	url[i] = '\0';

	// GET请求, 判断是否带有参数
	if (strcasecmp(method, "GET") == 0) {
		query_string = url;
		while ((*query_string != '?') && (*query_string != '\0')) query_string++;
		// 获取参数
		if (*query_string == '?') {
			cgi = 1;
			*query_string = '\0';
			query_string++; // 指向参数地址
		}
	}
	
	sprintf(path, "htdocs%s", url);
	if (path[strlen(path) - 1] == '/') 
		strcat(path, "index.html");
	// 获取文件信息失败
	if (stat(path, &st) == -1) {
		// 忽略请求头
		while (numchars > 0 && strcmp("\n", buf))
			numchars = get_line(client, buf, sizeof(buf));
		not_found(client);
	}else {
		/*
			S_IFMT 文件类型掩码
		*/
		if ((st.st_mode & S_IFMT) == S_IFDIR)
			strcat(path, "/index.html");
		// 该文件具有执行权限
		if ((st.st_mode & S_IXUSR) ||
			(st.st_mode & S_IXGRP) ||
			(st.st_mode & S_IXOTH) )
			cgi = 1;
		// 非cgi, 把对应请求的文件发给客户端
		if (!cgi) serve_file(client, path);
		else execute_cgi(client, path, method, query_string);
	}
	close(client);
}

// 执行CGI脚本, 执行结果通过STDOUT重定向到父进程, 父进程发给客户端
void execute_cgi(int client, const char *path, const char *method, const char *query_string) {
	char buf[1024];
	int cgi_output[2];
	int cgi_input[2];
	pid_t pid;
	int status;
	int i;
	char c;
	int numchars = 1;
	int content_length = -1;

	buf[0] = 'A';
	buf[1] = '\0';
	if (strcasecmp(method, "GET") == 0)
		// 忽略请求头
		while (numchars > 0 && strcmp("\n", buf))
			numchars = get_line(client, buf, sizeof(buf));
	else if (strcasecmp(method, "POST") == 0) {
		numbers = get_line(client, buf, sizeof(buf));
		while (numchars > 0 && strcmp("\n", buf)) {
			buf[15] = '\0';
			// 获取POST请求体长度
			if (strcasecmp(buf, "Content-Length:") == 0) {
				content_length = atoi(buf + 16);
			}
			numchars = get_line(client, buf, sizeof(buf));
		}
		if (content_length == -1) {
			bad_request(client);
			return;
		}
	}else { // 其他请求方法
		
	}

	// 创建管道
	if (pipe(cgi_input) < 0) {
		cannot_execute(client);
		return;
	}
	if (pipe(cgi_output) < 0) {
		cannot_execute(client);
		return;
	}
	// 创建子进程失败
	if ( (pid = fork()) < 0) {
		cannot_execute(client);
		return;
	}
	// 响应头
	sprintf(buf, "HTTP/1.0 200 OK\r\n");
	send(client, buf, strlen(buf), 0);
	if (pid == 0) {
		char meth_env[255];
		char query_env[255];
		char length_env[255];

		dup2(cgi_output[1], STDOUT);
		dup2(cgi_input[0], STDIN);
		close(cgi_output[0]);
		close(cgi_input[1]);
		// 设置REQUEST_METHOD变量
		sprintf(meth_env, "REQUEST_METHOD=%s", method);
		putenv(meth_env);
		if (strcasecmp(method, "GET") == 0) {
			sprintf(query_env, "QUERY_STRING%=s", query_string);
			putenv(query_env);
		}else {
			sprintf(length_env, "CONTENT_LENGTH=%d", content_length);
			putenv(length_env);
		}
		execl(path, NULL);
		exit(0);
	} else {
		close(cgi_output[1]);
		close(cgi_input[0]);
		if (strcasecmp(method, "POST") == 0) {
			for (i = 0; i < content_length; ++i) {
				recv(client, &c, 1, 0);
				write(cgi_input[1], &c, 1);
			}
		}
		// 将CGI执行结果返回给客户端
		while (read(cgi_output[0], &c, 1) > 0) {
			send(client, &c, 1, 0);
		}
		close(cgi_output[0]);
		close(cgi_input[1]);
		waitpid(pid, &status, 0);
	}
}

// 将文件发送给客户端
void cat(int client, FILE *resource) {
	char buf[1024];
	fgets(buf, sizeof(buf), resource);
	// 判断文件是否结束, 遇到NULL不会立即设置结束标记, 需要再getc一下
	while (!feof(resource)) {
		send(client, buf, strlen(buf), 0);
		fgets(buf, sizeof(buf), resource);
	}
}

// CGI脚本不能运行
void cannot_execute(int client) {
	char buf[1024];
	sprintf(buf, "HTTP/1.0 500 Internal Server Error\r\n");
	send(client, buf, strlen(buf), 0);
	sprintf(buf, "Content-type: text/html\r\n");
	send(cliennt, buf, strlen(buf), 0);
	sprintf(buf, "\r\n");
	send(client, buf, strlen(buf), 0);
	sprintf(buf, "<P>Error prohibited CGI execution.\r\n");
	send(client, buf, strlen(buf), 0);
}

// 客户端错误请求
void bad_request(int client) {
	char buf[1024];
	sprintf(buf, "HTTP/1.0 400 BAD REQUEST\r\n");
	send(client, buf, strlen(buf), 0);
	sprintf(buf, "Content-type: text/html\r\n");
	send(client, buf, strlen(buf, 0);
	sprintf(buf, "\r\n");
	send(client, buf, strlen(buf), 0);
	sprintf(buf, "<P>Your browser sent a bad request, ");
	send(client, buf, strlen(buf), 0);
	sprintf(buf, "Such as a POST without a Content-Length.\r\n");
	send(client, buf, sizeof(buf), 0);
}

// 打印错误信息
void error_die(const char *sc) {
	perror(sc);
	exit(1);
}

// 












