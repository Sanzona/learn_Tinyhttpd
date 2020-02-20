#ifndef HTTPD_H
#define HTTPD_H

#define ISspace(x) isspace((int)(x))
#define SERVER_STRING "Server: jdbhttpd/0.1.0\r\n"
#define STDIN 	0
#define STDOUT 	1
#define STDERR 	2

void accept_request(void *);
void bad_request(int);
void cat(int, FILE *);
void cannot_execute(int);
void error_die(const char*);
void execute_cgi(int, const char *, const char *, const char *);
int get_line(int, char *, int);
char *mime_type(const char *filename);
void headers(int, const char *);
void not_found(int);
void serve_file(int, const char *);
int starup(u_short *);
void unimplemented(int);

#endif
