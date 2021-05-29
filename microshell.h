#ifndef MICROSHELL_C
# define MICROSHELL_C

# include <unistd.h>
# include <stdlib.h>
# include <sys/wait.h>
# include <string.h>

typedef struct s_tokens
{
    char **cmds;
    int std_out;
    int std_in;
    struct s_tokens *next;
}        t_tokens;

#endif