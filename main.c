#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>

int flen(char *path)
{
    FILE    *f;
    char    c;
    int     size;

    f = fopen(path, "rt");
    size = 0;
    while ((c = fgetc(f)) != EOF)
        size++;
    fclose(f);
    return (size);
}

char    *create_content(char    *path, int size)
{
    FILE    *f;
    char    c;
    char    *ret;
    int     i;

    ret = (char *) malloc ((size + 1) * sizeof(char));
    if (!ret)
        return (NULL);
    f = fopen(path, "rt");
    i = 0;
    while ((c = fgetc(f)) != EOF)
    {
        ret[i] = c;
        i++;
    }
    fclose(f);
    return (ret);
}

int nb_to_replace(char  *content, char  *to_replace)
{
    int     count;
    char    *result;

    count = 0;
    result = strstr(content, to_replace);
    while (result)
    {
        count++;
        result = strstr(result + 1, to_replace);
    }
    return (count);
}

char    *replacing(char *content, char  *to_replace, char   *new_str)
{
    int     len_search;
    int     len_replace;
    int     new_len;
    int     i;
    int     j;
    char    *new_content;

    len_search = strlen(to_replace);
    len_replace = strlen(new_str);
    new_len = strlen(content);
    i = 0;
    j = 0;
    new_content = (char *) malloc (new_len + 1);
    if (!new_content)
    {
        free(content);
        return (NULL);
    }
    while (content[i])
    {
        if (strstr(&content[i], to_replace) == &content[i])
        {
            new_len += len_replace - len_search;
            new_content = realloc(new_content, new_len + 1);
            strncpy(&new_content[i + j], new_str, len_replace);
            i += len_search;
            j += len_replace - len_search;
        }
        else
        {
            new_content[i + j] = content[i];
            i++;
        }
    }
    free(content);
    return (new_content);
}

int create_new_file(char    *path, char    *content)
{
    FILE    *f;

    f = fopen(path, "w");
    if (f == NULL)
        return (1);
    fputs(content, f);
    fclose(f);
    return (0);
}

int prompt(char *path, char *to_replace, char *new_str)
{
    int     size;
    char    *content;
    
    size = flen(path);
    content = create_content(path, size);
    if (!content)
        return (1);
    content = replacing(content, to_replace, new_str);
    if (create_new_file(path, content))
    {
        free(content);
        return (1);
    }
    free(content);
    return (0);
}

int replaceInFolder(char    *folderPath, char   *to_replace, char   *new_str)
{
    DIR*            dir;
    struct dirent*  ent;

    dir = opendir(folderPath);
    if (dir == NULL)
    {
        printf("Something wrong happened during opening folder!");
        return (1);
    }
    while ((ent = readdir(dir)) != NULL)
    {
        if (strcmp(ent->d_name, ".") != 0 && strcmp(ent->d_name, "..") != 0) {
            char* filePath = (char*)malloc(sizeof(char) * (strlen(folderPath) + strlen(ent->d_name) + 2));
            sprintf(filePath, "%s/%s", folderPath, ent->d_name);
            prompt(filePath, to_replace, new_str);
            free(filePath);
        }
    }
    closedir(dir);
    printf("Done!");
    return (0);
}

int main(int argc, char **argv)
{
    if (argc < 4)
        return (1);
    else
        return (prompt(argv[1], argv[2], argv[3]));
    return (0);
}