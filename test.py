from ctypes import *

comb = cdll.LoadLibrary(".//libcomb.so")
# comb = cdll.LoadLibrary("comb/libcomb_linux_x64_v1.0.so")

# cardList = (c_int*60)()
# seed = (c_char_p)('1234'.encode())

# comb.getCardList(cardList, seed)

borad = (c_int*60)()
card = (c_int*3)()
# vars = (c_double*11)(1.00, 0.721, 0.3993, 0.1947,
#                      0.069, 0.0312, 0.75, 0.03, 0.08465, 0.08164, 18)
exp = (c_double*20)()

n = int(input("n:"))
for j in range(n):
    inp = input("enter num:").split(' ')
    for i in range(3):
        card[i] = int(inp[i])
    act = int(input("where to place:"))
    for i in range(3):
        borad[act*3+i] = card[i]

for j in range(n, 20):
    inp = input("enter num:").split(' ')
    for i in range(3):
        card[i] = int(inp[i])
    for i in range(20):
        exp[i] = -1
    act = comb.getActionByTime(borad, card, 0, exp, 5, 12)
    k = sorted(enumerate(exp), key=lambda x: x[1], reverse=True)
    if j >= 5 and k[0][1]-k[1][1] < 2:
        print('run 30s')
        act = comb.getActionByTime(borad, card, 0, exp, 30, 12)
        k = sorted(enumerate(exp), key=lambda x: x[1], reverse=True)
    print(*k[:5], sep='\n')
    # act = int(input("where to place:"))
    print(f'act {act}')
    for i in range(3):
        borad[act*3+i] = card[i]
