static int recvfrom_timeout(int sock, void *buf, int max_size, struct sockaddr *addr, socklen_t *length_ptr, int timeout/*ms*/)
{
	int retval = 0;
	int len = 0;
	fd_set rfds;
	struct timeval tv;
	FD_ZERO(&rfds);
	FD_SET(sock, &rfds);
	tv.tv_sec = timeout / 1000;
	tv.tv_usec = (timeout % 1000) * 1000;
	retval = select(sock + 1, &rfds, NULL, NULL, &tv);
	if (retval == -1) {
		return -1;
	} else if (retval) {
		len = recvfrom(sock, buf, max_size, 0, addr, length_ptr);
	} else {
		return -1;
	}
	return len;
}
