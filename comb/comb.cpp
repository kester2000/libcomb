#include "comb.h"

#include <algorithm>
#include <cstring>
#include <random>

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

void getCardList(int cardList[20][3], char *seedChar)
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