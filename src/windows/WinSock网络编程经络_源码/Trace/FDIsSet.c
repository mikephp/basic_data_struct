
int PASCAL FAR __WSAFDIsSet(SOCKET fd, fd_set FAR *set)
{
    u_int i = set->fd_count;

    while (i--)
    {
        if (set->fd_array[i] == fd)
            return TRUE;
    }

    return FALSE;
}