#include "comb.h"

#include <algorithm>
#include <atomic>
#include <cstring>
#include <random>
#include <thread>
#include <vector>

#include "rwlock.h"

static const int NUM[3][3] = {
    {3, 4, 8},
    {1, 5, 9},
    {2, 6, 7},
};

static const int NUM4[3][4] = {
    {8, 4, 3, 0},
    {9, 5, 1, 0},
    {7, 6, 2, 0},
};

static const int LINES[3][5][5] = {
    {
        {8, 13, 17},
        {4, 9, 14, 18},
        {1, 5, 10, 15, 19},
        {2, 6, 11, 16},
        {3, 7, 12},
    },
    {
        {1, 2, 3},
        {4, 5, 6, 7},
        {8, 9, 10, 11, 12},
        {13, 14, 15, 16},
        {17, 18, 19},
    },
    {
        {1, 4, 8},
        {2, 5, 9, 13},
        {3, 6, 10, 14, 17},
        {7, 11, 15, 18},
        {12, 16, 19},
    },
};

static const int allLimit[544][5] = {
    {8, 8, 8, 8, 8}, {8, 8, 8, 8, 4}, {8, 8, 8, 8, 3}, {8, 8, 8, 8, 0}, {8, 8, 8, 4, 8}, {8, 8, 8, 4, 4},
    {8, 8, 8, 4, 3}, {8, 8, 8, 4, 0}, {8, 8, 8, 3, 8}, {8, 8, 8, 3, 4}, {8, 8, 8, 3, 3}, {8, 8, 8, 3, 0},
    {8, 8, 8, 0, 8}, {8, 8, 8, 0, 4}, {8, 8, 8, 0, 3}, {8, 8, 8, 0, 0}, {8, 8, 4, 8, 8}, {8, 8, 4, 8, 4},
    {8, 8, 4, 8, 3}, {8, 8, 4, 8, 0}, {8, 8, 4, 4, 8}, {8, 8, 4, 4, 4}, {8, 8, 4, 4, 3}, {8, 8, 4, 4, 0},
    {8, 8, 4, 3, 8}, {8, 8, 4, 3, 4}, {8, 8, 4, 3, 3}, {8, 8, 4, 3, 0}, {8, 8, 4, 0, 8}, {8, 8, 4, 0, 4},
    {8, 8, 4, 0, 3}, {8, 8, 4, 0, 0}, {8, 8, 3, 8, 8}, {8, 8, 3, 8, 4}, {8, 8, 3, 8, 3}, {8, 8, 3, 8, 0},
    {8, 8, 3, 4, 8}, {8, 8, 3, 4, 4}, {8, 8, 3, 4, 3}, {8, 8, 3, 4, 0}, {8, 8, 3, 3, 8}, {8, 8, 3, 3, 4},
    {8, 8, 3, 3, 3}, {8, 8, 3, 3, 0}, {8, 8, 3, 0, 8}, {8, 8, 3, 0, 4}, {8, 8, 3, 0, 3}, {8, 8, 3, 0, 0},
    {8, 8, 0, 8, 8}, {8, 8, 0, 8, 4}, {8, 8, 0, 8, 3}, {8, 8, 0, 8, 0}, {8, 8, 0, 4, 8}, {8, 8, 0, 4, 4},
    {8, 8, 0, 4, 3}, {8, 8, 0, 4, 0}, {8, 8, 0, 3, 8}, {8, 8, 0, 3, 4}, {8, 8, 0, 3, 3}, {8, 8, 0, 3, 0},
    {8, 8, 0, 0, 8}, {8, 8, 0, 0, 4}, {8, 8, 0, 0, 3}, {8, 8, 0, 0, 0}, {8, 4, 8, 8, 4}, {8, 4, 8, 8, 3},
    {8, 4, 8, 8, 0}, {8, 4, 8, 4, 8}, {8, 4, 8, 4, 4}, {8, 4, 8, 4, 3}, {8, 4, 8, 4, 0}, {8, 4, 8, 3, 8},
    {8, 4, 8, 3, 4}, {8, 4, 8, 3, 3}, {8, 4, 8, 3, 0}, {8, 4, 8, 0, 8}, {8, 4, 8, 0, 4}, {8, 4, 8, 0, 3},
    {8, 4, 8, 0, 0}, {8, 4, 4, 8, 4}, {8, 4, 4, 8, 3}, {8, 4, 4, 8, 0}, {8, 4, 4, 4, 8}, {8, 4, 4, 4, 4},
    {8, 4, 4, 4, 3}, {8, 4, 4, 4, 0}, {8, 4, 4, 3, 8}, {8, 4, 4, 3, 4}, {8, 4, 4, 3, 3}, {8, 4, 4, 3, 0},
    {8, 4, 4, 0, 8}, {8, 4, 4, 0, 4}, {8, 4, 4, 0, 3}, {8, 4, 4, 0, 0}, {8, 4, 3, 8, 4}, {8, 4, 3, 8, 3},
    {8, 4, 3, 8, 0}, {8, 4, 3, 4, 8}, {8, 4, 3, 4, 4}, {8, 4, 3, 4, 3}, {8, 4, 3, 4, 0}, {8, 4, 3, 3, 8},
    {8, 4, 3, 3, 4}, {8, 4, 3, 3, 3}, {8, 4, 3, 3, 0}, {8, 4, 3, 0, 8}, {8, 4, 3, 0, 4}, {8, 4, 3, 0, 3},
    {8, 4, 3, 0, 0}, {8, 4, 0, 8, 4}, {8, 4, 0, 8, 3}, {8, 4, 0, 8, 0}, {8, 4, 0, 4, 8}, {8, 4, 0, 4, 4},
    {8, 4, 0, 4, 3}, {8, 4, 0, 4, 0}, {8, 4, 0, 3, 8}, {8, 4, 0, 3, 4}, {8, 4, 0, 3, 3}, {8, 4, 0, 3, 0},
    {8, 4, 0, 0, 8}, {8, 4, 0, 0, 4}, {8, 4, 0, 0, 3}, {8, 4, 0, 0, 0}, {8, 3, 8, 8, 4}, {8, 3, 8, 8, 3},
    {8, 3, 8, 8, 0}, {8, 3, 8, 4, 4}, {8, 3, 8, 4, 3}, {8, 3, 8, 4, 0}, {8, 3, 8, 3, 8}, {8, 3, 8, 3, 4},
    {8, 3, 8, 3, 3}, {8, 3, 8, 3, 0}, {8, 3, 8, 0, 8}, {8, 3, 8, 0, 4}, {8, 3, 8, 0, 3}, {8, 3, 8, 0, 0},
    {8, 3, 4, 8, 4}, {8, 3, 4, 8, 3}, {8, 3, 4, 8, 0}, {8, 3, 4, 4, 4}, {8, 3, 4, 4, 3}, {8, 3, 4, 4, 0},
    {8, 3, 4, 3, 8}, {8, 3, 4, 3, 4}, {8, 3, 4, 3, 3}, {8, 3, 4, 3, 0}, {8, 3, 4, 0, 8}, {8, 3, 4, 0, 4},
    {8, 3, 4, 0, 3}, {8, 3, 4, 0, 0}, {8, 3, 3, 8, 4}, {8, 3, 3, 8, 3}, {8, 3, 3, 8, 0}, {8, 3, 3, 4, 4},
    {8, 3, 3, 4, 3}, {8, 3, 3, 4, 0}, {8, 3, 3, 3, 8}, {8, 3, 3, 3, 4}, {8, 3, 3, 3, 3}, {8, 3, 3, 3, 0},
    {8, 3, 3, 0, 8}, {8, 3, 3, 0, 4}, {8, 3, 3, 0, 3}, {8, 3, 3, 0, 0}, {8, 3, 0, 8, 4}, {8, 3, 0, 8, 3},
    {8, 3, 0, 8, 0}, {8, 3, 0, 4, 4}, {8, 3, 0, 4, 3}, {8, 3, 0, 4, 0}, {8, 3, 0, 3, 8}, {8, 3, 0, 3, 4},
    {8, 3, 0, 3, 3}, {8, 3, 0, 3, 0}, {8, 3, 0, 0, 8}, {8, 3, 0, 0, 4}, {8, 3, 0, 0, 3}, {8, 3, 0, 0, 0},
    {8, 0, 8, 8, 4}, {8, 0, 8, 8, 3}, {8, 0, 8, 8, 0}, {8, 0, 8, 4, 4}, {8, 0, 8, 4, 3}, {8, 0, 8, 4, 0},
    {8, 0, 8, 3, 4}, {8, 0, 8, 3, 3}, {8, 0, 8, 3, 0}, {8, 0, 8, 0, 8}, {8, 0, 8, 0, 4}, {8, 0, 8, 0, 3},
    {8, 0, 8, 0, 0}, {8, 0, 4, 8, 4}, {8, 0, 4, 8, 3}, {8, 0, 4, 8, 0}, {8, 0, 4, 4, 4}, {8, 0, 4, 4, 3},
    {8, 0, 4, 4, 0}, {8, 0, 4, 3, 4}, {8, 0, 4, 3, 3}, {8, 0, 4, 3, 0}, {8, 0, 4, 0, 8}, {8, 0, 4, 0, 4},
    {8, 0, 4, 0, 3}, {8, 0, 4, 0, 0}, {8, 0, 3, 8, 4}, {8, 0, 3, 8, 3}, {8, 0, 3, 8, 0}, {8, 0, 3, 4, 4},
    {8, 0, 3, 4, 3}, {8, 0, 3, 4, 0}, {8, 0, 3, 3, 4}, {8, 0, 3, 3, 3}, {8, 0, 3, 3, 0}, {8, 0, 3, 0, 8},
    {8, 0, 3, 0, 4}, {8, 0, 3, 0, 3}, {8, 0, 3, 0, 0}, {8, 0, 0, 8, 4}, {8, 0, 0, 8, 3}, {8, 0, 0, 8, 0},
    {8, 0, 0, 4, 4}, {8, 0, 0, 4, 3}, {8, 0, 0, 4, 0}, {8, 0, 0, 3, 4}, {8, 0, 0, 3, 3}, {8, 0, 0, 3, 0},
    {8, 0, 0, 0, 8}, {8, 0, 0, 0, 4}, {8, 0, 0, 0, 3}, {8, 0, 0, 0, 0}, {4, 8, 8, 8, 4}, {4, 8, 8, 8, 3},
    {4, 8, 8, 8, 0}, {4, 8, 8, 4, 4}, {4, 8, 8, 4, 3}, {4, 8, 8, 4, 0}, {4, 8, 8, 3, 4}, {4, 8, 8, 3, 3},
    {4, 8, 8, 3, 0}, {4, 8, 8, 0, 4}, {4, 8, 8, 0, 3}, {4, 8, 8, 0, 0}, {4, 8, 4, 8, 4}, {4, 8, 4, 8, 3},
    {4, 8, 4, 8, 0}, {4, 8, 4, 4, 4}, {4, 8, 4, 4, 3}, {4, 8, 4, 4, 0}, {4, 8, 4, 3, 4}, {4, 8, 4, 3, 3},
    {4, 8, 4, 3, 0}, {4, 8, 4, 0, 4}, {4, 8, 4, 0, 3}, {4, 8, 4, 0, 0}, {4, 8, 3, 8, 4}, {4, 8, 3, 8, 3},
    {4, 8, 3, 8, 0}, {4, 8, 3, 4, 4}, {4, 8, 3, 4, 3}, {4, 8, 3, 4, 0}, {4, 8, 3, 3, 4}, {4, 8, 3, 3, 3},
    {4, 8, 3, 3, 0}, {4, 8, 3, 0, 4}, {4, 8, 3, 0, 3}, {4, 8, 3, 0, 0}, {4, 8, 0, 8, 4}, {4, 8, 0, 8, 3},
    {4, 8, 0, 8, 0}, {4, 8, 0, 4, 4}, {4, 8, 0, 4, 3}, {4, 8, 0, 4, 0}, {4, 8, 0, 3, 4}, {4, 8, 0, 3, 3},
    {4, 8, 0, 3, 0}, {4, 8, 0, 0, 4}, {4, 8, 0, 0, 3}, {4, 8, 0, 0, 0}, {4, 4, 8, 8, 3}, {4, 4, 8, 8, 0},
    {4, 4, 8, 4, 4}, {4, 4, 8, 4, 3}, {4, 4, 8, 4, 0}, {4, 4, 8, 3, 4}, {4, 4, 8, 3, 3}, {4, 4, 8, 3, 0},
    {4, 4, 8, 0, 4}, {4, 4, 8, 0, 3}, {4, 4, 8, 0, 0}, {4, 4, 4, 8, 3}, {4, 4, 4, 8, 0}, {4, 4, 4, 4, 4},
    {4, 4, 4, 4, 3}, {4, 4, 4, 4, 0}, {4, 4, 4, 3, 4}, {4, 4, 4, 3, 3}, {4, 4, 4, 3, 0}, {4, 4, 4, 0, 4},
    {4, 4, 4, 0, 3}, {4, 4, 4, 0, 0}, {4, 4, 3, 8, 3}, {4, 4, 3, 8, 0}, {4, 4, 3, 4, 4}, {4, 4, 3, 4, 3},
    {4, 4, 3, 4, 0}, {4, 4, 3, 3, 4}, {4, 4, 3, 3, 3}, {4, 4, 3, 3, 0}, {4, 4, 3, 0, 4}, {4, 4, 3, 0, 3},
    {4, 4, 3, 0, 0}, {4, 4, 0, 8, 3}, {4, 4, 0, 8, 0}, {4, 4, 0, 4, 4}, {4, 4, 0, 4, 3}, {4, 4, 0, 4, 0},
    {4, 4, 0, 3, 4}, {4, 4, 0, 3, 3}, {4, 4, 0, 3, 0}, {4, 4, 0, 0, 4}, {4, 4, 0, 0, 3}, {4, 4, 0, 0, 0},
    {4, 3, 8, 8, 3}, {4, 3, 8, 8, 0}, {4, 3, 8, 4, 3}, {4, 3, 8, 4, 0}, {4, 3, 8, 3, 4}, {4, 3, 8, 3, 3},
    {4, 3, 8, 3, 0}, {4, 3, 8, 0, 4}, {4, 3, 8, 0, 3}, {4, 3, 8, 0, 0}, {4, 3, 4, 8, 3}, {4, 3, 4, 8, 0},
    {4, 3, 4, 4, 3}, {4, 3, 4, 4, 0}, {4, 3, 4, 3, 4}, {4, 3, 4, 3, 3}, {4, 3, 4, 3, 0}, {4, 3, 4, 0, 4},
    {4, 3, 4, 0, 3}, {4, 3, 4, 0, 0}, {4, 3, 3, 8, 3}, {4, 3, 3, 8, 0}, {4, 3, 3, 4, 3}, {4, 3, 3, 4, 0},
    {4, 3, 3, 3, 4}, {4, 3, 3, 3, 3}, {4, 3, 3, 3, 0}, {4, 3, 3, 0, 4}, {4, 3, 3, 0, 3}, {4, 3, 3, 0, 0},
    {4, 3, 0, 8, 3}, {4, 3, 0, 8, 0}, {4, 3, 0, 4, 3}, {4, 3, 0, 4, 0}, {4, 3, 0, 3, 4}, {4, 3, 0, 3, 3},
    {4, 3, 0, 3, 0}, {4, 3, 0, 0, 4}, {4, 3, 0, 0, 3}, {4, 3, 0, 0, 0}, {4, 0, 8, 8, 3}, {4, 0, 8, 8, 0},
    {4, 0, 8, 4, 3}, {4, 0, 8, 4, 0}, {4, 0, 8, 3, 3}, {4, 0, 8, 3, 0}, {4, 0, 8, 0, 4}, {4, 0, 8, 0, 3},
    {4, 0, 8, 0, 0}, {4, 0, 4, 8, 3}, {4, 0, 4, 8, 0}, {4, 0, 4, 4, 3}, {4, 0, 4, 4, 0}, {4, 0, 4, 3, 3},
    {4, 0, 4, 3, 0}, {4, 0, 4, 0, 4}, {4, 0, 4, 0, 3}, {4, 0, 4, 0, 0}, {4, 0, 3, 8, 3}, {4, 0, 3, 8, 0},
    {4, 0, 3, 4, 3}, {4, 0, 3, 4, 0}, {4, 0, 3, 3, 3}, {4, 0, 3, 3, 0}, {4, 0, 3, 0, 4}, {4, 0, 3, 0, 3},
    {4, 0, 3, 0, 0}, {4, 0, 0, 8, 3}, {4, 0, 0, 8, 0}, {4, 0, 0, 4, 3}, {4, 0, 0, 4, 0}, {4, 0, 0, 3, 3},
    {4, 0, 0, 3, 0}, {4, 0, 0, 0, 4}, {4, 0, 0, 0, 3}, {4, 0, 0, 0, 0}, {3, 8, 8, 8, 3}, {3, 8, 8, 8, 0},
    {3, 8, 8, 4, 3}, {3, 8, 8, 4, 0}, {3, 8, 8, 3, 3}, {3, 8, 8, 3, 0}, {3, 8, 8, 0, 3}, {3, 8, 8, 0, 0},
    {3, 8, 4, 8, 3}, {3, 8, 4, 8, 0}, {3, 8, 4, 4, 3}, {3, 8, 4, 4, 0}, {3, 8, 4, 3, 3}, {3, 8, 4, 3, 0},
    {3, 8, 4, 0, 3}, {3, 8, 4, 0, 0}, {3, 8, 3, 8, 3}, {3, 8, 3, 8, 0}, {3, 8, 3, 4, 3}, {3, 8, 3, 4, 0},
    {3, 8, 3, 3, 3}, {3, 8, 3, 3, 0}, {3, 8, 3, 0, 3}, {3, 8, 3, 0, 0}, {3, 8, 0, 8, 3}, {3, 8, 0, 8, 0},
    {3, 8, 0, 4, 3}, {3, 8, 0, 4, 0}, {3, 8, 0, 3, 3}, {3, 8, 0, 3, 0}, {3, 8, 0, 0, 3}, {3, 8, 0, 0, 0},
    {3, 4, 8, 8, 0}, {3, 4, 8, 4, 3}, {3, 4, 8, 4, 0}, {3, 4, 8, 3, 3}, {3, 4, 8, 3, 0}, {3, 4, 8, 0, 3},
    {3, 4, 8, 0, 0}, {3, 4, 4, 8, 0}, {3, 4, 4, 4, 3}, {3, 4, 4, 4, 0}, {3, 4, 4, 3, 3}, {3, 4, 4, 3, 0},
    {3, 4, 4, 0, 3}, {3, 4, 4, 0, 0}, {3, 4, 3, 8, 0}, {3, 4, 3, 4, 3}, {3, 4, 3, 4, 0}, {3, 4, 3, 3, 3},
    {3, 4, 3, 3, 0}, {3, 4, 3, 0, 3}, {3, 4, 3, 0, 0}, {3, 4, 0, 8, 0}, {3, 4, 0, 4, 3}, {3, 4, 0, 4, 0},
    {3, 4, 0, 3, 3}, {3, 4, 0, 3, 0}, {3, 4, 0, 0, 3}, {3, 4, 0, 0, 0}, {3, 3, 8, 8, 0}, {3, 3, 8, 4, 0},
    {3, 3, 8, 3, 3}, {3, 3, 8, 3, 0}, {3, 3, 8, 0, 3}, {3, 3, 8, 0, 0}, {3, 3, 4, 8, 0}, {3, 3, 4, 4, 0},
    {3, 3, 4, 3, 3}, {3, 3, 4, 3, 0}, {3, 3, 4, 0, 3}, {3, 3, 4, 0, 0}, {3, 3, 3, 8, 0}, {3, 3, 3, 4, 0},
    {3, 3, 3, 3, 3}, {3, 3, 3, 3, 0}, {3, 3, 3, 0, 3}, {3, 3, 3, 0, 0}, {3, 3, 0, 8, 0}, {3, 3, 0, 4, 0},
    {3, 3, 0, 3, 3}, {3, 3, 0, 3, 0}, {3, 3, 0, 0, 3}, {3, 3, 0, 0, 0}, {3, 0, 8, 8, 0}, {3, 0, 8, 4, 0},
    {3, 0, 8, 3, 0}, {3, 0, 8, 0, 3}, {3, 0, 8, 0, 0}, {3, 0, 4, 8, 0}, {3, 0, 4, 4, 0}, {3, 0, 4, 3, 0},
    {3, 0, 4, 0, 3}, {3, 0, 4, 0, 0}, {3, 0, 3, 8, 0}, {3, 0, 3, 4, 0}, {3, 0, 3, 3, 0}, {3, 0, 3, 0, 3},
    {3, 0, 3, 0, 0}, {3, 0, 0, 8, 0}, {3, 0, 0, 4, 0}, {3, 0, 0, 3, 0}, {3, 0, 0, 0, 3}, {3, 0, 0, 0, 0},
    {0, 8, 8, 8, 0}, {0, 8, 8, 4, 0}, {0, 8, 8, 3, 0}, {0, 8, 8, 0, 0}, {0, 8, 4, 8, 0}, {0, 8, 4, 4, 0},
    {0, 8, 4, 3, 0}, {0, 8, 4, 0, 0}, {0, 8, 3, 8, 0}, {0, 8, 3, 4, 0}, {0, 8, 3, 3, 0}, {0, 8, 3, 0, 0},
    {0, 8, 0, 8, 0}, {0, 8, 0, 4, 0}, {0, 8, 0, 3, 0}, {0, 8, 0, 0, 0}, {0, 4, 8, 4, 0}, {0, 4, 8, 3, 0},
    {0, 4, 8, 0, 0}, {0, 4, 4, 4, 0}, {0, 4, 4, 3, 0}, {0, 4, 4, 0, 0}, {0, 4, 3, 4, 0}, {0, 4, 3, 3, 0},
    {0, 4, 3, 0, 0}, {0, 4, 0, 4, 0}, {0, 4, 0, 3, 0}, {0, 4, 0, 0, 0}, {0, 3, 8, 3, 0}, {0, 3, 8, 0, 0},
    {0, 3, 4, 3, 0}, {0, 3, 4, 0, 0}, {0, 3, 3, 3, 0}, {0, 3, 3, 0, 0}, {0, 3, 0, 3, 0}, {0, 3, 0, 0, 0},
    {0, 0, 8, 0, 0}, {0, 0, 4, 0, 0}, {0, 0, 3, 0, 0}, {0, 0, 0, 0, 0},
};

static const double VARS[11] = {1.00, 0.721, 0.3993, 0.1947, 0.069, 0.0312, 0.75, 0.03, 0.08465, 0.08164, 18};

typedef struct __CombContext {
    const int (*cardList)[3];
    int *perm;
    int maxScore;
    CRWMtx scoreMtx;
    int cursor;
    std::mutex cursorMtx;
} CombContext;

void getCardList(int cardList[20][3], const char *seedChar)
{
    std::string seedName(seedChar);
    int id[56];
    for (int i = 0; i < 56; i++) id[i] = i;
    std::seed_seq seed(seedName.begin(), seedName.end());
    std::mt19937 g(seed);
    shuffle(id, id + 56, g);
    int begin = 20;
    for (int i = 0; i < 20; i++) {
        if (id[i] >= 54) {
            begin = 0;
            break;
        }
    }
    for (int i = 0; i < 20; i++) {
        int tmp = id[i + begin] / 2;
        if (tmp == 27) {
            cardList[i][0] = 10;
            cardList[i][1] = 10;
            cardList[i][2] = 10;
        } else {
            cardList[i][0] = NUM[0][tmp / 9];
            cardList[i][1] = NUM[1][tmp / 3 % 3];
            cardList[i][2] = NUM[2][tmp % 3];
        }
    }
}

int getScore(const int cardList[20][3])
{
    int score = 0;
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 5; j++) {
            int len = 5 - abs(j - 2);
            int now = 10;
            for (int k = 0; k < len; k++) {
                int num = cardList[LINES[i][j][k]][i];
                if (num != now && num != 10) {
                    if (now == 10) {
                        now = num;
                    } else {
                        now = 0;
                        break;
                    }
                }
            }
            score += now * len;
        }
    }
    for (int i = 0; i < 3; i++) {
        score += cardList[0][i];
    }
    return score;
}

int getScoreByPerm(const int cardList[20][3], int perm[20])
{
    int board[20][3];
    for (int i = 0; i < 20; i++) {
        memcpy(board[perm[i]], cardList[i], 3 * sizeof(int));
    }
    return getScore(board);
}

static int getPossibleScore(const int limit[15], int nxt)
{
    int score = 0;
    for (int i = 0; i < 15; i++) {
        int c = 5 - abs(i % 5 - 2);
        int val;
        if (i < nxt) {
            val = limit[i];
        } else {
            val = (i / 5 == 0 ? 8 : (i / 5 == 1 ? 9 : 7));
        }
        score += val * c;
    }
    score += 30;
    return score;
}

static bool setCard(const int cardList[20][3], int perm[20], int id, int val)
{
    int k = -1;
    for (int i = 0; i < 20; i++) {
        if (perm[i] == -1) {
            bool flag = true;
            int tmp = val;
            if (cardList[i][0] != 10) {
                while (flag && tmp) {
                    if (cardList[i][0] != tmp % 10 && cardList[i][1] != tmp % 10 && cardList[i][2] != tmp % 10) {
                        flag = false;
                    }
                    tmp /= 10;
                }
            }
            if (flag) {
                if (k == -1 || cardList[k][0] + cardList[k][1] + cardList[k][2] >
                                   cardList[i][0] + cardList[i][1] + cardList[i][2]) {
                    k = i;
                }
            }
        }
    }
    if (k == -1) {
        return false;
    }
    perm[k] = id;
    return true;
}

static void setZero(const int cardList[20][3], int perm[20])
{
    int k = -1;
    for (int i = 0; i < 20; i++) {
        if (perm[i] == -1) {
            if (k == -1 ||
                cardList[k][0] + cardList[k][1] + cardList[k][2] < cardList[i][0] + cardList[i][1] + cardList[i][2]) {
                k = i;
            }
        }
    }
    perm[k] = 0;
}

static void getMaxByLimit(CombContext *ctx, int limit[15], int nxt)
{
    int possibleScore = getPossibleScore(limit, nxt);
    {
        CReadLock _l(ctx->scoreMtx);
        if (possibleScore < ctx->maxScore) {
            return;
        }
    }
    int id[20], limitVal[20], perm[20];
    for (int i = 0; i < 20; i++) {
        id[i] = i;
        limitVal[i] = 0;
        perm[i] = -1;
    }
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 5; j++) {
            if (limit[i * 5 + j]) {
                int len = 5 - abs(j - 2);
                for (int k = 0; k < len; k++) {
                    limitVal[LINES[i][j][k]] = limitVal[LINES[i][j][k]] * 10 + limit[i * 5 + j];
                }
            }
        }
    }
    std::sort(id, id + 20, [&limitVal](int l, int r) { return limitVal[l] > limitVal[r]; });
    int ii;
    for (ii = 0; ii < 20 && limitVal[id[ii]]; ii++) {
        if (!setCard(ctx->cardList, perm, id[ii], limitVal[id[ii]])) {
            return;
        }
    }
    setZero(ctx->cardList, perm);
    for (; ii < 20; ii++) {
        if (id[ii] != 0) {
            if (!setCard(ctx->cardList, perm, id[ii], limitVal[id[ii]])) {
                return;
            }
        }
    }
    int score = getScoreByPerm(ctx->cardList, perm);
    bool flag;
    {
        CReadLock _l(ctx->scoreMtx);
        flag = ctx->maxScore < score;
    }
    if (flag) {
        CWriteLock _l(ctx->scoreMtx);
        if (ctx->maxScore < score) {
            ctx->maxScore = score;
            memcpy(ctx->perm, perm, 20 * sizeof(int));
        }
    }
    if (nxt < 15) {
        for (int i = 0; i < 4; i++) {
            limit[nxt] = NUM4[nxt / 5][i];
            getMaxByLimit(ctx, limit, nxt + 1);
        }
    }
}

static void *getMaxScoreThread(void *args)
{
    CombContext *ctx = (CombContext *)args;
    int limit[15] = {0};
    while (true) {
        {
            std::lock_guard<std::mutex> _l(ctx->cursorMtx);
            if (ctx->cursor >= 544) {
                break;
            }
            memcpy(limit, allLimit[ctx->cursor++], 5 * sizeof(int));
        }
        getMaxByLimit(ctx, limit, 5);
        memset(limit, 0, sizeof(limit));
    }
    return NULL;
}

int getMaxScore(const int cardList[20][3], int perm[20], int initScore, int threadCnt)
{
    CombContext context;
    context.cardList = cardList;
    context.perm = perm;
    context.maxScore = initScore;
    context.cursor = 0;
    if (threadCnt == 1) {
        getMaxScoreThread(&context);
    } else {
        std::vector<std::thread> threads;
        for (int i = 0; i < threadCnt; i++) {
            threads.emplace_back(std::thread(getMaxScoreThread, &context));
        }
        for (int i = 0; i < threadCnt; i++) {
            threads[i].join();
        }
    }
    return context.maxScore;
}

enum Status {
    EMPTY,
    PARTIAL,
    FULL,
    BROKEN,
};

double getExpScore(const int cardList[20][3], const double *vars)
{
    // copy from https://github.com/jeffxzy/NumcombSolver
    if (!vars) {
        vars = VARS;
    }
    double ret = 0, waiting[10] = {0}, decide[20][2] = {0};
    int blockCount = 0, lastNum = 0, desired[10] = {0}, needs[10] = {0};
    Status status;
    int length, score, num, filled;
    double scale, times, lastScore = 0;

    // 当前已经放下几块
    for (int i = 0; i < 20; i++)
        if (cardList[i][0]) blockCount++;

    // 计算0方块的分数
    if (cardList[0][0] == 0) {
        ret += vars[10];
    } else {
        ret += cardList[0][0] + cardList[0][1] + cardList[0][2];
    }

    // 对于每一行
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 5; j++) {
            // 原代码中的rowStatus
            length = 5 - abs(j - 2);
            score = 0;
            num = 0;
            filled = 0;

            bool flag = true;
            for (int k = 0; k < length && flag; k++) {
                int now = cardList[LINES[i][j][k]][i];
                if (now != 0) filled++;
                if (now != 0 && now != 10) {
                    if (num != 0 && num != now) {
                        status = BROKEN;
                        score = 0;
                        flag = false;
                    } else {
                        num = now;
                    }
                }
            }
            if (flag) {
                if (filled == length) {
                    status = FULL;
                    score = num * length;
                } else if (filled == 0) {
                    status = EMPTY;
                    score = 10 * length;
                } else {
                    status = PARTIAL;
                    if (num == 0) {
                        num = 10;
                    }
                    score = num * length;
                }
            }

            scale = vars[length - filled];

            ret += scale * score;

            // 越后期，已连通的价值越高。
            if (status != FULL) ret -= scale * (1 - pow(0.993, blockCount)) * score;

            // 尽可能使得游戏开局没有相邻元素，变量var[7]。
            if (blockCount < 10) {
                if (status == PARTIAL) {
                    if (num == lastNum && num != 0 && num != 10) {
                        ret -= lastScore * vars[7];
                        ret -= sqrt(lastNum / 2);
                    }
                    lastNum = num;
                    lastScore = score;
                } else {
                    lastNum = 0;
                    lastScore = 0;
                }
                // 尽可能使得游戏开局不破坏行。
                if (status == BROKEN) {
                    ret -= sqrt(num);
                }
            }
            // 尽可能使得游戏开局不一次开太多行，变量var[6]。
            if (num != 0 && num != 10 && status == PARTIAL) {
                desired[num] += length - filled;
                waiting[num] += scale * score;
            }
            // 降低交错点的期望得分，变量var[8], var[9]。
            if (status == PARTIAL) {
                for (int k = 0; k < length; k++) {
                    if (cardList[LINES[i][j][k]][0] == 0) {
                        decide[LINES[i][j][k]][0] += 1;
                        decide[LINES[i][j][k]][1] += scale * score;
                    }
                }
            }
            // 计算每个数字有多少行
            if (num != 0 && num != 10) {
                needs[num]++;
            }
        }
    }
    // 降低多排得分比例
    for (int i = 1; i <= 9; i++) {
        if (!(desired[i] < 5 || needs[i] < 3)) {
            ret -= pow(desired[i] * vars[6] / 10, 2) * waiting[i];
        }
    }
    // 降低交点牌得分概率
    scale = pow(blockCount / 20.0, 2);
    times = 0.4;
    for (int i = 0; i < 20; i++) {
        if (cardList[i][0] == 10) {
            times = 1;
            break;
        }
    }
    scale *= times;
    for (int i = 0; i < 20; i++) {
        if (abs(decide[i][0] - 2) < 1e-3) {
            ret -= scale * vars[8] * decide[i][1];
        } else if (abs(decide[i][0] - 3) < 1e-3) {
            ret -= scale * vars[9] * decide[i][1];
        }
    }
    return ret;
}

int getSimpleAction(const int cardList[20][3], const int card[3], const double *vars)
{
    int mCardList[20][3];
    int act = -1;
    double score = 0;
    double tmp;
    for (int i = 0; i < 20; i++) {
        if (cardList[i][0] == 0) {
            memcpy(mCardList, cardList, 60 * sizeof(int));
            memcpy(mCardList[i], card, 3 * sizeof(int));
            tmp = getExpScore(mCardList, vars);
            if (tmp > score) {
                act = i;
                score = tmp;
            }
        }
    }
    return act;
}

static void getNextCard(int restCnt, int wildCnt, const int vis[27], int nextCard[3])
{
    int n = 20 - restCnt;
    if (wildCnt == 0) {
        if (rand() % (n * n - 91 * n + 1540) < (90 - 2 * n)) {
            nextCard[0] = 10;
            nextCard[1] = 10;
            nextCard[2] = 10;
            return;
        }
    } else if (wildCnt == 1) {
        if (rand() % (46 - n) < 1) {
            nextCard[0] = 10;
            nextCard[1] = 10;
            nextCard[2] = 10;
            return;
        }
    };
    int val[54] = {0}, top = 0;
    for (int i = 0; i < 27; i++) {
        for (int k = vis[i]; k < 2; k++) {
            val[top++] = i;
        }
    }
    int v = val[rand() % top];
    nextCard[0] = NUM[0][v / 9];
    nextCard[1] = NUM[1][v / 3 % 3];
    nextCard[2] = NUM[2][v % 3];
}

static int getActionScoreOnce(const int cardList[20][3], const int card[3], int action, const double *vars)
{
    int mCardList[20][3], nextCard[3];
    memcpy(mCardList, cardList, 60 * sizeof(int));
    memcpy(mCardList[action], card, 3 * sizeof(int));
    int vis[27] = {0};
    int restCnt = 0, wildCnt = 0;
    for (int i = 0; i < 20; i++) {
        if (mCardList[i][0] == 0) {
            restCnt++;
        } else if (mCardList[i][0] == 10) {
            wildCnt++;
        } else {
            vis[(mCardList[i][0] - 1) / 3 * 9 + (mCardList[i][1] - 1) / 3 * 3 + (mCardList[i][2] - 1) / 3]++;
        }
    }
    while (restCnt) {
        getNextCard(restCnt, wildCnt, vis, nextCard);
        restCnt--;
        if (nextCard[0] == 10) {
            wildCnt++;
        } else {
            vis[(nextCard[0] - 1) / 3 * 9 + (nextCard[1] - 1) / 3 * 3 + (nextCard[2] - 1) / 3]++;
        }
        action = getSimpleAction(mCardList, nextCard, vars);
        memcpy(mCardList[action], nextCard, 3 * sizeof(int));
    }

    return getScore(mCardList);
}

typedef struct __ExpContext {
    std::atomic_int64_t scoreSum[20];
    std::atomic_int32_t scoreCnt[20];

    const int (*cardList)[3];
    const int *card;
    const double *vars;

    int mode;  // 0 is count, 1 is time;
    std::atomic_int cursor;
    int cnt;
    time_t startTime;
    int second;
} ExpContext;

static void *getExpScoreThread(void *arg)
{
    ExpContext *context = (ExpContext *)arg;
    int score;
    while (1) {
        if (context->mode == 0) {
            int now = context->cursor++;
            if (now >= context->cnt) {
                break;
            }
        } else {
            if (time(0) - context->startTime > context->second) {
                break;
            }
        }
        for (int i = 0; i < 20; i++) {
            if (context->cardList[i][0] == 0) {
                score = getActionScoreOnce(context->cardList, context->card, i, context->vars);
                context->scoreCnt[i]++;
                context->scoreSum[i] += score;
            }
        }
    }
    return 0;
}

int getActionByCount(const int cardList[20][3], const int card[3], const double *vars, double expScores[20],
                     int tryCount, int tryThread)
{
    ExpContext context;
    memset(&context, 0, sizeof(context));
    context.cardList = cardList;
    context.card = card;
    context.vars = vars;
    context.mode = 0;
    context.cnt = tryCount;
    context.cursor = 0;
    if (tryThread == 1) {
        getExpScoreThread(&context);
    } else {
        std::vector<std::thread> threads;
        for (int i = 0; i < tryThread; i++) {
            threads.emplace_back(std::thread(getExpScoreThread, &context));
        }
        for (int i = 0; i < tryThread; i++) {
            threads[i].join();
        }
    }

    int act = -1;
    double score = 0;
    for (int i = 0; i < 20; i++) {
        if (cardList[i][0] == 0) {
            expScores[i] = 1.0 * context.scoreSum[i] / context.scoreCnt[i];
            if (expScores[i] > score) {
                act = i;
                score = expScores[i];
            }
        }
    }
    return act;
}

int getActionByTime(const int cardList[20][3], const int card[3], const double *vars, double expScores[20],
                    int trySecond, int tryThread)
{
    ExpContext context;
    memset(&context, 0, sizeof(context));
    context.cardList = cardList;
    context.card = card;
    context.vars = vars;
    context.mode = 1;
    context.startTime = time(0);
    context.second = trySecond;
    if (tryThread == 1) {
        getExpScoreThread(&context);
    } else {
        std::vector<std::thread> threads;
        for (int i = 0; i < tryThread; i++) {
            threads.emplace_back(std::thread(getExpScoreThread, &context));
        }
        for (int i = 0; i < tryThread; i++) {
            threads[i].join();
        }
    }

    int act = -1;
    double score = 0;
    for (int i = 0; i < 20; i++) {
        if (cardList[i][0] == 0) {
            expScores[i] = 1.0 * context.scoreSum[i] / context.scoreCnt[i];
            if (expScores[i] > score) {
                act = i;
                score = expScores[i];
            }
        }
    }
    return act;
}