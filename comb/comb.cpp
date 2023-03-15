#include "comb.h"

#include <algorithm>
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

static const int limit[544][5] = {
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

class LimitContainer {
public:
    LimitContainer() : cursor(0) {}

    bool getNext(int *dest)
    {
        std::lock_guard<std::mutex> _l(mtx);
        if (cursor >= 544) {
            return false;
        }
        memcpy(dest, limit[cursor++], 5 * sizeof(int));
        return true;
    }

    void clear()
    {
        std::lock_guard<std::mutex> _l(mtx);
        cursor = 0;
    }

private:
    int cursor;
    std::mutex mtx;
};

typedef struct __CombContext {
    int (*cardList)[3];
    int *perm;
    int maxScore;
    CRWMtx scoreMtx;
    LimitContainer lc;
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

int getScore(int cardList[20][3])
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

int getScoreByPerm(int cardList[20][3], int perm[20])
{
    int board[20][3];
    for (int i = 0; i < 20; i++) {
        memcpy(board[perm[i]], cardList[i], 3 * sizeof(int));
    }
    return getScore(board);
}

int getPossibleScore(int limit[15], int nxt)
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

bool setCard(int cardList[20][3], int perm[20], int id, int val)
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

void setZero(int cardList[20][3], int perm[20])
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

void getMaxByLimit(CombContext *ctx, int limit[15], int nxt)
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

void *getMaxScoreThread(void *args)
{
    CombContext *ctx = (CombContext *)args;
    int limit[15] = {0};
    while (ctx->lc.getNext(limit)) {
        getMaxByLimit(ctx, limit, 5);
        memset(limit, 0, sizeof(limit));
    }
    return NULL;
}

int getMaxScore(int cardList[20][3], int perm[20], int initScore, int threadCnt)
{
    CombContext context;
    context.cardList = cardList;
    context.perm = perm;
    context.maxScore = initScore;
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