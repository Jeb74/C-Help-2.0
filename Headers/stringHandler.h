#ifndef C_HELP_2_0_STRINGHANDLER_H
#define C_HELP_2_0_STRINGHANDLER_H

#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>

#define SAFEDIM 2
#define GARBAGE -1

#if !defined CHELP_MAIN
//#error "You cannot include this library directly."
#endif

#define xtdynmem(type,size) (type*)calloc(size,sizeof(type))

/* ###################################################################################
 *                                  TYPE DEFINITIONS
 * ###################################################################################
 */

typedef struct {
    bool negative;
    bool positive;
    enum {
        MATCHED,
        NOT_MATCHED
    } typeVal;
} ValidationRegister;

typedef enum {
    TOP,
    MIDDLE,
    BOTTOM
} ListCoords;

typedef union {
    unsigned int _n;
    unsigned char _c;
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

typedef struct {
    int pos[SAFEDIM];
    cmd_s cmds;
    char *args;
} commands;

typedef struct commands* Commands;

/* ###################################################################################
 *                                 TYPE DEFINITIONS </>
 * ###################################################################################
 */

int numConverter(char num);
int xtdNumConverter(const char nums[] , _Bool skipchars);
_Bool strComp(const char string[], const char string_[]);
int detect(int line, const char func[]);
void clearBuffer();

/**
 * @param getIns In this section you have to pass the address of a pointer with no memory allocated that points to NULL.
 * @param DIM This is the maximum dimension of the input that you are going to receive from the user.
 * @param fixedDim Here you can specify if the dimension is fixed or the input can have a lower dimension.
 * @param conditions This is the parameter where you have to specify the conditions of an acceptable input.
 * @param message This is the message that would be shown to the user when he has to give the input.
 *
 * @return This function has no return but it modify the pointer that you passed in the first parameter slot and gives you\n
 * the input of the user.
 */
void input(char **getins, int DIM, _Bool fixedDim, const char conditions[], const char message[]);
_Bool sString (char **getIns, const char *cmds);
_Bool ccLess_Comparison(const char string[], const char string_[]);
void freeIt(char **var);
match *contains(char *_chars, char *_str, _Bool caseSensitive);
int **checkMatches(match **matches);
char *numToString(int num);
char *charToString(char _char);
void bubbleSortInt(int **array, int dimension);
char **strcut(char *str, char _char);

#endif //C_HELP_2_0_STRINGHANDLER_H
