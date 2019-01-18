//gcc touch.c -o touch.o -lX11 -lXtst

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <X11/Xlib.h>
#include <X11/extensions/XTest.h>

int main(int argc, char *argv[])
{
	Display *display = XOpenDisplay(NULL);
	
	struct sockaddr_in saddr, caddr;
	socklen_t addrlen = sizeof(struct sockaddr_in);
	memset(&saddr, 0, addrlen);
	
	saddr.sin_family = AF_INET;
	saddr.sin_addr.s_addr = INADDR_ANY;
	saddr.sin_port = htons(10110);
	
	int o, x, y, v, cfd = -1, sfd = socket(AF_INET, SOCK_STREAM, 0);
	setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR, &(int){1}, 4);
	bind(sfd, (struct sockaddr*)&saddr, addrlen);
	listen(sfd, SOMAXCONN);
	
	while(1)
	{
		if(cfd == -1)
			cfd = accept(sfd, (struct sockaddr*)&caddr, &addrlen);
		
		if((v = (recv(cfd, &o, 4, MSG_WAITALL) == 4)) && !o)
			v = (recv(cfd, &x, 4, MSG_WAITALL) + recv(cfd, &y, 4, MSG_WAITALL)) == 8;
			
		if(v == 0)
		{
			cfd = -1;
			continue;
		}
		
		if(ntohl(o) == 0)
			XWarpPointer(display, None, None, 0, 0, 0, 0, ntohl(x), ntohl(y));
		else if(ntohl(o) == 1)
			XTestFakeButtonEvent(display, 1, 1, 0);
		else if(ntohl(o) == 2)
			XTestFakeButtonEvent(display, 1, 0, 0);
		else if(ntohl(o) == 3)
			XTestFakeButtonEvent(display, 3, 1, 0);
		else if(ntohl(o) == 4)
			XTestFakeButtonEvent(display, 3, 0, 0);
		XFlush(display);
	}
}
