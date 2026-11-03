/* create.c - create inverted index database (create_database) */
#include "inverted_search.h"

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

static sub_node *create_sub_node(const char *filename)
{
    sub_node *node = (sub_node *)malloc(sizeof(sub_node));
    if (node == NULL)
        return NULL;

    strncpy(node->filename, filename, SIZE - 1);
    node->filename[SIZE - 1] = '\0';
    node->word_count = 1;
    node->sub_link = NULL;
    return node;
}

static sub_node *find_subnode(sub_node *head, const char *filename)
{
    while (head != NULL)
    {
        if (strcmp(head->filename, filename) == 0)
            return head;
        head = head->sub_link;
    }
    return NULL;
}

static int add_or_increment_subnode(main_node *main, const char *filename)
{
    sub_node *existing = find_subnode(main->sub_link, filename);
    if (existing != NULL)
    {
        existing->word_count++;
        return SUCCESS;
    }

    sub_node *new_sub = create_sub_node(filename);
    if (new_sub == NULL)
        return FAILURE;

    if (main->sub_link == NULL)
    {
        main->sub_link = new_sub;
    }
    else
    {
        sub_node *tail = main->sub_link;
        while (tail->sub_link != NULL)
            tail = tail->sub_link;
        tail->sub_link = new_sub;
    }

    main->file_count += 1;
    return SUCCESS;
}

static int file_present_in_hashtable(hash_t hash_table[], const char *filename)
{
    for (int i = 0; i < HASH_SIZE; i++)
    {
        main_node *m = hash_table[i].h_link;
        while (m != NULL)
        {
            sub_node *s = m->sub_link;
            while (s != NULL)
            {
                if (strcmp(s->filename, filename) == 0)
                    return 1;
                s = s->sub_link;
            }
            m = m->main_link;
        }
    }
    return 0;
}

Status create_database(file_node *file_head, hash_t hash_table[])
{
    if (file_head == NULL || hash_table == NULL)
        return FAILURE;

    /* If hash table already has data, treat as already created */
    for (int i = 0; i < HASH_SIZE; i++)
    {
        if (hash_table[i].h_link != NULL)
        {
            printf(COLOR_YELLOW "already created\n" COLOR_RESET);
            return SUCCESS;
        }
    }

    for (file_node *file_node_ptr = file_head; file_node_ptr != NULL; file_node_ptr = file_node_ptr->link)
    {
        if (file_node_ptr->file[0] == '\0')
            continue;

        /* Skip duplicate file entries in the provided file list */
        int duplicate = 0;
        for (file_node *scan = file_head; scan != file_node_ptr; scan = scan->link)
        {
            if (strcmp(scan->file, file_node_ptr->file) == 0)
            {
                duplicate = 1;
                break;
            }
        }
        if (duplicate)
            continue;

        /* If this file already exists in the hashtable (from previous run), skip it */
        if (file_present_in_hashtable(hash_table, file_node_ptr->file))
            continue;

        FILE *file = fopen(file_node_ptr->file, "r");
        if (file == NULL)
            continue;

        char raw_word[SIZE];
        while (fscanf(file, "%99s", raw_word) == 1)
        {
            normalize_word(raw_word);
            if (raw_word[0] == '\0')
                continue;

            int index = get_hash_index(raw_word);
            main_node *prev_main = NULL;
            main_node *current_main = hash_table[index].h_link;

            while (current_main != NULL && strcmp(current_main->word, raw_word) < 0)
            {
                prev_main = current_main;
                current_main = current_main->main_link;
            }

            if (current_main != NULL && strcmp(current_main->word, raw_word) == 0)
            {
                if (add_or_increment_subnode(current_main, file_node_ptr->file) == FAILURE)
                {
                    fclose(file);
                    return FAILURE;
                }
            }
            else
            {
                main_node *new_main = create_main_node(raw_word);
                if (new_main == NULL)
                {
                    fclose(file);
                    return FAILURE;
                }

                sub_node *new_sub = create_sub_node(file_node_ptr->file);
                if (new_sub == NULL)
                {
                    free(new_main);
                    fclose(file);
                    return FAILURE;
                }

                new_main->sub_link = new_sub;
                new_main->file_count = 1;
                new_main->main_link = current_main;

                if (prev_main == NULL)
                {
                    hash_table[index].h_link = new_main;
                }
                else
                {
                    prev_main->main_link = new_main;
                }
            }
        }

        fclose(file);
    }

    printf(COLOR_GREEN "data base created\n" COLOR_RESET);
    return SUCCESS;
}
