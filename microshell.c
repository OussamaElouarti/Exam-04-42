#include "microshell.h"
#include <stdio.h>

int ft_strlen(char *str)
{
    int i = 0;
    while (str[i])
        i++;
    return (i);
}

void ft_error(char * str, char * str1)
{
    write(2, str, ft_strlen(str));
    if (str1)
        write(2, str1, ft_strlen(str1));
    write(2, "\n", 1);
}

t_tokens    *init_token()
{
    t_tokens *tmp;

    if (!(tmp = malloc(sizeof(t_tokens))))
        ft_error("error: fatal", NULL);
    if (!(tmp->cmds = malloc(sizeof(char *) * 1024)))
        ft_error("error: fatal", NULL);
    tmp->std_in = 0;
    tmp->std_out = 1;
    return (tmp);
}

void    ft_pipe(t_tokens *token)
{
    int pip[2];
    token->next = init_token();
    pipe(pip);
    token->std_out = pip[1];
    token->next->std_in = pip[0];
}

int add_cmd(t_tokens *token, char **argv, int *i)
{
    int k = 0;
    while (argv[*i] && (strcmp((const char *)argv[*i], "|") && strcmp((const char *)argv[*i], ";")))
    { 
        token->cmds[k] = argv[(*i)];
        (*i)++;
        k++;
    }
    (*i)++;
    token->cmds[k] = NULL;
    return (*i);
}

void    exec_cmd(t_tokens *token, int  *start, int  current)
{
    int pid;

    if (!strcmp(token->cmds[0], "cd"))
    {
        if (token->cmds[2])
            ft_error("error: cd: bad arguments", NULL);
        else if (chdir(token->cmds[1]))
            ft_error("error: cd: cannot change ", token->cmds[1]);
        return ;
    }
    pid = fork();
    if (pid == 0)
    {
        if (token->std_out != 1)
        {
            close(token->next->std_in);
            dup2(token->std_out, 1);
        }
        if (token->std_in != 0)
            dup2(token->std_in, 0);
        execve(token->cmds[0], token->cmds, NULL);
        ft_error("error: cannot execute ", token->cmds[0]);
        exit(1);
    }
    if (token->std_out != 1)
        close(token->std_out);
    if (token->std_in != 0)
        close(token->std_in);
   if (token->std_out == 1)
   {
       while (*start < current)
       {
           wait(NULL);
           (*start)++;
       }
   }
}

void    ft_free(t_tokens *token)
{
    if (token)
    {
        if(token->cmds)
            free(token->cmds);
        free(token);
        token = NULL;
    }
}

int main(int argc, char **argv)
{
    t_tokens *token;
    t_tokens *before;
    int     start;
    int i = 1;

    if (argc < 2)
        return (0);
    start = i;
    while (i < argc)
    {
        if (token == NULL || start == i)
            token = init_token();
        if (!(strcmp((const char *)argv[i], ";")))
        {
            i++;
            continue ;
        }
        i = add_cmd(token, argv, &i);
        if (argv[i - 1] &&!strcmp((const char *)argv[i - 1], "|"))
            ft_pipe(token);
        exec_cmd(token, &start, i);
        before = token;
        if (token->std_out != 1)
            token = token->next;
        ft_free(before);
    }
    return (0);
}