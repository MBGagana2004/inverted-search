#include "inverted_search.h"

/* display.c - functions to display the inverted index database */

Status display_database(hash_t hash_table[])
{
    if (hash_table == NULL)
        return FAILURE;

    printf("\n--------------- Database ---------------\n");
    printf("%-5s %-16s %-10s %-20s %-10s\n", "Index", "Word", "FileCnt", "Filename", "WordCnt");
    printf("%-5s %-16s %-10s %-20s %-10s\n", "-----", "----", "-------", "--------", "--------");

    for (int i = 0; i < HASH_SIZE; i++)   // HASH_SIZE = 28
    {
        if (hash_table[i].h_link == NULL)
        {
            continue;
        }

        main_node *mtemp = hash_table[i].h_link;

        while (mtemp != NULL)
        {
            sub_node *stemp = mtemp->sub_link;

            while (stemp != NULL)
            {
                printf("%-5d %-16s %-10d %-20s %-10d\n",
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

    return SUCCESS;
}