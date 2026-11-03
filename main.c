/*Name: MB Gagana
date: 6/7/2026
batch: int26001B_005
description:
Inverted Search Project (DSA)
Developed an Inverted Search application in C that creates and manages a searchable database from multiple text files.
The project includes functions to create the database, display the database, search for keywords,
save the database to a file,update the database with new data, and exit the application. It uses data structures,
file handling, linked lists,and hashing to provide efficient data storage and fast searching*/

/* main.c - program entry and menu handling for inverted search */
#include "inverted_search.h"

/* main function */
int main(int argc, char *argv[])
{
    int choice;
    char word[SIZE];
    char filename[SIZE];

    file_node *file_head = NULL;

    /* hash table initialization */
    hash_t hash_table[HASH_SIZE];

    for (int i = 0; i < HASH_SIZE; i++)
    {
        hash_table[i].index = i;
        hash_table[i].h_link = NULL;
    }

    while (1)
    {
        printf(COLOR_CYAN "\n========== INVERTED SEARCH MENU ==========" COLOR_RESET "\n");
        printf("1. Create Data Base\n");
        printf("2. Display Data Base\n");
        printf("3. Search Data Base\n");
        printf("4. Save Data Base\n");
        printf("5. Update Data Base\n");
        printf("6. Exit\n");
        printf("Enter your choice: ");

        scanf("%d", &choice);

        switch (choice)
        {
            case 1:
                /* validate files first */
                if (validate_and_insert_files(argc, argv, &file_head) == SUCCESS)
                {
                    if (create_database(file_head, hash_table) == SUCCESS)
                        printf(COLOR_GREEN "data base created successfully\n" COLOR_RESET);
                    else
                        printf(COLOR_RED "Database creation failed\n" COLOR_RESET);
                }
                else
                {
                    printf(COLOR_RED "File validation failed\n" COLOR_RESET);
                }
                break;

            case 2:
                display_database(hash_table);
                break;

            case 3:
                printf("Enter word to search: ");
                scanf("%s", word);

                search_database(hash_table, word);
                break;

            case 4:
                printf("Enter filename to save database: ");
                scanf("%s", filename);

                save_database(hash_table, filename);
                break;

            case 5:
                printf("Enter filename to update database: ");
                scanf("%s", filename);

                update_database(hash_table, filename);
                break;

            case 6:
                printf(COLOR_CYAN "Exiting program...\n" COLOR_RESET);
                exit(0);

            default:
                printf(COLOR_RED "Invalid choice. Try again.\n" COLOR_RESET);
        }
    }

    return 0;
}