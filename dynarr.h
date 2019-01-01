/*
dynarr.h - Portable, single-file, multi-purpose, dynamic array library for values of any type

To the extent possible under law, the author(s) have dedicated all copyright and related and neighboring
rights to this software to the public domain worldwide. This software is distributed without any warranty.
You should have received a copy of the CC0 Public Domain Dedication along with this software.
If not, see <http://creativecommons.org/publicdomain/zero/1.0/>.
*/

/*
dynarr supports the following three configurations:
#define DYNARR_EXTERN
    Default, should be used when using dynarr in multiple compilation units within the same project.
#define DYNARR_IMPLEMENTATION
    Must be defined in exactly one source file within a project for dynarr to be found by the linker.
#define DYNARR_STATIC
    Defines all dynarr functions as static, useful if dynarr is only used in a single compilation unit.

dynarr supports the following additional options:
#define DYNARR_NOASSERT
    Disables the use of assert in various dynarr functions, must be defined globally to work properly.
#define DYNARR_ZALLOC(S)
    Overrides the zalloc function used by dynarr with your own. This is calloc but with just 1 argument.
#define DYNARR_REALLOC(P, S)
    Overrides the realloc function used by dynarr with your own. Defaults to the standard realloc.
#define DYNARR_FFREE(P)
    Overrides the free function used by dynarr with your own. Defaults to the standard free.

dynarr usage:
    A dynarr is a dynamic array of any one type with a multi-purpose interface, thus allowing it to act as just a dynamic array,
    a stack, a queue, a binary-searchable list, or even all of the above at once. What data structure it acts as depends on the
    functions used on it, e.g. a dynarr that is only ever modified through DYNARR_PUSH and DYNARR_POP would effectively be a stack.
    All dynarr functions expect a valid dynarr pointer (type "any*"), passing an invalid or NULL pointer will cause SEGFAULT or worse.

dynarr initialization:
    A dynarr containing elements of a given type is represented as a pointer to said type that is initialized using DYNARR_NEW(type).
    Once initialized, it can be used as an argument in the various DYNARR_XXX macros. After use it should be freed using DYNARR_FREE.

dynarr arguments:
    Due to them being macros, many DYNARR_XXX macros will end up evaluating their arguments multiple times. Arguments with
    side effects should therefore be avoided. An exception are value arguments (type "any"), which are evaluated exactly once.
*/

//header section
#ifndef DYNARR_H
#define DYNARR_H

//process configuration
#ifdef DYNARR_STATIC
    #define DYNARR_IMPLEMENTATION
    #define DARRDEF static
#else //DYNARR_EXTERN
    #define DARRDEF extern
#endif

/*
any* DYNARR_NEW(type)
    creates a new dynarr instance for given type and returns it, or NULL on failure
int DYNARR_SIZE(any*)
    returns the size of the given dynarr, O(1)
any DYNARR_AT(any*, int)
    returns the element at given index in given dynarr, works as lvalue, O(1)
any DYNARR_FIRST(any*)
    returns the first element in given dynarr, works as lvalue, O(1)
any DYNARR_LAST(any*)
    returns the last element in given dynarr, works as lvalue, O(1)
int DYNARR_VALID(any*, int)
    returns non-zero if the given index is valid for given dynarr, O(1)
void DYNARR_CLEAR(any*)
    clears given dynarr, setting its size to 0, O(1)
void DYNARR_FREE(any*)
    frees given dynarr and all its internal data
int DYNARR_PUSH(any*, any)
    appends the given element to the end of given dynarr (growing if needed), amortized O(1)
    returns the index the element was placed at, or -1 on allocation failure
any DYNARR_POP(any*)
    removes the last element in given dynarr and returns it, must not be empty, O(1)
any DYNARR_DEQUEUE(any*)
    removes the first element in given dynarr and returns it, must not be empty, O(1)
int DYNARR_INSERT(any*, int, any)
    inserts the given element at given index in given dynarr, shifting other elements forward, O(n)
    returns the index the element was placed at, or -1 on allocation failure
int DYNARR_SHOVE(any*, int, any)
    faster alternative to DYNARR_INSERT that doesn't maintain order of other elements, amortized O(1)
void DYNARR_REMOVE(any*, int)
    removes the element at given index in given dynarr while maintaining order of remaining elements, O(n)
void DYNARR_DITCH(any*, int)
    faster alternative to DYNARR_REMOVE that doesn't maintain order of remaining elements, O(1)
int DYNARR_RESIZE(any*, int)
    resizes given dynarr to the given size, possibly also adjusting its capacity, O(n)
    returns the new size after the resize as requested, or -1 on allocation failure
int DYNARR_CAPACITY(any*, int)
    adjusts the internal capacity of the given dynarr to most closely match the given number of elements, O(n)
    returns the capacity after resizing, may not match what was requested, or -1 on allocation failure
int DYNARR_FIND_LIN(any*, any*)
    returns the index of the first element equal to the value pointed to by given key, -1 if not found, O(n)
int DYNARR_FIND_BIN(any*, int(*)(const void*, const any*), void*)
    returns the index of an element that evaluates as equal to given key according to given function, O(logn)
    dynarr must be sorted in ascending order beforehand, otherwise result is undefined, -1 if not found
void DYNARR_SORT_INS(any*, int(*)(const any*, const any*))
    insertion sorts the elements in given dynarr according to given comparison function, O(n*n)
    comparison function should return 1 if first argument is greater than second argument
    0 if it is equal, and -1 if it is smaller, dynarr will be sorted smallest to greatest
void DYNARR_SORT_STD(any*, int(*)(const any*, const any*))
    same as DYNARR_SORT_INS but uses the C standard qsort function for sorting instead, O(n*logn)
*/

//macros
#ifndef DYNARR_NOASSERT
    #include <assert.h> //assert
    #define DARR_ASSERT(E) assert(E)
#else
    #define DARR_ASSERT(E) ((void)0)
#endif
#define DYNARR_NEW(T) ((T*)dynarrNew(sizeof(T)))
#define DYNARR_SIZE(A) (DARR_SIZE(A))
#define DYNARR_AT(A, I) (*(DARR_ASSERT(DYNARR_VALID(A, I)), &(A)[DARR_OFFS(A)+(I)]))
#define DYNARR_FIRST(A) (*(DARR_ASSERT(DARR_SIZE(A)), &(A)[DARR_OFFS(A)]))
#define DYNARR_LAST(A) (*(DARR_ASSERT(DARR_SIZE(A)), &(A)[DARR_OFFS(A)+DARR_SIZE(A)-1]))
#define DYNARR_VALID(A, I) ((I >= 0)&&(I < DARR_SIZE(A)))
#define DYNARR_CLEAR(A) (DARR_OFFS(A) = DARR_SIZE(A) = 0, (void)0)
#define DYNARR_FREE(A) dynarrFree(A)
#define DYNARR_PUSH(A, V) (dynarrGrow((void**)&(A)) ? -1 : ((A)[DARR_OFFS(A)+DARR_SIZE(A)] = V, DARR_SIZE(A)++))
#define DYNARR_POP(A) (DARR_ASSERT(DARR_SIZE(A)), (A)[DARR_OFFS(A)+--DARR_SIZE(A)])
#define DYNARR_DEQUEUE(A) (DARR_ASSERT(DARR_SIZE(A)), DARR_SIZE(A)--, (A)[DARR_OFFS(A)++])
#define DYNARR_INSERT(A, I, V) (DARR_ASSERT(DYNARR_VALID(A, I)), dynarrGrow((void**)&(A)) ? -1 : \
    (memmove(&(A)[DARR_OFFS(A)+(I)+1], &(A)[DARR_OFFS(A)+(I)], DARR_ELEM(A)*(DARR_SIZE(A)-(I))), (A)[DARR_OFFS(A)+(I)] = V, DARR_SIZE(A)++, I))
#define DYNARR_SHOVE(A, I, V) (DARR_ASSERT(DYNARR_VALID(A, I)), (DYNARR_PUSH(A, (A)[DARR_OFFS(A)+(I)]) == -1) ? -1 : ((A)[DARR_OFFS(A)+(I)] = V, I))
#define DYNARR_REMOVE(A, I) (DARR_ASSERT(DYNARR_VALID(A, I)), (void)memmove(&(A)[DARR_OFFS(A)+(I)], &(A)[DARR_OFFS(A)+(I)+1], DARR_ELEM(A)*(--DARR_SIZE(A)-(I))))
#define DYNARR_DITCH(A, I) (DARR_ASSERT(DYNARR_VALID(A, I)), (A)[DARR_OFFS(A)+(I)] = DYNARR_POP(A), (void)0)
#define DYNARR_RESIZE(A, S) dynarrResize((void**)&(A), S)
#define DYNARR_CAPACITY(A, C) dynarrCapacity((void**)&(A), C)
#define DYNARR_FIND_LIN(A, K) dynarrFindLinear(A, K)
#define DYNARR_FIND_BIN(A, F, K) dynarrFindBinary(A, (int(*)(const void*, const void*))(F), K)
#define DYNARR_SORT_INS(A, F) dynarrSortInsert(A, (int(*)(const void*, const void*))(F))
#define DYNARR_SORT_STD(A, F) dynarrSortStandard(A, (int(*)(const void*, const void*))(F))
#define DARR_RAW(A) (((struct dynarr*)(A))[-1])
#define DARR_CAPA(A) (DARR_RAW(A).capa)
#define DARR_ELEM(A) (DARR_RAW(A).elem)
#define DARR_OFFS(A) (DARR_RAW(A).offs)
#define DARR_SIZE(A) (DARR_RAW(A).size)

//includes
#include <string.h> //memmove

//structs
struct dynarr {
    int capa, elem, offs, size;
};

//function declarations
DARRDEF void* dynarrNew(int);
DARRDEF void dynarrFree(void*);
DARRDEF int dynarrGrow(void**);
DARRDEF int dynarrResize(void**, int);
DARRDEF int dynarrCapacity(void**, int);
DARRDEF int dynarrFindLinear(const void*, const void*);
DARRDEF int dynarrFindBinary(const void*, int(*)(const void*, const void*), const void*);
DARRDEF void dynarrSortInsert(void*, int(*)(const void*, const void*));
DARRDEF void dynarrSortStandard(void*, int(*)(const void*, const void*));

#endif //DYNARR_H

//implementation section
#ifdef DYNARR_IMPLEMENTATION
#undef DYNARR_IMPLEMENTATION

//macros
#ifndef DYNARR_ZALLOC
    #define DYNARR_ZALLOC(S) calloc(1, S)
#endif
#ifndef DYNARR_REALLOC
    #define DYNARR_REALLOC(P, S) realloc(P, S)
#endif
#ifndef DYNARR_FFREE
    #define DYNARR_FFREE(P) free(P)
#endif
#define DARR_EPTR(A, I) (&((char*)(A))[DARR_ELEM(A)*(DARR_OFFS(A)+(I))])

//includes
#include <stdlib.h> //memory allocation

//public functions
DARRDEF void* dynarrNew (int elem) {
    //allocate new dynarr with space for one element
    struct dynarr* darr = DYNARR_ZALLOC(sizeof(struct dynarr) + elem);
    //check for allocation failure
    if (!darr) return NULL;
    //fill in values
    darr->capa = 1;
    darr->elem = elem;
    //return
    return &darr[1];
}
DARRDEF void dynarrFree (void* a) {
    DYNARR_FFREE(&DARR_RAW(a));
}
DARRDEF int dynarrGrow (void** a) {
    //grow if currently at capacity
    if (DARR_OFFS(*a)+DARR_SIZE(*a) == DARR_CAPA(*a))
        if (DARR_OFFS(*a) >= DARR_SIZE(*a)) {
            //double the available capacity by offset reset
            memcpy(*a, DARR_EPTR(*a, 0), DARR_ELEM(*a)*DARR_SIZE(*a));
            DARR_OFFS(*a) = 0;
        } else {
            //double the available capacity by reallocation
            void** ptr = DYNARR_REALLOC(&DARR_RAW(*a), sizeof(struct dynarr) + DARR_ELEM(*a)*DARR_CAPA(*a)*2);
            //check for realloc failure
            if (!ptr) return -1;
            //assign to dynarr pointer
            *a = &((struct dynarr*)ptr)[1];
            //update allocated capacity
            DARR_CAPA(*a) *= 2;
        }
    //return
    return 0;
}
DARRDEF int dynarrResize (void** a, int s) {
    //check if shrinking or growing
    if (s < DARR_SIZE(*a)) {
        //simply shrink size to s
        DARR_SIZE(*a) = (s < 0) ? 0 : s;
        //reset offset if shrunk to 0
        if (!DARR_SIZE(*a)) DARR_OFFS(*a) = 0;
    } else if (s > DARR_SIZE(*a)) {
        //make space if new size above capacity
        if (DARR_OFFS(*a)+s > DARR_CAPA(*a)) {
            //reset offset if there is any
            if (DARR_OFFS(*a)) {
                memmove(*a, DARR_EPTR(*a, 0), DARR_ELEM(*a)*DARR_SIZE(*a));
                DARR_OFFS(*a) = 0;
            }
            //grow if offset reset not enough
            if (s > DARR_CAPA(*a)) {
                //grow to exactly the requested size
                void** ptr = DYNARR_REALLOC(&DARR_RAW(*a), sizeof(struct dynarr) + DARR_ELEM(*a)*s);
                //check for realloc failure
                if (!ptr) return -1;
                //assign to dynarr pointer
                *a = &((struct dynarr*)ptr)[1];
                //update allocated capacity
                DARR_CAPA(*a) = s;
            }
        }
        //zero out any new elements added
        memset(DARR_EPTR(*a, DARR_SIZE(*a)), 0, DARR_ELEM(*a)*(s-DARR_SIZE(*a)));
        //update to new size
        DARR_SIZE(*a) = s;
    }
    //return
    return DARR_SIZE(*a);
}
DARRDEF int dynarrCapacity (void** a, int c) {
    //clamp to used size
    if (c < DARR_SIZE(*a)) c = DARR_SIZE(*a);
    //reset offset if there is any
    if (DARR_OFFS(*a)) {
        memmove(*a, DARR_EPTR(*a, 0), DARR_ELEM(*a)*DARR_SIZE(*a));
        DARR_OFFS(*a) = 0;
    }
    //adjust capacity if necessary
    if (c != DARR_CAPA(*a)) {
        //adjust capacity by reallocation
        void** ptr = DYNARR_REALLOC(&DARR_RAW(*a), sizeof(struct dynarr) + DARR_ELEM(*a)*c);
        //check for realloc failure
        if (!ptr) return -1;
        //assign to dynarr pointer
        *a = &((struct dynarr*)ptr)[1];
        //update allocated capacity
        DARR_CAPA(*a) = c;
    }
    //return
    return DARR_CAPA(*a);
}
DARRDEF int dynarrFindLinear (const void* a, const void* k) {
    for (int i = 0; i < DARR_SIZE(a); i++)
        if (!memcmp(k, DARR_EPTR(a, i), DARR_ELEM(a))) return i;
    return -1;
}
DARRDEF int dynarrFindBinary (const void* a, int(*comp)(const void*, const void*), const void* k) {
    void* res = bsearch(k, DARR_EPTR(a, 0), DARR_SIZE(a), DARR_ELEM(a), comp);
    if (!res) return -1; //element not found
    return ((char*)res - DARR_EPTR(a, 0)) / DARR_ELEM(a);
}
DARRDEF void dynarrSortInsert (void* a, int(*comp)(const void*, const void*)) {
    for (int j = 1; j < DARR_SIZE(a); j++)
        for (int i = j; (i > 0)&&(comp(DARR_EPTR(a, i-1), DARR_EPTR(a, i)) > 0); i--) {
            char temp[DARR_ELEM(a)];
            memcpy(temp, DARR_EPTR(a, i), DARR_ELEM(a));
            memcpy(DARR_EPTR(a, i), DARR_EPTR(a, i-1), DARR_ELEM(a));
            memcpy(DARR_EPTR(a, i-1), temp, DARR_ELEM(a));
        }
}
DARRDEF void dynarrSortStandard (void* a, int(*comp)(const void*, const void*)) {
    qsort(DARR_EPTR(a, 0), DARR_SIZE(a), DARR_ELEM(a), comp);
}

#endif //DYNARR_IMPLEMENTATION