from ctypes import *

comb = cdll.LoadLibrary("./libcomb.so")

cardList = (c_int*60)()
perm = (c_int*20)()
seed = (c_char_p)('22073789477'.encode())
comb.getCardList(cardList, seed)
l=[i for i in cardList]
print(l)
print(l.count(8))
print(l.count(4))
print(l.count(3))
# p = [4, 8, 6, 10, 13, 5, 16, 7, 2, 12, 3, 1, 9, 18, 19, 14, 15, 17, 0, 11]
# for i in range(20):
# perm[i] = p[i]
# score = comb.getMaxScore(cardList, perm, 0, 1)
# print(score)
