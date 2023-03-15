from ctypes import *

comb = cdll.LoadLibrary("comb/libcomb.so")

cardList = (c_int*60)()
board = (c_int*60)()
seed = (c_char_p)('22073789477'.encode())
comb.getCardList(cardList, seed)
print([i for i in cardList])
p = [4, 8, 6, 10, 13, 5, 16, 7, 2, 12, 3, 1, 9, 18, 19, 14, 15, 17, 0, 11]
for i, j in enumerate(p):
    for k in range(3):
        board[j*3+k] = cardList[i*3+k]
score = comb.getScore(board)
print([i for i in board])
print(score)
