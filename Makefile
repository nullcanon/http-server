http_server:http_server.c
	gcc -o $@ $^ -lpthread

.PHONY:
clean:
	rm http_server
