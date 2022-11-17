#include "ft_ping.h"

/**
 * is_opt
 * @param av
 * @return -1: invalid
 *          0: false
 *          1: true
 */
int     is_opt(char *av)
{
    t_check_opt check_opt[] = {
            { "-v", OPT_BOOL }
    };
    int len = sizeof(check_opt) / sizeof(check_opt[0]);

    for (int i = 0; i < len; ++i) {
        if (!memcmp(check_opt[i].opt, av, 2))
            return 1;
    }
    return 0;
}

/**
 * check_arg
 * @param ac
 * @param av
 * @param opt
 * @return indice dell'ultimo argomento diverso da un'opzione
 *          oppure -1 se l'opzione non è valida
 */
int     check_arg(int ac, char **av, char *opt)
{
    int ret = 0;

    for (int i = 1; i < ac; ++i) {
        switch (is_opt(av[i])) {
            case -1:
                return -1;
            case 0:
                ret = i;
                break;
            case 1:
                printf("Trovato\n");
                break;
        }
    }
    return ret;
}