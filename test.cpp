#include <iostream>

#include "comb/comb.h"

using namespace std;

int main(int argc, char* argv[])
{
    const char* seed = "宽容";
    int threadCnt = 1;
    int cardList[20][3], perm[20];
    if (argc >= 2) {
        seed = argv[1];
    }
    if (argc >= 3) {
        threadCnt = atoi(argv[2]);
    }
    getCardList(cardList, seed);
    for (int i = 0; i < 20; i++) {
        for (int j = 0; j < 3; j++) {
            cout << cardList[i][j] << ' ';
        }
        cout << endl;
    }
    int score = getMaxScore(cardList, perm, 0, threadCnt);
    cout << score << endl;
    for (int i = 0; i < 20; i++) {
        cout << perm[i] << ' ';
    }
    cout << endl;
    return 0;
}