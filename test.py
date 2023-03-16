from ctypes import *

# comb = cdll.LoadLibrary("comb/libcomb_window_v1.0.so")
comb = cdll.LoadLibrary("comb/libcomb_linux_x64_v1.0.so")

cardList = (c_int*60)()
perm = (c_int*20)()
seed = (c_char_p)('宽容'.encode())
comb.getCardList(cardList, seed)
score = comb.getMaxScore(cardList, perm, 0, 12)
print(score)
print([i for i in cardList])
print([i for i in perm])
