#include "ft_ssl.h"

int main(int argc, char** argv)
{
    t_args arguments;

    if (parse_args(argc, argv, &arguments) != 0)
        return 1;
    return dispatch(&arguments);
    return 0;
}