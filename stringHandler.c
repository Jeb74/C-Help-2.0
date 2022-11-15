#include "Headers/stringHandler.h"

/* ###################################################################################
 * CHAR CONVERTERS TO NUMBERS FUNCTIONS
 * ###################################################################################
 */

static _Bool checkIfSpecial(const char _char) {
    return (_char == '!'
            || _char == '.'
            || _char == '('
            || _char == ')'
            || _char == '|'
            || _char == '-'
            || _char == '>'
            || _char == '^'
    );
}

static _Bool checkIfNumber(const char _char) {
    return (_char >= '0' && _char <= '9');
}

int numConverter(const char num) {
    if (num >= '0' && num <= '9') {
        return (int)num-48;
    }
    else return 0;
}

int toInteger(const char nums[] , _Bool skipchars) {
    int cnum = 0;
    if (!skipchars) {
        for (int i = 0; checkIfNumber(nums[i]); i++) {
            if (cnum == 0) cnum = numConverter(nums[i]);
            else cnum = cnum * 10 + numConverter(nums[i]);
        }
    }
    else {
        for (int j = 0; j < (int)strlen(nums); j++) {
            for (int i = j; checkIfNumber(nums[i]); i++) {
                if (cnum == 0) cnum = numConverter(nums[i]);
                else cnum = cnum * 10 + numConverter(nums[i]);
                j = i;
            }
        }
    }
    return cnum;
}

int detect(const int line, const char func[]) {
    static int info_line = -1;
    static char info_func[] = "Not_A_Function_Just_Initialization";
    static int call_id = 0;
    if (info_line != line || strcmp(info_func, func) != 0) {
        info_line = line;
        strcpy(info_func, func);
        call_id = rand();
    }
    return call_id;
}

static cl *getListTop(cl *list) {
    cl *top = list;
    while ((*top).prev != NULL) {
        top = top->prev;
    }
    return top;
}

static cl *getPositionInList(cl *list, cl *element, ListCoords *tbm) {
    switch(element->type) {
        case n_char_t:
        case n_int_t:
        case n_list_t: {
            if (list->type < n_char_t || list->type > n_list_t) {
                *tbm = TOP;
                return list;
            }
            else {
                while(list->next != NULL && list->next->type >= n_char_t && list->next->type <= n_list_t) {
                    list = list->next;
                }
                if (list->next == NULL) *tbm = BOTTOM;
                else *tbm = MIDDLE;
                return list;
            }
        }
            break;
        default: {
            while(list->next != NULL) list = list->next;
            *tbm = BOTTOM;
            return list;
        }
    }
}

static cl *sortCList(cl *list, cl *element) {
    if (list == NULL) return element;
    ListCoords tbm;
    cl *tmp = getPositionInList(list, element, &tbm);
    switch(tbm) {
        case TOP: {
            tmp->prev = element;
            element->next = tmp;
        }
            break;
        case MIDDLE: {
            element->next = tmp->next;
            tmp->next = element;
            element->prev = tmp;
            element->next->prev = element;
        }
            break;
        case BOTTOM: {
            tmp->next = element;
            element->prev = tmp;
        }
    }
    return getListTop(list);
}

static cl *setElement(cl *list, data data, type type) {
    cl *tmp = (cl*) calloc(1, sizeof(cl));
    (*tmp).prev = NULL;
    (*tmp).data = data;
    (*tmp).type = type;
    (*tmp).next = NULL;
    return sortCList(list, tmp);
}

static cl *getElement(cl *list, _Bool reset) {
    static cl *ptr = NULL;
    if (ptr == NULL) ptr = list;
    else ptr = (*ptr).next;
    if (reset) ptr = NULL;
    return ptr;
}

static void *freeList(cl *list) {
    if (!list) return NULL;
    while((*list).next != NULL) {
        list = (*list).next;
    }
    cl *tmp = list;
    if ((*list).prev != NULL){
        list = (*list).prev;
        (*list).next = NULL;
    }
    else {
        free(tmp);
        return NULL;
    }
    free(tmp);
    freeList(list);
}

/*
 * !(A|Z) negated char range
 * (c|c) char range
 * (..^!?^^zxy) char list
 * !(..^!?^zxy) negated char list
 * (0->) unlimited int
 * (^1|z) negated conversion symbol
 * (-1) incoming
 */

static void regexChars(cl **list, const char conditions[], int *pos, _Bool negated) {
    data data;
    data._c = '\0';
    type type;
    for (int i = *pos; conditions[i] != ')'; i++) {
        if (conditions[i] == '|') i++;
        if (conditions[i] == '^') i++;
        type = char_t;
        if (negated) type = n_char_t;
        data._c = conditions[i];
        *list = setElement(*list, data, type);
        *pos = i;
    }
}

static void regexIntegers(cl **list, const char conditions[], int *pos, _Bool negated) {
    data data;
    data._n = 0;
    type type;
    for (int i = *pos; conditions[i] != ')'; i++) {
        for (int j = i; checkIfNumber(conditions[j]); j++){
            if (data._n == 0) {
                data._n = numConverter(conditions[j]);
            } else {
                data._n = data._n * 10 + numConverter(conditions[j]);
            }
            type = int_t;
            if (negated) type = n_int_t;
            i = j;
        }
        if (conditions[i+1] == '-' && conditions[i+2] == '>') {
            type = unlimited;
            i += 2;
        }
        if (conditions[i] != '|') {
            *list = setElement(*list, data, type);
            data._n = 0;
            *pos = i + 1;
        }
    }
}

static bool *regexCompiler(cl **list, const char conditions[], bool condition_types[SAFEDIM]) {
    _Bool negated = false;
    data data;
    type type;
    for (int i = 0; i <= strlen(conditions)-1;) {
        if (conditions[i] == ')') {
            if (i == strlen(conditions)-1) break;
            else i++;
        }
        if (conditions[i] == '!') {
            negated = true;
            i++;
        }
        i++;
        if (conditions[i] == '.') {
            condition_types[1] = true;
            for (int j = i+2, ignoreSymbol = 0; true; j++) {
                if (conditions[j] == ')' && !ignoreSymbol) {
                    i = j;
                    break;
                }
                if (conditions[j] == '^') {
                    j++;
                }
                type = list_t;
                if (negated) type = n_list_t;
                data._c = conditions[j];
                *list = setElement(*list, data, type);
            }
        }
        else if (checkIfNumber(conditions[i])) {
            condition_types[0] = true;
            regexIntegers(&(*list), conditions, &i, negated);
        }
        else {
            condition_types[1] = true;
            regexChars(&(*list), conditions, &i, negated);
        }
        if (negated) negated = false;
    }
    *list = getListTop(*list);
}

/*
 * ERROR CODES:
 * - 89: less parenthesis than expected
 * - -89: more parenthesis than expected
 * - 46: expected more '.' to define a list
 * - 124: No range extremities separator
 * - -33: Wrong negation symbol position
 * - 33: Illegal argument
 * "(A|Z)(..!"£$%&)!(..^!"£$%&)!(A|Z)"
 */

static void evaluateSyntax(const char conditions[]) {
    for (int i = 0, parenthesis = 0, verticalSlash = 0, ignoreSymbol = 0; i < strlen(conditions); i++) {
        if (parenthesis == 0 && conditions[i] != '!' && conditions[i] != '(') exit(33);
        else if (parenthesis == 0 && conditions[i] == '(') {
            parenthesis++;
            i++;
        }
        if (parenthesis == 1 && conditions[i] == '(' && conditions[i-1] != '^' && i != '0') exit(-89);
        if (parenthesis == 1 && conditions[i] == ')') {
            if (conditions[i-1] == '^'
                && (conditions[i+1] == '!' || conditions[i+1] == '(' || i+1 == strlen(conditions))
                && ignoreSymbol) exit(89);
            else  parenthesis--;
        }
        if (parenthesis) {
            if (conditions[i-1] == '(' && conditions[i] == '.' && conditions[i+1] == '.') {
                i += 2;
                for (int j = i; true; j++) {
                    if (conditions[j] == ')' && (conditions[j-1] != '^' || !ignoreSymbol)) break; // break
                    if (conditions[j] == '^') {
                        ignoreSymbol++;
                        if (!checkIfSpecial(conditions[j+1]) && !checkIfNumber(conditions[j+1])) exit(33);
                        else {
                            j++;
                            ignoreSymbol--;
                        }
                    }
                    else if (checkIfSpecial(conditions[j]) || checkIfNumber(conditions[j])) exit(33);
                    i = j;
                }
            }
            else if (conditions[i-1] == '(' && checkIfNumber(conditions[i])) {
                for (int j = i, onlyNumbers = false, unlimited = false; conditions[j] != ')' && unlimited != 1; j++) {
                    i = j;
                    if (!onlyNumbers && !unlimited) {
                        if (conditions[j] == '|' && checkIfNumber(conditions[j-1])) {
                            onlyNumbers = true;
                            j++;
                        }
                        if (conditions[j] == '-' && checkIfNumber(conditions[j-1]) && !onlyNumbers) {
                            if (conditions[j+1] == '>' && conditions[j+2] == ')') {
                                unlimited = true;
                                i = j+1;
                            }
                            else exit(33);
                        }
                        else if (!(checkIfNumber(conditions[j]))) exit(33);
                    }
                    if (onlyNumbers) {
                        if (!(checkIfNumber(conditions[j]))) {
                            exit(33);
                        }
                        else i = j;
                    }
                }
            }
            else {
                for (int j = i; true; j++) {
                    if (conditions[j] == '^') {
                        ignoreSymbol++;
                        if (checkIfSpecial(conditions[j + 1]) || checkIfNumber(conditions[j + 1])) {
                            ignoreSymbol--;
                            j++;
                        }
                        else exit(33);
                        if ((conditions[j+1] == '|' && conditions[j-2] == '(') || (conditions[j+1] == ')' && conditions[j-2] == '|')) j++;
                        else exit(33);
                    }
                    else if (checkIfSpecial(conditions[j]) || checkIfNumber(conditions[j])) exit(33);
                    else {
                        if ((conditions[j+1] == '|' && conditions[j-1] == '(') || (conditions[j+1] == ')' && conditions[j-1] == '|')) j++;
                    }
                    if (conditions[j] == ')' && (conditions[j-1] != '^' || !ignoreSymbol)) {
                        i = j-1;
                        break;
                    }
                    i = j;
                }
            }
        }
    }
}

static _Bool checkNumbers(cl *list) {
    cl *tmp = list;
    if (tmp->type == unlimited || tmp->type == int_t || tmp->type == n_int_t) return true;
    if (tmp->next == NULL) return false;
    return checkNumbers(tmp->next);
}

static unsigned int hashThis(const char string[]) {
    unsigned int hash = 0;
    for (unsigned int i = 0; i < strlen(string); i++) {
        if (!checkIfSpecial(string[i]) || i && string[i-1] == '^') hash += (string[i] * (i+1));
    }
    return hash;
}

static void charValidation(cl *list, char val, ValidationRegister *reg) {
    reg->negative = false;
    reg->positive = false;
    reg->typeVal = NOT_MATCHED;
    getElement(list, true);
    do {
        list = getElement(list, false);
        if (list->type < n_char_t || list->type > n_list_t) reg->negative = true;
        switch(list->type) {
            case n_char_t: {
                cl *tmp2 = getElement(list, false);
                if (val >= list->data._c && val <= tmp2->data._c) {
                    reg->typeVal = MATCHED;
                    return;
                }
                list = tmp2;
            }
                break;
            case n_list_t: {
                if (val == list->data._c) {
                    reg->typeVal = MATCHED;
                    return;
                }
            }
                break;
            case char_t: {
                cl *tmp2 = getElement(list, false);
                if (val >= list->data._c && val <= tmp2->data._c) {
                    reg->typeVal = MATCHED;
                    return;
                }
                list = tmp2;
            }
                break;
            case list_t: {
                if (val == list->data._c) {
                    reg->typeVal = MATCHED;
                    return;
                }
            }
        }
    } while(list->next != NULL);
    reg->positive = true;
}

static void intValidation(cl *list, int val, ValidationRegister *reg) {
    reg->negative = false;
    reg->positive = false;
    reg->typeVal = NOT_MATCHED;
    getElement(list, true);
    do {
        list = getElement(list, false);
        if (list->type < n_char_t || list->type > n_list_t) reg->negative = true;
        switch(list->type) {
            case n_int_t: {
                cl *tmp2 = getElement(list, false);
                if (val >= list->data._n && val <= tmp2->data._n) {
                    reg->typeVal = MATCHED;
                    return;
                }
                list = tmp2;
            }
                break;
            case int_t: {
                cl *tmp2 = getElement(list, false);
                if (val >= list->data._n && val <= tmp2->data._n) {
                    reg->typeVal = MATCHED;
                    return;
                }
                list = tmp2;
            }
                break;
            case unlimited: {
                if (val >= list->data._n) {
                    reg->typeVal = MATCHED;
                    return;
                }
            }
        }
    } while(list->next);
    reg->positive = true;
}

static void checkValidity(const int val, const type type, cl *list, ValidationRegister *reg) {
    switch (type) {
        case int_t: return intValidation(list, val, reg);
        case char_t: return charValidation(list, (char)val, reg);
        default:;
    }
}

static bool getInsertion(char **getIns, const int DIM, const _Bool fixedDim, const char conditions[]) {
    int start = 1; // dimensione iniziale dell'input (minimo 1 carattere)
    static int code = 0; // codice identificativo della lista di condizioni precedente.
    static cl *list = NULL; // lista di condizioni.
    
                                    //  numbers  -  chars
    static bool condition_types[SAFEDIM] = {false, false}; // array per il check del tipo di condizioni presenti.
    *getIns = xtdynmem(char, start); // alloco la memoria iniziale per l'inserimento.
    int ipo = (int)hashThis(conditions); // effettuo l'hashing delle condizioni inserite e ne ottengo il codice attuale.
    if (code != ipo) { // se il codice è diverso dal precedente allora libero la lista di condizioni precedenti e la ricalcolo.
        list = freeList(list);
        regexCompiler(&list, conditions, condition_types);
        code = ipo;
    }
    ipo = 0;

    // CHECKING CHARS
    bool building = false;
    char _char = '\0';
    while (_char != '\n') {
        ValidationRegister reg; // creo un registro per definire lo stato di validità del valore che viene processato.
        _char = (char)getchar();
        bool isNumber = checkIfNumber(_char);
        if (isNumber && condition_types[0]) {
            building = true;
            ipo *= 10;
            ipo += numConverter(_char);
        }

        if (_char != '\n') {
            checkValidity(_char, char_t, getListTop(list), &reg);
            if (!reg.negative && reg.typeVal == MATCHED) return false;
            if (reg.negative && reg.positive && reg.typeVal == NOT_MATCHED) {
                if (!building || !isNumber) return false;
            }
        }

        if (!isNumber && building) {
            building = false;
            checkValidity(ipo, int_t, getListTop(list), &reg);
            if (!reg.negative && reg.typeVal == MATCHED) return false;
            if (reg.negative && reg.positive && reg.typeVal == NOT_MATCHED) return false;
        }


        if (_char != '\n') {
            (*getIns)[start-1] = _char;
            start++;
            *getIns = (char*) realloc((*getIns), start*sizeof(char));
        }
    }
    (*getIns)[--start] = '\0';
    if (!start) return false;
    // MATCHING DIMENSION
    if (start < DIM && !fixedDim || start == DIM) return true;
    else return false;
}

void clearBuffer() {
    while(getchar() != '\n');
}

void input(char **getIns, const int DIM, const _Bool fixedDim, const char conditions[], const char message[]) {
    bool valid;
    if (conditions != NULL && conditions[0] != 0) {
        evaluateSyntax(conditions); // Controllo che la sintassi dell'espressione regolare sia corretta, 
        // in caso contrario esce dal programma con un exit code specifico.
    }
    if (*getIns != NULL) freeIt(getIns); // Verifico che il puntatore da allocare passato come parametro sia vuoto, in caso contrario lo libero.
    do {
        fprintf(stdout, message); // Stampo il messaggio specificato come avviso di input
        valid = getInsertion(getIns, DIM, fixedDim, conditions); // Ottengo l'inserimento e ne verifico la validità.
        if (strlen(*getIns) != (ftell(stdin) - 2) && !valid) { // in caso l'inserimento non sia valido perchè troppo lungo o perchè è stato inserito un carattere
            // non abilitato, e quindi il processo di lettura è terminato prematuramente allora effettuo un clear del buffer principale
            // e libero la memoria allocata fino ad ora.
            clearBuffer();
            free(*getIns);
        }
    } while(!valid);
}

void freeIt(char **var) {
    if (*var != NULL) free(*var);
    *var = NULL;
}

_Bool strComp(const char string[], const char string_[]) {
    if (strlen(string) != strlen(string_)) return false;
    for (int i = 0; string[i] && string_[i]; i++) {
        if ((string[i] != string_[i])) return false;
    }
    return true;
}

static _Bool ignoreCaps(char _char, char _char2) {
    if ((_char >= 'A' && _char <= 'Z') || (_char >= 'a' && _char <= 'z')) {
        if ((_char2 >= 'A' && _char2 <= 'Z') || (_char2 >= 'a' && _char2 <= 'z')) {
            return (_char == _char2 || (_char - 32) == _char2 || _char  == (_char2 - 32));
        }
    }
    return _char == _char2;
}

_Bool strCompNCS(const char string[], const char string_[]) {
    unsigned int comparison;
    if (strlen(string) != strlen(string_)) return false;
    for (int i = 0; i < strlen(string); i++) {
        if (ignoreCaps(string[i], string_[i])) comparison = 1;
        else return false;
    }
    return comparison;
}

static int *containsSupportPos(char *_chars, _Bool caseSensitive) {
    int *insucc = (int*) calloc(strlen(_chars), sizeof(int));
    insucc[0] = 0;
    for (int i = 1, j = 0; i < strlen(_chars); i++) {
        if ((!caseSensitive && ignoreCaps(_chars[j], _chars[i])) || _chars[j] == _chars[i]) {
            j++;
            insucc[i] = j;
        }
        else if (j != 0) {
            j = insucc[j-1];
        }
        else {
            insucc[i] = 0;
        }
    }
    return insucc;
}

match *contains(char *_chars, char *_str, _Bool caseSensitive) {
    match *result = (match*) calloc(1, sizeof(match));
    (*result).success = false;
    (*result).index = 0;
    (*result).indexes = (int*) calloc(1, sizeof(int));
    int *insucc = containsSupportPos(_chars, caseSensitive);
    for (int i = 0, j = 0; i < strlen(_str);) {
        if ((!caseSensitive && ignoreCaps(_str[i], _chars[j])) || _str[i] == _chars[j]) {
            i++;
            j++;
        }
        if (j == strlen(_chars)) {
            (*result).success = true;
            (*result).index++;
            (*result).indexes = (int*) realloc((*result).indexes, (*result).index * sizeof(int));
            (*result).indexes[(*result).index-1] = i-j;
            j = 0;
        }
        else if (i < strlen(_str)) {
            _Bool selection = (!caseSensitive) ? !ignoreCaps(_chars[j], _str[i]) : _chars[j] != _str[i];
            if (selection) {
                if (j != 0) {
                    j = insucc[j - 1];
                } else {
                    i++;
                }
            }
        }
    }

    free(insucc);
    return result;
}

int **checkMatches(match **matches) {
    if ((**matches).success) {
        int **result = (int**) calloc(2, sizeof(int*));
        result[0] = (int*) calloc(1, sizeof(int));
        result[0][0] = (**matches).index;
        result[1] = (int*) calloc(result[0][0], sizeof(int));
        for (int i = 0; i < result[0][0]; i++) {
            result[1][i] = (**matches).indexes[i];
        }
        free((**matches).indexes);
        free(*matches);
        return result;
    }
    free((**matches).indexes);
    free(*matches);
    return NULL;
}

char *numToString(int num) {
    char *numb = (char*) calloc(11, sizeof(char));
    numb[0] = '0';
    int dim = 0;
    while (num != 0) {
        for (int i = dim; dim != 0 && i > -1; i--) {
            numb[i] = numb[i - 1];
        }
        numb[0] = (char)((num % 10) + 48);
        num /= 10;
        dim++;
    }
    numb = (char*) realloc(numb, sizeof(char)*(strlen(numb) + 1));
    return numb;
}

char *charToString(const char _char) {
    static char str[1];
    str[0] = _char;
    return str;
}

void bubbleSortInt(int *array, int dimension) {
    for (int i = 0; i < dimension - 1; i++) {
        for (int j = 0, tmp; j < dimension - i - 1; j++) {
            if (array[j] > array[j+1]) {
                tmp = array[j];
                array[j] = array[j+1];
                array[j+1] = tmp;
            }
        }
    }
}

char **strcut(char *str, char _char) {
    int countChars = 0;
    int *cPos = NULL;
    for (int i = 0; i < strlen(str); i++) {
        if (str[i] == _char && i != strlen(str) - 1) {
            countChars++;
            cPos = (cPos == NULL) ? xtdynmem(int, countChars) : (int*) realloc(cPos, sizeof(int)*countChars);
            cPos[countChars-1] = i;
        }
    }
    if (!countChars) return NULL;
    char **result = xtdynmem(char*, countChars+SAFEDIM);
    for (int i = 0; i < countChars; i++) {
        result[i] = (i != 0) ? xtdynmem(char, cPos[i] - cPos[i-1] + 1) : xtdynmem(char, cPos[i] + 1);
        for (int j = (i == 0) ? 0 : cPos[i-1] + 1; j < cPos[i]; j++) {
            result[i][j - ((i == 0) ? 0 : (cPos[i-1] + 1))] = str[j];
        }
        if (i == countChars-1) {
            result[i+1] = xtdynmem(char, strlen(str) - cPos[countChars - 1]);
            for (int j = cPos[countChars - 1] + 1; j < strlen(str); j++) {
                result[i+1][j - cPos[countChars - 1] - 1] = str[j];
            }
        }
        result[i+2] = NULL;
    }
    return result;
}

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

_Bool sizedCmpNC(const char *str, const char *str_, const int init, const int final) {
    int size = final + 1 - init;
    if (strlen(str_) != size) return false;
    char tmp[size];
    for (int i = init; i <= final; i++) {
        tmp[i-init] = str_[i];
    }
    return strCompNCS(str, tmp);
}


Commands getCommands(const char *cmds) {
    Commands cmd = NULL;
    unsigned long long len = strlen(cmds);
    if (len == 0) exit(1); // da decidere

    return cmd;
}

_Bool sString (char **getIns, const char *cmds) {
    if (cmds == NULL) exit(1); // da decidere
    Commands commands = getCommands(cmds);
    return false;
}
