#include <beam/container/vec.h>
#include <beam/file.h>
#include <beam/log.h>

typedef Vec(int32_t) IntVec;
#define ABS(x) (x) >= 0 ? (x) : -(x)

int int32_compare(int32_t* x, int32_t* y) {
    return *x - *y;
}

size_t sol1(IntVec* v1, IntVec* v2) {
    VecSort(v1, &int32_compare);
    VecSort(v2, &int32_compare);

    size_t dist = 0;
    for(size_t s = 0; s < v1->length; s++) {
        dist += ABS(v1->data[s] - v2->data[s]);
    }

    return dist;
}

size_t sol2(IntVec* v1, IntVec* v2) {
    // we only need v2 to be sorted
    VecSort(v2, &int32_compare);

    size_t dist = 0;

    size_t i    = 0;
    int*   elem = NULL;
    VecForeachPtr(v1, elem, i) {
        int* found = bsearch(
            elem,
            v2->data,
            v2->length,
            sizeof(v2->data[0]),
            (GenericCompare)&int32_compare
        );

        if(found) {
            // since array is sorted all same values will be grouped together
            // go back to starting, because maybe the search found item in the middle of group
            while((found > v2->data) && found[-1] == *elem) {
                found--;
            }

            // dist += value * freq
            size_t freq = 0;
            while(*found == *elem) {
                dist += *elem;
                freq++;
                found++;
            }
            printf("%d as freq = %zu\n", *elem, freq);
        }
    }

    return dist;
}

int main() {
    LogInit(false);

    String file = {0};
    StringInit(&file);

    if(!ReadCompleteFile(
           "source/aoc/inputs/day1",
           (void*)&file.data,
           &file.length,
           &file.capacity
       )) {
        LOG_ERROR("failed to read complete file.");
        return EXIT_FAILURE;
    }

    IntVec v1 = {0}, v2 = {0};

    VecInit(&v1, NULL, NULL);
    VecInit(&v2, NULL, NULL);

    const char* iter = file.data;
    while(iter < file.data + file.length) {
        const char* col1_begin = iter;
        char*       col1_end   = NULL;
        VecPushBack(&v1, ((int32_t[]) {strtoul(col1_begin, &col1_end, 10)}));


        const char* col2_begin = col1_end;
        char*       col2_end   = NULL;
        VecPushBack(&v2, ((int32_t[]) {strtoul(col2_begin, &col2_end, 10)}));

        iter = col2_end + 1;
    }

    size_t dist = sol2(&v1, &v2);
    printf("dist = %zu\n", dist);

    VecDeinit(&v1);
    VecDeinit(&v2);
    StringDeinit(&file);

    return EXIT_FAILURE;
}
