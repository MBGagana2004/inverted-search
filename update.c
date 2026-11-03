#include "inverted_search.h"
/* update.c - update inverted index database from backup file */

static int has_valid_txt_extension(const char *filename)
{
    const char *dot = strrchr(filename, '.');
    return (dot != NULL && strcmp(dot + 1, "txt") == 0);
}

static int file_exists(const char *filename)
{
    FILE *file = fopen(filename, "r");
    if (file == NULL)
        return 0;
    fclose(file);
    return 1;
}

static main_node *create_main_node(const char *word)
{
    main_node *node = (main_node *)malloc(sizeof(main_node));
    if (node == NULL)
        return NULL;

    strncpy(node->word, word, SIZE - 1);
    node->word[SIZE - 1] = '\0';
    node->file_count = 0;
    node->sub_link = NULL;
    node->main_link = NULL;
    return node;
}

static sub_node *create_sub_node(const char *filename, int word_count)
{
    sub_node *node = (sub_node *)malloc(sizeof(sub_node));
    if (node == NULL)
        return NULL;

    strncpy(node->filename, filename, SIZE - 1);
    node->filename[SIZE - 1] = '\0';
    node->word_count = word_count;
    node->sub_link = NULL;
    return node;
}

static main_node *find_main_node(main_node *head, const char *word)
{
    while (head != NULL)
    {
        if (strcmp(head->word, word) == 0)
            return head;
        head = head->main_link;
    }
    return NULL;
}

static sub_node *find_sub_node(sub_node *head, const char *filename)
{
    while (head != NULL)
    {
        if (strcmp(head->filename, filename) == 0)
            return head;
        head = head->sub_link;
    }
    return NULL;
}

static int validate_data(int index, const char *word, int file_count, const char *filename, int word_count)
{
    if (index < 0 || index >= HASH_SIZE)
        return 0;
    if (word == NULL || word[0] == '\0')
        return 0;
    if (file_count <= 0)
        return 0;
    if (filename == NULL || filename[0] == '\0')
        return 0;
    if (word_count <= 0)
        return 0;
    return 1;
}

Status update_database(hash_t hash_table[], char *filename)
{
    if (filename == NULL || filename[0] == '\0')
    {
        printf(COLOR_RED "Error: Invalid filename.\n" COLOR_RESET);
        return FAILURE;
    }

    if (hash_table == NULL)
    {
        printf(COLOR_RED "Error: Hash table is NULL.\n" COLOR_RESET);
        return FAILURE;
    }

    if (!has_valid_txt_extension(filename))
    {
        printf(COLOR_RED "Error: File must have .txt extension.\n" COLOR_RESET);
        return FAILURE;
    }

    if (!file_exists(filename))
    {
        printf(COLOR_RED "Error: File does not exist.\n" COLOR_RESET);
        return FAILURE;
    }

    FILE *file = fopen(filename, "r");
    if (file == NULL)
    {
        printf(COLOR_RED "Error: Cannot open file in read mode.\n" COLOR_RESET);
        return FAILURE;
    }

    int index, file_count, word_count;
    char word[SIZE], backup_filename[SIZE];
    Status result = SUCCESS;

    /* Expect backup lines in format: # index;word;filecount;filename;wordcount;# */
    while (fscanf(file, "# %d;%99[^;];%d;%99[^;];%d;#", &index, word, &file_count, backup_filename, &word_count) == 5)
    {
        if (!validate_data(index, word, file_count, backup_filename, word_count))
        {
            printf(COLOR_RED "Error: Invalid data format in backup file.\n" COLOR_RESET);
            continue;
        }

        if (hash_table[index].h_link == NULL)
        {
            main_node *new_main = create_main_node(word);
            if (new_main == NULL)
            {
                printf(COLOR_RED "Error: Memory allocation failed for main node.\n" COLOR_RESET);
                result = FAILURE;
                break;
            }

            sub_node *new_sub = create_sub_node(backup_filename, word_count);
            if (new_sub == NULL)
            {
                printf(COLOR_RED "Error: Memory allocation failed for sub node.\n" COLOR_RESET);
                free(new_main);
                result = FAILURE;
                break;
            }

            new_main->sub_link = new_sub;
            new_main->file_count = 1;
            hash_table[index].h_link = new_main;
            printf(COLOR_GREEN "updated\n" COLOR_RESET);
        }
        else
        {
            main_node *found_main = find_main_node(hash_table[index].h_link, word);

            if (found_main != NULL)
            {
                sub_node *found_sub = find_sub_node(found_main->sub_link, backup_filename);

                if (found_sub != NULL)
                {
                    if (found_sub->word_count != word_count)
                    {
                        found_sub->word_count = word_count;
                        printf(COLOR_GREEN "updated\n" COLOR_RESET);
                    }
                    else
                    {
                        printf(COLOR_YELLOW "already updated\n" COLOR_RESET);
                    }
                }
                else
                {
                    sub_node *new_sub = create_sub_node(backup_filename, word_count);
                    if (new_sub == NULL)
                    {
                        printf(COLOR_RED "Error: Memory allocation failed for sub node.\n" COLOR_RESET);
                        result = FAILURE;
                        break;
                    }

                    sub_node *tail = found_main->sub_link;
                    while (tail->sub_link != NULL)
                        tail = tail->sub_link;
                    tail->sub_link = new_sub;
                    found_main->file_count += 1;
                    printf(COLOR_GREEN "updated\n" COLOR_RESET);
                }
            }
            else
            {
                main_node *new_main = create_main_node(word);
                if (new_main == NULL)
                {
                    printf(COLOR_RED "Error: Memory allocation failed for main node.\n" COLOR_RESET);
                    result = FAILURE;
                    break;
                }

                sub_node *new_sub = create_sub_node(backup_filename, word_count);
                if (new_sub == NULL)
                {
                    printf(COLOR_RED "Error: Memory allocation failed for sub node.\n" COLOR_RESET);
                    free(new_main);
                    result = FAILURE;
                    break;
                }

                new_main->sub_link = new_sub;
                new_main->file_count = 1;

                main_node *tail = hash_table[index].h_link;
                while (tail->main_link != NULL)
                    tail = tail->main_link;
                tail->main_link = new_main;
                printf(COLOR_GREEN "updated\n" COLOR_RESET);
            }
        }
    }

    fclose(file);

    if (result == SUCCESS)
        printf(COLOR_GREEN "Database updated successfully from backup file.\n" COLOR_RESET);
    else
        printf(COLOR_RED "Database update completed with errors.\n" COLOR_RESET);

    return result;
}
