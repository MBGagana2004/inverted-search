#include "inverted_search.h"

/* search.c - search for a word in the inverted index database */

static int get_hash_index(const char *word)
{
    if (word == NULL || word[0] == '\0')
        return HASH_SIZE - 2;

    char c = tolower((unsigned char)word[0]);
    if (c >= 'a' && c <= 'z')
        return c - 'a';

    return HASH_SIZE - 2;
}

static void normalize_word(char *word)
{
    int write = 0;
    for (int read = 0; word[read] != '\0'; read++)
    {
        char c = word[read];
        if (isalnum((unsigned char)c))
        {
            word[write++] = (char)tolower((unsigned char)c);
        }
    }
    word[write] = '\0';
}

Status search_database(hash_t hash_table[], char *word)
{
    if (word == NULL || word[0] == '\0' || hash_table == NULL)
        return FAILURE;

    char normalized[SIZE];
    strncpy(normalized, word, SIZE - 1);
    normalized[SIZE - 1] = '\0';
    normalize_word(normalized);

    if (normalized[0] == '\0')
        return FAILURE;

    int index = get_hash_index(normalized);
    main_node *current = hash_table[index].h_link;

    if (current == NULL)
    {
        printf("Word '%s' not found in database.\n", word);
        return FAILURE;
    }

    while (current != NULL)
    {
        int cmp = strcmp(current->word, normalized);
        if (cmp == 0)
        {
            printf("Word: %s\n", current->word);
            printf("File count: %d\n", current->file_count);

            sub_node *sub_current = current->sub_link;
            while (sub_current != NULL)
            {
                printf("Filename: %s, Word count: %d\n",
                       sub_current->filename,
                       sub_current->word_count);
                sub_current = sub_current->sub_link;
            }

            return SUCCESS;
        }

        if (cmp > 0)
            break;

        current = current->main_link;
    }

    printf("Word '%s' not found in database.\n", word);
    return FAILURE;
}
