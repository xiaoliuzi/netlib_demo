#include <uv.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

uv_loop_t *loop;

void alloc_buffer(uv_handle_t *handle, size_t suggested_size, uv_buf_t *buf) {
        buf->base = (char*)malloc(suggested_size);
        buf->len = suggested_size;

}

void echo_read(uv_stream_t *client, ssize_t nread, const uv_buf_t *buf) {
     char *str = malloc(nread + 1);
     strncpy(str, buf->base, nread);
     str[nread] = '\0';
     printf("%s\n", str);
     free(str);
     uv_stop(loop);
}

void echo_write(uv_write_t *req, int status)
{
        if(status) {
                fprintf(stderr, "Write error %s\n", uv_strerror(status));
        }
        uv_stream_t *client = req->handle;
        uv_read_start(client, alloc_buffer, echo_read);

        free(req);
}

// call back function
void on_connect(uv_connect_t *req, int status) {
        if (status < 0) {
                fprintf(stderr, "New connection error %s\n", uv_strerror(status));
                return;
        }
        uv_stream_t *client = req->handle;
        uv_write_t *reqWrite = (uv_write_t*)malloc(sizeof(uv_write_t));

        char *str ="hello libuv";
        uv_buf_t wrbuf = uv_buf_init(str, strlen(str));
        uv_write(reqWrite, client, &wrbuf, 1, echo_write);

}

int main()
{
        loop = uv_default_loop();
        uv_tcp_t* socket = (uv_tcp_t*)malloc(sizeof(uv_tcp_t));
        uv_tcp_init(loop, socket);

        uv_connect_t* connect = (uv_connect_t*)malloc(sizeof(uv_connect_t));
		//
        struct sockaddr_in dest;
        uv_ip4_addr("127.0.0.1", 7000, &dest);
        uv_tcp_connect(connect, socket,(struct sockaddr*)&dest, on_connect);
        uv_run(loop, UV_RUN_DEFAULT);
}

