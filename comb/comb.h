#pragma once
extern "C" {
void getCardList(int cardList[20][3], const char* seedChar);

int getScore(const int cardList[20][3]);
int getScoreByPerm(const int cardList[20][3], const int perm[20]);

int getMaxScore(const int cardList[20][3], int perm[20], int initScore, int threadCnt);

double getExpScore(const int cardList[20][3], const double* vars);
int getActionByCount(const int cardList[20][3], const int card[3], const double* vars, double expScores[20],
                     int tryCount, int tryThread);
int getActionByTime(const int cardList[20][3], const int card[3], const double* vars, double expScores[20], int trySecond,
                    int tryThread);
}