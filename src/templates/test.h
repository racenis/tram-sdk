// Tramway Drifting and Dungeon Exploration Simulator SDK Runtime

#ifndef TRAM_SDK_TEMPLATES_TEST_H
#define TRAM_SDK_TEMPLATES_TEST_H

#include <cstdio>
#include <cstring>
#include <chrono>

// This is the framwork for testing the library.
// It not actually part of library. But only test. I am testing.

struct _test_case {
    const char* fullname;
    const char* abbrv;
    
    int totl_asserts = 0;
    int fail_asserts = 0;
    
    static constexpr std::chrono::time_point<std::chrono::high_resolution_clock> INVALID_TIME = {};
    
    std::chrono::time_point<std::chrono::high_resolution_clock> times_starts[10];
    std::chrono::time_point<std::chrono::high_resolution_clock> times_ends[10];
    const char* times_names[10];
    int totl_times = 0;
    
    void timestart(const char* name) {
        times_starts[totl_times] = std::chrono::high_resolution_clock::now();
        times_ends[totl_times] = INVALID_TIME;
        times_names[totl_times] = name;
        
        totl_times++;
    }
    
    void timefinish(const char* name) {
        int idx = 0;
        while (idx < 10 && strcmp(times_names[idx], name) != 0) idx++;
        if (idx >= 10) {
            printf("Forgot to call TIME_START(\"%s\")!\n", name);
            return;
        }
        
        if (times_ends[idx] != INVALID_TIME) {
            printf("Called TIME_FINISH(\"%s\") twice!\n", name);
            return;
        }
        
        times_ends[idx] = std::chrono::high_resolution_clock::now();
    }
    
    void fail() {
        totl_asserts++;
        fail_asserts++;
    }
    
    void succ() {
        totl_asserts++;
    }
    
    void print() {
        int p = totl_asserts - fail_asserts;
        int t = totl_asserts;
        
        printf("Test %s %s (%i/%i)\n", fullname, abbrv, p, t);
        

        decltype(*times_ends - *times_starts) time_sum = {};
        for (int i = 0; i < totl_times; i++) {
            if (times_ends[i] == INVALID_TIME) {
                printf("Forgot to call TIME_FINISH(\"%s\") twice!\n", times_names[i]);
                continue;
            }
            auto time = times_ends[i] - times_starts[i];
            time_sum += time;
            auto print_time = std::chrono::duration_cast<std::chrono::milliseconds>(time).count();
            char pad[80] = "";
            int pads = 32 - strlen(times_names[i]);
            for (int j = 0; j < pads; j++) strcat(pad, " ");
            
            printf("- %s%s%ims\n", times_names[i], pad, print_time);
        }
        
        if (totl_times > 1) {
            auto print_time = std::chrono::duration_cast<std::chrono::milliseconds>(time_sum).count();
            printf("- Total                           %ims\n", print_time);
        }
        
    }
    
};

static _test_case _all_test_cases[100];
static int _test_case_count = 0;
static _test_case* _last_test_case = nullptr;

#define ASSERT(X) if (!(X)) { printf("Assert %s fail on line %i\n", #X, __LINE__); _last_test_case->fail(); } else { _last_test_case->succ(); }





#define TIME_START(NAME) \
    _last_test_case->timestart(NAME);
    
#define TIME_FINISH(NAME) \
    _last_test_case->timefinish(NAME);

#define TEST_CASE(X, Y) \
    _last_test_case = &_all_test_cases[_test_case_count]; \
    _test_case_count++; \
    \
    _last_test_case->fullname = X; \
    _last_test_case->abbrv = Y;
    

#define START_TEST \
    int main(int argc, char** argv) { \
        printf("Running all tests...\n");
        
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
            c->print(); \
        }\
        \
        return fail ? -1 : 0;\
    }


#endif  // TRAM_SDK_TEMPLATES_TEST_H