
// Shakkimoottori.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

//#include <iostream>
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <windows.h>
#include <iostream>

using namespace std;

int board[8][8] = { {1,3,4,5,6,4,3,2}, {2,1,1,1,1,1,1,1}, {0,0,0,0,0,0,0,0}, {0,0,0,0,0,0,0,0}, {0,0,0,0,0,0,0,0}, {0,0,0,0,0,0,0,0}, {-1,-1,-1,-1,-1,-1,-1,-1}, {-2,-3,-4,-5,-6,-4,-3,-2} };

int kx[3], ky[3];   // for easy definition of kx[current_player] and no negative indexes, kx[0] = player -1 and kx[2] = player 1

const int max_pos = 400;

int max_depth = 15;
int depth = 0;


#define X0 0
#define Y0 1
#define X 2
#define Y 3
#define CAP_PIECE 4
#define CHECK 5
#define VALUE 6
#define INCL 7
#define VAL 4
#define EVALD 5
#define DEPTH 8
#define LINK 9
#define AVAL 10

int possiblePositions[8][max_pos] = { 0 };

int capturable_x[50];
int capturable_y[50];
int capturable[50];
int init_player;


int checkPlayer;
int reversePiece;

int n, n_capturable; // numerator for possible positions, possible captures
int current_player = 1;

int checkPossibleMovesForPiece(int x, int y, int n_init = 0);


class PieceMoved {
public:

    int pieceMoved[100] = { 0 };
    int original_x[100] = { 0 };
    int original_y[100] = { 0 };
    int new_x[100] = { 0 };
    int new_y[100] = { 0 };
    int captured[100] = { 0 };
};

void Color(int color)
{
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
}

void printBoard() {
    for (int y = 7; y >= 0; y--) {
        for (int x = 0; x < 8; x++) {
            if (board[y][x] < 0)
                Color(4);
            else if (board[y][x] > 0)
                Color(2);
            else
                Color(7);
            cout << abs(board[y][x]) << " ";
        }
        cout << endl;
    }
    Color(7);
    cout << "----------------" << endl;

}

int checkChecksforMove(int x0, int y0, int x, int y, int myChecks = 0) {


    int reversePiece = board[y][x];
    board[y][x] = board[y0][x0];
    board[y0][x0] = 0;

    int n_checks = 0;
    int check_x[10] = { 0 };
    int check_y[10] = { 0 };
    int x_hyp = 0;
    int y_hyp = 0;
    int king_y, king_x, d;

    if (abs(board[y][x]) == 5) {       // moving the king
        king_x = x; king_y = y;
    }
    else {
        king_x = kx[current_player + 1]; king_y = ky[current_player + 1];
    }

    // check for enemy knights
    for (int rot = 0; rot < 8; rot++) {
        switch (rot) {
        case 0:
            x_hyp = king_x + 2;
            y_hyp = king_y + 1;
            break;
        case 1:
            x_hyp = king_x + 1;
            y_hyp = king_y + 2;
            break;
        case 2:
            x_hyp = king_x - 2;
            y_hyp = king_y - 1;
            break;
        case 3:
            x_hyp = king_x - 1;
            y_hyp = king_y - 2;
            break;
        case 4:
            x_hyp = king_x + 1;
            y_hyp = king_y - 2;
            break;
        case 5:
            x_hyp = king_x + 2;
            y_hyp = king_y - 1;
            break;
        case 6:
            x_hyp = king_x - 1;
            y_hyp = king_y + 2;
            break;
        case 7:
            x_hyp = king_x - 2;
            y_hyp = king_y + 1;
            break;
        }

        if (board[y_hyp][x_hyp] * current_player == -3) {
            n_checks++;
        }
    }

    for (int rot = 0; rot <= 7; rot++) {
        d = 0;
        switch (rot) {      // start at +1 current direction
        case 0: x_hyp = king_x + 1;     // 0-3 diagonals, 4-7 files
            y_hyp = king_y + 1;
            break;
        case 1: x_hyp = king_x - 1;
            y_hyp = king_y - 1;
            break;
        case 2: x_hyp = king_x + 1;
            y_hyp = king_y - 1;
            break;
        case 3: x_hyp = king_x - 1;
            y_hyp = king_y + 1;
            break;

        case 4: x_hyp = king_x;
            y_hyp = king_y - 1;
            break;
        case 5: x_hyp = king_x;
            y_hyp = king_y + 1;
            break;
        case 6: x_hyp = king_x - 1;
            y_hyp = king_y;
            break;
        case 7: x_hyp = king_x + 1;
            y_hyp = king_y;
            break;
        }

        while (x_hyp <= 7 && x_hyp >= 0 && y_hyp >= 0 && y_hyp <= 7) { // inside borders

            if ((board[y_hyp][x_hyp] * current_player) > 0) { // own piece
                break;
            }
            // enemy queen or rook at the same file, or king next to king
            else if (((board[y_hyp][x_hyp] * current_player) == -2) || (((board[y_hyp][x_hyp] * current_player) == -5) && (d == 0)) || ((board[y_hyp][x_hyp] * current_player) == -6)) {
                if ((rot >= 4) && (rot <= 7)) {
                    check_x[n_checks] = x_hyp;
                    check_y[n_checks] = y_hyp;
                    n_checks++;
                    break;
                }
            }

            // enemy queen or bishop at the same diagonal, or king next to king, or enemy pawn next diagonal
            else if (((board[y_hyp][x_hyp] * current_player) == -4) || ((board[y_hyp][x_hyp] * current_player) == -5 && (d == 0)) || ((board[y_hyp][x_hyp] * current_player) == -6) || (((board[y_hyp][x_hyp] * current_player) == -1 && d == 0 && y_hyp == king_y + current_player && x_hyp != king_x))) {
                if ((rot >= 0) && (rot < 4)) {
                    check_x[n_checks] = x_hyp;
                    check_y[n_checks] = y_hyp;
                    n_checks++;
                    break;
                }
            }

            d++; // increment distance

            switch (rot) {      // start at +1 current direction
            case 0: x_hyp++;     // 0-3 diagonals, 4-7 files
                y_hyp++;;
                break;
            case 1: x_hyp--;
                y_hyp--;
                break;
            case 2: x_hyp++;
                y_hyp--;
                break;
            case 3: x_hyp--;
                y_hyp++;
                break;

            case 4: y_hyp--;
                break;
            case 5: y_hyp++;
                break;
            case 6: x_hyp--;
                break;
            case 7: x_hyp++;
                break;

            }
        }
    }

    board[y0][x0] = board[y][x];
    board[y][x] = reversePiece;

    // if (myChecks == 1)
     //    current_player *= -1;

    return n_checks;
}



int addPositionIfLegal(int x0, int y0, int x, int y, int n_init = 0, int node_nro = -1) {  // returns  -3 = does not compute -2 = illegal position(king under check)
                                                                        //  -1 = outside of board
                                                                        //           0 = own piece in square, 1 = empty square, 2 = enemy piece in square(capture)
                                                                        // 3 = check, 4 = enemy piece capture and check
    bool enemyPiece = false;
    if ((x0 == x && y0 == y) || (board[y0][x0] == 0) || y > 10) {
        printf("no way");
        return -3;
    }
    // maybe refactor, but not really worth it
    if (x <= 7 && x >= 0 && y >= 0 && y <= 7) { // inside borders
        if (board[y][x] == 0) {   //empty square at target
            possiblePositions[X][n + n_init] = x; possiblePositions[Y][n + n_init] = y;
            possiblePositions[X0][n + n_init] = x0; possiblePositions[Y0][n + n_init] = y0;
            n++;
        }
        else if (((board[y][x] * current_player) < 0 && abs(board[y0][x0]) != 1)
            || ((board[y][x] * current_player) < 0 && abs(board[y0][x0]) == 1 && x0 != x)) {   // enemy piece at target(additional rules for pawn)
            possiblePositions[X][n + n_init] = x; possiblePositions[Y][n + n_init] = y;
            possiblePositions[X0][n + n_init] = x0; possiblePositions[Y0][n + n_init] = y0;
            possiblePositions[CAP_PIECE][n + n_init] = abs(board[y][x]);
            n++;
            enemyPiece = true;
        }
        else {
            return 0;   // own piece
        }
        // check for checks
        if (checkChecksforMove(x0, y0, x, y, 0) != 0) {      // checks found(illegal move)
            n--; // erase last possible move
            return -2;
        }
        else {
            /*
            if (checkChecksforMove(x0, y0, x, y, 1) != 0) {
                possiblePositions[CHECK][n + n_init - 1] = 1; // -1 since n already incremented earlier
                if (enemyPiece)
                    return 4;   // check with capture
                else
                    return 3;   // check
            }
            else {*/
            if (enemyPiece) {
                return 2;   // enemy piece
            }
            else
                return 1;   // empty square


        }
    }
    else {
        return -1; // outside of borders 
    }
}




int getPieceValue(int pc) {
    int r = 0;
    switch (abs(pc)) {
    case 1:
        return 1;
    case 2:
        return 5;
    case 3:
        return 3;
    case 4:
        return 3;
    case 5:
        return 100;

    case 6:
        return 8;
    };

}


void findKings() {          // find kings and save positions
    for (kx[0] = 0; kx[0] < 7; kx[0]++) {
        for (ky[0] = 0; ky[0] < 7; ky[0]++) {
            if (board[ky[0]][kx[0]] == -5)
                break;
        }
        if (board[ky[0]][kx[0]] == -5)
            break;
    }
    kx[2] = 0, ky[2] = 0;
    for (kx[2] = 0; kx[2] < 7; kx[2]++) {
        for (ky[2] = 0; ky[2] < 7; ky[2]++) {
            if (board[ky[2]][kx[2]] == 5)
                break;
        }
        if (board[ky[2]][kx[2]] == 5)
            break;
    }
}


int checkPossibleMovesForPiece(int x, int y, int n_init) { // x, y of piece, initial position of array to store moves
    if (board[y][x] == 0) {
        return 0;
    }

    n = 0;
    int x_hyp = x, y_hyp = y;

    if (abs(board[y][x]) == 1) {         // pawn   todo: might refactor
        if (((y < 7) && (current_player == 1)) || ((y > 0) && (current_player == -1))) { // possible to move
            if (current_player == 1 && board[y + 1][x] == 0) {  // white piece
                if (y == 1) {// possible to move 2
                    addPositionIfLegal(x, y, x, 3, n_init);
                }
                if (x < 7 && y < 7) {
                    if (board[y + 1][x + 1] < 0) {// enemy piece
                        addPositionIfLegal(x, y, x + 1, y + 1, n_init);
                    }
                }
                if (x > 0 && y < 7) {
                    if (board[y + 1][x - 1] < 0) {// enemy piece
                        addPositionIfLegal(x, y, x - 1, y + 1, n_init);
                    }
                }
            }
            else {      // black piece
                if (y == 6 && board[y - 1][x] == 0) {
                    addPositionIfLegal(x, y, x, 4, n_init);
                }
                if (x > 0 && y > 0) {
                    if (board[y - 1][x - 1] > 0) {// enemy piece
                        addPositionIfLegal(x, y, x - 1, y - 1, n_init);
                    }
                }
                if (x < 7 && y > 0) {
                    if (board[y - 1][x + 1] > 0) {// enemy piece
                        addPositionIfLegal(x, y, x + 1, y - 1, n_init);
                    }
                }
            }
            if (board[y + current_player][x] == 0) { // one forward
                addPositionIfLegal(x, y, x, y + current_player, n_init);
            }
        }
    }
    else if (abs(board[y][x]) == 2 || abs(board[y][x]) == 6) {    // castle or queen
        for (int rot = 0; rot < 4; rot++) {
            x_hyp = x;
            y_hyp = y;

            int ret = 1;
            while (ret == 1) { // iterate while in borders, legal move and empty or enemy square               
                switch (rot) {      // move (hypothetical) to current direction
                case 0: x_hyp++;
                    break;
                case 1: x_hyp--;
                    break;
                case 2: y_hyp--;
                    break;
                case 3: y_hyp++;
                    break;
                }

                ret = addPositionIfLegal(x, y, x_hyp, y_hyp, n_init);
            }
        }
    }

    if (abs(board[y][x]) == 4 || abs(board[y][x]) == 6) {    // bishop
        for (int rot = 0; rot < 4; rot++) {
            x_hyp = x;
            y_hyp = y;

            int ret = 1;
            while (ret == 1) { // iterate while in borders and empty or enemy square               
                switch (rot) {      // move (hypothetical) to current direction
                case 0: x_hyp++;
                    y_hyp--;
                    break;
                case 1: x_hyp--;
                    y_hyp++;
                    break;
                case 2: x_hyp--;
                    y_hyp--;
                    break;
                case 3: x_hyp++;
                    y_hyp++;
                    break;
                }
                ret = addPositionIfLegal(x, y, x_hyp, y_hyp, n_init);
            }
        }
    }
    else if (abs(board[y][x]) == 3) {    // knight
        int ret;
        for (int rot = 0; rot < 8; rot++) {
            switch (rot) {      // handle every direction
            case 0:
                x_hyp = x + 2;
                y_hyp = y + 1;
                break;
            case 1:
                x_hyp = x + 1;
                y_hyp = y + 2;
                break;
            case 2:
                x_hyp = x - 2;
                y_hyp = y - 1;
                break;
            case 3:
                x_hyp = x - 1;
                y_hyp = y - 2;
                break;
            case 4:
                x_hyp = x + 1;
                y_hyp = y - 2;
                break;
            case 5:
                x_hyp = x + 2;
                y_hyp = y - 1;
                break;
            case 6:
                x_hyp = x - 1;
                y_hyp = y + 2;
                break;
            case 7:
                x_hyp = x - 2;
                y_hyp = y + 1;
                break;

            }
            ret = addPositionIfLegal(x, y, x_hyp, y_hyp, n_init);
        }
    }
    else if (abs(board[y][x]) == 5) {    // king
        int ret;
        for (int rot = 0; rot < 8; rot++) {
            switch (rot) {      // handle every direction
            case 0:
                x_hyp = x + 1;
                y_hyp = y;
                break;
            case 1:
                x_hyp = x - 1;
                y_hyp = y;
                break;
            case 2:
                x_hyp = x;
                y_hyp = y + 1;
                break;
            case 3:
                x_hyp = x;
                y_hyp = y - 1;
                break;
            case 4:
                x_hyp = x + 1;
                y_hyp = y + 1;
                break;
            case 5:
                x_hyp = x + 1;
                y_hyp = y - 1;
                break;
            case 6:
                x_hyp = x - 1;
                y_hyp = y + 1;
                break;
            case 7:
                x_hyp = x - 1;
                y_hyp = y - 1;
                break;
            }
            addPositionIfLegal(x, y, x_hyp, y_hyp, n_init);
        }
    }

    return n;
}

int checkAllPossibleMoves() {
    for (int i = 0; i < max_pos; i++)  // empty the captured pieces array
        possiblePositions[CAP_PIECE][i] = 0;

    int n_allMoves = 0;
    for (int x = 0; x < 8; x++) {
        for (int y = 0; y < 8; y++) {
            if (board[y][x] * current_player > 0 || (current_player == 0 && board[y][x] != 0)) {
                n_allMoves += checkPossibleMovesForPiece(x, y, n_allMoves);
            }
        }
    }
    return n_allMoves;
}



class PossibleMoves {
public:
    int possibleMoves_x0[max_pos] = { 0 };
    int possibleMoves_y0[max_pos] = { 0 };
    int possibleMoves_x[max_pos] = { 0 };
    int possibleMoves_y[max_pos] = { 0 };
    int possibleMoves_value[max_pos] = { 0 };
    int n_all;

    PossibleMoves() {
        n_all = 0;
    }

    void add(int x0, int y0, int x, int y, int val, int n) {
        possibleMoves_x[n] = x; possibleMoves_y[n] = y;
        possibleMoves_x0[n] = x0; possibleMoves_y0[n] = y0;
        possibleMoves_value[n] = val;
        n_all++;
    }

};


int evalOrderFromBest[2][50];
int evaluations = 0;



void switchPlayer() {
    current_player *= -1;
}


class MoveHistory {     // only checks if piece can be put there, supposing it's movement allows it
public: int current_move = 0; // moves start at index 1!
      int  n_moves = 0;

      PieceMoved pieceMoved;

      int newMove(int x0, int y0, int x, int y) {
          if (addPositionIfLegal(x0, y0, x, y) >= 1) { // empty or enemy square and not illegal check
              n_moves++;
              current_move++;
              pieceMoved.original_x[current_move] = x0;
              pieceMoved.original_y[current_move] = y0;
              pieceMoved.new_x[current_move] = x;
              pieceMoved.new_y[current_move] = y;
              pieceMoved.pieceMoved[current_move] = board[y0][x0];

              if (abs(pieceMoved.pieceMoved[current_move]) == 5) {    // update position of king
                  kx[1 + current_player] = x; ky[1 + current_player] = y;
              }
              board[pieceMoved.original_y[current_move]][pieceMoved.original_x[current_move]] = 0;
              pieceMoved.captured[current_move] = board[pieceMoved.new_y[current_move]][pieceMoved.new_x[current_move]];
              board[pieceMoved.new_y[current_move]][pieceMoved.new_x[current_move]] = pieceMoved.pieceMoved[current_move];

              depth++;
              switchPlayer();
              return 0;
          }
          else
              return -1;  // not a legal move in current context
      }

      void reverseMoves(int n) {
          for (int i = 0; i < n && (current_move > 0); i++) {
              board[pieceMoved.original_y[current_move]][pieceMoved.original_x[current_move]] = pieceMoved.pieceMoved[current_move];
              board[pieceMoved.new_y[current_move]][pieceMoved.new_x[current_move]] = pieceMoved.captured[current_move];

              if (abs(pieceMoved.pieceMoved[current_move]) == 5) {    // update position of king
                  kx[1 + (current_player * -1)] = pieceMoved.original_x[current_move]; ky[1 + (current_player * -1)] = pieceMoved.original_y[current_move];
              }
              current_move--;
          }

          if (n % 2 == 1)
              switchPlayer();
          depth -= n;
      }


      void forwardMoves(int n) {
          for (int i = 0; (i < n) && current_move < n_moves; i++) {
              current_move++;
              board[pieceMoved.original_y[current_move]][pieceMoved.original_x[current_move]] = 0;
              pieceMoved.captured[current_move] = board[pieceMoved.new_y[current_move]][pieceMoved.new_x[current_move]];
              board[pieceMoved.new_y[current_move]][pieceMoved.new_x[current_move]] = pieceMoved.pieceMoved[current_move];

              if (abs(pieceMoved.pieceMoved[current_move]) == 5) {    // update position of king
                  kx[1 + current_player] = pieceMoved.new_x[current_move]; ky[1 + current_player] = pieceMoved.new_y[current_move];
              }
          }
          if (n % 2 == 1)
              switchPlayer();
      }

      void resetBoard() {
          while (current_move > 0)
              reverseMoves(1);
      }
};

MoveHistory moveHistory;


#define eval_n 0
#define eval_v 1

#define v_soldier_forward 1
#define v_check 3

class Movi {
public:
    int movi[30000000][11];
    int i_last_depth[20000000], n_last_depth;
    int n, n_movis = -1, player, movisFound;


    Movi() {
        n = 0; n_movis = -1, n_last_depth = 0;
    }

    void printMovi(int movin) {
        printf("Movi nro %d X0: %d Y0: %d X: %d Y: %d\n", movin, movi[movin][X0], movi[movin][Y0], movi[movin][X], movi[movin][Y]);
    }
    void addMovi(int x0, int y0, int x, int y, int val = -1, int depth = -1, int link = -1) {
        n_movis++;
        movi[n_movis][X0] = x0;
        movi[n_movis][Y0] = y0;
        movi[n_movis][X] = x;
        movi[n_movis][Y] = y;
        movi[n_movis][VAL] = val;
        movi[n_movis][DEPTH] = depth;
        movi[n_movis][LINK] = link;
        movi[n_movis][AVAL] = val;
        if (n_movis % 1000000 == 0) {
            printf("moveja: %d\n", n_movis);
        }
    }

    void playMovi(int n) {
        moveHistory.newMove(movi[n][X0], movi[n][Y0], movi[n][X], movi[n][Y]);
    }

    int playMoviGetResult(int n) {
        return moveHistory.newMove(movi[n][X0], movi[n][Y0], movi[n][X], movi[n][Y]);
    }

    int compareMovis(int movi1, int movi2) {
        if (movi[movi1][X0] == movi[movi2][X0] &&
            movi[movi1][Y0] == movi[movi2][Y0] &&
            movi[movi1][X] == movi[movi2][X] &&
            movi[movi1][Y] == movi[movi2][Y]
            ) {
            return 1;
        }
        else
            return -1;
    }

    int getIndexOfLastDepth(int d) {
        int ind = -1;
        for (int i = n_movis; i >= 0; i--) {
            if (movi[n_movis][DEPTH] == d) {
                ind = i;
                break;
            }
        }
        return ind;
    }

    int getNofDepth(int d) {
        int n = 0;
        for (int i = n_movis; i >= 0; i--) {
            n++;
        }
        return n;

    }

    int getIndexOfNthOfDepth(int n, int d) {
        int i;
        bool found = false;

        for (i = n_movis; i >= 0; i--) {
            if (movi[i][DEPTH] == d) {
                if (n == d) {
                    found = true;
                    break;
                }
                n++;
            }
        }
        if (found)
            return i;
        else
            return -1;
    }

    int getIndexOfLastEntry() {
        return n_movis;
    }

    void link(int n, int n_to) {

        movi[n][LINK] = n_to;
        movi[n][AVAL] += movi[n_to][AVAL];  // accumulate value(negative for opponent)
        i_last_depth[n_last_depth] = n;  // only moves at last depth are linked
        n_last_depth++; 
    }

    int* getListOfLinkedIndexes(int n_movi) {
        int ind[50];
        int n = 0;

        while (movi[n_movi][LINK] != -1) {
            n++;
            ind[n] = movi[n_movi][LINK];
        }

        ind[0] = n;

        return ind;
    }

    int playLinkedMoves(int movi_nro) {
        int ind[50];
        int n = 0;
        //        printf("%d", i_last_depth[12403]);
        while (movi[movi_nro][LINK] != -1) {  // list starting from deepest move
            ind[n] = movi_nro;
            n++;
            movi_nro = movi[movi_nro][LINK];
        }
        //printBoard();
        ind[n] = movi_nro;

        for (int i = n; i >= 0; i--) {   // play in reverse order
            playMovi(ind[i]);
            //printBoard();
        }

        return ind[0]; // return index of last movi played
    }

    int playLinkedMovesOnDisplay(int movi_nro) {
        int ind[50];
        int n = 0;
        //        printf("%d", i_last_depth[12403]);
        while (movi[movi_nro][LINK] != -1) {  // list starting from deepest move
            ind[n] = movi_nro;
            n++;
            movi_nro = movi[movi_nro][LINK];
        }
        printBoard();
        ind[n] = movi_nro;

        for (int i = n; i >= 0; i--) {   // play in reverse order
            playMovi(ind[i]);
            printBoard();
        }

        return ind[0]; // return index of last movi played
    }
};

Movi movi;

int evaluate(int player, int init_player, int depth, int* best_value = 0) {
    int eval[max_pos] = { 0 };
    int best = 0;
    int i_best = 0;

    int temp;
    bool switchMade = true;

    if (player != -1 && player != 1)
        printf("invalid player!");
    current_player = player;

    int n_possible = checkAllPossibleMoves();
    evaluations = n_possible;

    if (n_possible > 0) {
        for (int i = 0; i < n_possible; i++) {

            if (abs(board[possiblePositions[Y0][i]][possiblePositions[X0][i]]) == 1 && possiblePositions[Y][i] != possiblePositions[Y0][i])
                eval[i] += v_soldier_forward;


            if (possiblePositions[CHECK][i] == 1) {
                eval[i] += v_check;
            }

            if (possiblePositions[CAP_PIECE][i] != 0) {
                eval[i] += getPieceValue(possiblePositions[CAP_PIECE][i]) + 1;
            }
            if (eval[i] > best) {
                best = eval[i]; i_best = i;
            }

            movi.addMovi(possiblePositions[X0][i], possiblePositions[Y0][i], possiblePositions[X][i], possiblePositions[Y][i], eval[i] * (current_player * init_player), depth);

            evalOrderFromBest[eval_n][i] = i;      // copy to arrangement table, not in order yet
            evalOrderFromBest[eval_v][i] = eval[i];
        }

        // order the table
        while (switchMade == true) {
            switchMade = false;
            for (int i = 0; i < n_possible - 1; i++) {
                if (evalOrderFromBest[eval_v][i] < evalOrderFromBest[eval_v][i + 1]) {  // swap
                    temp = evalOrderFromBest[eval_v][i];
                    evalOrderFromBest[eval_v][i] = evalOrderFromBest[eval_v][i + 1];
                    evalOrderFromBest[eval_v][i + 1] = temp;

                    temp = evalOrderFromBest[eval_n][i];
                    evalOrderFromBest[eval_n][i] = evalOrderFromBest[eval_n][i + 1];
                    evalOrderFromBest[eval_n][i + 1] = temp;
                    switchMade = true;
                }
            }
        }

    }
    else {      // checkmate or stalemate
        if (checkChecksforMove(0, 0, 0, 0, 0) != 0) {
            printf("Checkmate!");
            printBoard();
            printf("King of player %d at %d, %d", current_player, kx[current_player + 1], ky[current_player + 1]);
            return -2;
        }
        else {
            printf("Stalemate!");
            return -1;
        }
    }

    movi.movisFound = n_possible;
    //    *best_value = best;
    return best;
}

#define N_MOVI 4
#define I_VAR 0
#define I_MOVI 1

class Variation {
public:
    int current_variation[20][2] = { 0 };
    int variations[100000][6][2] = { 0 };
    int variation_move[10000][6] = { 0 };
    int variation_value[100000] = { 0 };
    //unsigned int variations[100000][12][5] = { 0 };
    char variation_length[100000] = { 0 };
    int n_variations = 0;
    int n_variation_moves = 0;
    int current_variation_index = 0;
    int n_var_moves_at_depth[10] = { 0 };

    void init_variation_value() {
        for (int i = 0; i < 100000; i++)
            variation_value[i] = -1;
    }
    int next() {
        if (current_variation_index < n_variations) {
            current_variation_index++;
            return 0;
        }
        else
            return -1;
    }

    int addVariationMove(int movi_nro, int dep) {
        variation_move[n_variation_moves][dep] = movi_nro;
        n_variation_moves++;
        n_var_moves_at_depth[dep]++;
 
        return n_variation_moves;
    }

    int getVariationMove(int movi_nro, int dep) {
        for (int varmove = 0; varmove < n_variation_moves; varmove++) {
            if (movi.compareMovis(movi_nro, variation_move[varmove][dep]) != -1) {
                return varmove;
            }
        }
        return -1;
    }

    int getMovi(int varmove, int dep) {
        for (int movi_nro = 0; movi_nro < movi.n_movis; movi_nro++) {
            if (movi.compareMovis(movi_nro, variation_move[varmove][dep]) != -1) {
                return movi_nro;
            }
        }
        return -1;
    }

    void playVariation(int n) {
        printBoard();
        movi.playMovi(variations[n][2][I_MOVI]);
        printBoard();
        movi.playMovi(variations[n][0][I_MOVI]);
        printBoard();
    }
    void setVariation(int movi_nro, bool after_analysis = false) {
        int ind[50];
        int n = 0;

        if(!after_analysis) 
            current_variation_index++;

        while (movi.movi[movi_nro][LINK] != -1) {  // list starting from deepest move
            ind[n] = movi_nro;
            n++;
            movi_nro = movi.movi[movi_nro][LINK];
        }
//        printBoard();
        ind[n] = movi_nro;

        int var_move;
        for (int dep = 0; dep <= 2; dep += 2) {   // replay in reverse order
            var_move = -1;
            if (dep >= 0) {
                var_move = getVariationMove(ind[dep], dep);  // get existing variation movi indexes for players moves to current depth or assign new ones
                if (var_move == -1) {
                    var_move = addVariationMove(ind[dep], dep);
                }
                current_variation[dep][I_VAR] = var_move;       // variation move index
                current_variation[dep][I_MOVI] = ind[dep];   // movi index
                if (after_analysis)
                {
                    movi.playMovi(getMovi(var_move, dep)); printBoard();
                }
                if (!after_analysis) {
                    variations[current_variation_index][dep][I_VAR] = var_move;
                    variations[current_variation_index][dep][I_MOVI] = ind[dep];
                }
            }
        }
    }

    int findVariationIndex() {
        int var_i = 0;
        while ((current_variation[0][I_VAR] != variations[var_i][0][I_VAR]) || (current_variation[2][I_VAR] != variations[var_i][2][I_VAR]) && (var_i <= n_variations))
            var_i++;
        return var_i;
    }
    int playVariationMoveAtDepth(int dep) {
        if (dep % 2 == 0) {
            return movi.playMoviGetResult(current_variation[dep][1]);
        }
        else {
            printf("Enemy move");
            return -2;
        }
    }

    int playVariationMove(int dep) {
        if (dep % 2 == 0) {
            return movi.playMoviGetResult(current_variation[dep][I_MOVI]);
        }
        else {
            printf("Enemy move");
            return -2;
        }
    }
    
    int isVariationPreventable() {
        int max_depth = 2;
        int z;
        int init_index;
        bool preventable = false;
        int i_preventing_movi;
        for (int dep = max_depth; dep >= 0; dep--) {
            //printBoard();
            playVariationMove(dep);
            //    printf("Illegal move");
//            printBoard();

            init_index = movi.getIndexOfLastEntry();
            z = evaluate(current_player, init_player, depth);
            dep -= 2;
            for (int i_opp_move_movi = init_index + 1; i_opp_move_movi < movi.movisFound + init_index; i_opp_move_movi++) {
                movi.playMovi(i_opp_move_movi);
//               printBoard();
                if (playVariationMove(dep) == -1) {
                    preventable = true;
                    i_preventing_movi = i_opp_move_movi;
                    break;
                }
  //              printBoard();
                moveHistory.reverseMoves(2);
            }
        }
        if (preventable)
            return i_preventing_movi;
        else
            return 0;
    }


    void addVariation(int movi_nro) {        // add new variation from all linked moves starting at movi_nro

        current_variation_index = n_variations;

        int ind_movi[50];
        int n = 0;
        //        printf("%d", i_last_depth[12403]);
        while (movi.movi[movi_nro][LINK] != -1) {  // list starting from deepest move
            ind_movi[n] = movi_nro;
            n++;
            movi_nro = movi.movi[movi_nro][LINK];
        }

        ind_movi[n] = movi_nro;

        variation_length[current_variation_index] = n;

        for (int i = n; i >= 0; i--) {
            //movi.printMovi(ind_movi[i]);
            if (i % 2 == 0) {
                variations[current_variation_index][i][I_MOVI] = ind_movi[i];
            }
        }

        bool match = false;
        int var_match = -1;
        for (int i = 0; i < n_variations; i++) { // check if variation already exists
            match = true;
            for (int dep = 0; dep < n; dep++) {
                //     movi.printMovi(variations[current_variation_index][dep]);
                //     printf("compared to \n");
                //     movi.printMovi(variations[i][dep]);
                if (movi.compareMovis(variations[i][dep][I_MOVI], variations[current_variation_index][dep][I_MOVI]) == -1 && (dep % 2 == 0)) {
                    match = false;
                    break;
                }
            }
            if (match) {
                current_variation_index = i;
                break;
            }
        }

        if (!match)
            n_variations++;
    }

    void updateVariationValue(int val) {       // update with lowest evaluation
        if (current_variation_index == 75) {
            //            printf("as");
        }
        if (variation_value[current_variation_index] == -1)
            variation_value[current_variation_index] = val;
        else if (variation_value[current_variation_index] > val) {
            variation_value[current_variation_index] = val;
        }
    }

    int getBestVariation() {
        int bestValue = -9999;
        int bestVar = -1;
        for (int i = 1; i <= n_variations; i++) {
            if (variation_value[i] > bestValue) {
                bestValue = variation_value[i];
                bestVar = i;
            }
        }
        return bestVar;
    }
};

Variation variation;

int eval2(int max_depth) {
    int positions_evaluated = 0;
    int z, bestValue, reply, thread_nro = 0;
    int newMove = -1;
    bool doSubstract = true;

    depth = 0;
    int maxim_depth = 3;
    int i_first_move = 0;
    int i_second_move = 0;
    int n_second_moves = 0;
    int var = 0;
    int current_movi = 0;
    int n_first_moves;
    int i_inner_movi;
    int last_inner_moves = 0;
    int n_previous_first_moves[20] = { 0 };
    int sum_first_moves = 0;
    init_player = current_player;
    int prev = false;
    variation.init_variation_value();

    z = evaluate(current_player, init_player, depth);

    for (int depth_reached = 0; depth_reached <= maxim_depth; depth_reached++) {
        printf("Depth %d movis analyzed %d\n", depth_reached, current_movi);

        if (depth_reached == maxim_depth) {
            variation.n_variation_moves = 0;
        }
        if (depth_reached == 0)     // first iteration
            n_first_moves = movi.movisFound;
        else {
            n_first_moves = n_second_moves;      // n of moves at greatest depth
            n_second_moves = 0;
        }
        for (i_first_move = 0; i_first_move < n_first_moves; i_first_move++) {  // first move

            if (movi.movi[0][3] > 10) {
                printf("");
            }

            while (depth > 0)
                moveHistory.reverseMoves(1);

            if (depth_reached == 0) {
                movi.playMovi(i_first_move);
                i_inner_movi = i_first_move;
            }
            else {
                i_inner_movi = movi.playLinkedMoves(movi.i_last_depth[i_first_move] + sum_first_moves);  // play linked moves to greatest depth reached
            }
            if (depth_reached == maxim_depth && (current_player != init_player)) { // actually '== current_player' but this is after move and player switch to get i_inner_movi value
                variation.setVariation(i_inner_movi);  // todo combine this with movi.link
                //variation.addVariation(i_inner_movi);
            }
            //printBoard();

            current_movi = movi.getIndexOfLastEntry(); // set starting index for inner loop movis
            //printBoard();
            z = evaluate(current_player, init_player, depth);
            
            if (depth_reached == maxim_depth) {       // set variation value according to best opponent reply
                variation.variation_value[variation.current_variation_index] = movi.movi[i_inner_movi][AVAL] - z;
                if (abs(z) >= 2 || abs(movi.movi[i_inner_movi][AVAL]) >= 2) {
                    //printBoard();
                    moveHistory.resetBoard();
                    //                variation.playVariation(variation.current_variation_index);
                    prev = variation.isVariationPreventable();
                    moveHistory.resetBoard();

                    if (prev) {
                        printf("value %d preventable %d\n", variation.variation_value[variation.current_variation_index], prev);
                        movi.playLinkedMovesOnDisplay(movi.i_last_depth[i_first_move] + sum_first_moves);
                        moveHistory.resetBoard();
                        printf("preventing move\n");
                        movi.playMovi(prev);
                        printBoard();
                        moveHistory.resetBoard();
                    }
                    movi.playLinkedMoves(movi.i_last_depth[i_first_move] + sum_first_moves);
                }
            }
            else {
                for (i_second_move = 0; i_second_move < movi.movisFound; i_second_move++) { // response move

                    current_movi++;

                    movi.playMovi(current_movi);
                    //printf("Movi %d, variation %d\n", current_movi, variations);
                   //printBoard();

                    //if(depth_reached < maxim_depth) 
                        movi.link(current_movi, i_inner_movi); // link to first played 

                    // get the best possible answer to each variation
                    //if ((current_player == init_player) && depth_reached == maxim_depth) // actually != but played after switch to get link
                    //    variation.updateVariationValue(movi.movi[current_movi][AVAL]);  // todo get this from evaluate-function directly

                    n_second_moves++;

                    moveHistory.reverseMoves(1);
                    //printBoard();
                }
            }
            moveHistory.reverseMoves(1);
        }
        
        if (depth_reached == 0) // first iteration analyzes 2(increment extra), after that 1
            depth_reached++;
        while (depth > 0)
            moveHistory.reverseMoves(1);

        if (depth_reached >= 2) {
            n_previous_first_moves[depth_reached] = n_first_moves;
            sum_first_moves = 0;
            for (int i = 0; i <= depth_reached - 2; i++)
                sum_first_moves += n_previous_first_moves[2 + i];
        }
    }
/*
    int best_value = -999, i_movi_best_valued_variation = 0;
    for (int i_last_depth_movi = (movi.n_last_depth - n_second_moves); i_last_depth_movi < movi.n_last_depth; i_last_depth_movi++) {
        if (movi.movi[i_last_depth_movi][VAL] > best_value) {
            best_value = movi.movi[i_last_depth_movi][VAL];
            i_movi_best_valued_variation = i_last_depth_movi;
        }
    }
    */
    variation.n_variations = variation.current_variation_index;

    int best_value = -9999, i_best = 0;
    for (int i_variation = 0; i_variation < variation.n_variations; i_variation++) {
        if (variation.variation_value[i_variation] > best_value) {
            best_value = variation.variation_value[i_variation];
            i_best = i_variation;
        }
    }    
    variation.current_variation_index = i_best;
    
    variation.isVariationPreventable();
    for (int dep = 0; dep < maxim_depth; dep += 2) {
        for (int i_var_move = 0; i_var_move < variation.n_var_moves_at_depth[dep]; i_var_move++) {


        }

    }
    
    int best_var = variation.getBestVariation();
    movi.playLinkedMovesOnDisplay(variation.variations[best_var][2][I_MOVI]);
    printf("Variation nro %d movi nro %d value %d", best_var, variation.variations[best_var], variation.variation_value[best_var]);

    return 0;
}



int main()
{
    findKings();

    int z = 0;

    //z = evalTurnPair();
    z = eval2(3);

    printf("total score: %d\n", z);
    //    n = evaluate(-1); // evaluate for player 1(white)


}


// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
