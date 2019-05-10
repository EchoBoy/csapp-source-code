//
// Created by ligand on 2019-05-04.
//

/**
 * tiny.c 一个简单的 HTTP/1.0 服务器仅支持GET方法、支持静态、动态内容。
 */


#include "../csapp.h"

void doit(int fd);
void read_request_headers(rio_t *rp);
int parse_uri(char *uri, char *filename, char *cgiargs);
void serve_static(int fd, char *filename, int filesize);
void get_filetype(char *filename, char *filetype);
void serve_dynamic(int fd, char *filename, char *cgiargs);
void clienterror(int fd, char *cause, char *errnum, char *shortmsg, char *longmsg);

int main(int argc, char **argv) {
  int listenfd, connfd;
  char hostname[MAXLINE], port[MAXLINE];
  socklen_t clientlen;
  struct sockaddr_storage clientaddr;

  if (argc != 2) {
    fprintf(stderr, "usage: %s <port>", argv[0]);
    exit(0);
  }

  if ((listenfd = open_listenfd(argv[1])) <= 0) {
    fprintf(stderr, "listening port %s failed", argv[1]);
    exit(1);
  }

  // 事件循环
  while (1) {
    clientlen = sizeof(clientaddr);
    connfd = accept(listenfd, (SA *) &clientaddr, &clientlen);
    getnameinfo((SA *) &clientaddr, clientlen, hostname, MAXLINE, port, MAXLINE, 0);
    printf("Accepted connection form %s:%s\n", hostname, port);
    doit(connfd);
    close(connfd);
  }
}

void doit(int fd) {
  int is_static;
  char buf[MAXLINE];
  char method[MAXLINE], uri[MAXLINE], version[MAXLINE];
  rio_t rio;

  // 首行读取 请求方法、资源标识符、HTTP版本。只对 GET 方法进行处理
  rio_readinitb(&rio, fd);
  rio_readlineb(&rio, buf, MAXLINE);
  printf("Request headers:\n%s", buf);
  sscanf(buf, "%s %s %s", method, uri, version);
  if (strcasecmp(method, "GET")) {
    clienterror(fd, method, "501", "Not implemented", "Tiny does not implement this method.");
    return;
  }
  // 忽略请求头
  read_request_headers(&rio);

  // 解析 URI，判断文件是否存在。
  char filename[MAXLINE], cgiargs[MAXLINE];
  struct stat sbuf;
  is_static = parse_uri(uri, filename, cgiargs);
  if (stat(filename, &sbuf) < 0) {
    clienterror(fd, filename, "404", "Not found", "Tiny couldn't find this file.");
    return;
  }

  // 根据静态/动态请求，检查不同的文件权限，并相应的返回
  if (is_static) {
    // 静态文件直接返回
    if (!(S_ISREG(sbuf.st_mode)) || !(S_IRUSR & sbuf.st_mode)) {
      clienterror(fd, filename, "403", "Forbidden", "Tiny couldn't read the file");
      return;
    }
    serve_static(fd, filename, sbuf.st_size);
  } else {
    // 动态内容
    if (!(S_ISREG(sbuf.st_mode)) || !(S_IXUSR & sbuf.st_mode)) {
      clienterror(fd, filename, "403", "Forbidden", "Tiny couldn't run the CGI program");
      return;
    }
    serve_dynamic(fd, filename, cgiargs);
  }
}

void clienterror(int fd, char *cause, char *errnum, char *shortmsg, char *longmsg) {
  char head[MAXLINE], body[MAXLINE];

  // 构建 HTTP 返回 body
  sprintf(body,
          "<html><head><title>Tiny Error</title></head><body><p>%s: %s</p><p>%s: %s</p><p>The Tiny Web Server</p></body></html>",
          errnum,
          shortmsg,
          longmsg,
          cause);

  // 构建 HTTP 头
  sprintf(head, "HTTP/1.0 %s %s\r\nServer: Tiny Web\r\nContent-Type: text/html\r\nContent-Length: %d\r\n\r\n",
          errnum, shortmsg, (int) strlen(body));
  rio_writen(fd, head, strlen(head));
  rio_writen(fd, body, strlen(body));
}

void read_request_headers(rio_t *rp) {
  char buf[MAXLINE];

  rio_readlineb(rp, buf, MAXLINE);
  while (strcmp(buf, "\r\n")) {
    puts(buf);
    rio_readlineb(rp, buf, MAXLINE);
  }
}

int parse_uri(char *uri, char *filename, char *cgiargs) {
  char *ptr;

  if (!strstr(uri, "cgi-bin")) {
    // 静态
    strcpy(cgiargs, "");
    strcpy(filename, ".");
    strcat(filename, uri);
    if (uri[strlen(uri) - 1] == '/')
      strcat(filename, "home.html");
    return 1;
  } else {
    // 动态
    ptr = index(uri, '?');
    if (ptr) {
      strcpy(cgiargs, ptr + 1);
      *ptr = '\0';
    } else {
      strcpy(cgiargs, "");
    }
    strcpy(filename, ".");
    strcat(filename, uri);
    return 0;
  }
}

void serve_static(int fd, char *filename, int filesize) {
  int srcfd;
  char *srcp, filetype[MAXLINE], headbuf[MAXLINE];

  // send head;
  get_filetype(filename, filetype);
  sprintf(headbuf,
          "HTTP/1.0 200 OK\r\nServer: Tiny Web Server\r\nConnection: close\r\nContent-length: %d\r\nContent-Type: %s\r\n\r\n",
          filesize,
          filetype);
  rio_writen(fd, headbuf, strlen(headbuf));
  puts("Response headers:\n");
  puts(headbuf);

  if ((srcfd = open(filename, O_RDONLY, 0)) <= 0) {
    printf("Error open file: %s", filename);
    exit(1);
  }

  srcp = mmap(0, filesize, PROT_READ, MAP_PRIVATE, srcfd, 0);
  close(srcfd);
  rio_writen(fd, srcp, filesize);
  munmap(srcp, filesize);
}

void get_filetype(char *filename, char *filetype) {
  if (strstr(filename, ".html"))
    strcpy(filetype, "text/html");
  else if (strstr(filename, ".gif"))
    strcpy(filetype, "image/gif");
  else if (strstr(filename, ".png"))
    strcpy(filetype, "image/png");
  else if (strstr(filename, ".jpg"))
    strcpy(filetype, "image/jpg");
  else
    strcpy(filetype, "text/plain");
}

extern char **environ;

void serve_dynamic(int fd, char *filename, char *cgiargs) {
  char *emptylist[] = {NULL}, buf[MAXLINE];

  sprintf(buf, "HTTP/1.0 200 OK\r\nServer: Tiny Web Server\r\n\r\n");
  rio_writen(fd, buf, strlen(buf));

  if (Fork() == 0) {
    setenv("QUERY_STRING", cgiargs, 1);
    dup2(fd, STDOUT_FILENO);
    execve(filename, emptylist, environ);
  }
  wait(NULL);
}


