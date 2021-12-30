/*

    Autores:
        Jiménez Ruiz Gustavo Alfredo
        Santander Martínez Ángel Antonio

    Fecha de finalización:
        24 de noviembre del 2021

    Objetivo:
        Creación de un servidor que simule el comportamiento SSH de forma remota

*/

// Importación de bibliotecas
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>

#define MAX_INPUT_SIZE 256
char **read_command();
char **split(char* str);
char *read_prompt();

// Función para retornar un doble apuntador a caracter (lista) de los argumentos del comando
char **read_command(){
    char *comando=read_prompt();
    char **argumentos=split(comando);
    return argumentos;
}

// Función para leer el comando del cliente
char *read_prompt(){
    char *str = malloc(MAX_INPUT_SIZE);
    printf("Telnet> ");
    str=fgets(str, MAX_INPUT_SIZE, stdin);

    // Se intercambia '\n' por '\0'
    if ((strlen(str) > 1) && (str[strlen (str) - 1] == '\n')){
        str[strlen (str) - 1] = '\0';
        str=realloc(str,sizeof(char)*(strlen(str)));//Ajustarlo al tamaño real del dato
        printf("The command is: %s\n",str);
        return str;
    }else{
        puts("Couldn't read anything");
        return NULL;
    }
}

// Función para separar el comando en argumentos a través del token ' '
char **split(char *str){
    char ** res  = NULL;
    char *aux;
    int n_spaces = 0;
    aux=strtok(str, " ");
    while (aux!=NULL) {
        res = realloc (res, sizeof (char*) * ++n_spaces);
        if (res == NULL)
            exit (-1);
        res[n_spaces-1] = aux;
        aux = strtok (NULL, " ");
    }

    res = realloc (res, sizeof (char*) * (n_spaces+1));
    res[n_spaces] = '\0';
    return res;
}
