/* validation.c - validate input files and manage file list */
#include "inverted_search.h"
#include <dirent.h>

static int has_valid_txt_extension(const char *filename)
{
    const char *dot = strrchr(filename, '.');
    if (dot == NULL)
    {
        return 0;
    }

    const char *ext = dot + 1;
    while (*ext != '\0')
    {
        if (tolower((unsigned char)*ext) != *ext)
        {
            return 0;
        }
        ext++;
    }

    return (strcmp(dot + 1, "txt") == 0);
}

static int file_exists(const char *filename)
{
    FILE *file = fopen(filename, "r");
    if (file == NULL)
    {
        return 0;
    }

    fclose(file);
    return 1;
}

static int is_duplicate_file(file_node *head, const char *filename)
{
    while (head != NULL)
    {
        if (strcmp(head->file, filename) == 0)
        {
            return 1;
        }
        head = head->link;
    }

    return 0;
}

static void insert_file_at_last(file_node **head, const char *filename)
{
    file_node *new_node = (file_node *)malloc(sizeof(file_node));
    //if (new_node == NULL)
    //{
      //  return;
    //}
        if (new_node == NULL)
        {
            printf(COLOR_RED "Memory allocation failed\n" COLOR_RESET);
         return;
        }

    //strcpy(new_node->file, filename);
    //new_node->link = NULL;
    strncpy(new_node->file, filename, SIZE - 1);
    new_node->file[SIZE - 1] = '\0';

    new_node->link = NULL;

    if (*head == NULL)
    {
        *head = new_node;
        return;
    }

    file_node *current = *head;
    while (current->link != NULL)
    {
        current = current->link;
    }

    current->link = new_node;
}

Status validate_and_insert_files(int argc, char *argv[], file_node **file_head)
{
    if (file_head == NULL)
    {
        return FAILURE;
    }

    int inserted = 0;

    if (argc >= 2)
    {
        for (int i = 1; i < argc; i++)
        {
            if (argv[i] == NULL || argv[i][0] == '\0')
                continue;

            if (!has_valid_txt_extension(argv[i]))
                continue;

            if (!file_exists(argv[i]))
                continue;

            if (is_duplicate_file(*file_head, argv[i]))
                continue;

            insert_file_at_last(file_head, argv[i]);
            inserted++;
        }
    }
    else
    {
        /* No command-line files provided: scan current directory for .txt files */
        DIR *d = opendir(".");
        if (d != NULL)
        {
            struct dirent *entry;
            while ((entry = readdir(d)) != NULL)
            {
                const char *name = entry->d_name;
                if (!has_valid_txt_extension(name))
                    continue;

                if (!file_exists(name))
                    continue;

                if (is_duplicate_file(*file_head, name))
                    continue;

                insert_file_at_last(file_head, name);
                inserted++;
            }
            closedir(d);
        }
    }

    return (inserted > 0) ? SUCCESS : FAILURE;
}
