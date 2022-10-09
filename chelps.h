#ifndef CHELPS_LIBRARY_H
#define CHELPS_LIBRARY_H

//TODO: Nuova sString.
/*
 * Command syntax:
 * rmv['Char'/'CharList'>'Position']                                                    -> remove
 * add['Char'/'CharList'>'Position']                                                    -> add
 * Position List: init/middle/final/all/'number'
 *      Position can be resized: init[3]        -> from 0 to 3
 *                               middle[3#7]    -> from 3 to 7
 *                               final[-3]      -> from (end - 3) to end
 * rep['Char'/'CharList'>'with'>'Position']                                             -> replace
 * cmp['Char'/'CharList'] -> cmp['Char'/'CharList':GO] -> cmp['Char'/'CharList':GOL]    -> compare
 *      GO -> Give Occurrences
 *      GOL -> Give Occurrences List
 * cnv[mn/MX>'Position']                                                                -> convert (a position from 'a' to 'A' or viceversa)
 *
 * to define a char or a string it must be wrapped by '':
 *      rmv['This string'>0]                    -> Removes the entire string "This string" starting from the position 0.
 */

#define MINDIM 2
#define MAXWORD_LIST 20
#define MAXCOMMAND_DIM 15

#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>

typedef union {
    int _n;
    char _c;
} data;
typedef enum {
    char_t = 0,
    int_t = 1,
    list_t = 2,
    n_char_t = 3,
    n_int_t = 4,
    n_list_t = 5,
    unlimited = 6
} type;

typedef struct conditions_list {
    struct conditions_list *prev;
    data data;
    type type;
    struct conditions_list *next;
} cl;

typedef struct {
    _Bool success;
    int index;
    int *indexes;
} match;

typedef enum {
    REMOVE,
    ADD,
    REPLACE,
    COMPARE,
    CONVERT
} cmd_s;

struct {
    int pos[MINDIM];
    cmd_s cmds;
    char *args;
} commands;

typedef struct commands* Commands;

#define ASCII_END '\0'

#define Z_CHAR 'z'
#define A_CHAR 'a'
#define CA_CHAR 'A'
#define CZ_CHAR 'Z'
#define SPACE_CHAR ' '

#define SS_SEPARATOR ';'

#ifndef SS_COMMANDS
#define SS_COMMANDS
#define REMOVE_INITIAL_SPACES "rispaces"
#define REMOVE_CENTRAL_SPACES "rcspaces"
#define REMOVE_ALL_SPACES "raspaces"
#define INITIAL_CHAR_AS_CAPS "capsinits"
#define INITIAL_CHARS_OF_SUBSTRINGS_AS_CAPS "capsinitw"
#define ALL_CHARS_AS_CAPS "allcaps"
#define NO_CAPS "nocaps"
#define CLASSICAL_NAMES_FORMAT "classicalnames"
#define RESET_COMMAND "NULL"
#endif

int numchecker(char num);
int xtdnumchecker(const char nums[] , _Bool skipchars);
_Bool word_comparison(const char string[], const char string_[]);
int detect(int line, const char func[]);

/**
 * @param getIns In this section you have to pass the address of a pointer with no memory allocated that points to NULL.
 * @param DIM This is the maximum dimension of the input that you are going to receive from the user.
 * @param fixedDim Here you can specify if the dimension is fixed or the input can have a lower dimension.
 * @param conditions This is the parameter where you have to specify the conditions of an acceptable input.
 * @param message This is the message that would be shown to the user when he has to give the input.
 * @param caller This is a special section dedicated to the macro caller_ or caller_t
 *
 * @return This function has no return but it modify the pointer that you passed in the first parameter slot and gives you\n
 * the input of the user.
 */
void cString(char **getins, int DIM, _Bool fixedDim, const char conditions[], const char message[], int caller);

/**
 * @return This function returns a Bool value based on Defined Correspondence or default true if no given matches.
 * @return This function can modify a string with the given commands listed above.
 */
_Bool sString (char **getIns, const char *cmds);
_Bool ccLess_Comparison(const char string[], const char string_[]);
void freeIt(char **var);
match *contains(char *_chars, char *_str, _Bool caseSensitive);
int **checkMatches(match **matches);
char *numToString(int num);
char *charToString(const char _char);
void bubbleSortInt(int **array, int dimension);
char **strcut(char *str, char _char);

#define xtdynmem(type,size) (type*)calloc(size,sizeof(type))
#define caller_ detect(__LINE__,__func__)

#endif //CHELPS_LIBRARY_H
