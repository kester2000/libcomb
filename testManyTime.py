from ctypes import *
import random

comb = cdll.LoadLibrary(".//libcomb.so")


scoreSum = 0
for time in range(10000):
    s = str(random.random())
    cardList = (c_int*60)()
    seed = (c_char_p)(s.encode())
    comb.getCardList(cardList, seed)

    board = (c_int*60)()
    card = (c_int*3)()
    exp = (c_double*20)()

    for j in range(20):
        for i in range(3):
            card[i] = cardList[j*3+i]
        for i in range(20):
            exp[i] = -1
        comb.getActionByTime(board, card, 0, exp, 5, 12)
        k = sorted(enumerate(exp), key=lambda x: x[1], reverse=True)
        # print(*k[:5], sep='\n')
        # act = int(input("where to place:"))
        act = k[0][0]
        for i in range(3):
            board[act*3+i] = card[i]
    s = comb.getScore(board)
    scoreSum += s
    print(f'no.{time+1} score {s}, avg {scoreSum/(time+1)}')
