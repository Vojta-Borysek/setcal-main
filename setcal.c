/*
    VUT týmový projekt - setcal
    autoři:
        - Jan Kapsa (xkapsa00)
        - Petr Teichgráb (xteich01)
        - Michael Polívka (xpoliv07)
        - Vojtěch Borýsek (xborys02)
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define MAX_LINES 1000
#define MAX_CHARS 30

typedef enum {EOK, EFILE, ESYNTAX, ENF, EMEM} err;

typedef struct {
    int id;
    int size;
    int cap;
    int *items;
} set_t;
typedef struct {
    int cap;
    int size;
    set_t *sets;
} set_list_t;

typedef struct {
    int x;
    int y;
} pair_t;
typedef struct {
    int id;
    int size;
    int cap;
    pair_t *items;
} rel_t;
typedef struct {
    int cap;
    int size;
    rel_t *rels;
} rel_list_t;

typedef struct {
    int cap;
    int size;
    int *keys;
    char **values;
} map_t;
map_t map;
err e;

// set functions
void set_ctor(set_t *s);
void set_dtor(set_t *s);
void set_resize(set_t *s, int new_cap);
void set_append(set_t *s, int item);
int set_find(set_list_t list, int id);
void set_print(set_t s);
void set_list_ctor(set_list_t *s);
void set_list_dtor(set_list_t *s);
void set_list_resize(set_list_t *s, int new_cap);
void set_list_append(set_list_t *s, set_t* set);
bool empty(set_t set);
unsigned int card(set_t set);
set_t set_union (set_t set1, set_t set2);
set_t set_intersect (set_t set1, set_t set2);
set_t set_minus (set_t set1, set_t set2);
bool set_subseteq (set_t set1, set_t set2);
bool set_equals (set_t set1, set_t set2);
bool set_subset(set_t set1, set_t set2);
set_t set_complement(set_t universe, set_t set2);

// rel functions
void rel_ctor(rel_t *r);
void rel_dtor(rel_t *r);
void rel_resize(rel_t *r, int new_cap);
void rel_append(rel_t *r, int x, int y);
int rel_find(rel_list_t list, int id);
void rel_print(rel_t r);
void rel_list_ctor(rel_list_t *r);
void rel_list_dtor(rel_list_t *r);
void rel_list_resize(rel_list_t *r, int new_cap);
void rel_list_append(rel_list_t *r, rel_t* rel);
set_t domain(rel_t *rel);
set_t codomain(rel_t *rel);
bool reflexive(rel_t rel);
bool symmetric(rel_t rel);
bool antisymmetric(rel_t rel);
bool transitive(rel_t rel);
bool rel_function(rel_t rel);
bool surjective(rel_t rel, set_t setA, set_t setB);
bool injective (rel_t rel, set_t setA);
bool bijective(rel_t rel, set_t setA, set_t setB);

// mapping functions
void map_ctor();
void map_dtor();
void map_resize(int new_cap);
void map_append(char* item);
int map_value(char* value);
char *map_key(int key);
int map_command(char *name);

// data validation
bool is_set (set_t set);
bool is_rel (rel_t rel);
int convert_cmd_to_int (char* given_word);
bool is_command (int num_of_arg, char* given_word);

// get data functions
void get_data(FILE *f, set_list_t *set_list, rel_list_t *rel_list);
void process_split(char **array, int size, int line, set_list_t *set_list, rel_list_t *rel_list);
void fix_value(char *dst, char *src, int len, int location);

// other functions
void execute_command(int cmd_key, int *args, int argc, set_list_t set_list, rel_list_t rel_list);

int main(int argc, char **argv) {
    // vars
    map_ctor();
    set_list_t set_list;
    set_list_ctor(&set_list);
    rel_list_t rel_list;
    rel_list_ctor(&rel_list);
    e = EOK;

    // validate if there is an argument
    if (argc < 2) {
        printf("error: no argument");
        return 1;
    }

    // validate if supplied argument is a file path
    FILE *file = fopen(argv[1], "r");
    if (file == NULL) {
        printf("error: could not open supplied file\n");
        return (int)EFILE;
    } 

    // get data from file
    get_data(file, &set_list, &rel_list);
    if (e != EOK) {
        // free memory
        set_list_dtor(&set_list);
        rel_list_dtor(&rel_list);
        map_dtor();
        return (int)e;
    }
    // close file
    fclose(file);

    // free memory
    set_list_dtor(&set_list);
    rel_list_dtor(&rel_list);
    map_dtor();
    
    return 0;
}

// execute
void execute_command(int cmd_key, int *args, int argc, set_list_t set_list, rel_list_t rel_list) {
    int *ids = (int *) malloc(argc * sizeof(int));
    for (int i = 0; i < argc; i++)
    {
        if (cmd_key <= 8 || (cmd_key >= 16 && i > 0)) {
            ids[i] = set_find(set_list, args[i]);
        } else {
            ids[i] = rel_find(rel_list, args[i]);
        }
        if (ids[i] == -1) {
            e = ENF;
            printf("error: could not find set or relation with id %d\n", args[i]);
            free(ids);
            return;
        }
    }
    switch (cmd_key)
    {
    // empty
    case 0: {
        printf("%s\n", empty(set_list.sets[ids[0]]) ? "true" : "false");
        break;
    }
    // card
    case 1: {        
        printf("%d\n", card(set_list.sets[ids[0]]));
        break;
    }
    // complement
    case 2: {       
        set_t complementSet = set_complement(set_list.sets[0], set_list.sets[ids[0]]);
        if (e != EOK) {
            set_dtor(&complementSet);
            return;
        }
        set_print(complementSet);
        set_dtor(&complementSet);
        break;
    }
    // union
    case 3: {        
        set_t UnionSet = set_union(set_list.sets[ids[0]], set_list.sets[ids[1]]);
        if (e != EOK) {
            set_dtor(&UnionSet);
            return;
        }
        set_print(UnionSet);
        set_dtor(&UnionSet);
        break;
    }
    // intersect
    case 4: {        
        set_t interSectSet = set_intersect(set_list.sets[ids[0]], set_list.sets[ids[1]]);
        if (e != EOK) {
            set_dtor(&interSectSet);
            return;
        }
        set_print(interSectSet);
        set_dtor(&interSectSet);
        break;
    }
    // minus
    case 5: {        
        set_t minusSet = set_minus(set_list.sets[ids[0]], set_list.sets[ids[1]]);
        if (e != EOK) {
            set_dtor(&minusSet);
            return;
        }
        set_print(minusSet);
        set_dtor(&minusSet);
        break;
    }
    // subseteq
    case 6: {        
        printf("%s\n", set_subseteq(set_list.sets[ids[0]], set_list.sets[ids[1]]) ? "true" : "false");
        break;
    }
    // subset
    case 7: {        
        printf("%s\n", set_subset(set_list.sets[ids[0]], set_list.sets[ids[1]]) ? "true" : "false");
        break;
    }
    // equals
    case 8: {        
        printf("%s\n", set_equals(set_list.sets[ids[0]], set_list.sets[ids[1]]) ? "true" : "false");
        break;
    }
    //reflexive
    case 9: {
        printf("%s\n", reflexive(rel_list.rels[ids[0]]) ? "true" : "false");
        break;
    }
    //symmetric
    case 10: {
        printf("%s\n", symmetric(rel_list.rels[ids[0]]) ? "true" : "false");
        break;
    }
    //antisymmetric
    case 11: {
        printf("%s\n", antisymmetric(rel_list.rels[ids[0]]) ? "true" : "false");
        break;
    }
    //transitive
    case 12: {
        printf("%s\n", transitive(rel_list.rels[ids[0]]) ? "true" : "false");
        break;
    }
    //function
    case 13: {
        printf("%s\n", rel_function(rel_list.rels[ids[0]]) ? "true" : "false");
        break;
    }    
    // domain
    case 14: {
        set_t df = domain(&rel_list.rels[ids[0]]);
        if (e != EOK) {
            set_dtor(&df);
            return;
        }
        set_print(df);
        set_dtor(&df);
        break;
    }
    // codomain
    case 15: {    
        set_t hf = codomain(&rel_list.rels[ids[0]]);
        if (e != EOK) {
            set_dtor(&hf);
            return;
        }
        set_print(hf);
        set_dtor(&hf);
        break;
    }
    //injective
    case 16: {
        bool result = injective(rel_list.rels[ids[0]], set_list.sets[ids[1]]);
        printf("%s\n", result ? "true" : "false");
        break;
    }
    //surjective
    case 17: {        
        bool result = surjective(rel_list.rels[ids[0]], set_list.sets[ids[1]], set_list.sets[ids[2]]);
        printf("%s\n", result ? "true" : "false");
        break;
    }
    //bijective
    case 18: {        
        bool result = bijective(rel_list.rels[ids[0]], set_list.sets[ids[1]], set_list.sets[ids[2]]);
        printf("%s\n", result ? "true" : "false");
        break;
    }    
    default: {
        printf("default statement hit in execute_command\n");
        break;
    }
    }
    free(ids);
}

/****************************************************************
 *                      SET FUNCTIONS                           *
*****************************************************************/

// DISCLAIMER: code is inspired by: 
// https://wis.fit.vutbr.cz/FIT/st/cwk.php.cs?title=Demo9&csid=773153&id=14723

// constructor
void set_ctor(set_t *s)
{
    if (s == NULL) return;
    s->size = 0;
    s->cap = 0;
    s->items = NULL;
}
// destructor
void set_dtor(set_t *s)
{
    if (s == NULL) return;
    if (s->cap) {        
        free(s->items);
    }
}
// resizes set (reallocate memory)
void set_resize(set_t *s, int new_cap)
{
    int *resized;
    resized = (int *) realloc(s->items, new_cap * sizeof(int));
    if (resized != NULL)
    {
        s->items = resized;
        s->cap = new_cap;
    } else {
        e = EMEM;
        printf("error: could not allocate memory\n");
    }
}
// appends new int to set's data
void set_append(set_t *s, int item)
{
    if (s->size == s->cap) {
        set_resize(s, s->cap ? s->cap + s->cap : 1);
        if (e != EOK) return;
    }
 
    if (s->size < s->cap)
    {
        s->items[s->size] = item;
        s->size++;
    }
}
// finds set by id given a list of sets
// returns id if found, returns -1 if not
int set_find(set_list_t list, int id) {
    for (int i = 0; i < list.size; i++)
    {
        if (list.sets[i].id == id) {
            return i;
        }
    }
    return -1;
}
// prints set
void set_print(set_t s)
{
    printf("%c ", s.id == 1 ? 'U' : 'S');
    for (int i = 0; i < s.size; i++)
    {
        printf("%s ", map_key(s.items[i]));
    }
    printf("\n");
}

void set_list_ctor(set_list_t *s) {
    s->cap = 0;
    s->size = 0;
    s->sets = NULL;
}
void set_list_dtor(set_list_t *s) {
    if (s->sets != NULL)
    {
        for (int i = 0; i < s->size; i++)
        {
            set_dtor(&s->sets[i]);
        }
        free(s->sets);
    }  
}
void set_list_resize(set_list_t *s, int new_cap)
{
    set_t *resized;
    resized = (set_t *) realloc(s->sets, new_cap * sizeof(set_t));
    if (resized != NULL)
    {
        s->sets = resized;
        s->cap = new_cap;
    } else {
        e = EMEM;
        printf("error: could not allocate memory\n");
    }
}
void set_list_append(set_list_t *s, set_t* set)
{
    if (s->size == s->cap) {
        set_list_resize(s, s->cap ? s->cap + s->cap : 1);
        if (e != EOK) return;
    }
 
    if (s->size < s->cap)
    {
        s->sets[s->size] = *set;
        s->size++;
    }
}

// returns true if set is empty
bool empty(set_t set) {
    return !set.size;
}
// returns set's size
unsigned int card(set_t set) {
    return set.size;
}
// returns true if two sets are equal
bool set_equals(set_t set1, set_t set2) {
    bool isAsubseteq = set_subseteq(set1, set2);
    bool isBsubseteq = set_subseteq(set2, set1);
    return (isAsubseteq && isBsubseteq) ? true : false;
}
//returns union of two sets
set_t set_union (set_t set1, set_t set2)
{
    set_t UnionSet;
    set_ctor(&UnionSet); 
    bool isInSet = false;
    for(int i = 0; i < set1.size; i++)
    {
        set_append(&UnionSet, set1.items[i]);
        if (e != EOK) return UnionSet;
    }
    for(int i = 0; i < set2.size; i++)
    {
        for(int j = 0; j < UnionSet.size; j++)
        {
            if (set2.items[i] == UnionSet.items[j])
            {
                isInSet = true;
            }
        }
        if(!isInSet)
        {
            set_append(&UnionSet, set2.items[i]);
            if (e != EOK) return UnionSet;
        }
        isInSet = false;
    }
    return UnionSet;
}
// returns an intersection of two sets
set_t set_intersect (set_t set1, set_t set2)
{
    set_t interSectSet;
    set_ctor(&interSectSet);
    bool isInSets = false;
    for(int i = 0; i < set1.size; i++)
    {
        for(int j = 0; j < set2.size; j++)
        {
            if(set1.items[i] == set2.items[j])
            {
                isInSets = true;
            }
        }
        if(isInSets)
        {
            set_append(&interSectSet, set1.items[i]);
            if (e != EOK) return interSectSet;
        }
        isInSets = false;
    }
    if(interSectSet.items == NULL)
    {
        printf("empty set");
    }
    return interSectSet;
}
// returns TODO
set_t set_minus (set_t set1, set_t set2)
{
    set_t minusSet;
    set_ctor(&minusSet);
    bool isInSets = false;
    for(int i = 0; i < set1.size; i++)
    {
        for(int j = 0; j < set2.size; j++)
        {
            if(set1.items[i] == set2.items[j])
            {
                isInSets = true;
            }
        }
        if(!isInSets)
        {
            set_append(&minusSet, set1.items[i]);
            if (e != EOK) return minusSet;
        }
        isInSets = false;
    }
    return minusSet;
}
//returns true or false if set is subseteq of second set
bool set_subseteq (set_t set1, set_t set2)
{
    int sameElementAmount = 0;
    for(int i = 0; i < set1.size; i++)
    {
        for(int j = 0; j < set2.size; j++)
        {
            if(set1.items[i] == set2.items[j])
            {
                sameElementAmount++;
            }
        }
    }
    if(sameElementAmount == set1.size)
    {
        return true;
    }
    return false;
    
}
//return true or false if set is subset of second set
bool set_subset(set_t set1, set_t set2)
{
    int sameElementAmount = 0;
    if(set_subseteq(set1, set2))
    {
        for(int i = 0; i < set1.size; i++)
        {
            for(int j = 0; j < set2.size; j++)
            {
                if(set1.items[i] == set2.items[j])
                {
                    sameElementAmount++;
                }
            }
        }
    }
    else
    {
        return false;
    }
    return (sameElementAmount < set2.size) ? true : false; 
}
//returns absolute complement of set (universe \ set)
set_t set_complement(set_t universe, set_t set2)
{
    set_t s = set_minus(universe, set2);
    if (e != EOK) return s;
    return set_minus(universe, set2);
}

/****************************************************************
 *                      REL FUNCTIONS                           *
*****************************************************************/

// DISCLAIMER: code is inspired by: 
// https://wis.fit.vutbr.cz/FIT/st/cwk.php.cs?title=Demo9&csid=773153&id=14723

// constructor
void rel_ctor(rel_t *r)
{
    if (r == NULL) return;
    r->size = 0;
    r->cap = 0;
    r->items = NULL;
}
// destructor
void rel_dtor(rel_t *r)
{
    if (r == NULL) return;
    if (r->cap) {
        free(r->items);
    }
    r->items = NULL;
    r->cap = r->size = 0;
}
// resize rel (reallocate memory)
void rel_resize(rel_t *r, int new_cap)
{
    pair_t *resized;
    resized = (pair_t *) realloc(r->items, new_cap * sizeof(pair_t));
    if (resized != NULL)
    {
        r->items = resized;
        r->cap = new_cap;
    } else {
        e = EMEM;
        printf("error: could not allocate memory\n");
    }
}
// appends new ints to rel's data
void rel_append(rel_t *r, int x, int y)
{
    if (r->size == r->cap) {
        rel_resize(r, r->cap ? r->cap + r->cap : 1);
        if (e != EOK) return;
    }
 
    if (r->size < r->cap)
    {
        r->items[r->size].x = x;
        r->items[r->size].y = y;
        r->size++;
    }
}
// finds rel by id given a list of rels
// returns id if found, returns -1 if not
int rel_find(rel_list_t list, int id) {
    for (int i = 0; i < list.size; i++)
    {
        if (list.rels[i].id == id) {
            return i;
        }
    }
    return -1;
}
// prints rel
void rel_print(rel_t r)
{
    printf("R ");
    for (int i = 0; i < r.size; i++)
    {
        printf("(%s %s) ", map_key(r.items[i].x), map_key(r.items[i].y));
    }
    printf("\n");
}

// relation list functions
void rel_list_ctor(rel_list_t *r) {
    r->cap = 0;
    r->size = 0;
    r->rels = NULL;
}
void rel_list_dtor(rel_list_t *r) {
    if (r->rels != NULL)
    {
        for (int i = 0; i < r->size; i++)
        {
            rel_dtor(&r->rels[i]);
        }
        free(r->rels);
    }  
}
void rel_list_resize(rel_list_t *r, int new_cap)
{
    rel_t *resized;
    resized = (rel_t *) realloc(r->rels, new_cap * sizeof(rel_t));
    if (resized != NULL)
    {
        r->rels = resized;
        r->cap = new_cap;
    } else {
        e = EMEM;
        printf("error: could not allocate memory\n");
    }
}
void rel_list_append(rel_list_t *r, rel_t* rel)
{
    if (r->size == r->cap) {
        rel_list_resize(r, r->cap ? r->cap + r->cap : 1);
        if (e != EOK) return;
    }
 
    if (r->size < r->cap)
    {
        r->rels[r->size] = *rel;
        r->size++;
    }
}

// returns a domain of a relation
set_t domain(rel_t *rel) {
    set_t s;
    set_ctor(&s);
    for (int i = 0; i < rel->size; i++)
    {
        set_append(&s, rel->items[i].x);
        if (e != EOK) return s;
    }
    return s;
}
// returns a codomain of a relation
set_t codomain(rel_t *rel) {
    set_t s;
    set_ctor(&s);
    for (int i = 0; i < rel->size; i++)
    {
        set_append(&s, rel->items[i].y);
        if (e != EOK) return s;
    }
    return s;
}
// returns true if relation is transitive
bool transitive(rel_t rel) {
    bool result = true;
    //int matrix[map.size][map.size] = {0};
    int **matrix = (int**)calloc(map.size, map.size * sizeof(int*));
    for (int i = 0; i < map.size; i++) {
        matrix[i] = (int*)calloc(map.size, map.size * sizeof(int));
    }
    for(int i = 0; i < rel.size;i++){
        matrix[rel.items[i].x][rel.items[i].y] = 1;
    }
    for(int i = 0; i < map.size;i++){
        for(int j = 0; j < map.size;j++){
            if (matrix[i][j] && i != j){
                for(int k = 0; k < map.size;k++){
                    if(matrix[j][k] && !matrix[i][k]){
                        result = false;
                    }
                }
            }
        }
    }
    for (int i = 0; i < map.size; i++) {
        free(matrix[i]);
    }
    free(matrix);
    return result;
}
// returns true if relation is antisymmetric
bool antisymmetric(rel_t rel) {
    bool result = true;
    int **matrix = (int**)calloc(map.size, map.size * sizeof(int*));
    for (int i = 0; i < map.size; i++) {
        matrix[i] = (int*)calloc(map.size, map.size * sizeof(int));
    }
    for(int i = 0; i < rel.size;i++){
        matrix[rel.items[i].x][rel.items[i].y] = 1;
    }
    for(int i = 0; i < map.size;i++){
        for(int j = i; j < map.size;j++){
            if(matrix[i][j] == 1 && matrix[j][i] == 1 && !(i == j)  ){
                result = false;
                break;
            }
        }
    }
    for (int i = 0; i < map.size; i++) {
        free(matrix[i]);
    }
    free(matrix);
    return result;
}
// returns true if relation is reflexive
bool reflexive(rel_t rel) {
    bool result = true;
    //int matrix[map.size][map.size] = {0};
    int **matrix = (int**)malloc(map.size * sizeof(int*));
    for (int i = 0; i < map.size; i++) {
        matrix[i] = (int*)calloc(map.size, map.size * sizeof(int));
    }
    
    for(int i = 0; i < rel.size;i++){
        matrix[rel.items[i].x][rel.items[i].y] = 1;
    }
    for (int i = 0; i < map.size;i++){
        if(!matrix[i][i]){
            result = false;
                break;
        }
    }
    for (int i = 0; i < map.size; i++) {
        free(matrix[i]);
    }
    free(matrix);
    return result;
}
// returns true if relation is symmetric
bool symmetric(rel_t rel) {
    bool result = true;
    //int matrix[map.size][map.size] = {0};
    int **matrix = (int**)malloc(map.size * sizeof(int*));
    for (int i = 0; i < map.size; i++) {
        matrix[i] = (int*)calloc(map.size, map.size * sizeof(int));
    }
    for(int i = 0; i < rel.size;i++){
        matrix[rel.items[i].x][rel.items[i].y] = 1;
    }
    for(int i = 0; i < map.size;i++){
        for(int j = i; j < map.size;j++){
            if(!(matrix[i][j] == matrix[j][i])){
                result = false;
                break;
            }
        }
    }
    for (int i = 0; i < map.size; i++) {
        free(matrix[i]);
    }
    free(matrix);
    return result;
}
//return true if relation is a function
bool rel_function(rel_t rel) {
    int *hf = (int*)calloc(map.size, map.size * sizeof(int));
    for(int i = 0; i < rel.size; i++){
        if(hf[rel.items[i].y] == 0){
            hf[rel.items[i].y] = 1;
        }
        else{
            free(hf);
            return false;
        }
    }
    free(hf);
    return true;
}
//return true if relation is surjective
bool  surjective(rel_t rel, set_t setA, set_t setB){
    int *hf = (int*)calloc(map.size, map.size * sizeof(int));
    int *df = (int*)calloc(map.size, map.size * sizeof(int));
    for(int i = 0; i < rel.size; i++){
        hf[rel.items[i].y] = 1;
        df[rel.items[i].x] = 1;
    }
    for(int i = 0; i < setB.size; i++){
        if(hf[setB.items[i]] == 0){
            free(hf);
            free(df);
            return false;
        }
    }
    for(int i = 0; i < setA.size; i++){
        if(df[setA.items[i]] == 0){
            free(hf);
            free(df);
            return false;
        }
    }
    free(hf);
    free(df);
    return true;
}
//return true if relation is injective
bool  injective(rel_t rel, set_t setA){
    int *hf = (int*)calloc(map.size, map.size * sizeof(int));
    int *df = (int*)calloc(map.size, map.size * sizeof(int));
    for(int i = 0; i < rel.size; i++){
        if(hf[rel.items[i].y] == 0 && df[rel.items[i].x] == 0){
        hf[rel.items[i].y] = 1;
        df[rel.items[i].x] = 1;
        }
        else{
            free(df);
            free(hf);
            return false;
        }
    }
    for (int i = 0; i < setA.size;i++){
        if(df[setA.items[i]] == 0){
            free(df);
            free(hf);
            return false;
        }
    }

    free(df);
    free(hf);
    return true;
}
//return true if relation is bijective
bool bijective(rel_t rel, set_t setA, set_t setB) {
    return surjective(rel, setA, setB) && injective(rel, setA);
}

/****************************************************************
 *                      DATA VALIDATION                      *
*****************************************************************/
//checking if it is set
bool is_set (set_t set) {
    for (int i = 0; i < set.size; i++) {
        for (int j = i+1; j < set.size; j++) {
            if (set.items[i] == set.items[j]){
                //Set can't have two indentical elements 
                return false;
            }
        }
    }
    return true;
}
//checking if it is rel
bool is_rel (rel_t rel) {
    for (int i = 0; i < rel.size; i++) {
        for (int j = i+1; j < rel.size; j++) {
            if (rel.items[i].x == rel.items[j].x && rel.items[i].y == rel.items[j].y) {
                //Relation can't have two indentical elements
                return false;
            } 
        }
    }
    return true;
}
//changing command from string to int
int convert_cmd_to_int (char* given_word) {
    char* commands[] = {"empty", "card", "complement", "union", "intersect", "minus", "subseteq", "subset", "equals", "reflexive", "symmetric", "antisymmetric", "transitive", "function", "domain", "codomain", "injective", "surjective", "bijective", 0};
    char** ptr = commands;
    int i = 0;
    //loop trough commands
    while (i <= 18) {
        int value = strcmp(given_word, *ptr);
        //checking if given word is in commands
        if (value == 0) {
            //returning position (int) of word in commands
            return i;
        }
        i++;
        ptr++;
    }
    return -1;
}
//checking if command is valid and if it has right number of arguments
bool is_command (int num_of_arg, char* given_word) {
    int id = convert_cmd_to_int (given_word);
    //broken down by how many arguments I need 
    if ((id >= 0 && id <= 2) || (id >= 9 && id <= 15)) {
        if (num_of_arg == 3) {
            return true;
        }
        return false;
    }
    //broken down by how many arguments I need
    else if (id >= 3 && id <= 8) {
        if (num_of_arg == 4) {
            return true;
        }
    }
    //broken down by how many arguments I need
    else if (id >= 16 && id <= 18) {
        if (num_of_arg == 5) {
            return true;
        }
        return false;
    }
    return false;
}

/****************************************************************
 *                      GET DATA FUNCTIONS                      *
*****************************************************************/

// mapping
void map_ctor() {
    map.cap = 0;
    map.size = 0;
    map.keys = NULL;
    map.values = NULL;
}
void map_dtor() {
    if (map.keys != NULL) {
        free(map.keys);
    }
    if (map.values != NULL) {
        for (int i = 0; i < map.size; i++)
        {
            free(map.values[i]);
        }
        free(map.values);
    }
}
void map_resize(int new_cap)
{
    int *resized_keys;
    resized_keys = (int *) realloc(map.keys, new_cap * sizeof(int));
    char **resized_values;
    resized_values = (char **) realloc(map.values, new_cap * sizeof(char*));
    
    if (resized_keys && resized_values)
    {
        map.keys = resized_keys;
        map.values = resized_values;
        map.cap = new_cap;
    } else {
        e = EMEM;
        printf("error: could not allocate memory\n");
    }
}
void map_append(char* item)
{
    if (map.size == map.cap) {
        map_resize(map.cap ? map.cap + map.cap : 1);
        if (e != EOK) return;
    }
 
    if (map.size < map.cap)
    {
        map.values[map.size] = malloc((strlen(item)+1) * sizeof(char));
        strcpy(map.values[map.size], item);
        map.keys[map.size] = map.size;
        map.size++;
    }
}
int map_value(char* value) {
    for (int i = 0; i < map.size; i++)
    {
        if (!strcmp(value, map.values[i])) {
            return map.keys[i];
        }
    }
    return -1; 
}
char *map_key(int key) {
    return map.values[key];
}

// reads and splits data into universe, sets, rels and commands variables
void get_data(FILE *f, set_list_t *set_list, rel_list_t *rel_list) {
    int c;
    int a_cap = 1;
    int a_size = 0;
    int a_max = 0;
    int s_cap = 2;
    int s_size = 0;
    int line_count = 0;
    char **array = (char **) malloc(a_cap * sizeof(char*));
    if (array == NULL) {
        e = EMEM;
        printf("error: could not allocate memory\n");
        return;
    }
    do {        
        c = fgetc(f);
        if (s_size == 0 && a_size+1 > a_max) {
            array[a_size] = malloc(s_cap * sizeof(char));
            if (array == NULL) {
                e = EMEM;
                printf("error: could not allocate memory\n");
                return;
            }
        }
        if (c == 13) continue;
        if (c != 32 && c != 10 && c != EOF) {
            if (s_size == s_cap) {
                s_cap += s_cap;
                array[a_size] = (char *) realloc(array[a_size], s_cap * sizeof(char));
                if (array[a_size] == NULL) {
                    e = EMEM;
                    printf("error: could not reallocate memory\n");
                    return;
                }
            }
            array[a_size][s_size] = c;
            s_size++;
            if (s_size > MAX_CHARS) {
                e = EFILE;
                printf("error: string has more than %d characters\n", MAX_CHARS);
                for (int i = 0; i < a_max; i++)
                {
                    free(array[i]);
                }
                free(array);
                return;
            }
        } else {
            if (s_size == s_cap) {
                s_cap += s_cap;
                array[a_size] = (char *) realloc(array[a_size], s_cap * sizeof(char));
                if (array[a_size] == NULL) {
                    e = EMEM;
                    printf("error: could not reallocate memory\n");
                    return;
                }
            }
            array[a_size][s_size] = '\0';
            a_size++;

            if (c == 32) {
                if (a_size == a_cap) {
                    a_cap += a_cap;
                    char **resized = (char **) realloc(array, a_cap * sizeof(char*));
                    if (resized == NULL) {
                        e = EMEM;
                        printf("error: could not reallocate memory\n");
                        for (int i = 0; i < a_max; i++)
                        {
                            free(array[i]);
                        }
                        free(array);
                        return;
                    }
                    array = resized;
                }
            } else {
                if (a_size >= a_max) a_max = a_size;
                line_count++;
                if (line_count > MAX_LINES) {
                    e = EFILE;
                    printf("error: file has more then %d lines\n", MAX_LINES);
                    for (int i = 0; i < a_max; i++)
                    {
                        free(array[i]);
                    }
                    free(array);
                    return;
                }
                process_split(array, a_size, line_count, set_list, rel_list);
                if (e != EOK) return;
                a_size = 0;
            }

            s_size = 0;
        }
    } while(c != EOF);
    for (int i = 0; i < a_max; i++)
    {
        free(array[i]);
    }
    free(array);
}
void process_split(char **array, int size, int line, set_list_t *set_list, rel_list_t *rel_list) {
    if (!strcmp(array[0], "U")) {
        if (line != 1) {
            e = ESYNTAX;
            printf("error: 2 or more universes not supported (line: %d)\n", line);
            return;
        }
        // create mapping table
        for (int i = 1; i < size; i++)
        {
            map_append(array[i]);
            if (e != EOK) return;
        }
        set_t s;
        set_ctor(&s);
        s.id = line;
        // map values
        int key = -1;
        for (int i = 1; i < size; i++)
        {
            key = map_value(array[i]);
            set_append(&s, key);
            if (e != EOK) return;
        }
        if (!is_set(s)) {
            e = ESYNTAX;
            printf("error: universe is not a set\n");
            return;
        }
        // add set
        set_print(s);
        set_list_append(set_list, &s);
        if (e != EOK) return;

    } else if (!strcmp(array[0], "S")) {
        if (line == 1) {
            e = ESYNTAX;
            printf("error: no universe on line 1\n");
            return;
        }
        set_t s;
        set_ctor(&s);
        s.id = line;
        // map values
        int key = -1;
        for (int i = 1; i < size; i++)
        {
            key = map_value(array[i]);
            if (key == -1) {
                e = ESYNTAX;
                printf("error: item %s of set on line %d is not in the universe\n", array[i], line);
                return;
            }
            set_append(&s, key);
            if (e != EOK) return;
        }
        if (!is_set(s)) {
            e = ESYNTAX;
            printf("error: S on line %d is not a set\n", line);
            return;
        }
        // add set
        set_print(s);
        set_list_append(set_list, &s);
        if (e != EOK) return;
    } else if (!strcmp(array[0], "R")) {
        if (line == 1) {
            e = ESYNTAX;
            printf("error: no universe on line 1\n");
            return;
        }
        rel_t r;
        rel_ctor(&r);
        r.id = line;
        // map values
        int key_x = -1;
        int key_y = -1;
        for (int i = 1; i < size; i++)
        {
            // fix rel values
            int x_len = strlen(array[i]);
            char *x_value = (char *) malloc(x_len * sizeof(char));
            fix_value(x_value, array[i], x_len, 0);
            if (e != EOK) {
                free(x_value);
                return;
            }
            i++;
            int y_len = strlen(array[i]);
            char *y_value = (char *) malloc(y_len * sizeof(char));
            fix_value(y_value, array[i], y_len, 1);
            if (e != EOK) {
                free(x_value);
                free(y_value);
                return;
            }
            key_x = map_value(x_value);
            key_y = map_value(y_value);
            if (key_x == -1) {
                e = ESYNTAX;
                printf("error: item %s of relation on line %d is not in the universe\n", x_value, line);
                return;
            }
            if (key_y == -1) {
                e = ESYNTAX;
                printf("error: item %s of relation on line %d is not in the universe\n", y_value, line);
                return;
            }
            free(x_value);
            free(y_value);
            rel_append(&r, key_x, key_y);
            if (e != EOK) return;
        }
        // add rel
        if (!is_rel(r)) {
            e = ESYNTAX;
            printf("error: R on line %d is not a relation\n", line);
            return;
        }
        rel_print(r);
        rel_list_append(rel_list, &r);
        if (e != EOK) return;
    } else if (!strcmp(array[0], "C")) {
        if (line == 1) {
            e = ESYNTAX;
            printf("error: no universe on line 1\n");
            return;
        }
        // convert name
        int key = convert_cmd_to_int(array[1]);
        if (key == -1) {
            e = ESYNTAX;
            printf("error: command on line %d is not valid\n", line);
            return;
        }
        if (!is_command(size, array[1])) {
            e = ESYNTAX;
            printf("error: command on line %d is not valid\n", line);
            return;
        }
        int *args = (int*) malloc((size-2) * sizeof(int));
        for (int i = 2; i < size; i++)
        {
            args[i-2] = atoi(array[i]);
            if (args[i-2] == 0) {
                e = ESYNTAX;
                printf("error: wrong command arguments on line %d\n", line);
                free(args);
                return;
            }
        }
        // execute command
        execute_command(key, args, size-2, *set_list, *rel_list);
        free(args);
        if (e != EOK) return;
    } else {
        // invalid syntax
        e = ESYNTAX;
        printf("error: invalid line identificator on line %d\n", line);
        return;
    }
}
void fix_value(char *dst, char *src, int len, int location) {
    if (location) {
        // => fixing string with )
        if (src[len-1] != ')') {            
            e = ESYNTAX;        
            printf("error: data in relation is not ending with )\n");
            return;    
        }
        for (int i = 0; i < len; i++)
        {
            dst[i] = src[i];
        }
        dst[len-1] = '\0';
    } else {
        // => fixing string with (
        if (src[0] != '(') {            
            e = ESYNTAX;        
            printf("error: data in relation is not starting with (\n");
            return;    
        }
        for (int i = 0; i < len; i++)
        {
            dst[i] = src[i+1];
        }
    }
}
