#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#define MAX_NAME_SZ 256
char *read_prompt();
char **split(char* str);
char **read_command();
// int main(){
//     char **argumentos=read_command();
//     int i=0;
//     while(argumentos[i]!=NULL){
//         printf("Argumento[%d]: %s\n",i,argumentos[i]);
//         i++;
//     }
//     return 0;
// }
char **read_command(){
    char *comando=read_prompt();
    char **argumentos=split(comando);
    return argumentos;
}
char *read_prompt(){
    char *str = malloc(MAX_NAME_SZ);
    /* Ask user for str. */
    /* Get the str, with size limit. */
    printf("ssh>");
    str=fgets(str, MAX_NAME_SZ, stdin);

    /* Remove trailing newline, if there. */

    if ((strlen(str) > 0) && (str[strlen (str) - 1] == '\n'))
        str[strlen (str) - 1] = '\0';
    str=realloc(str,sizeof(char)*(strlen(str)));//Ajustarlo al tamaño real del dato
    printf("El comando %s\n",str);
    return str;
}
char **split(char *str){
    char ** res  = NULL;
    char *p;
    p=strtok(str, " ");
    int n_spaces = 0, i;
    while (p) {
        res = realloc (res, sizeof (char*) * ++n_spaces);
        if (res == NULL)
        exit (-1); /* memory allocation failed */
        res[n_spaces-1] = p;
        p = strtok (NULL, " ");
        // printf("%s\n",str[1]);
    }

    /* realloc one extra element for the last NULL */

    res = realloc (res, sizeof (char*) * (n_spaces+1));
    res[n_spaces] = 0;

    /* free the memory allocated */

    return res;
}
