#include "comb.h"

#include <algorithm>
#include <random>

static const int NUM[3][3] = {
    {3, 4, 8},
    {1, 5, 9},
    {2, 6, 7},
};

void getCardList(int cardList[20][3], char *seedChar)
{
    std::string seedName(seedChar);
    int id[56];
    for (int i = 0; i < 56; i++)
        id[i] = i;
    std::seed_seq seed(seedName.begin(), seedName.end());
    std::mt19937 g(seed);
    shuffle(id, id + 56, g);
    int begin = 20;
    for (int i = 0; i < 20; i++)
    {
        if (id[i] >= 54)
        {
            begin = 0;
            break;
        }
    }
    for (int i = 0; i < 20; i++)
    {
        int tmp = id[i + begin] / 2;
        if (tmp == 27)
        {
            cardList[i][0] = 10;
            cardList[i][1] = 10;
            cardList[i][2] = 10;
        }
        else
        {
            cardList[i][0] = NUM[0][tmp / 9];
            cardList[i][1] = NUM[1][tmp / 3 % 3];
            cardList[i][2] = NUM[2][tmp % 3];
        }
    }
}

int getScore(int cardList[20][3])
{
    return 123;
}