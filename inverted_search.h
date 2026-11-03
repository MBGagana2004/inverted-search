#ifndef INVERTED_SEARCH_H
#define INVERTED_SEARCH_H
/* inverted_search.h - shared types and function declarations for inverted search */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>   // if using tolower(), isalpha(), etc.

#define SIZE 100
#define HASH_SIZE 28      // 26 alphabets + 1+1 for special characters and digits

/* ANSI terminal color codes */
#define COLOR_RED "\x1b[31m"
#define COLOR_GREEN "\x1b[32m"
#define COLOR_YELLOW "\x1b[33m"
#define COLOR_BLUE "\x1b[34m"
#define COLOR_MAGENTA "\x1b[35m"
#define COLOR_CYAN "\x1b[36m"
#define COLOR_RESET "\x1b[0m"

typedef enum
{
    SUCCESS,
    FAILURE
} Status;
/* file list */
typedef struct filenode{

 char file[SIZE];
 struct filenode  *link;
}file_node;
/*main node*/
typedef struct mainnode{

 char word[SIZE];
 int file_count;
 struct subnode *sub_link;
 struct mainnode *main_link;
}main_node;
/*hash node*/
typedef struct hash_table{
    int index;
    struct mainnode *h_link;
}hash_t;

/*sub node*/
typedef struct subnode{
    char filename[SIZE];
    int word_count;
    struct subnode *sub_link;
}sub_node;

#endif




/* ================= FUNCTION DECLARATIONS ================= */

/* Validate command-line files and insert valid .txt files into list */
Status validate_and_insert_files(int argc, char *argv[], file_node **file_head);

/* Create inverted index database */
Status create_database(file_node *file_head, hash_t hash_table[]);

/* Display database */
Status display_database(hash_t hash_table[]);

/* Save database to file */
Status save_database(hash_t hash_table[], char *filename);

/* Update database from file */
Status update_database(hash_t hash_table[], char *filename);

/* Search a word in database */
Status search_database(hash_t hash_table[], char *word);