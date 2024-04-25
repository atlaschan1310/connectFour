import connectFour as four

def make_move(board):
    numChess = 0
    numPlayer1 = 0
    numPlayer2 = 0
    ply1 = list()
    ply2 = list()
    for height in range(6):
        for col in range(7):
            curChess = board[col][5-height]
            if curChess != 0:
                numChess += 1
                if curChess == 1:
                    numPlayer1 += 1
                    ply1.append(col+1)
                else:
                    numPlayer2 += 1
                    ply2.append(col+1)
    
    player1 = True if numPlayer1 == numPlayer2 else False
    assert (numChess == numPlayer1 + numPlayer2)
    assert (numPlayer1 - numPlayer2 <= 1)
    
    s = ""
    for i in range(numPlayer2):
        s += str(ply1[i])
        s += str(ply2[i])
    if not player1:
        s += str(ply1[-1])
    nextMove = four.solve(s, 23)
    return nextMove - 1
