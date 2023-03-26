#include <cstring>
#include <iostream>
#include <string>
#include <ctime>

#include "comb/comb.h"
using namespace std;

int main(int argc, char* argv[])
{
    int cardList[20][3], board[20][3];
    long long sum = 0;
    int cnt = 0;
    string tt = to_string(time(0));
    for (int i = 0; i < 10000; i++) {
        string s = tt + to_string(i);
        getCardList(cardList, s.c_str());
        memset(board, 0, 60 * sizeof(int));
        for (int j = 0; j < 20; j++) {
            int action = getSimpleAction(board, cardList[j], 0);
            memcpy(board[action], cardList[j], 3 * sizeof(int));
        }
        int score = getScore(board);
        sum += score;
        cnt++;
    }
    cout << cnt << ' ' << sum * 1.0 / cnt << endl;
    // const int card[3] = {8, 5, 7};
    // for (int i = 0; i < 20; i++) {
    //     memset(cardList, 0, sizeof(cardList));
    //     memcpy(cardList[i], card, sizeof(card));
    //     cout << i << ' ' << getExpScore(cardList, 0) << endl;
    // }
    // getCardList(cardList, "宽容");
    // memset(board, 0, sizeof(board));
    // for (int i = 0; i < 20; i++) {
    //     int act = getSimpleAction(board, cardList[i], 0);
    //     memcpy(board[act], cardList[i], 3 * sizeof(int));
    //     cout << i << ' ' << cardList[i][0] << ' ' << cardList[i][1] << ' ' << cardList[i][2] << ' ' << act << endl;
    //     break;
    // }
    return 0;
}