#pragma once
extern "C" {
void getCardList(int cardList[20][3], char *seedChar);

int getScore(int cardList[20][3]);
int getScoreByPerm(int cardList[20][3], int perm[20]);

int getMaxScore(int cardList[20][3], int perm[20], int initScore, int threadCnt);
}