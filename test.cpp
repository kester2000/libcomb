#include <iostream>

#include "comb/comb.h"

using namespace std;

int main()
{
    const char s[60] = "22073789477";
    int cardList[20][3], perm[20];
    int score = getMaxScore(cardList, perm, 0, 12);
    cout << score << endl;
    return 0;
}