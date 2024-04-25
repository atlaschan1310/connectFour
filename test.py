import connectFour
import time

if __name__ == "__main__":
    d = 20
    for i in range(1,8):
        for j in range(1,8):
            for k in range(1,8):
                s = str(i)
                s += str(j)
                s += str(k)
                start = time.time()
                nextMove = connectFour.solve(s, d)
                print(f'Time: {time.time() - start}')
                print(s)
                print("nextMove = ", nextMove)
