// Tramway Drifting and Dungeon Exploration Simulator SDK Runtime


#ifndef TRAM_SDK_TEMPLATES_TEST_H
#define TRAM_SDK_TEMPLATES_TEST_H

#include <cstdio>
#include <cstring>

// This is the framwork for testing the library.
// It not actually part of library. But only test. I am testing.

struct _test_case {
    const char* fullname;
    const char* abbrv;
    
    int totl_asserts = 0;
    int fail_asserts = 0;
    
    void fail() {
        totl_asserts++;
        fail_asserts++;
    }
    
    void succ() {
        totl_asserts++;
    }
};

static _test_case _all_test_cases[100];
static int _test_case_count = 0;
static _test_case* _last_test_case = nullptr;

#define ASSERT(X) if (!(X)) { printf("Assert %s fail on line %i\n", #X, __LINE__); _last_test_case->fail(); } else { _last_test_case->succ(); }







#define TEST_CASE(X, Y) \
    _last_test_case = &_all_test_cases[_test_case_count]; \
    _test_case_count++; \
    \
    _last_test_case->fullname = X; \
    _last_test_case->abbrv = Y;
    

#define START_TEST \
    int main(int argc, char** argv) {
        
#define END_TEST \
    bool fail = false;\
\
printf("\nRESULTS:\n");\
\
    for (int i = 0; i < _test_case_count; i++) {\
        auto c = &_all_test_cases[i];\
        \
        if (c->fail_asserts > 0) {\
            fail = true;\
        }\
        \
        int p = c->totl_asserts - c->fail_asserts;\
        int t = c->totl_asserts;\
        \
        printf("Test %s %s (%i/%i)\n", c->fullname, c->abbrv, p, t);\
    }\
\
    return fail ? -1 : 0;\
}


#endif  // TRAM_SDK_TEMPLATES_TEST_H