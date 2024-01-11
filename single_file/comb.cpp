/*
 * 拼装毁灭者V1.1
 * 作者：宽容<877367792@qq.com>
 * 日期：2023-10-17
 */

#include <algorithm>
#include <atomic>
#include <cstring>
#include <iostream>
#include <random>
#include <vector>

// 用种子获取所有牌
void getCardList(int cardList[20][3], const char *seedChar);

// 把 cardList 按 perm 填入的分数
int getScoreByPerm(const int cardList[20][3], const int perm[20]);

// 获得 cardList 的最大得分，返回到 perm 中。只关注比 initScore 大的答案（在多种子搜索最高分时有帮助）。
int getMaxScore(const int cardList[20][3], int perm[20], int initScore);

bool check(std::string s)
{
    return s.size() == 3 && (s[0] == '8' || s[0] == '4' || s[0] == '3') &&
           (s[1] == '9' || s[1] == '5' || s[1] == '1') && (s[2] == '7' || s[2] == '6' || s[2] == '2');
}

int main()
{
    std::string mode, buf;
    int cardList[20][3] = {0};
    int perm[20];
    std::cin >> mode;
    if (mode == "seed") {
        std::cin >> buf;
        getCardList(cardList, buf.c_str());
        std::cout << buf << std::endl;
    } else if (mode == "card") {
        for (int i = 0; i < 20; i++) {
            std::cin >> buf;
            if (buf == "any") {
                cardList[i][0] = 10;
                cardList[i][1] = 10;
                cardList[i][2] = 10;
            } else if (check(buf)) {
                cardList[i][0] = buf[0] - '0';
                cardList[i][1] = buf[1] - '0';
                cardList[i][2] = buf[2] - '0';
            } else {
                std::cout << buf << "不符合格式" << std::endl;
                return -1;
            }
        }
    } else {
        std::cout << "请先输入mode(seed/card)!" << std::endl;
        return -1;
    }
    for (int i = 0; i < 20; i++) {
        if (cardList[i][0] == 10) {
            std::cout << "any ";
        } else {
            std::cout << cardList[i][0] << cardList[i][1] << cardList[i][2] << ' ';
        }
    }
    std::cout << std::endl;
    int score = getMaxScore(cardList, perm, 0);
    std::cout << score << std::endl;
    for (int i = 0; i < 20; i++) {
        std::cout << perm[i] << ' ';
    }
    std::cout << std::endl;
    return 0;
}

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

// 帮助算分的序号
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

// 最优解储存在这个结构体内
typedef struct __CombContext {
    const int (*cardList)[3];
    int *perm;
    int maxScore;
} CombContext;

// 计算 癞子2 跳过非癞子20 下的所有块
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

// 把 cardList 按 0-19 填入的分数
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

// 把 cardList 按 perm 填入的分数
int getScoreByPerm(const int cardList[20][3], int perm[20])
{
    int board[20][3];
    for (int i = 0; i < 20; i++) {
        memcpy(board[perm[i]], cardList[i], 3 * sizeof(int));
    }
    return getScore(board);
}

// 计算最大可能分数，下标小于 nxt 的按 limit[i] 取分，下标大于等于 nxt 则按 8/9/7 取分，0号位按30分
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

// 按照val的要求，填写蜂巢盘中的编号为id的块。如果能够满足要求则返回true，否则返回false。
// 在上一层保证了复杂的要求一定比简单的要求先运行到本函数中。
// 如果多个都能满足要求，填入单块分数最小的那一个（所有都按这个规则填，一定会剩下最大的填入0号位）。
static bool setCard(const int cardList[20][3], int perm[20], int id, int val)
{
    // cardList[k] 为选中的块
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
            // flag为true即为cardList[i]满足要求
            if (flag) {
                // 未有选中或者本次比之前最大的还大，则修改k
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

// 选一块最大的填入0
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

// 递归求解
// limit 0-4 控制843的线，limit 5-9 控制951的线，limit 10-14 控制762的线
// 根据limit和nxt最大可能分数，下标小于 nxt 的线一定为 limit[i]，下标大于等于 nxt 则不作任何要求
// 如果前面的要求能够填完，则枚举 limit[nxt]（如果nxt是0-4，则按8430的顺序枚举，其他同理），
// 并调用getMaxByLimit(ctx,limit, nxt + 1)
static void getMaxByLimit(CombContext *ctx, int limit[15], int nxt)
{
    // 跳过一些对称的情况
    if ((nxt == 4 && limit[3] > limit[1]) || (nxt == 5 && limit[4] > limit[0])) {
        return;
    }
    int possibleScore = getPossibleScore(limit, nxt);
    // 如果最大可能分数都没之前的大，则直接返回
    // 换句话说就是你限制都是123这些歪瓜裂枣，没必要算
    if (possibleScore < ctx->maxScore) {
        return;
    }
    int id[20], limitVal[20], perm[20];
    for (int i = 0; i < 20; i++) {
        id[i] = i;
        limitVal[i] = 0;
        perm[i] = -1;
    }
    // 算一下1-19的格子分别被哪些值限制了
    // limitVal[1] = 798 代表1号位要填 897
    // limitVal[7] = 74  代表7号位要填 7?4
    // limitVal[9] = 5   代表9号位要填 ?5?
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
    // 根据限制值从大到小排序，使得setCard的顺序符合算法内置逻辑。
    std::sort(id, id + 20, [&limitVal](int l, int r) { return limitVal[l] > limitVal[r]; });
    int ii;
    // 有限制的先填
    for (ii = 0; ii < 20 && limitVal[id[ii]]; ii++) {
        if (!setCard(ctx->cardList, perm, id[ii], limitVal[id[ii]])) {
            return;
        }
    }
    // 填0号位
    setZero(ctx->cardList, perm);
    // 剩下随意填
    for (; ii < 20; ii++) {
        if (id[ii] != 0) {
            if (!setCard(ctx->cardList, perm, id[ii], limitVal[id[ii]])) {
                return;
            }
        }
    }
    // 算分
    int score = getScoreByPerm(ctx->cardList, perm);
    if (ctx->maxScore < score) {
        ctx->maxScore = score;
        memcpy(ctx->perm, perm, 20 * sizeof(int));
    }
    // 没到边界值则递归
    if (nxt < 15) {
        for (int i = 0; i < 4; i++) {
            limit[nxt] = NUM4[nxt / 5][i];
            getMaxByLimit(ctx, limit, nxt + 1);
        }
    }
}

// 初始化，并开始递归求解
int getMaxScore(const int cardList[20][3], int perm[20], int initScore)
{
    CombContext context;
    context.cardList = cardList;
    context.perm = perm;
    context.maxScore = initScore;
    int limit[15] = {0};
    getMaxByLimit(&context, limit, 0);
    return context.maxScore;
}