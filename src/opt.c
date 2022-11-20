#include "ft_ping.h"

/**
 * is_opt
 * @param av
 * @return -1: invalid
 *          0: false
 *          1: true
 */
int     is_opt(const char *av, char *type)
{
    t_check_opt check_opt[] = {
            { "-v", OPT_BOOL },
            { "-t", OPT_ARG },
            { "-h", OPT_BOOL },
            { "-c", OPT_ARG },
            { "-s", OPT_ARG }
    };
    int len = sizeof(check_opt) / sizeof(check_opt[0]);

    for (int i = 0; i < len; ++i) {
        if (!memcmp(check_opt[i].opt, av, 2)) {
            *type = check_opt[i].type;
            return 1;
        }
    }
    return (av[0] == '-') ? -1 : 0;
}

/**
 * check_arg
 * @param ac
 * @param av
 * @param opt
 * @return indice dell'ultimo argomento diverso da un'opzione
 *          oppure -1 se l'opzione non è valida
 */
int     check_arg(int ac, char **av, int *opt)
{
    int ret = 0;
    char type = 0;

    for (int i = 1; i < ac; ++i) {
        switch (is_opt(av[i], &type)) {
            case -1:
                return -1;
            case 0:
                ret = i;
                break;
            case 1:
                if (type == OPT_ARG) {
                    if (av[i][2] == 0) {
                        opt[av[i][1]] = atoi(av[i+1]);
                        ++i;
                    } else {
                        opt[av[i][1]] = atoi(&av[i][2]);
                    }
                } else {
                    opt[av[i][1]] = 1;
                }
                break;
        }
    }
    return ret;
}