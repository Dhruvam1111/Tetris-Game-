#include <iostream>
#include <vector>
#include <conio.h>
#include <windows.h>
#include <ctime>
#include <fstream>
using namespace std;
const int W = 10;
const int H = 20;
const int INIT_SPD = 500;
const int SPD_INC = 50;
const int MIN_SPD = 100;
const int LINES_LVL = 5;

// Declaring tetrominoes
vector<vector<vector<int>>> tets = {
    {{1, 1, 1, 1}},         // I
    {{1, 1}, {1, 1}},       // O
    {{1, 1, 1}, {0, 1, 0}}, // T
    {{0, 1, 1}, {1, 1, 0}}, // S
    {{1, 1, 0}, {0, 1, 1}}, // Z
    {{1, 0, 0}, {1, 1, 1}}, // L
    {{0, 0, 1}, {1, 1, 1}}  // J
};

vector<vector<int>> brd(H, vector<int>(W, 0));
int curX, curY, curPiece, nxtPiece;
vector<vector<int>> curTet;
bool gameOver = false;
bool isPaused = false; // Added pause state
int scr = 0;
int dropSpd = INIT_SPD;
int linesClr = 0;
int lvl = 1;
DWORD lastFall = 0;
string plyrName;
int hiScr = 0;
bool smashUsed = false;

// Function prototypes
void gotoXY(int x, int y);
void hideCur();
void setCol(int fg, int bg = 0);
int getCol(int piece);
bool isValid(int x, int y, const vector<vector<int>> &shape);
void place();
void clrLines();
vector<vector<int>> rot(const vector<vector<int>> &shape);
void spawn();
void hardDrp();
void drawBrd();
void handleInp();
void loadHiScr();
void saveHiScr();
void gameLp();
void drawNum(int num);
void cntdwn();
void playSnd(int freq, int dur);
void drawPauseScreen(); // Added function for pause screen

// Function definitions
void gotoXY(int x, int y)
{
    COORD coord = {static_cast<SHORT>(x), static_cast<SHORT>(y)};
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}

void hideCur()
{
    HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO cursorInfo = {1, false};
    SetConsoleCursorInfo(console, &cursorInfo);
}

void setCol(int fg, int bg)
{
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), fg | (bg << 4));
}

int getCol(int piece)
{
    int colors[] = {11, 14, 13, 10, 12, 6, 9};
    return colors[piece % 7];
}

bool isValid(int x, int y, const vector<vector<int>> &shape)
{
    for (size_t i = 0; i < shape.size(); i++)
    {
        for (size_t j = 0; j < shape[i].size(); j++)
        {
            if (shape[i][j])
            {
                int newX = x + j;
                int newY = y + i;
                if (newX < 0 || newX >= W || newY >= H || (newY >= 0 && brd[newY][newX]))
                {
                    return false;
                }
            }
        }
    }
    return true;
}

void place()
{
    for (size_t i = 0; i < curTet.size(); i++)
    {
        for (size_t j = 0; j < curTet[i].size(); j++)
        {
            if (curTet[i][j])
            {
                brd[curY + i][curX + j] = curPiece + 1;
            }
        }
    }
}

void clrLines()
{
    int cleared = 0;
    for (int i = H - 1; i >= 0; i--)
    {
        bool fullLine = true;
        for (int j = 0; j < W; j++)
        {
            if (!brd[i][j])
            {
                fullLine = false;
                break;
            }
        }

        if (fullLine)
        {
            cleared++;
            for (int k = i; k > 0; k--)
            {
                brd[k] = brd[k - 1];
            }
            brd[0] = vector<int>(W, 0);
            i++;
        }
    }

    if (cleared > 0)
    {
        scr += cleared * 100;
        linesClr += cleared;

        if (linesClr >= LINES_LVL * lvl)
        {
            lvl++;
            dropSpd = max(MIN_SPD, dropSpd - SPD_INC);
        }

        playSnd(800, 200);
    }
}

vector<vector<int>> rot(const vector<vector<int>> &shape)
{
    size_t rows = shape.size(), cols = shape[0].size();
    vector<vector<int>> rotated(cols, vector<int>(rows, 0));
    for (size_t i = 0; i < rows; i++)
        for (size_t j = 0; j < cols; j++)
            rotated[j][rows - 1 - i] = shape[i][j];
    return rotated;
}

void spawn()
{
    curPiece = nxtPiece;
    nxtPiece = rand() % tets.size();
    curTet = tets[curPiece];
    curX = W / 2 - curTet[0].size() / 2;
    curY = 0;
    if (!isValid(curX, curY, curTet))
        gameOver = true;
}

void hardDrp()
{
    playSnd(600, 300);
    while (isValid(curX, curY + 1, curTet))
    {
        curY++;
    }
    place();
    clrLines();
    spawn();
}

void drawPauseScreen()
{
    int centerX = W;
    int centerY = H / 2;

    setCol(14);
    gotoXY(centerX - 4, centerY - 1);
    cout << "===========";
    gotoXY(centerX - 4, centerY);
    cout << "|  PAUSED |";
    gotoXY(centerX - 4, centerY + 1);
    cout << "===========";
    gotoXY(centerX - 10, centerY + 3);
    cout << "Press P to continue playing";
}

void drawBrd()
{
    gotoXY(0, 0);
    setCol(15);
    cout << plyrName << "'s Score: " << scr << "   Level: " << lvl << "   High Score: " << hiScr << "\n";

    for (int i = 0; i < H; i++)
    {
        cout << "|";
        for (int j = 0; j < W; j++)
        {
            bool isTet = false;
            for (size_t ti = 0; ti < curTet.size(); ti++)
            {
                for (size_t tj = 0; tj < curTet[ti].size(); tj++)
                {
                    if (curTet[ti][tj] && curY + ti == i && curX + tj == j)
                    {
                        setCol(getCol(curPiece));
                        cout << "[]";
                        isTet = true;
                        break;
                    }
                }
                if (isTet)
                    break;
            }
            if (!isTet)
            {
                if (brd[i][j])
                {
                    setCol(getCol(brd[i][j] - 1));
                    cout << "[]";
                }
                else
                {
                    setCol(8);
                    cout << " .";
                }
            }
        }
        setCol(15);
        cout << "|\n";
    }

    for (int i = 0; i < 4; i++)
    {
        gotoXY(W * 2 + 5, 3 + i);
        cout << "        ";
    }

    setCol(14);
    gotoXY(W * 2 + 5, 2);
    cout << "Next Piece:";
    for (size_t i = 0; i < tets[nxtPiece].size(); i++)
    {
        gotoXY(W * 2 + 5, 3 + i);
        for (size_t j = 0; j < tets[nxtPiece][i].size(); j++)
        {
            if (tets[nxtPiece][i][j])
            {
                setCol(getCol(nxtPiece));
                cout << "[]";
            }
            else
            {
                cout << "  ";
            }
        }
    }

    setCol(14);
    gotoXY(W * 2 + 5, 15);
    cout << "RULE BOOK:";
    gotoXY(W * 2 + 5, 16);
    cout << "Left Arrow  - Move Left";
    gotoXY(W * 2 + 5, 17);
    cout << "Right Arrow - Move Right";
    gotoXY(W * 2 + 5, 18);
    cout << "Up Arrow    - Rotate";
    gotoXY(W * 2 + 5, 19);
    cout << "Down Arrow  - Soft Drop";
    gotoXY(W * 2 + 5, 20);
    cout << "Spacebar    - Hard Drop";
    gotoXY(W * 2 + 5, 21);
    cout << "P           - Pause Game";
    gotoXY(W * 2 + 5, 22);
    cout << "S           - Smash (Once per game, requires 500 points)";
    gotoXY(W * 2 + 5, 23);
    cout << "Q           - Quit";
}

void handleInp()
{
    if (_kbhit())
    {
        int key = _getch();
        if (key == 'q' || key == 'Q')
            gameOver = true;
        if (key == 'p' || key == 'P')
        { // Pause toggle
            isPaused = !isPaused;
            if (isPaused)
            {
                playSnd(400, 200);
            }
            else
            {
                playSnd(600, 200);
                lastFall = GetTickCount(); // Reset fall timer when unpausing
            }
        }
        if (!isPaused)
        { // Only process other inputs when not paused
            if (key == 32)
                hardDrp();
            if (key == 's' || key == 'S')
            {
                if (scr >= 500 && !smashUsed)
                {
                    for (int i = 0; i < H; i++)
                    {
                        for (int j = 0; j < W; j++)
                        {
                            brd[i][j] = 0;
                        }
                    }
                    smashUsed = true;
                    playSnd(1000, 300);
                }
            }
            if (key == 224)
            {
                key = _getch();
                if (key == 75 && isValid(curX - 1, curY, curTet))
                {
                    curX--;
                }
                if (key == 77 && isValid(curX + 1, curY, curTet))
                {
                    curX++;
                }
                if (key == 80 && isValid(curX, curY + 1, curTet))
                {
                    curY++;
                }
                if (key == 72)
                {
                    vector<vector<int>> rotated = rot(curTet);
                    if (isValid(curX, curY, rotated))
                    {
                        curTet = rotated;
                    }
                }
            }
        }
    }
}

void loadHiScr()
{
    ifstream file("highscore.txt");
    if (file.is_open())
    {
        file >> hiScr;
        file.close();
    }
}

void saveHiScr()
{
    ofstream file("highscore.txt");
    if (file.is_open())
    {
        file << hiScr;
        file.close();
    }
}

void playSnd(int freq, int dur)
{
    Beep(freq, dur);
    Sleep(50);
}

void drawNum(int num)
{
    vector<string> num3 = {
        "*",
        "    *",
        "*",
        "    *",
        "*"};
    vector<string> num2 = {
        "*",
        "    *",
        "",
        "*    ",
        "*"};
    vector<string> num1 = {
        "  *  ",
        "  *  ",
        "  *  ",
        "  *  ",
        "  *  "};
    vector<string> start = {
        "*****  *****  ***  ****  *****",
        "*        *   *   * *   *   *  ",
        "*****    *   ***** ****    *  ",
        "    *    *   *   * *  *    *  ",
        "*****    *   *   * *   *   *  "};

    vector<string> design;
    switch (num)
    {
    case 3:
        design = num3;
        break;
    case 2:
        design = num2;
        break;
    case 1:
        design = num1;
        break;
    case 0:
        design = start;
        break;
    }

    int startY = H / 2 - 2;
    int startX = W * 2 + 5;

    for (size_t i = 0; i < design.size(); i++)
    {
        gotoXY(startX, startY + i);
        cout << design[i];
    }
}

void cntdwn()
{
    system("cls");
    setCol(14);
    for (int i = 3; i >= 1; i--)
    {
        drawNum(i);
        Sleep(1000);
        system("cls");
    }
    drawNum(0);
    Sleep(500);
    system("cls");
}

void gameLp()
{
    lastFall = GetTickCount();
    while (!gameOver)
    {
        handleInp();

        if (isPaused)
        {
            drawPauseScreen();
            while (isPaused && !gameOver)
            {
                handleInp();
                Sleep(100);
            }
            if (gameOver)
                break;
            system("cls"); // Clear the pause screen
            continue;
        }

        if (GetTickCount() - lastFall >= dropSpd)
        {
            if (isValid(curX, curY + 1, curTet))
            {
                curY++;
            }
            else
            {
                place();
                clrLines();
                spawn();
            }
            lastFall = GetTickCount();
        }
        drawBrd();
        Sleep(10);
    }

    if (scr > hiScr)
    {
        hiScr = scr;
        saveHiScr();
    }

    playSnd(200, 500);
}

int main()
{
    loadHiScr();

    system("cls");
    setCol(14);
    cout << "===== TETRIS GAME =====\n";
    setCol(15);
    cout << "Enter player name: ";
    cin >> plyrName;

    while (true)
    {
        hideCur();
        srand(time(0));
        nxtPiece = rand() % tets.size();
        cntdwn();
        system("cls");
        spawn();
        gameLp();

        setCol(12);
        cout << "\nGAME OVER. Final Score: " << scr << "\n";
        setCol(15);
        cout << "Play Again? (Y/N): ";
        char c;
        cin >> c;
        if (c != 'y' && c != 'Y')
            break;
        gameOver = false;
        isPaused = false; // Reset pause state
        scr = 0;
        lvl = 1;
        linesClr = 0;
        smashUsed = false;
        brd = vector<vector<int>>(H, vector<int>(W, 0));
    }
}