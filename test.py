from ctypes import *

comb = cdll.LoadLibrary("comb/libcomb_window_v1.0.so")

cardList = (c_int*60)()
perm = (c_int*20)()
seed = (c_char_p)('22073789477'.encode())
comb.getCardList(cardList, seed)
print([i for i in cardList])
score = comb.getMaxScore(cardList, perm, 0, 1)
print(score)
print([i for i in perm])
