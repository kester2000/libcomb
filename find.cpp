#include <cstring>
#include <mutex>
#include <thread>
#include <vector>

#include "comb/comb.h"
#include "comb/rwlock.h"

using namespace std;
using ll = long long;

const char* filePath = nullptr;
const ll piece = 100000;

int maxScore = 0;
CRWMtx scoreMtx;

ll cursor = 0;
mutex cursorMtx;

void findThread()
{
    ll tmpSeed;
    int tmpScore;
    int cardList[20][3];
    int perm[20];
    char buf[64];
    while (true) {
        {
            lock_guard<mutex> _l(cursorMtx);
            tmpSeed = cursor;
            cursor += piece;
        }
        for (ll seed = tmpSeed; seed < tmpSeed + piece; seed++) {
            sprintf(buf, "1145140%lld", seed);
            getCardList(cardList, buf);
            int sum = 0;
            int cnt = 0;
            for (int i = 0; i < 20; i++) {
                for (int j = 0; j < 3; j++) {
                    sum += cardList[i][j];
                    cnt += cardList[i][j] == 10;
                }
            }
            if (cnt == 6) {
                sum -= 6;
            }
            {
                CReadLock _l(scoreMtx);
                tmpScore = maxScore;
            }
            if (sum < tmpScore) {
                continue;
            }
            int score = getMaxScore(cardList, perm, tmpScore, 1);
            if (tmpScore < score) {
                bool flag = false;
                {
                    CWriteLock _l(scoreMtx);
                    if (maxScore < score) {
                        maxScore = score;
                        flag = true;
                    }
                }
                if (flag) {
                    if (filePath) {
                        FILE* fp = fopen(filePath, "a");
                        fprintf(fp, "seed: %s %lld [", buf, maxScore);
                        for (int i = 0; i <= 18; i++) {
                            fprintf(fp, "%d, ", perm[i]);
                        }
                        fprintf(fp, "%d]\n", perm[19]);
                        fclose(fp);
                    } else {
                        printf("seed: %s %lld [", buf, score);
                        for (int i = 0; i <= 18; i++) {
                            printf("%d, ", perm[i]);
                        }
                        printf("%d]\n", perm[19]);
                    }
                }
            }
        }
    }
}

int main(int argc, char* argv[])
{
    int threadCnt = 12;
    for (int i = 1; i < argc;) {
        if (!strcmp(argv[i], "-t")) {
            threadCnt = atoi(argv[i + 1]);
            i += 2;
        } else if (!strcmp(argv[i], "-f")) {
            filePath = argv[i + 1];
            i += 2;
        } else {
            i++;
        }
    }
    vector<thread> threads;
    for (int i = 0; i < threadCnt; i++) {
        threads.emplace_back(thread(findThread));
    }
    for (int i = 0; i < threadCnt; i++) {
        threads[i].join();
    }
}