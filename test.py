from ctypes import *

comb = cdll.LoadLibrary("comb/libcomb.so")

cardList = (c_int*60)()
seed = (c_char_p)('22073789477'.encode())
comb.getCardList(cardList, seed)
print([i for i in cardList])
p = [4, 8, 6, 10, 13, 5, 16, 7, 2, 12, 3, 1, 9, 18, 19, 14, 15, 17, 0, 11]
perm = (c_int*20)()
for i in range(20):
    perm[i] = p[i]
score = comb.getScoreByPerm(cardList, perm)
print(score)
