#include "binSearch.h"
int binSearch(const int s[], const int size, const int target) {
    int top = size - 1;
    int bottom = 0;
    int mid;
    mid = (top + bottom + 1) / 2;
    while (top > bottom) {
        if (s[mid] > target) {
            top = mid - 1;
        } else {
            bottom = mid;
        }
        mid = (top + bottom + 1) / 2;
    }
    return s[top] == target ? top : -1;
}