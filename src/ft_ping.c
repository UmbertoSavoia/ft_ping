#include "ft_ping.h"

int error(void)
{
    return printf("ft_ping: usage error: Destination address required\n");
}

int main(int ac, char **av)
{
    if (ac < 2)
        return error();

    char    opts[256] = {0};
    int     arg = 0;

    switch ((arg = check_arg(ac, av, opts))) {
        case -1:
            printf("ft_ping: invalid option\n");
            //TODO print help
            return 1;
        case 0:
            return error();
    }
    printf("%d\n", arg);
}
