#include <sys/socket.h>
#include <netinet/in.h>
#include <err.h>
#include <strings.h>
#include <stdio.h>
#include <unistd.h>

const char reply[] =
	"HTTP/1.1 200 OK\r\n"
	"Content-type: text/plain\r\n"
	"\r\n"
	"OK\r\n";

int main() {
	int s;

	if ((s = socket(PF_INET, SOCK_STREAM, 0)) < 0)
		err(1, "socket");

	struct sockaddr_in sin;
	bzero(&sin, sizeof(sin));
	sin.sin_len = sizeof(sin);
	sin.sin_family = AF_INET;
	sin.sin_port = htons(8081);
	sin.sin_addr.s_addr = htonl(INADDR_ANY);

	if (bind(s, (struct sockaddr*)&sin, sizeof(sin)) < 0)
		err(1, "bind");

	if (listen(s, 4) < 0)
		err(1, "listen");

    int nreq = 0;

	char buf[65536];
	while (1) {
		int s2 = accept(s, NULL, NULL);
		if (s2 < 0)
			err(1, "accept");

		if (recv(s2, buf, 65536, 0) < 0)
			err(1, "recv");

		usleep(1000000 * (nreq & 1) + 500000); // 0.5 sec - 1.5 sec - 0.5 sec - 1.5 sec ...

		if (send(s2, reply, sizeof(reply), 0) < 0)
			err(1, "send");

		close(s2);

        nreq++;
	}

	return 0;
}
