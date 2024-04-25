//
//  ConnectFour.h
//  Network_C
//
//  Created by yang qian on 2024/3/31.
//

#ifndef ConnectFour_h
#define ConnectFour_h

#include <stdio.h>
#include <iostream>
#include <string>
#include <chrono>
#include <iomanip>
#include <unordered_map>

namespace connectFour
{

// bitmask corresponds to board as follows in 7x6 case:
//  .  .  .  .  .  .  .  TOP
//  5 12 19 26 33 40 47
//  4 11 18 25 32 39 46
//  3 10 17 24 31 38 45
//  2  9 16 23 30 37 44
//  1  8 15 22 29 36 43
//  0  7 14 21 28 35 42  BOTTOM

//  heuristic points
//  .  .  .  .  .  .  .  TOP
//  3 4  5  7  5 4 3
//  4 6  8 10  8 6 4
//  5 8 11 13 11 8 5
//  5 8 11 13 11 8 5
//  4 6  8 10  8 6 4
//  3 4  5  7  5 4 3  BOTTOM

typedef uint64_t board;

static constexpr board HEIGHT = 6;
static constexpr board WIDTH = 7;
static constexpr board H1 = HEIGHT + 1;
static constexpr board H2 = HEIGHT + 2;
static constexpr board SIZE = HEIGHT * WIDTH;
static constexpr board SIZE1 = H1 * WIDTH;
static constexpr board COL1 = ((board)1<<H1) - (board(1));
static constexpr board ALL1 = ((board)1<<SIZE1) - (board(1));
static constexpr board BOTTOM = ALL1 / COL1;
static constexpr board TOP = (board)(BOTTOM<<HEIGHT);

static constexpr unsigned int LOCKSIZE = 26;
static constexpr unsigned int TRANSSIZE = 8306069;
static constexpr int SYMMREC = 10;

enum class STATUS {UNKNOWN, LOSS, DRAWLOSS, DRAW, DRAWWIN, WIN, LOSSWIN};

typedef struct {
    unsigned bigLock:LOCKSIZE;
    unsigned bigWork:6;
    unsigned newLock:LOCKSIZE;
    unsigned newScore:3;
    unsigned bigScore:3;
} hashEntry;

inline int isLegal(board newBoard) {
    return (newBoard & TOP) == 0ull;
}

inline board haswon(board newBoard)
{
  board diag1 = newBoard & (newBoard>>HEIGHT);
  board hori = newBoard & (newBoard>>H1);
  board diag2 = newBoard & (newBoard>>H2);
  board vert = newBoard & (newBoard>>1);
  return ((diag1 & (diag1 >> 2*HEIGHT)) |
          (hori & (hori >> 2*H1)) |
          (diag2 & (diag2 >> 2*H2)) |
          (vert & (vert >> 2)));
}

inline int islegalhaswon(board newBoard)
{
  return isLegal(newBoard) && haswon(newBoard);
}

class bitboard
{
public:
    int nplies;
    board colors[2];
    int moves[SIZE];
    char height[WIDTH];
    
public:
    bitboard() : nplies(0) {
        reset_board();
    }
    
    virtual ~bitboard() {}
    
    void reset_board() {
        nplies = 0;
        colors[0] = 0ull;
        colors[1] = 0ull;
        for (unsigned long long i = 0; i < WIDTH; i++) {
            height[i] = (char)(i*H1);
        }
        for (unsigned long long i = 0; i < SIZE; i++) {
            moves[i] = 0;
        }
    }

    void makeMove(int n) {
        assert(n >= 0 && n < 7);
        colors[nplies&1] ^= (board)(1)<<height[n]++;
        moves[nplies++] = n;
    }
    
    void backMove() {
        int n = moves[--nplies];
        colors[nplies&1] ^= (board)(1)<<--height[n];
    }
    
    board positionCode() const {
        return colors[nplies&1]+ colors[0] + colors[1] + BOTTOM;
    }
    
    void printMoves() const {
        for (int i = 0; i < nplies; i++) {
            printf("%d", 1+moves[i]);
        }
    }
    
    int isPlayable(int col) const {
        return isLegal(colors[nplies&1] | ((board)(1)<<height[col]));
    }
    
}; // end of class bitboard

class connectFour : public bitboard
{
public:
    hashEntry* hashTable;
    unsigned int lock;
    unsigned int htIndex;
    u_int64_t posed;
    int history[2][SIZE1];
    
    int bookply;
    int reportply;
    int depthply;
    int treeDepth;
    
    int shortCut1[7] = {4,3,3,3,5,5,4};
    std::unordered_map<std::string,int> shortCut2 =
    {
        {"11",4},{"12",2},{"13",3},{"14",2},{"15",4},{"16",3},{"17",4},
        {"21",5},{"22",5},{"23",5},{"24",4},{"25",4},{"26",4},{"27",4},
        {"31",4},{"32",6},{"33",4},{"34",4},{"35",3},{"36",3},{"37",3},
        {"41",4},{"42",6},{"43",6},{"44",4},{"45",1},{"46",6},{"47",4},
        {"51",5},{"52",5},{"53",3},{"54",4},{"55",4},{"56",2},{"57",4},
        {"61",4},{"62",4},{"63",4},{"64",4},{"65",1},{"66",3},{"67",6},
        {"71",4},{"72",2},{"73",4},{"74",3},{"75",6},{"76",1},{"77",4},
    };
    
public:
    connectFour(int treeD) : posed(0ull), lock(0), htIndex(0), treeDepth(treeD) {
        alloc_hashTable();
    }
    
    virtual ~connectFour() {
        destroy_hashTable();
    }
    
    void readMoves(const std::string& s) {
        for (const char c : s) {
            int move = static_cast<int>(c - '1');
            makeMove(move);
        }
        bookply = nplies;
        reportply = nplies + 2;
        depthply = nplies + treeDepth;
    }
    
public:
    void alloc_hashTable() {
        hashTable = new hashEntry[TRANSSIZE];
//        printf("allocate Transposition Table of size: %luMB\n", sizeof(hashEntry) * TRANSSIZE / (1024*1024));
    }
    
    void reset_hashTable() {
        for (int i = 0; i < TRANSSIZE; i++) {
            hashTable[i] = (hashEntry){0,0,0,0,0};
        }
    }
    
    void destroy_hashTable() {
        if (hashTable) {
            delete [] hashTable;
            hashTable = nullptr;
        }
    }
    
    void hash() {
        board htemp = positionCode();
        if (nplies < SYMMREC) {
            board htemp2 = 0ull;
            for (board htmp = htemp; htmp != 0ull; htmp>>=H1) {
                htemp2 = htemp2<<H1 | (htmp & COL1);
            }
            if (htemp2 < htemp) {
                htemp = htemp2;
            }
        }
        lock = static_cast<unsigned int>(SIZE1 > LOCKSIZE ? htemp >> (SIZE1 - LOCKSIZE) : htemp);
        htIndex = static_cast<unsigned int>(htemp % TRANSSIZE);
    }
    
    int transpose() {
        hash();
        hashEntry hashTemp = hashTable[htIndex];
        if (hashTemp.bigLock == lock) {
            return static_cast<int>(hashTemp.bigScore);
        }
        if (hashTemp.newLock == lock) {
            return static_cast<int>(hashTemp.newScore);
        }
        return static_cast<int>(STATUS::UNKNOWN);
    }
    
    void transStore(unsigned int index, unsigned int lock, int score, int work) {
        posed++;
        unsigned int curLock = hashTable[index].bigLock;
        int curWork = hashTable[index].bigWork;
        if (curLock == lock || work >= curWork) {
            hashTable[index].bigLock = lock;
            hashTable[index].bigScore = score;
            hashTable[index].bigWork = work;
        }
        else {
            hashTable[index].newLock = lock;
            hashTable[index].newScore = score;
        }
    }
    
    void init_history() {
        for (int side=0; side<2; side++) {
          for (int i=0; i<(WIDTH+1)/2; i++) {
            for (int h=0; h<H1/2; h++) {
              history[side][H1*i+h] = history[side][H1*(WIDTH-1-i)+HEIGHT-1-h] =
              history[side][H1*i+HEIGHT-1-h] = history[side][H1*(WIDTH-1-i)+h] =
               4+std::min(3,i) + std::max(-1,std::min(3,h)-std::max(0,3-i)) + std::min(3,std::min(i,h)) + std::min(3,h);
            }
          }
            history[side][6] = 0;
            history[side][13] = 0;
            history[side][20] = 0;
            history[side][27] = 0;
            history[side][34] = 0;
            history[side][41] = 0;
            history[side][48] = 0;
        }
    }
    
    int ab(int alpha, int beta) {
        if (nplies == SIZE - 1) return static_cast<int>(STATUS::DRAW);
    
        /*cutoff depth, return unknown status*/
        if (nplies > depthply) {
            return static_cast<int>(STATUS::UNKNOWN);
        }
            
        int side = nplies & 1;
        int otherSide = side ^ 1;
        board other = colors[otherSide];
        board newbrd = 0ull;
        int nav = 0; // number of available moves
        int av[WIDTH]; // available move list
        
        /* determine available move list*/
        for (int i = 0; i < WIDTH; i++) {
            newbrd = other | ((board)(1)<<height[i]);
            if (!isLegal(newbrd)) continue;
            int winonTop = islegalhaswon(other | ((board)(2)<<height[i])); // if double treat
            if (haswon(newbrd)) {
                if (winonTop) return static_cast<int>(STATUS::LOSS);
                nav = 0;
                av[nav++] = i;
                /* double treat, one forced move means loss*/
                while (++i < WIDTH) {
                    if (islegalhaswon(other | ((board)(1)<<(height[i])))) return static_cast<int>(STATUS::LOSS);
                }
                break;
            }
            if (!winonTop) av[nav++] = i; // no double treat, place one available move
        }
        
        if (nav == 0) return static_cast<int>(STATUS::LOSS);
        if (nplies == SIZE - 2) return static_cast<int>(STATUS::DRAW);
        if (nav == 1) {
            makeMove(av[0]);
            int lsw = static_cast<int>(STATUS::LOSSWIN);
            int score = lsw - ab(lsw - beta, lsw - alpha);
            backMove();
            return score;
        }
        
        int ttscore = transpose();
        /* if bitboard is already in the hashtable and status is known */
        if (ttscore != static_cast<int>(STATUS::UNKNOWN)) {
            if (ttscore == static_cast<int>(STATUS::DRAWLOSS)) {
                beta = static_cast<int>(STATUS::DRAW);
                if (beta <= alpha) return ttscore;
            }
            else if (ttscore == static_cast<int>(STATUS::DRAWWIN)) {
                alpha = static_cast<int>(STATUS::DRAW);
                if (beta <= alpha) return ttscore;
            }
            else {
                return ttscore;
            }
        }
        
        unsigned int curhtIndex = htIndex;
        unsigned int curLock = lock;
        uint64_t curposcnt = posed;
        int bestMove = 0;
        int score = static_cast<int>(STATUS::LOSS);
        int val;
        int v;
        int l;
        
        /* determine the priority of next moves firstly according to pre-stored heuristic points */
        for (int i = 0; i < nav; i++) {
            val = history[side][static_cast<int>(height[av[i]])];
            l = i;
            for (int j = i + 1; j < nav; j++) {
                v = history[side][static_cast<int>(height[av[j]])];
                if (v > val) {
                    val = v;
                    l = j;
                }
            }
            int toMove = av[l];
            while(l > i) {
                av[l] = av[l-1];
                l--;
            }
            av[i] = toMove;
            makeMove(av[i]); // the move determined by the heuristic points
            
            /* recursively evaluate the best moves */
            int lsw = static_cast<int>(STATUS::LOSSWIN);
            val = lsw - ab(lsw - beta, lsw - alpha);
            backMove();
            if (val > score) {
                bestMove = i;
                if ((score=val) > alpha && nplies >= bookply && (alpha=val) >= beta) {
                    if (score == static_cast<int>(STATUS::DRAW) && i < nav - 1) {
                        score = static_cast<int>(STATUS::DRAWWIN);
                    }
                    if (bestMove > 0) {
                        for (int k = 0; k < bestMove; k++) {
                            history[side][static_cast<int>(height[av[k]])]--; // punish all bad moves suggested by heuristics
                        }
                        history[side][static_cast<int>(height[av[bestMove]])] += bestMove; // reward correct moves
                    }
                    break;
                }
            }
        }
        
        if (score == static_cast<int>(STATUS::LOSSWIN) - ttscore) score = static_cast<int>(STATUS::DRAW);
        curposcnt = posed - curposcnt;
        int work = 0;
        while ((curposcnt >>= 1) != 0) work++;
        transStore(curhtIndex, curLock, score, work);
//        if (nplies <= reportply) {
//            printMoves();
//            printf("%c%d\n", "#-<=>+"[score], work);
//        }
        return score;
    }
    
    int shortCut(const std::string &s) {
        int len = s.size();
        switch (len) {
            case 0:
                return 4;
            case 1:
                return shortCut1[stoi(s) - 1];
            case 2:
                return shortCut2[s];
        }
        return -1;
    }
    
    int solve(const std::string& s) {
        
        if (s.size() <= 2) return shortCut(s);
        reset_board();
        readMoves(s);
        
        int side = nplies & 1;
        int otherSide = side ^ 1;
        
        if (haswon(colors[otherSide])) {
//            printf("I lose!!!\n");
            return 1;
        }
        
        for (int i = 0; i < WIDTH; i++) {
            if (islegalhaswon(colors[side] | ((board)1 << height[i]))) {
//                printf("about to +\n");
                return i + 1;
            }
        }
        
        reset_hashTable();
        init_history();
        int score = ab(static_cast<int>(STATUS::LOSS), static_cast<int>(STATUS::WIN));
//        printf("about to %c\n", "#-<=>+"[score]);
        
        bool storeSecond = true;
        int stat;
        int workMark = -1;
        int curWork;
        int nextMove = -1;
        for (int i = 0; i < WIDTH; i++) {
            makeMove(i);
            stat = transpose();
            if (stat != static_cast<int>(STATUS::UNKNOWN)) {
                storeSecond = false;
                if (stat == static_cast<int>(STATUS::LOSSWIN) - score) {
                    curWork = hashTable[htIndex].bigWork;
                    if (curWork > workMark) {
                        workMark = curWork;
                        nextMove = i;
                    }
                }
            }
            backMove();
        }
        
        if (storeSecond) {
            for (int i = 0; i < WIDTH; i++) {
                makeMove(i);
                for (int j = 0; j < WIDTH; j++) {
                    makeMove(j);
                    stat = transpose();
                    if (stat == score) {
                        curWork = hashTable[htIndex].bigWork;
                        if (curWork > workMark) {
                            workMark = curWork;
                            nextMove = i;
                        }
                    }
                    backMove();
                }
                backMove();
            }
        }
//        printf("nextMove:%d\n", nextMove + 1);
        assert(nextMove != -1);
        return nextMove + 1;
    }
    
};// end of class connectFour

} // end of namespace connectFour

#endif /* ConnectFour_h */
