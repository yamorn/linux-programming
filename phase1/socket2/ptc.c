#include "ptc.h"

ssize_t readline(int fd, void *buf, size_t maxlen){
	ssize_t n, rc;
	char c, *ptr;

	ptr = buf;
	for(n = 1; n < maxlen; n++){
		again:
		if((rc = read(fd, &c, 1)) == 1){
			*ptr++ = c;
			if(c == '\n')
				break; /* newline is stored, like fgets() */
		}else if(rc == 0){
			*ptr = 0; /* line terminal char '\0' */
			return(n - 1);
		}else{
			if(errno == EINTR)
				goto again;
			return(-1); /* error, errno set by read() */
		}
	}
	*ptr = 0; /* null terminate like fgets(); */
	return(n);
}

ssize_t writen(int fd, const void *buf, size_t nbytes){
	size_t nleft;
	ssize_t nw;
	const char *ptr;

	ptr = buf;
	nleft = nbytes;
	while(nleft > 0){
		if((nw = write(fd, ptr, nleft)) <= 0){
			if(nw < 0 && errno == EINTR)
				nw = 0; /* and call write() again */
			else
				return(-1); /* error */
		}
		nleft -= nw;
		ptr += nw;
	}
	return(nbytes - nleft);
}





























