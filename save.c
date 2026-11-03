#include "inverted_search.h"

/* save.c - save inverted index database to a backup file */

static int has_valid_txt_extension(const char *filename)
{
    const char *dot = strrchr(filename, '.');
    return (dot != NULL && strcmp(dot + 1, "txt") == 0);
}

Status save_database(hash_t hash_table[], char *filename)
{
    if (hash_table == NULL || filename == NULL || filename[0] == '\0')
        return FAILURE;

    if (!has_valid_txt_extension(filename))
        return FAILURE;

    FILE *file = fopen(filename, "w");
    if (file == NULL)
        return FAILURE;

    for (int i = 0; i < HASH_SIZE; i++)
    {
        if (hash_table[i].h_link == NULL)
            continue;

        main_node *mtemp = hash_table[i].h_link;
        while (mtemp != NULL)
        {
            sub_node *stemp = mtemp->sub_link;
            while (stemp != NULL)
            {
                fprintf(file, "# %d;%s;%d;%s;%d;#\n",
                        i,
                        mtemp->word,
                        mtemp->file_count,
                        stemp->filename,
                        stemp->word_count);
                stemp = stemp->sub_link;
            }
            mtemp = mtemp->main_link;
        }
    }

    fclose(file);
    return SUCCESS;
}
