from ctypes import *

sotest = cdll.LoadLibrary("comb\\libcomb.so")

hello = (c_int*60)()
seed = (c_char_p)('灰羽'.encode())
sotest.getCardList(hello, seed)
print([i for i in hello])
