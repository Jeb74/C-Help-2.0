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

/* ###################################################################################
 *                                 MAIN FUNCTIONS
 * ###################################################################################
 */

/**
 * @param string In this section you have to pass the address of a pointer with no memory allocated that points to NULL.
 * @param DIM This is the maximum dimension of input that this function is going to store.
 * @param fixedDim Here you can specify if the dimension is fixed or the input may be smaller.
 * @param conditions This is the parameter where you have to specify the conditions of an acceptable input.
 * @param message This is the message that will be shown to the user when he has to give the input.
 *
 * @return This function has no return but it modify the pointer that you passed in the first parameter slot and stores\n
 * in it user's input.
 */
void input(char **string, int DIM, _Bool FixedDim, const char conditions[], const char message[]);

_Bool sString (char **getIns, const char *cmds);

void freeIt(char **var);

/* ###################################################################################
 *                                 MAIN FUNCTIONS </>
 * ###################################################################################
 */

/* ###################################################################################
 *                                 INTEGERS HANDLERS
 * ###################################################################################
 */

int numConverter(char num);
int toInteger(const char nums[] , _Bool skipchars);
char *numToString(int num);
void bubbleSortInt(int *array, int dimension);

/* ###################################################################################
 *                                 INTEGERS HANDLERS </>
 * ###################################################################################
 */

/* ###################################################################################
 *                                 STRINGS HANDLERS
 * ###################################################################################
 */

_Bool strComp(const char string[], const char string_[]);
_Bool strCompNCS(const char string[], const char string_[]);
char *charToString(char _char);
match *contains(char *_chars, char *_str, _Bool caseSensitive);
int **checkMatches(match **matches);
char **strcut(char *str, char _char);
_Bool sizedCmpNC(const char *str, const char *str_, int init, int final);

/* ###################################################################################
 *                                 STRINGS HANDLERS </>
 * ###################################################################################
 */

/* ###################################################################################
 *                                 UTILITIES
 * ###################################################################################
 */

int detect(int line, const char func[]);
void clearBuffer();

/* ###################################################################################
 *                                 UTILITIES
 * ###################################################################################
 */

#endif //C_HELP_2_0_STRINGHANDLER_H
