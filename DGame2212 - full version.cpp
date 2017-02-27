#include <windows.h>
#include <stdio.h>
#include <assert.h>
#include <math.h>
#include <iostream>
#include "bass.h"

using namespace std;

enum ScreenType {SCR_MAIN, SCR_LEVELS, SCR_GAME};
enum UnitType {U_FAST, U_MEDIUM, U_STRONG};
enum TowerType {TW_GUN, TW_LASER, TW_SLOW, TW_ROCKET, TW_BOOST};
enum ControlState {NONE, NEW_TOWER, TOWER_SELECTED};

HWND hMainWnd;
HINSTANCE hInst;

HSAMPLE samp;
HCHANNEL ch;

const int UNIT_TYPE_COUNT = 3;
const int TOWER_TYPE_COUNT = 5;
const int MAX_LENGTH_STRING = 30;
const int WINDOW_W = 780;
const int WINDOW_H = 700;
const int TOP_GAME_BLOCK_H = 0.10 * WINDOW_H;
const int BOTTOM_GAME_BLOCK_H = 0.15 * WINDOW_H;
const int BUTTON_BASE_Y = 200;
const int BUTTON_HEIGHT = 30;
const int BUTTON_WIDTH = 280;
const int BUTTON_BASE_X = (WINDOW_W - BUTTON_WIDTH) / 2;
const int BUTTON_V_SPACE = 40;
const int HOLE_R = 17;
const HMENU BT_GAME_LEVELS_ID = (HMENU) 1;
const int LEVEL_COUNT = 4;
const HMENU BT_LEVEL[LEVEL_COUNT + 1] = {(HMENU)10, (HMENU)11, (HMENU)12, (HMENU)13, (HMENU)14};
const HMENU BT_GAME_MAINMENU_ID = (HMENU) 4;
const int BT_GAME_RETRY_ID = 5;
const int BT_GAME_EXIT_ID = 6;
const int BT_GAME_PAUSE_ID = 7;
const int BT_GAME_SELL_ID = 8;
const int BT_GAME_UPGRADE_ID = 9;
const int BT_GAME_CONTINUE_ID = 10;
const int BT_GAME_SELECT_LEVEL_ID = 11;
const int BT_GAME_NEXT_WAVE_ID = 12;
const int BT_GAME_SPEED_UP_ID = 13;
const COLORREF LABEL_TEXT_COLOR = RGB(0,0,0);
const COLORREF TITLE_TEXT_COLOR = RGB(255,0,0);
const COLORREF PATH_LINE_COLOR = RGB(130,130,130);
const int MAX_PATH_LENGTH = 50;
const int MAX_TOWERS = 100;
const int MAX_WAVES = 10;
const int MAX_UNITS_PER_WAVE = 1050;
const int MAX_TOWER_TYPES = 5;
const int MAX_TOWER_LEVEL = 6;
double TICK_MILLIS = 2; //! Чтобы можно было ускорять
const double REDRAW_AFTER_TICKS = 1;
const int GAME_TIMER = 1;
const int TOWER_R = 15;
const int TOWER_BUTTON_PLACE_W = 45;
const int DIGIT_GAP = 12;
const int GUN_EXTRA_LEN_PERCENT = 50;
const int MAX_SHOTS = 10000;
const int ADD_SCORE_NEXT_SECOND = 1;

const double GUN_SHOT_R[MAX_TOWER_LEVEL] = {3, 3.1, 3.2, 3.3, 3.4, 3.5}; //!!!Q
const double GUN_TOWER_OBVOD_HEIGHT[MAX_TOWER_LEVEL] = {1, 1.5, 2, 2.5, 3, 3.5};
const double GUN_FREQ[MAX_TOWER_LEVEL] = {1, 1, 1.2, 1.3, 1.4, 1.5};
const int GUN_UPGRADE_COST[MAX_TOWER_LEVEL] = {20, 30, 60, 60, 100, 0};
const int GUN_SELL_COST[MAX_TOWER_LEVEL] = {10, 20, 25, 50, 80, 120};
const double GUN_DAMAGE[MAX_TOWER_LEVEL] = {1, 2, 3, 5, 7, 10};
const double GUN_DAMAGE_R[MAX_TOWER_LEVEL] = {1, 1, 1, 1.1, 1.3, 1.5};

const int LAZER_SHOT_R = 3;
const int UNIT_R = 12;
const int HP_UP_DIST = 18;
const int HP_WIDTH = 26;
const int HP_HEIGHT = 4;
const int MAX_BUTTONS_COUNT = 50;
const int BUTTON_GAP = 12;
const int BUTTON_H = 30;
const int PANEL_H = 480;
const int PANEL_W = 400;
const int PANEL_TEXT_H = 100;
const int MILLIS_IN_SECOND = 1000;
const int RAMKA_D = 10;
const int LASER_BARR_R = 3;
const double PI = acos(-1);
const int MAX_UNITS_COUNT = 1000;
const int LASER_SHOT_R = 4;
const int ROCKET_SHOT_R = 10;
const double ROCKET_SHOT_ANGLE[] = {PI / 2, 2 * PI / 3 + PI / 2, 4 * PI / 3 + PI / 2};
const double UNIT_FAST_ANGLE[] = {PI / 2, 3 * PI / 4 + PI / 2, 3 * PI / 4 + PI}; //!relative OX
const double K_GRAVITY_ROCKET_SHOT = 20;
const double BANG_TIME_TICK = 200;
const double LASER_SHOW_TIME_TICK = 500;
const int BANG_R = 10;
const double MAX_ANGLE_UNITS_ROTATION = PI / 6;
const double ANGLE_UNITS_ROTATION_IN_TICK = PI / 500;
const double UNIT_MEDIUM_INSIDE_R = UNIT_R / 2;
const int UNIT_MEDIUM_RAYS_COUNT = 6;
const double UNIT_MEDIUM_RAY_HEIGHT = 2;
const double UNIT_STRONG_MAX_ROOF = UNIT_R / 4;
const double UNIT_STRONG_ADD_ROOF_IN_TICK = 0.02;
const int TARG_R = 3;

void testCreatePath();
void runAllTests();

struct String
{
    char s[MAX_LENGTH_STRING];
    int length;
    String (char *new_s, int new_length)
    {
        length = new_length;
        for (int i = 0; i < length; i++)
        {
            s[i] = new_s[i];
        }
    }
    void read()
    {
        scanf("%c", &s[0]);
        while (s[0] == '/n' || s[0] == ' ' || (int)s[0] == 10 || (int)s[0] == 13) {
            scanf("%c", &s[0]);
        }
        length = 1;
        char c;
        while (scanf("%c", &c) != EOF)
        {
            if (c == '/n' || c == ' ' || (int)c == 10 || (int)c == 13) {
                break;
            }
            s[length] = c;
            length++;
        }
        assert(length != 0);
    }
    bool equal(String t)
    {
        if (t.length != length) {
            return false;
        }
        for (int i = 0; i < length; i++) {
            if (s[i] != t.s[i]) {
                return false;
            }
        }
        return true;
    }
};

const String NAME_TOWER[] = { {"TW_GUN", 6}, {"TW_LASER", 8}, {"TW_SLOW", 7}, {"TW_ROCKET", 9}, {"TW_BOOST", 8} };
TowerType ID_TOWER_TYPE[] = {TW_GUN, TW_LASER, TW_SLOW, TW_ROCKET, TW_BOOST};
const String NAME_UNIT[] = { {"U_FAST", 6}, {"U_MEDIUM", 8}, {"U_STRONG", 8} };
UnitType ID_UNIT_TYPE[] = {U_FAST, U_MEDIUM, U_STRONG};

//start geometry code - candidate for moving to separate file

struct Point
{
    double x;
    double y;
};

Point minus_vect(Point p1, Point p2)
{
    Point result;
    result.x = p1.x - p2.x;
    result.y = p1.y - p2.y;
    return result;
}

Point plus_vect(Point p1, Point p2)
{
    Point result;
    result.x = p1.x + p2.x;
    result.y = p1.y + p2.y;
    return result;
}

double smul(Point v1, Point v2)
{
    return v1.x * v2.x + v1.y * v2.y;
}

double vmul(Point v1, Point v2)
{
    return v1.x * v2.y - v2.x * v1.y;
}

double dist_of_vect(Point v) {
    return sqrt(v.x * v.x + v.y * v.y);
}

double dist(Point p1, Point p2)
{
    return sqrt(pow(p1.x - p2.x, 2) + pow(p1.y - p2.y, 2));
}

POINT polarToDecart(Point center, double r, double alpha) { //! Угол между осью OX и лучём
    POINT res;
    res.x = center.x + r * cos(alpha);
    res.y = center.y - r * sin(alpha);
    return res;
}

//end geometry code

struct Wave
{
    int delayTick;
    int periodTick;
    UnitType units[MAX_UNITS_PER_WAVE];
    int unitCount;
};

struct TowerDesc
{
    TowerType type;
    int basePrice;
    int maxLevel;
};

struct Path
{
    Point points[MAX_PATH_LENGTH];
    int length;
};

struct Level
{
    int wavesCount;
    Wave waves[MAX_WAVES];
    Path path;
    int lifeCount;
    double healthK;
    double rewardK;
    int startMoney;
    int towerTypesCount;
    TowerDesc towerTypes[MAX_TOWER_TYPES];
};

struct Unit
{
    Point pos;
    int hp;
    int slowTimeLeft[MAX_TOWER_LEVEL];
    UnitType type;
    bool active;
    double startTick;
    double angle_rotation;
    bool clockWise;
    int id;
    double roof;//! Нужно только для Strong Unit, double чтобы равномерно всё было
    bool add_roof;//! Поэтому странно это добавлять в поля ВСЕХ юнитов.
    int maxHp; //! Для правильно рисования жизней Юнита
    int startWave;
};

struct Tower
{
    Point pos;
    int lastFocusX;
    int lastFocusY;
    int level;
    TowerType type;
    int lastShotTick;
    int lastFocusedUnit;
    int boostTowerId;
    bool selected;
    bool underCursor;
    bool active;
    int maxLevel;
};

struct Shot
{
    Point pos;
    Point v;
    int focusedUnit;
    int focusedUnitId;
    int tower;
    bool boost;
    int level;
    bool bang;
    int bang_begin_tick;
};

struct Button
{
    Point pos;
    int w;
    int h;
    char text[20];
    bool active;
    int id;
    bool underCursor;
    bool pressed;
};

struct Game
{
    HWND btGameLevels;
    HWND btLevel[LEVEL_COUNT + 1];
    HWND btGameMainMenu;
    HWND btOptions;
    HWND btResume;
    HWND btExit;
    HWND btPause;

    int points;
    int score;
    int curWave;
    int money;
    ScreenType curScreen;
    int curLevel;
    int lifeCount;
    int curTowerCount;
    int curUnitCount;
    int curUnitId;
    Unit units[MAX_UNITS_COUNT];
    Tower towers[MAX_TOWERS];

    Level level;

    double tick;
    double tickDiff;
    DWORD tickMillis;
    int lastDrawTick;

    HBRUSH buttonPressedBrush;
    HPEN pathPen;
    HFONT titleFont;
    HFONT labelFont;
    HPEN fastUnitPen;
    HBRUSH fastUnitBrush;
    HPEN mediumUnitPen;
    HPEN mediumUnitRayPen;
    HBRUSH mediumUnitBrush;
    HPEN strongUnitPen;
    HBRUSH strongUnitBrush;

    HPEN gunTowerPen[MAX_TOWER_LEVEL];
    HBRUSH gunTowerBrush[MAX_TOWER_LEVEL];
    HPEN gunTowerObvodPen[MAX_TOWER_LEVEL];

    HPEN damageAreaPen;
    HBRUSH hpFrontBrush;
    HBRUSH hpBackBrush;
    HPEN wrongPositionPen;
    HPEN buttonPen;
    HPEN focusedButtonPen;
    HPEN laserTowerPen;
    HBRUSH laserTowerBrush;
    HPEN laserShotPen;
    HPEN rocketShotPen;
    HPEN rocketTowerPen;
    HBRUSH rocketTowerBrush;
    HBRUSH rocketShotBrush;

    int curWaveStartTick;
    int curWaveFinishTick;
    int newTowerDescIndex;
    int selectedTowerIndex;
    int towerButtonDrawStartX;
    ControlState curControlState;
    int newTowerTypeIndex;
    int newTowerX;
    int newTowerY;
    bool canPlaceNewTower;
    Shot shots[MAX_SHOTS];
    int curShotCount;
    Button buttons[MAX_BUTTONS_COUNT];
    int curButtonCount;
    int panelButtonsCount;
    Button panelButtons[MAX_BUTTONS_COUNT];
    bool panelActive;
    char panelText[50];

    void init();
    void initPanelButtons();
    void initLevelButtons();
    void openScreenMain();
    void openScreenLevels();
    void openScreenGame();
    void closeScreenMain();
    void closeScreenLevels();
    void closeScreenGame();
    void showEndPanel(bool win);
    void configurePanelButton(int index, char* text, int id);
    int processMessage(UINT msg, WPARAM wParam, LPARAM lParam);

    void draw(HDC hdc);
    void drawScreenMain(HDC hdc);
    void drawScreenLevels(HDC hdc);
    void drawScreenGame(HDC hdc);
    void drawScreenTitle(char* title, HDC hdc);
    void drawPath(HDC hdc);
    void drawEnd(HDC hdc);
    void drawUnits(HDC hdc);
    void drawUnit(HDC hdc, Unit unit);
    void drawUnitLeavesStreap(HDC hdc, Unit unit);
    void drawFastUnit(HDC hdc, Unit unit);
    void drawMediumUnit(HDC hdc, Unit unit);
    void drawStrongUnit(HDC hdc, Unit unit);
    void drawShots(HDC hdc);
    void drawShot(HDC hdc, Shot shot);
    void drawShotGun(HDC hdc, Shot shot);
    void drawShotLaser(HDC hdc, Shot shot);
    void drawShotRocket(HDC hdc, Shot shot);
    void drawButtons(HDC);
    void drawButton(HDC hdc, Button button);
    void drawSelectedTowerInfo(HDC hdc);
    void drawPanel(HDC hdc);
    void drawTimeToNextWave (HDC hdc);
    void drawScore(HDC hdc);
    void drawTowerButtons(HDC hdc);
    void drawTower(HDC hdc, Tower tower);
    void drawTowerGun(HDC hdc, Tower tower);
    void drawTowerLaser(HDC hdc, Tower tower);
    void drawTowerRocket(HDC hdc, Tower tower);
    void drawBlockBorders(HDC hdc);
    void drawNewTower(HDC hdc);
    void drawLabelText(char* text, HDC hdc, int x0, int y0, int x1, int y1, int flags);
    void redraw();

    void update();
    void updateModelTime();
    void updateUnits();
    void updateUnitsPos();
    void updateUnitsFeatures();
    void updateUnitsActivity();
    void updateFlyingShots();
    void updateNewShots();
    void updateExplodeShots();
    void updateTowersFocus();
    void updateTowers();
    void updateNewTower();
    void updateWave();
    void updateShots();
    void updateButtons(Button* barr, int count);

    int getDamageR(Tower t);
    int getDamage(Shot s);
    int getButtonIndexById(int id);
    int getTowerUpgradeCost(Tower t);
    int getTowerSellPrice(Tower t);
    int getUnitPrice(Unit u);
    int getUnitScore(Unit u);
    double getShotPeriod(Tower tower);
    double getShotSpeed(Shot shot);
    double getUnitSpeed(Unit u);
    Point getCursorPos();

    void setButtonTextById(int id, char* text);
    void setDefaultControlState();
    void setPanelVisible(bool visible);
    void setCurLevel(int curLevel);
    void setButtonActiveById(int id, bool active);

    Level createLevel(int numberLevel);
    void clearDeadUnits();
    int calcInitUnitHp(UnitType type, int waveNumber, double healthK);
    int calcReward(UnitType type, int waveNumber, double rewardK);
    int calcPathLength();
    int calcButtonY0(int buttonNumber);
    Path createPath(int length, int* coordinates);
    int focusTower(Tower t);
    bool intersects(Unit u, Shot shot);
    double getMinDistFromPath(Point p);
    void startPlaceNewTower(int towerTypeIndex);
    void tryPlaceNewTower();
    void buttonPressed(int id);
    void towerSelected(int i);
    bool checkWin();
    bool checkLoose();

    TowerType stringToTowerType(String nameTower);
    UnitType stringToUnitType(String nameUnit);
};

Game game;

LRESULT CALLBACK WndProc(HWND hWnd,
 						 UINT uMsg,
						 WPARAM wParam,
						 LPARAM lParam)
{
	HDC hdc;
    PAINTSTRUCT ps;
    HDC hdcMem;
    HBITMAP bitMap;
    HBITMAP oldBitMap;
    RECT rect;
    GetClientRect(hWnd, &rect);

	switch (uMsg)
	{
		case WM_PAINT:
			hdc = BeginPaint(hWnd, &ps);


            hdcMem = CreateCompatibleDC(hdc);
            bitMap = CreateCompatibleBitmap(hdc, WINDOW_W, WINDOW_H);
            oldBitMap = (HBITMAP) SelectObject(hdcMem, bitMap);

            PatBlt(hdcMem, 0, 0, WINDOW_W, WINDOW_H, WHITENESS);

            game.draw(hdcMem);

            BitBlt(hdc, 0, 0, WINDOW_W, WINDOW_H, hdcMem, 0, 0, SRCCOPY);

            SelectObject(hdcMem, oldBitMap);
            DeleteObject(bitMap);
            DeleteDC(hdcMem);

			EndPaint(hWnd, &ps);
            break;
        case WM_ERASEBKGND:
            return (LRESULT) 1;
        case WM_TIMER:
            game.update();
            break;
        case WM_COMMAND:
            printf("COMMAND RECEIVED loword_wparam = %d\n", LOWORD(wParam));

                if ((HWND)lParam == game.btGameLevels)
                {
                    printf("BUTTON GAMELEVELS CLICKED\n");
                    game.closeScreenMain();
                    game.openScreenLevels();
                } else
                if ((HWND)lParam == game.btGameMainMenu)
                {
                    printf("BUTTON MAINMENU CLICKED\n");
                    game.closeScreenLevels();
                    game.openScreenMain();
                } else
                if ((HWND)lParam == game.btPause)
                {
                    ShowWindow(game.btResume, SW_SHOWNORMAL);
                    ShowWindow(game.btExit, SW_SHOWNORMAL);
                } else
                if ((HWND)lParam == game.btExit)
                {
                    game.closeScreenGame();
                    game.openScreenLevels();
                } else
                if ((HWND)lParam == game.btResume)
                {
                    ShowWindow(game.btResume, SW_HIDE);
                    ShowWindow(game.btExit, SW_HIDE);
                }
                else
                {
                    for (int i = 1; i <= LEVEL_COUNT; i++) {
                        if ((HWND)lParam == game.btLevel[i]) {
                            game.closeScreenLevels();
                            game.openScreenGame();
                            game.setCurLevel(i);
                        }
                    }
                }
                break;
		default:
		    if (!game.processMessage(uMsg, wParam, lParam)) {
                return DefWindowProc( hWnd, uMsg, wParam, lParam );
            }
	}
	return NULL;
};

int WINAPI WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow )
{
    runAllTests();
    hInst = hInstance;

	WNDCLASSEX wc;
	wc.cbSize = sizeof(wc);
	wc.lpfnWndProc = WndProc;
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpszMenuName = NULL;
	wc.lpszClassName =  "Hello Window Class";
	wc.cbWndExtra = NULL;
	wc.cbClsExtra = NULL;
	wc.hIcon = LoadIcon(NULL, IDI_WINLOGO);
	wc.hIconSm = LoadIcon(NULL, IDI_WINLOGO);
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wc.hInstance = hInstance;

	if (!RegisterClassEx(&wc))
	{
		MessageBox( NULL, "error", "Window", MB_OK);
		return NULL;
	}

	hMainWnd = CreateWindow(wc.lpszClassName,
							"DGame",
							WS_VISIBLE | WS_SYSMENU | WS_MINIMIZEBOX,
							400,
							100,
							WINDOW_W,
							WINDOW_H,
							HWND(NULL),
							NULL,
							HINSTANCE(hInstance),
							NULL);

    if (hMainWnd == 0)
	{
		MessageBox(NULL, "error, cant create window", "Window", MB_OK);
		return NULL;
	}

    RECT wndRect;
    RECT clientRect;

    GetWindowRect(hMainWnd, &wndRect);
    GetClientRect(hMainWnd, &clientRect);

    int iW = WINDOW_W + (wndRect.right - wndRect.left) - (clientRect.right - clientRect.left);
    int iH = WINDOW_H + (wndRect.bottom - wndRect.top) - (clientRect.bottom - clientRect.top);

    MoveWindow(hMainWnd, wndRect.left, wndRect.top, iW, iH, true);

    if (HIWORD(BASS_GetVersion())!=BASSVERSION) {
        MessageBox (NULL, "Ошибка версии BASS.", NULL, 0);
        return 1;
    }
    if (!BASS_Init (-1, 22050, BASS_DEVICE_3D , 0, NULL)) {
        MessageBox (NULL, "Не удалось инициализировать BASS.", NULL, 0);
        return 1;
    }

	ShowWindow(hMainWnd, nCmdShow);

	game.init();
	game.openScreenMain();

    MSG msg;

	while (GetMessage(&msg, NULL, NULL, NULL))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

    BASS_Free ();

	return msg.wParam;
};

void Game::draw(HDC hdc)
{
    if (curScreen == SCR_MAIN)
    {
        drawScreenMain(hdc);
    } else if (curScreen == SCR_LEVELS)
    {
        drawScreenLevels(hdc);
    } else if (curScreen == SCR_GAME)
    {
        drawScreenGame(hdc);
    }
}

void Game::drawScreenMain(HDC hdc)
{
    drawScreenTitle("DGame", hdc);
}

void Game::drawScreenLevels(HDC hdc)
{
    drawScreenTitle("Select level", hdc);
}

void Game::drawScreenGame(HDC hdc)
{
    drawScreenTitle("Level 1", hdc);
    char tmp[50];
    sprintf(tmp, "Wave: %d of %d", game.curWave + 1, game.level.wavesCount);
    game.drawLabelText(tmp, hdc, 0, 0, WINDOW_W, TOP_GAME_BLOCK_H, DT_CENTER | DT_VCENTER);
    drawTimeToNextWave(hdc);
    drawScore(hdc);
    sprintf(tmp, "$ %d", game.money);
    game.drawLabelText(tmp, hdc, 0, WINDOW_H - BOTTOM_GAME_BLOCK_H, WINDOW_W, WINDOW_H, DT_CENTER | DT_TOP);
    drawPath(hdc);
    drawUnits(hdc);
    drawEnd(hdc);
    if (curControlState == TOWER_SELECTED)
    {
        drawSelectedTowerInfo(hdc);
    }
    else
    {
       drawTowerButtons(hdc);
    }
    drawBlockBorders(hdc);
    for (int i = 0; i < curTowerCount; i++)
    {
        if (towers[i].active)
        {
            drawTower(hdc, towers[i]);
        }
    }
    drawButtons(hdc);
    drawShots(hdc);
    if (curControlState == NEW_TOWER)
    {
        drawNewTower(hdc);
    }
    if (panelActive)
    {
        drawPanel(hdc);
    }
}

void Game::drawTimeToNextWave (HDC hdc)
{
    char tmp[20];
    if (curWave == level.wavesCount - 1) {
        sprintf(tmp, "Last wave");
    }
    else {
        sprintf(tmp, "Next wave: %d", (int)((game.curWaveFinishTick - game.tick) * TICK_MILLIS + MILLIS_IN_SECOND) / MILLIS_IN_SECOND);
        //! Перевод из тиков в секунды с Округлением числа секунд в большую сторону.
    }
    game.drawLabelText(tmp, hdc, 0, 0, WINDOW_W, TOP_GAME_BLOCK_H, DT_LEFT | DT_VCENTER);
}

void Game::drawScore(HDC hdc)
{
    char tmp[20];
    sprintf(tmp, "Score %d", score);
    game.drawLabelText(tmp, hdc, 0, 0, WINDOW_W - RAMKA_D, TOP_GAME_BLOCK_H, DT_RIGHT | DT_VCENTER);
}

void Game::drawButtons(HDC hdc)
{
    for (int i = 0; i < curButtonCount; i++)
    {
        if (buttons[i].active)
        {
            drawButton(hdc, buttons[i]);
        }
    }
}

void Game::drawButton(HDC hdc, Button b)
{
    if (b.pressed == true) {
        SelectObject(hdc, buttonPressedBrush);
    } else {
        SelectObject(hdc, GetStockObject(NULL_BRUSH));
    }
    if (b.underCursor)
    {
        SelectObject(hdc, focusedButtonPen);
    }
    else
    {
        SelectObject(hdc, buttonPen);
    }
    Rectangle(hdc, b.pos.x, b.pos.y, b.pos.x + b.w, b.pos.y + b.h);

    drawLabelText(b.text, hdc, b.pos.x, b.pos.y, b.pos.x + b.w, b.pos.y + b.h, DT_CENTER | DT_VCENTER);
}

void Game::drawPath(HDC hdc)
{
    SelectObject(hdc, pathPen);
    MoveToEx(hdc, level.path.points[0].x, level.path.points[0].y, NULL);
    for(int i = 1; i < level.path.length; i++)
    {
        LineTo(hdc, level.path.points[i].x, level.path.points[i].y);
    }
}

void Game::openScreenMain()
{
    ShowWindow(btGameLevels, SW_SHOWNORMAL);
    curScreen = SCR_MAIN;
    RedrawWindow(hMainWnd, NULL, NULL, RDW_ERASE | RDW_INVALIDATE);
}

void Game::openScreenLevels()
{
    curScreen = SCR_LEVELS;
    for (int i = 1; i <= LEVEL_COUNT; i++) {
        ShowWindow(btLevel[i], SW_SHOWNORMAL);
    }
    ShowWindow(btGameMainMenu, SW_SHOWNORMAL);
    redraw();
}

void Game::redraw()
{
    RedrawWindow(hMainWnd, NULL, NULL, RDW_ERASE | RDW_INVALIDATE);
}

void Game::openScreenGame()
{
    curScreen = SCR_GAME;
    ShowWindow(btPause, SW_SHOWNORMAL);
    redraw();
}

void Game::closeScreenMain()
{
    ShowWindow(btGameLevels, SW_HIDE);
}

void Game::closeScreenLevels()
{
    for (int i = 1; i <= LEVEL_COUNT; i++) {
        ShowWindow(btLevel[i], SW_HIDE);
    }
    ShowWindow(btGameMainMenu, SW_HIDE);
}

void Game::closeScreenGame()
{
    ShowWindow(btPause, SW_HIDE);
    ShowWindow(btResume, SW_HIDE);
    ShowWindow(btExit, SW_HIDE);
    KillTimer(hMainWnd, GAME_TIMER);
}

void Game::init()
{
    btGameLevels = CreateWindow("button", "Game Levels", WS_CHILD | BS_PUSHBUTTON,
        BUTTON_BASE_X, calcButtonY0(0), BUTTON_WIDTH, BUTTON_HEIGHT, hMainWnd, BT_GAME_LEVELS_ID, hInst, NULL);
    for (int i = 1; i <= LEVEL_COUNT; i++) { //! Кнопки Уровней
        char nameLevel[20];
        sprintf(nameLevel, "Level %d", i);
        btLevel[i] = CreateWindow("button", nameLevel, WS_CHILD | BS_PUSHBUTTON,
        BUTTON_BASE_X, calcButtonY0(i - 1), BUTTON_WIDTH, BUTTON_HEIGHT, hMainWnd, BT_LEVEL[i], hInst, NULL);
    }
    btGameMainMenu = CreateWindow("button", "Main Menu", WS_CHILD | BS_PUSHBUTTON,
        BUTTON_BASE_X, calcButtonY0(5), BUTTON_WIDTH, BUTTON_HEIGHT, hMainWnd, BT_GAME_MAINMENU_ID, hInst, NULL);
    pathPen = CreatePen(PS_SOLID, 7, PATH_LINE_COLOR);
    titleFont = CreateFont(35, 0, 0, 0, FW_DONTCARE, FALSE, FALSE, FALSE, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, "Arial");
    labelFont = CreateFont(24, 0, 0, 0, FW_DONTCARE, FALSE, FALSE, FALSE, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, "Arial");
    fastUnitPen = CreatePen(PS_SOLID, 2, RGB(255, 0 ,0));
    fastUnitBrush = CreateSolidBrush(RGB(255, 0, 0));
    mediumUnitPen = CreatePen(PS_SOLID, 2, RGB(0, 255, 0));
    mediumUnitRayPen = CreatePen(PS_SOLID, UNIT_MEDIUM_RAY_HEIGHT, RGB(0, 255, 0));
    mediumUnitBrush = CreateSolidBrush(RGB(0, 255, 0));
    strongUnitPen = CreatePen(PS_SOLID, 2, RGB(0, 0, 0));
    strongUnitBrush = CreateSolidBrush(RGB(0, 0, 255));

    for (int i = 0; i < MAX_TOWER_LEVEL; i++) {
        gunTowerObvodPen[i] = CreatePen(PS_SOLID, (int)GUN_TOWER_OBVOD_HEIGHT[i], RGB(0, 0, 0));
        gunTowerPen[i] = CreatePen(PS_SOLID, 2 * GUN_SHOT_R[i], RGB(50 * i, 255 - 50 * i, 0));
        gunTowerBrush[i] = CreateSolidBrush(RGB(50 * i, 255 - 50 * i, 0));
    }

    laserTowerPen = CreatePen(PS_COSMETIC, LASER_BARR_R, RGB(40, 250, 255));
    laserShotPen = CreatePen(PS_COSMETIC, LASER_SHOT_R, RGB(40, 250, 255));
    laserTowerBrush = CreateSolidBrush(RGB(242, 53, 115));

    rocketShotPen = CreatePen(PS_COSMETIC, 0, RGB(185, 122, 87));
    rocketTowerBrush = CreateSolidBrush(RGB(0, 0, 0));
    rocketShotBrush = CreateSolidBrush(RGB(185, 122, 87));

    buttonPressedBrush = CreateSolidBrush(RGB(245, 252, 0));
    damageAreaPen = CreatePen(PS_DOT, 1, RGB(0, 255, 0));
    wrongPositionPen = CreatePen(PS_DOT, 1, RGB(255, 0, 0));
    hpFrontBrush = CreateSolidBrush(RGB(0, 255, 0));
    hpBackBrush = CreateSolidBrush(RGB(255, 0, 0));
    buttonPen = (HPEN) GetStockObject(BLACK_PEN);
    focusedButtonPen = CreatePen(PS_SOLID, 2, RGB(0, 0, 0));

    initPanelButtons();
}

void Game::initPanelButtons()
{
    for (int i = 0; i < MAX_BUTTONS_COUNT; i++)
    {
        panelButtons[i].active = true;
        panelButtons[i].underCursor = false;
        panelButtons[i].w = BUTTON_WIDTH;
        panelButtons[i].h = BUTTON_HEIGHT;
        panelButtons[i].pos.x = BUTTON_BASE_X;
        panelButtons[i].pos.y = TOP_GAME_BLOCK_H + BUTTON_HEIGHT + PANEL_TEXT_H + i * (BUTTON_HEIGHT + BUTTON_V_SPACE);
    }
}

int Game::calcButtonY0(int buttonNumber)
{
    return BUTTON_BASE_Y + buttonNumber * (BUTTON_HEIGHT + BUTTON_V_SPACE);
}

void Game::drawScreenTitle(char* title, HDC hdc)
{
    RECT rect;
    rect.left = 0;
    rect.right = WINDOW_W;
    rect.top = 0;
    rect.bottom = TOP_GAME_BLOCK_H;
    SetTextColor(hdc, TITLE_TEXT_COLOR);
    SelectObject(hdc, titleFont);
    DrawText(hdc, title, -1, &rect, DT_SINGLELINE | DT_CENTER | DT_VCENTER);
}

void Game::drawLabelText(char* text, HDC hdc, int x0, int y0, int x1, int y1, int flags)
{
    RECT rect;
    rect.left = x0;
    rect.right = x1;
    rect.top = y0;
    rect.bottom = y1;
    SetTextColor(hdc, LABEL_TEXT_COLOR);
    SelectObject(hdc, labelFont);
    DrawText(hdc, text, -1, &rect, DT_SINGLELINE | flags);
}

Path Game::createPath(int length, int* coordinates)
{
    Path result;
    result.length = length;
    for(int i = 0; i < length; i++)
    {
        result.points[i].x = coordinates[2 * i];
        result.points[i].y = coordinates[2 * i + 1];
    }
    return result;
}

void Game::setCurLevel(int curLevel)
{
    game.curLevel = curLevel;
    points = 0;
    tick = 0;
    lastDrawTick = 0;
    tickMillis = GetTickCount();
    curTowerCount = 0;
    curUnitCount = 0;
    curUnitId = 0;
    curShotCount = 0;
    curWave = -1;
    score = 0;
    TICK_MILLIS = 2;
    SetTimer(hMainWnd, GAME_TIMER, (UINT) TICK_MILLIS, NULL);
    level = createLevel(curLevel);
    curWaveFinishTick = level.waves[0].delayTick;
    money = level.startMoney;
    lifeCount = level.lifeCount;
    towerButtonDrawStartX = (WINDOW_W - level.towerTypesCount * TOWER_BUTTON_PLACE_W) / 2;

    initLevelButtons();
    setPanelVisible(false);
    setDefaultControlState();
}

void Game::initLevelButtons()
{
    curButtonCount = 5;

    sprintf(buttons[0].text, "pause");
    buttons[0].w = 100;
    buttons[0].h = BUTTON_H;
    buttons[0].pos.y = WINDOW_H - 0.33 * (BOTTOM_GAME_BLOCK_H + BUTTON_H);
    buttons[0].pos.x = WINDOW_W - buttons[0].w - BUTTON_GAP;
    buttons[0].id = BT_GAME_PAUSE_ID;

    buttons[1].w = 80;
    buttons[1].h = BUTTON_H;
    buttons[1].pos.y = WINDOW_H - 0.5 * (BOTTOM_GAME_BLOCK_H + BUTTON_H);
    buttons[1].pos.x = WINDOW_W - buttons[1].w - BUTTON_GAP;
    buttons[1].id = BT_GAME_SELL_ID;

    buttons[2].w = 80;
    buttons[2].h = BUTTON_H;
    buttons[2].pos.y = WINDOW_H - 0.5 * (BOTTOM_GAME_BLOCK_H + BUTTON_H);
    buttons[2].pos.x = WINDOW_W - 2 * buttons[1].w - 2 * BUTTON_GAP;
    buttons[2].id = BT_GAME_UPGRADE_ID;

    sprintf(buttons[3].text, "next");
    buttons[3].w = 100;
    buttons[3].h = BUTTON_H;
    buttons[3].pos.y = WINDOW_H - 0.5 * (BOTTOM_GAME_BLOCK_H + BUTTON_H);
    buttons[3].pos.x = WINDOW_W - buttons[0].w - 2 * BUTTON_GAP - buttons[0].w;
    buttons[3].id = BT_GAME_NEXT_WAVE_ID;

    sprintf(buttons[4].text, "speed up");
    buttons[4].w = 100;
    buttons[4].h = BUTTON_H;
    buttons[4].pos.y = WINDOW_H - 0.66 * (BOTTOM_GAME_BLOCK_H + BUTTON_H);
    buttons[4].pos.x = WINDOW_W - buttons[4].w - BUTTON_GAP;
    buttons[4].id = BT_GAME_SPEED_UP_ID;
    buttons[4].pressed = false;
}

void Game::drawEnd(HDC hdc)
{
    SelectObject(hdc, GetStockObject(WHITE_BRUSH));
    SelectObject(hdc, pathPen);
    Ellipse(hdc, level.path.points[level.path.length - 1].x - HOLE_R,
                 level.path.points[level.path.length - 1].y - HOLE_R,
                 level.path.points[level.path.length - 1].x + HOLE_R,
                 level.path.points[level.path.length - 1].y + HOLE_R);
    char tmp[50];
    sprintf(tmp, "%d", lifeCount);
    SelectObject(hdc, GetStockObject(HOLLOW_BRUSH));
    SetBkMode(hdc, TRANSPARENT);
    drawLabelText(tmp,
                  hdc,
                  level.path.points[level.path.length - 1].x - HOLE_R,
                  level.path.points[level.path.length - 1].y - HOLE_R,
                  level.path.points[level.path.length - 1].x + HOLE_R,
                  level.path.points[level.path.length - 1].y + HOLE_R,
                  DT_CENTER | DT_VCENTER);
    SetBkMode(hdc, NULL);
}

void runAllTests()
{
    testCreatePath();
}

void testCreatePath()
{
    Game g;
    int arr[] = {-5, 6, 7, 0};
    Path p = g.createPath(2, arr);
    assert(p.length == 2);
    assert(p.points[0].x == -5);
    assert(p.points[1].y == 0);
}

void Game::drawUnits(HDC hdc)
{
    for(int i = 0; i < curUnitCount; i++)
    {
        drawUnit(hdc, units[i]);
    }
}

void Game::drawUnit(HDC hdc, Unit unit)
{
    if (tick < unit.startTick || !unit.active)
    {
        return;
    }
    if (unit.type == U_FAST)
    {
        drawFastUnit(hdc, unit);
    } else if (unit.type == U_MEDIUM)
    {
        drawMediumUnit(hdc, unit);
    } else if (unit.type == U_STRONG)
    {
        drawStrongUnit(hdc, unit);
    }
    drawUnitLeavesStreap(hdc, unit);
}

void Game::drawFastUnit(HDC hdc, Unit unit)
{
    SelectObject(hdc, fastUnitBrush);
    SelectObject(hdc, fastUnitPen);
    POINT fast[3];
    for (int i = 0; i < 3; i++) {
        fast[i] = polarToDecart(unit.pos, UNIT_R, UNIT_FAST_ANGLE[i] + unit.angle_rotation);
    }
    Polygon(hdc, fast, 3);
}

void Game::drawMediumUnit(HDC hdc, Unit unit)
{
    SelectObject(hdc, mediumUnitBrush);
    SelectObject(hdc, mediumUnitPen);
    Ellipse(hdc, unit.pos.x - UNIT_MEDIUM_INSIDE_R,
                 unit.pos.y - UNIT_MEDIUM_INSIDE_R,
                 unit.pos.x + UNIT_MEDIUM_INSIDE_R,
                 unit.pos.y + UNIT_MEDIUM_INSIDE_R);
    SelectObject(hdc, mediumUnitRayPen);
    for (int i = 0; i < UNIT_MEDIUM_RAYS_COUNT; i++) {
        MoveToEx(hdc, unit.pos.x, unit.pos.y, NULL);
        POINT p = polarToDecart(unit.pos, UNIT_R, unit.angle_rotation + 2 * PI * i / UNIT_MEDIUM_RAYS_COUNT);
        LineTo(hdc, p.x, p.y);
    }
}

void Game::drawStrongUnit(HDC hdc, Unit unit)
{
    SelectObject(hdc, strongUnitBrush);
    SelectObject(hdc, strongUnitPen);
    POINT strong[8];
    for (int i = 0; i < 4; i++) {
        strong[2 * i + 1] = polarToDecart(unit.pos, UNIT_R, PI / 4 + i * PI / 2 + unit.angle_rotation);
    }
    for (int i = 0; i < 3; i++) { //! ROOF!
        strong[2 * i] = polarToDecart(unit.pos, UNIT_R + unit.roof, i * PI / 2 + unit.angle_rotation);
    }
    strong[6] = strong[7];
    Polygon(hdc, strong, 7);
}

void Game::drawUnitLeavesStreap(HDC hdc, Unit unit)
{
    SelectObject(hdc, GetStockObject(NULL_PEN));
    SelectObject(hdc, hpBackBrush);
    Rectangle(hdc,
                unit.pos.x - HP_WIDTH / 2,
                unit.pos.y - HP_UP_DIST,
                unit.pos.x + HP_WIDTH / 2,
                unit.pos.y - HP_UP_DIST + HP_HEIGHT);

    SelectObject(hdc, hpFrontBrush);
    int greenWidth = HP_WIDTH * unit.hp / unit.maxHp;
    Rectangle(hdc,
                unit.pos.x - HP_WIDTH / 2,
                unit.pos.y - HP_UP_DIST,
                unit.pos.x - HP_WIDTH / 2 + greenWidth,
                unit.pos.y - HP_UP_DIST + HP_HEIGHT);
}

void Game::updateModelTime()
{
    DWORD curMillis = GetTickCount();
    tickDiff = 0;
    if (!panelActive)
    {
        tickDiff = (curMillis - tickMillis) / TICK_MILLIS;
        tick += tickDiff;
    }
    tickMillis = curMillis;

    if (tick - lastDrawTick >= REDRAW_AFTER_TICKS || panelActive)
    {
        redraw();
        lastDrawTick = tick;
    }
}

void Game::update()
{
    updateModelTime();
    updateWave();
    updateUnits();
    updateTowers();
    updateNewTower();
    updateShots();
    if (panelActive)
    {
        updateButtons(panelButtons, panelButtonsCount);
    }
    else
    {
        updateButtons(buttons, curButtonCount);
        if (!checkLoose())
        {
            checkWin();
        }
    }
}

void Game::updateTowers()
{
    Point cursorPos = getCursorPos();
    for (int i = 0; i < curTowerCount; i++)
    {
        if (towers[i].active)
        {
            towers[i].underCursor = dist(cursorPos, towers[i].pos) < TOWER_R;
        }
    }

    updateTowersFocus();
}

void Game::updateTowersFocus()
{
    for (int i = 0; i < curTowerCount; i++)
    {
        if (!towers[i].active)
        {
            continue;
        }
        bool needNewFocus = false;
        if (towers[i].lastFocusedUnit == -1)
        {
            needNewFocus = true;
        } else
        {
            if (units[towers[i].lastFocusedUnit].active == false)
            {
                needNewFocus = true;
            } else
            {
                if (dist(towers[i].pos, units[towers[i].lastFocusedUnit].pos) > getDamageR(towers[i]))
                {
                    needNewFocus = true;
                }
            }
        }
        if (needNewFocus)
        {
            towers[i].lastFocusedUnit = focusTower(towers[i]);
        }

        if (towers[i].lastFocusedUnit >= 0)
        {
            towers[i].lastFocusX = units[towers[i].lastFocusedUnit].pos.x;
            towers[i].lastFocusY = units[towers[i].lastFocusedUnit].pos.y;
        }
    }
}

void Game::updateWave()
{
    if (curWaveFinishTick <= tick && curWave < level.wavesCount - 1)
    {
        curWaveStartTick = tick;
        curWave++;
        curWaveFinishTick = tick;
        clearDeadUnits();
        for (int i = 0; i < level.waves[curWave].unitCount; i++)
        {
            units[curUnitCount].active = false; //! Так как Юнит ещё не существует, а начнёт существования только тогда, когда придёт его время выхода.
            units[curUnitCount].type = level.waves[curWave].units[i];
            units[curUnitCount].pos.x = level.path.points[0].x;
            units[curUnitCount].pos.y = level.path.points[0].y;
            units[curUnitCount].startTick = curWaveStartTick + i * level.waves[curWave].periodTick;
            units[curUnitCount].angle_rotation = 0;
            units[curUnitCount].id = curUnitId++;
            units[curUnitCount].startWave = curWave;
            units[curUnitCount].clockWise = false;
            units[curUnitCount].maxHp = calcInitUnitHp(units[curUnitCount].type, curWave, level.healthK);
            units[curUnitCount].hp = units[curUnitCount].maxHp;
            if (units[curUnitCount].type == U_STRONG) {
                units[curUnitCount].roof = 0;
                units[curUnitCount].add_roof = true;
            }
            curWaveFinishTick = max((int)(units[curUnitCount].startTick + calcPathLength() / getUnitSpeed(units[curUnitCount])), curWaveFinishTick);
            curUnitCount++;
        }

        if (curWave < level.wavesCount - 1)
        {
            curWaveFinishTick += level.waves[curWave + 1].delayTick;
        } else {
            //! Кнопка next неактивна
            setButtonActiveById(BT_GAME_NEXT_WAVE_ID, false);
        }
    }
}

void Game::updateButtons(Button* barr, int count)
{
    Point cursorPoint = getCursorPos();
    for (int i = 0; i < count; i++)
    {
        if (cursorPoint.x > barr[i].pos.x
            && cursorPoint.x < barr[i].pos.x + barr[i].w
            && cursorPoint.y > barr[i].pos.y
            && cursorPoint.y < barr[i].pos.y + barr[i].h)
        {
            barr[i].underCursor = true;
        }
        else
        {
            barr[i].underCursor = false;
        }
    }
}

void Game::updateNewTower()
{
    Point cursorPoint = getCursorPos();
    newTowerX = cursorPoint.x;
    newTowerY = cursorPoint.y;

    Point p;
    p.x = cursorPoint.x;
    p.y = cursorPoint.y;

    canPlaceNewTower = true;

    RECT allowedRect;
    allowedRect.top = TOP_GAME_BLOCK_H + TOWER_R;
    allowedRect.left = TOWER_R;
    allowedRect.right = WINDOW_W - TOWER_R;
    allowedRect.bottom = WINDOW_H - BOTTOM_GAME_BLOCK_H - TOWER_R;
    POINT cursorWinPoint;
    cursorWinPoint.x = cursorPoint.x;
    cursorWinPoint.y = cursorPoint.y;

    if (PtInRect(&allowedRect, cursorWinPoint))
    {
        for (int i = 0; i < curTowerCount; i++)
        {
            if (towers[i].active && dist(p, towers[i].pos) < 2 * TOWER_R + 1)
            {
                canPlaceNewTower = false;
            }

        }

        if (dist(level.path.points[level.path.length - 1], p) < HOLE_R + TOWER_R + 3)
        {
            canPlaceNewTower = false;
        }

        if (getMinDistFromPath(p) < TOWER_R + 5)
        {
            canPlaceNewTower = false;
        }
    }
    else
    {
        canPlaceNewTower = false;
    }
}

void Game::updateUnits()
{
    updateUnitsActivity();
    updateUnitsFeatures();
    updateUnitsPos();
}

void Game::updateUnitsActivity()
{
    for (int i = 0; i < curUnitCount; i++) {

        if (tick >= units[i].startTick && !units[i].active &&
            units[i].pos.x == level.path.points[0].x && units[i].pos.y == level.path.points[0].y) {
            units[i].active = true;
        }
    }
}

void Game::updateUnitsPos()
{
    for (int i = 0; i < curUnitCount; i++) {
        if (tick < units[i].startTick || !units[i].active) {
            continue;
        }
        double speed = getUnitSpeed(units[i]);
        double s = (tick - units[i].startTick) * speed;
        int curS = 0;
        for (int p = 1; p < level.path.length; p++)
        {
            curS += dist(level.path.points[p], level.path.points[p - 1]);
            if (curS > s)
            {
                Point p0 = level.path.points[p - 1];
                Point p1 = level.path.points[p];
                double k = (s - curS + dist(p0, p1)) / dist(p0, p1);
                units[i].pos.x = ceil(p0.x + k * (p1.x - p0.x));
                units[i].pos.y = ceil(p0.y + k * (p1.y - p0.y));
                break;
            }
            if (p == level.path.length - 1)
            {
                units[i].active = false;
                lifeCount--;
            }
        }
    }
}

void Game::updateUnitsFeatures()
{
    for (int i = 0; i < curUnitCount; i++) {
        //!rotation
        if (units[i].type == U_FAST || units[i].type == U_STRONG)
        {
            if (units[i].clockWise == true) {
                units[i].angle_rotation -= tickDiff * ANGLE_UNITS_ROTATION_IN_TICK;
            } else {
                units[i].angle_rotation += tickDiff * ANGLE_UNITS_ROTATION_IN_TICK;
            }
            if (units[i].angle_rotation >= MAX_ANGLE_UNITS_ROTATION) {
                units[i].clockWise = true;
            } else if (units[i].angle_rotation <= -MAX_ANGLE_UNITS_ROTATION) {
                units[i].clockWise = false;
            }
        }
        if (units[i].type == U_MEDIUM)
        {
            units[i].angle_rotation += tickDiff * ANGLE_UNITS_ROTATION_IN_TICK;
        }
        //!rotation
        //!change roof for Strong Unit
        if (units[i].type == U_STRONG) {
            units[i].roof += (units[i].add_roof == true ? 1 : -1) * tickDiff * UNIT_STRONG_ADD_ROOF_IN_TICK;
            if (units[i].roof > UNIT_STRONG_MAX_ROOF) {
                units[i].add_roof = false;
            }
            if (units[i].roof < UNIT_R * (sqrt(2) - 2) / 2) {
                units[i].add_roof = true;
            }
        }
    }
}

Level Game::createLevel(int numberLevel)
{
    char fileLevel[20]; //! Отличный ход!
    sprintf(fileLevel, "Levels/%d.txt", numberLevel);
    freopen(fileLevel, "r", stdin);
    Level l;

    String file_music_name("", 0); //!музыка
    file_music_name.read();
    BASS_ChannelStop(ch);
    BASS_SampleFree (samp);
    samp = BASS_SampleLoad(FALSE, file_music_name.s, 0, 0, 1, BASS_SAMPLE_LOOP);
    ch = BASS_SampleGetChannel(samp, FALSE);
    BASS_ChannelPlay(ch, true); //!музыка

    cin >> l.healthK >> l.rewardK >> l.lifeCount >> l.startMoney;
    cin >> l.path.length;
    int points[MAX_PATH_LENGTH * 2];
    for (int i = 0; i < 2 * l.path.length; i++)
    {
        cin >> points[i];
        if (i > 1) {
            points[i] += points[i - 2];
        }
    }
    l.path = createPath(l.path.length, points);

    cin >> l.towerTypesCount;
    for (int i = 0; i < l.towerTypesCount; i++)
    {
        String nameTower("", 0);
        nameTower.read();
        l.towerTypes[i].type = stringToTowerType(nameTower);
        cin >> l.towerTypes[i].basePrice;
        cin >> l.towerTypes[i].maxLevel;
    }
    cin >> l.wavesCount;
    for (int i = 0; i < l.wavesCount; i++)
    {
        cin >> l.waves[i].delayTick;
        cin >> l.waves[i].periodTick;
        cin >> l.waves[i].unitCount;
        for (int j = 0; j < l.waves[i].unitCount; j++)
        {
            String nameUnit("", 0);
            nameUnit.read();
            l.waves[i].units[j] = stringToUnitType(nameUnit);
        }
    }
    fclose(stdin);
    return l;
}

TowerType Game::stringToTowerType(String nameTower)
{
    for (int i = 0; i < TOWER_TYPE_COUNT; i++) {
        if (nameTower.equal(NAME_TOWER[i])) {
            return ID_TOWER_TYPE[i];
        }
    }
    cout << "Wrong format level Tower" << endl;
    assert(false);
}

UnitType Game::stringToUnitType(String nameUnit)
{
    for (int i = 0; i < UNIT_TYPE_COUNT; i++) {
        if (nameUnit.equal(NAME_UNIT[i])) {
            return ID_UNIT_TYPE[i];
        }
    }
    cout << "Wrong format level Unit" << endl;
    assert(false);
}

void Game::clearDeadUnits()
{
    int ac = 0;
    for (int i = 0; i < curUnitCount; i++)
    {
        if (units[i].active || units[i].startTick > tick ||
            (units[i].pos.x == level.path.points[0].x && units[i].pos.y == level.path.points[0].y))
        {
            units[ac] = units[i];
            for (int j = 0; j < curTowerCount; j++)
            {
                if (towers[j].lastFocusedUnit == i)
                {
                    towers[j].lastFocusedUnit = ac;
                }
            }
            for (int j = 0; j < curShotCount; j++)
            {
                if (shots[j].focusedUnit == i)
                {
                    shots[j].focusedUnit = ac;
                }
            }
            ac++;
        }
    }
    curUnitCount = ac;
}

int Game::calcInitUnitHp(UnitType type, int waveNumber, double healthK)
{
    double hp_without_type = 400 * (1 + (double)waveNumber / 5) * healthK;
    if (type == U_FAST) {
        return hp_without_type * 1;
    }
    if (type == U_MEDIUM) {
        return hp_without_type * 2;
    }
    if (type == U_STRONG) {
        return hp_without_type * 6;
    }
    return 1000;
}

int Game::calcReward(UnitType type, int waveNumber, double rewardK)
{
    double reward_without_type = (1 + (double)waveNumber / 5) * rewardK;
    if (type == U_FAST) {
        return reward_without_type * 1;
    }
    if (type == U_MEDIUM) {
        return reward_without_type * 1.5;
    }
    if (type == U_STRONG) {
        return reward_without_type * 2;
    }
    return 5;
}

int Game::calcPathLength()
{
    double result = 0;
    for (int i = 0; i < level.path.length - 1; i++)
    {
        result += dist(level.path.points[i], level.path.points[i + 1]);
    }
    return result;
}

int Game::processMessage(UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (msg == WM_KEYUP)
    {
        if (!panelActive)
        {
            int typeIndex = wParam - '1';
            if (typeIndex >= 0 && typeIndex < level.towerTypesCount)
            {
                startPlaceNewTower(typeIndex);
            }
            if (wParam == VK_ESCAPE)
            {
                setDefaultControlState();
            }
        }
    }
    else if (msg == WM_LBUTTONUP)
    {

        if (!panelActive)
        {
            if (curControlState == NEW_TOWER)
            {
                tryPlaceNewTower();
            }
        }
    }
    else if (msg == WM_LBUTTONDOWN)
    {
        if (panelActive)
        {
            for (int i = 0; i < panelButtonsCount; i++)
            {
                if (panelButtons[i].underCursor)
                {
                    buttonPressed(panelButtons[i].id);
                }
            }
        }
        else
        {
            for (int i = 0; i < curButtonCount; i++)
            {
                if (buttons[i].active && buttons[i].underCursor)
                {
                    buttonPressed(buttons[i].id);
                    return 0; //! Без этого сразу включалась другая панель и например переключалась волна.
                }
            }
            for (int i = 0; i < curTowerCount; i++)
            {
                if (towers[i].underCursor)
                {
                    towerSelected(i);
                }
            }
        }
    }
    return 0;
}

void Game::towerSelected(int i)
{
    if (selectedTowerIndex >= 0 && selectedTowerIndex < curTowerCount)
    {
        towers[selectedTowerIndex].selected = false;
    }
    towers[i].selected = true;
    selectedTowerIndex = i;
    curControlState = TOWER_SELECTED;
    setButtonActiveById(BT_GAME_PAUSE_ID, false);
    setButtonActiveById(BT_GAME_SPEED_UP_ID, false);
    setButtonActiveById(BT_GAME_NEXT_WAVE_ID, false);
    setButtonActiveById(BT_GAME_SELL_ID, true);
    if (towers[i].level == towers[i].maxLevel - 1) {
        setButtonActiveById(BT_GAME_UPGRADE_ID, false);
    } else
    setButtonActiveById(BT_GAME_UPGRADE_ID, true);

    char text[20];
    sprintf(text, "%d$ up", getTowerUpgradeCost(towers[i]));
    setButtonTextById(BT_GAME_UPGRADE_ID, text);
    sprintf(text, "%d$ sell", getTowerSellPrice(towers[i]));
    setButtonTextById(BT_GAME_SELL_ID, text);
}

void Game::tryPlaceNewTower()
{
    if (canPlaceNewTower)
    {
        towers[curTowerCount].type = level.towerTypes[newTowerTypeIndex].type;
        towers[curTowerCount].maxLevel = level.towerTypes[newTowerTypeIndex].maxLevel;
        towers[curTowerCount].pos.x = newTowerX;
        towers[curTowerCount].pos.y = newTowerY;
        towers[curTowerCount].level = 0;
        towers[curTowerCount].lastFocusedUnit = -1;
        towers[curTowerCount].selected = false;
        towers[curTowerCount].lastShotTick = -1000;
        towers[curTowerCount].underCursor = false;
        towers[curTowerCount].active = true;
        curTowerCount++;
        money -= level.towerTypes[newTowerTypeIndex].basePrice;

        setDefaultControlState();
    }
}

void Game::buttonPressed(int id)
{
    if (id == BT_GAME_UPGRADE_ID)
    {
        int cost = getTowerUpgradeCost(towers[selectedTowerIndex]);
        if (money >= cost)
        {
            money -= cost;
            towers[selectedTowerIndex].level++;
        }
        setDefaultControlState();
    }
    if (id == BT_GAME_SELL_ID)
    {
        money += getTowerSellPrice(towers[selectedTowerIndex]);
        towers[selectedTowerIndex].active = false;
        setDefaultControlState();
    }
    if (id == BT_GAME_PAUSE_ID)
    {
        sprintf(panelText, "PAUSE");

        configurePanelButton(0, "Continue", BT_GAME_CONTINUE_ID);
        configurePanelButton(1, "Retry", BT_GAME_RETRY_ID);
        configurePanelButton(2, "Select another level", BT_GAME_SELECT_LEVEL_ID);
        panelButtonsCount = 3;
        setPanelVisible(true);
    }
    if (id == BT_GAME_SPEED_UP_ID)
    {
        if (buttons[getButtonIndexById(BT_GAME_SPEED_UP_ID)].pressed == false) {
            TICK_MILLIS /= 2;

        } else {
            TICK_MILLIS *= 2;
        }
        buttons[getButtonIndexById(BT_GAME_SPEED_UP_ID)].pressed = !buttons[getButtonIndexById(BT_GAME_SPEED_UP_ID)].pressed;
    }
    if (id == BT_GAME_NEXT_WAVE_ID)
    {
        score += ADD_SCORE_NEXT_SECOND * (curWaveFinishTick - tick) * TICK_MILLIS / MILLIS_IN_SECOND;
        curWaveFinishTick = tick;
        setDefaultControlState();
    }
    if (id == BT_GAME_CONTINUE_ID)
    {
        setPanelVisible(false);
    }
    if (id == BT_GAME_SELECT_LEVEL_ID)
    {
        closeScreenGame();
        openScreenLevels();
    }
    if (id == BT_GAME_RETRY_ID)
    {
        cout << "!!!" << curLevel << endl;
        setCurLevel(curLevel);
    }
}

void Game::drawTower(HDC hdc, Tower tower)
{
    if (tower.type == TW_GUN)
    {
        drawTowerGun(hdc, tower);
    }
    if (tower.type == TW_LASER) {
        drawTowerLaser(hdc, tower);
    }
    if (tower.type == TW_ROCKET) {
        drawTowerRocket(hdc, tower);
    }
    if (tower.selected || tower.underCursor)
    {
        int r = getDamageR(tower);
        SelectObject(hdc, damageAreaPen);
        SelectObject(hdc, GetStockObject(HOLLOW_BRUSH));
        Ellipse(hdc, tower.pos.x - r,
         tower.pos.y - r,
         tower.pos.x + r,
         tower.pos.y + r);
    }
}

void Game::drawTowerGun(HDC hdc, Tower tower)
{
    SelectObject(hdc, gunTowerBrush[tower.level]);
    SelectObject(hdc, gunTowerObvodPen[tower.level]);
    Ellipse(hdc, tower.pos.x - TOWER_R,
             tower.pos.y - TOWER_R,
             tower.pos.x + TOWER_R,
             tower.pos.y + TOWER_R);

    SelectObject(hdc, gunTowerPen[tower.level]);

    MoveToEx(hdc, tower.pos.x, tower.pos.y, NULL);

    int l = TOWER_R + TOWER_R * GUN_EXTRA_LEN_PERCENT / 100;

    int gx = 0;
    int gy = 0;

    if (tower.lastFocusX == -1 && tower.lastFocusY == -1)
    {
        gx = tower.pos.x;
        gy = tower.pos.y - l;
    } else
    {
        Point unitP;
        unitP.x = tower.lastFocusX;
        unitP.y = tower.lastFocusY;
        gx = tower.pos.x + (unitP.x - tower.pos.x) * l / dist(tower.pos, unitP);
        gy = tower.pos.y + (unitP.y - tower.pos.y) * l / dist(tower.pos, unitP);
    }
    LineTo(hdc, gx, gy);
}

void Game::drawTowerLaser(HDC hdc, Tower tower)
{
    POINT trunk[3];
    SelectObject(hdc, laserTowerBrush);
    SelectObject(hdc, laserTowerPen);
    for (int i = 0; i <= 3; i++) {
        trunk[i] = polarToDecart(tower.pos, TOWER_R, PI / 2 + 2 * PI * i / 3);
    }
    Polygon(hdc, trunk, 4);
}

void Game::drawTowerRocket(HDC hdc, Tower tower)
{
    POINT trunk[4];
    SelectObject(hdc, rocketShotBrush);
    SelectObject(hdc, rocketShotPen);
    trunk[0].x = tower.pos.x + (double)TOWER_R * 0.7 * 3 / 4;
    trunk[0].y = tower.pos.y;
    trunk[1].x = tower.pos.x;
    trunk[1].y = tower.pos.y - TOWER_R * 0.7 * 1.5;
    trunk[2].x = tower.pos.x - (double)TOWER_R * 0.7 * 3 / 4;
    trunk[2].y = tower.pos.y;
    trunk[3].x = tower.pos.x + (double)TOWER_R * 0.7 * 3 / 4;
    trunk[3].y = tower.pos.y;
    for (int i = 0; i < 4; i++) {
        trunk[i].y -= TOWER_R * 0.7 * min((double)1, (double)(tick - tower.lastShotTick) / getShotPeriod(tower));
    }
    Polygon(hdc, trunk, 4);
    SelectObject(hdc, rocketTowerBrush);
    SelectObject(hdc, rocketTowerPen);
    Ellipse(hdc, tower.pos.x - TOWER_R * 0.7,
             tower.pos.y - TOWER_R * 0.7,
             tower.pos.x + TOWER_R * 0.7,
             tower.pos.y + TOWER_R * 0.7);
}

void Game::drawTowerButtons(HDC hdc)
{
    for (int i = 0; i < level.towerTypesCount; i++)
    {
        Tower t;
        t.pos.x = towerButtonDrawStartX + TOWER_BUTTON_PLACE_W * (i + 0.5);
        t.pos.y = WINDOW_H - BOTTOM_GAME_BLOCK_H / 2;
        t.type = level.towerTypes[i].type;
        t.lastFocusedUnit = -1;
        t.selected = false;
        t.underCursor = false;
        t.lastShotTick = -1000;
        t.lastFocusX = -1;
        t.lastFocusY = -1;
        t.level = 0;
        drawTower(hdc, t);
        char tmpstr[10];
        sprintf(tmpstr, "%d", i + 1);
        drawLabelText(tmpstr, hdc, t.pos.x - TOWER_R - DIGIT_GAP, t.pos.y - TOWER_R, t.pos.x - TOWER_R, t.pos.y + TOWER_R, DT_VCENTER | DT_CENTER);
        sprintf(tmpstr, "%d", level.towerTypes[i].basePrice);
        drawLabelText(tmpstr, hdc, t.pos.x - TOWER_R, t.pos.y + TOWER_R, t.pos.x + TOWER_R, t.pos.y + 3 * TOWER_R, DT_VCENTER | DT_CENTER);
    }
}

void Game::drawBlockBorders(HDC hdc)
{
    SelectObject(hdc, GetStockObject(BLACK_PEN));
    MoveToEx(hdc, 0, TOP_GAME_BLOCK_H, NULL);
    LineTo(hdc, WINDOW_W, TOP_GAME_BLOCK_H);

    MoveToEx(hdc, 0, WINDOW_H - BOTTOM_GAME_BLOCK_H, NULL);
    LineTo(hdc, WINDOW_W, WINDOW_H - BOTTOM_GAME_BLOCK_H);

}

void Game::drawNewTower(HDC hdc)
{
    Tower t;
    t.type = level.towerTypes[newTowerTypeIndex].type;
    t.pos.x = newTowerX;
    t.pos.y = newTowerY;
    t.lastFocusX = -1;
    t.lastFocusY = -1;
    t.selected = true;
    t.lastFocusedUnit = -1;
    t.lastShotTick = -1000;
    t.level = 0;
    drawTower(hdc, t);
    if (!canPlaceNewTower)
    {
        SelectObject(hdc, wrongPositionPen);
        SelectObject(hdc, GetStockObject(HOLLOW_BRUSH));
        int r = getDamageR(t);
        Ellipse(hdc, t.pos.x - r,
         t.pos.y - r,
         t.pos.x + r,
         t.pos.y + r);
    }

}

int Game::focusTower(Tower t)
{
    int index = -1;
    double minDist = 2000;
    for (int i = 0; i < curUnitCount; i++)
    {
        if (units[i].active && units[i].pos.x >= 0 && units[i].pos.y >= 0
            && units[i].pos.x <= WINDOW_W && units[i].pos.y <= WINDOW_H &&
            (units[i].pos.x != level.path.points[0].x || units[i].pos.y != level.path.points[0].y))
        {
            double d = dist(t.pos, units[i].pos);
            if (d < minDist)
            {
                index = i;
                minDist = d;
            }
        }
    }
    if (minDist <= getDamageR(t)) {
        return index;
    }
    return -1;
}

void Game::drawShots(HDC hdc)
{
    for (int i = 0; i < curShotCount; i++)
    {
        drawShot(hdc, shots[i]);
        if (towers[shots[i].tower].type == TW_ROCKET && units[shots[i].focusedUnit].active) {
            SelectObject(hdc, laserShotPen);
            SelectObject(hdc, laserTowerBrush);
            Ellipse(hdc, units[shots[i].focusedUnit].pos.x - 2 * TARG_R,
             units[shots[i].focusedUnit].pos.y - 2 * TARG_R,
             units[shots[i].focusedUnit].pos.x + 2 * TARG_R,
             units[shots[i].focusedUnit].pos.y + 2 * TARG_R);
        }
    }
}

void Game::drawShot(HDC hdc, Shot shot)
{
    //printf("draw shot from %d in %d %d\n", shot.tower, shot.pos.x, shot.pos.y);
    if (towers[shot.tower].type == TW_GUN) {
        drawShotGun(hdc, shot);
    }
    if (towers[shot.tower].type == TW_LASER) {
        drawShotLaser(hdc, shot);
    }
    if (towers[shot.tower].type == TW_ROCKET) {
        drawShotRocket(hdc, shot);
    }
}

void Game::drawShotGun(HDC hdc, Shot shot)
{
    SelectObject(hdc, gunTowerPen[towers[shot.tower].level]);
    SelectObject(hdc, gunTowerBrush[towers[shot.tower].level]);
    Ellipse(hdc, shot.pos.x - GUN_SHOT_R[towers[shot.tower].level],
         shot.pos.y - GUN_SHOT_R[towers[shot.tower].level],
         shot.pos.x + GUN_SHOT_R[towers[shot.tower].level],
         shot.pos.y + GUN_SHOT_R[towers[shot.tower].level]);
}

void Game::drawShotLaser(HDC hdc, Shot shot)
{
    SelectObject(hdc, laserShotPen);
    MoveToEx(hdc, towers[shot.tower].pos.x, towers[shot.tower].pos.y, NULL);
    LineTo(hdc, towers[shot.tower].pos.x + shot.v.x * 300, towers[shot.tower].pos.y + shot.v.y * 300);
}

void Game::drawShotRocket(HDC hdc, Shot shot)
{
    if (shot.bang == true) {
        SelectObject(hdc, rocketShotPen);
        SelectObject(hdc, rocketTowerBrush);
        Ellipse(hdc, shot.pos.x - BANG_R * (tick - shot.bang_begin_tick) / BANG_TIME_TICK,
         shot.pos.y - BANG_R * (tick - shot.bang_begin_tick) / BANG_TIME_TICK,
         shot.pos.x + BANG_R * (tick - shot.bang_begin_tick) / BANG_TIME_TICK,
         shot.pos.y + BANG_R * (tick - shot.bang_begin_tick) / BANG_TIME_TICK);
         return;
    }
    POINT trunk[4];
    SelectObject(hdc, rocketShotBrush);
    SelectObject(hdc, rocketShotPen);
    for (int i = 0; i < 4; i++) {
        trunk[i] = polarToDecart(shot.pos, ROCKET_SHOT_R, ROCKET_SHOT_ANGLE[i % 3]);
    }
    Polygon(hdc, trunk, 4);
}

void Game::updateShots()
{
    updateFlyingShots();
    updateNewShots();
    updateExplodeShots();
}

void Game::updateFlyingShots()
{
    for (int i = 0; i < curShotCount; i++)
    {
        if (towers[shots[i].tower].type == TW_ROCKET) {
            Point target = units[shots[i].focusedUnit].pos;
            if (units[shots[i].focusedUnit].active == false || shots[i].focusedUnit >= curUnitCount || shots[i].bang == true ||
                units[shots[i].focusedUnit].id != shots[i].focusedUnitId) {
                //!Идёт взрыв какой-то из ракет
            } else {
                //!Ракета продолжает движение
                Point cur_shot = shots[i].pos;
                double r = dist(target, cur_shot);
                double u_dist = getShotSpeed(shots[i]);
                double ac_dist = K_GRAVITY_ROCKET_SHOT * u_dist * u_dist / r; //! По формуле центростремительного ускорения
                Point ac_vect = minus_vect(target, cur_shot); //!Осталось подкорректировать длину вектора ускорения
                double coef = ac_dist / dist_of_vect(ac_vect);
                ac_vect.x *= coef;
                ac_vect.y *= coef;//! Теперь ac_vect - в точности вектор центростремительного ускорения
                Point u_vect;
                u_vect.x = ac_vect.y;
                u_vect.y = -ac_vect.x; //! вектор Скорости перпендикулярен вектору ускорения
                coef = u_dist / dist_of_vect(u_vect);
                u_vect.x *= coef;
                u_vect.y *= coef; //! Вектор скорости перпендикулярного центростремительному ускорению
                Point delta_u_vect;
                delta_u_vect.x = ac_vect.x * tickDiff; //!Изменение вектора скорости за счёт
                delta_u_vect.y = ac_vect.y * tickDiff; //!Центростремительного ускорения
                shots[i].v = plus_vect(u_vect, delta_u_vect);
                coef = u_dist / dist_of_vect(shots[i].v);
                shots[i].v.x *= coef;
                shots[i].v.y *= coef;
            }
        }
        shots[i].pos.x += tickDiff * shots[i].v.x;
        shots[i].pos.y += tickDiff * shots[i].v.y;
    }
}

void Game::updateNewShots()
{
    for (int i = 0; i < curTowerCount; i++)
    {
        if (towers[i].active && towers[i].lastFocusedUnit >= 0 && towers[i].lastShotTick < tick - getShotPeriod(towers[i]))
        {
            shots[curShotCount].pos.x = towers[i].pos.x;
            shots[curShotCount].pos.y = towers[i].pos.y;
            shots[curShotCount].level = towers[i].level;
            shots[curShotCount].focusedUnit = towers[i].lastFocusedUnit;
            shots[curShotCount].focusedUnitId = units[towers[i].lastFocusedUnit].id;
            shots[curShotCount].tower = i;
            shots[curShotCount].bang = false;
            shots[curShotCount].bang_begin_tick = 0;

            double d = dist(towers[i].pos, units[towers[i].lastFocusedUnit].pos);
            double speed = getShotSpeed(shots[curShotCount]);

            shots[curShotCount].v.x = (units[towers[i].lastFocusedUnit].pos.x - towers[i].pos.x) * speed / d;
            shots[curShotCount].v.y = (units[towers[i].lastFocusedUnit].pos.y - towers[i].pos.y) * speed / d;

            curShotCount++;

            towers[i].lastShotTick = tick;
        }
    }
}

void Game::updateExplodeShots()
{
    int activeShots = 0;
    for (int i = 0; i < curShotCount; i++)
    {
        bool active = true;
        if (towers[shots[i].tower].type == TW_ROCKET && !shots[i].bang && (units[shots[i].focusedUnit].active == false ||
            units[shots[i].focusedUnit].id != shots[i].focusedUnitId)) {
            shots[i].bang = true;
            shots[i].v.x = 0;
            shots[i].v.y = 0;
            shots[i].bang_begin_tick = tick;
        }
        if (towers[shots[i].tower].type == TW_ROCKET && shots[i].bang && tick - shots[i].bang_begin_tick > BANG_TIME_TICK) {
            active = false;
        }
        if (towers[shots[i].tower].type == TW_LASER && shots[i].bang && tick - shots[i].bang_begin_tick > LASER_SHOW_TIME_TICK) {
            active = false;
        }
        RECT gameRect;
        gameRect.left = 0;
        gameRect.right = WINDOW_W;
        gameRect.top = TOP_GAME_BLOCK_H;
        gameRect.bottom = WINDOW_H - BOTTOM_GAME_BLOCK_H;
        POINT shotP;
        shotP.x = shots[i].pos.x;
        shotP.y = shots[i].pos.y;
        if (!PtInRect(&gameRect, shotP) && towers[shots[i].tower].type == TW_GUN)
        {
            active = false;
        }
        if (active && !shots[i].bang)
        {
            for (int j = 0; j < curUnitCount; j++)
            {
                if (units[j].active && intersects(units[j], shots[i]) && towers[shots[i].tower].type == TW_ROCKET && shots[i].focusedUnit == j)
                {
                    units[j].hp -= getDamage(shots[i]);
                    if (units[j].hp <= 0)
                    {
                        units[j].active = false;
                        score += getUnitScore(units[j]);
                        money += getUnitPrice(units[j]);
                    }
                    if (!shots[i].bang) {
                        shots[i].bang = true;
                        shots[i].v.x = 0;
                        shots[i].v.y = 0;
                        shots[i].bang_begin_tick = tick;
                    }
                    //active = false;
                    break; //! Так как мы должны ударить только одного Юнита
                }
                if (units[j].active && intersects(units[j], shots[i]) && (towers[shots[i].tower].type == TW_GUN))
                {
                    units[j].hp -= getDamage(shots[i]);
                    if (units[j].hp <= 0)
                    {
                        units[j].active = false;
                        score += getUnitScore(units[j]);
                        money += getUnitPrice(units[j]);
                    }
                    active = false;
                    break; //! Так как мы должны ударить только одного Юнита
                }
                if (units[j].active && towers[shots[i].tower].type == TW_LASER) {
                    Point cur_tower = towers[shots[i].tower].pos; //! a
                    Point cur_shot = shots[i].pos; //! b
                    double a1 = cur_shot.y - cur_tower.y;//! Уравнение прямой от башни до Юнита
                    double b1 = cur_tower.x - cur_shot.x;
                    double c1 = 0 - cur_tower.x * a1 - cur_tower.y * b1;
                    double sdvig = 2 * LASER_SHOT_R * sqrt(a1 * a1 + b1 * b1);
                    //! Умножаем на 2, чтобы не делать лазерслишком толстым
                    if ((a1 * units[j].pos.x + b1 * units[j].pos.y + c1 - sdvig) *
                        (a1 * units[j].pos.x + b1 * units[j].pos.y + c1 + sdvig) < 0) {
                        units[j].hp -= getDamage(shots[i]);
                        if (units[j].hp <= 0)
                        {
                            units[j].active = false;
                            score += getUnitScore(units[j]);
                            money += getUnitPrice(units[j]);
                        }
                        shots[i].bang = true; //! Выстрел должен произойти ровно 1 раз
                        shots[i].bang_begin_tick = tick; //! Дальше он должен просто рисоваться
                    }
                }
            }
        }
        if (active)
        {
            shots[activeShots] = shots[i];
            activeShots++;
        }
    }
    //cout << activeShots << endl;
    curShotCount = activeShots;
}

int Game::getUnitScore(Unit u)
{
    int score_without_type = 10 * (1 + (double)u.startWave / 10);
    if (u.type == U_FAST) {
        return 1 * score_without_type;
    }
    if (u.type == U_MEDIUM) {
        return 2 * score_without_type;
    }
    if (u.type == U_STRONG) {
        return 4 * score_without_type;
    }
    return 15;
}

double Game::getShotSpeed(Shot shot)
{
    if (towers[shot.tower].type == TW_GUN) {
        return 1;
    }
    if (towers[shot.tower].type == TW_LASER) {
        return 3;
    }
    if (towers[shot.tower].type == TW_ROCKET) {
        return 0.5;
    }
}

double Game::getShotPeriod(Tower t)
{
    if (t.type == TW_GUN) {
        return 100 / GUN_FREQ[t.level];
    }
    if (t.type == TW_LASER) {
        return 800;
    }
    if (t.type == TW_ROCKET) {
        return 100;
    }
}

int Game::getDamage(Shot s)
{
    if (towers[s.tower].type == TW_LASER) {
        return 400;
    }
    if (towers[s.tower].type == TW_ROCKET) {
        return 3000;
    }
    if (towers[s.tower].type == TW_GUN) {
        return GUN_DAMAGE[towers[s.tower].level] * 100;
    }
    return 100;

}

int Game::getDamageR(Tower t)
{
    if (t.type == TW_GUN) {
        return 100 * GUN_DAMAGE_R[t.level];
    }
    if (t.type == TW_ROCKET) {
        return 300;
    }
    if (t.type == TW_LASER) {
        return 150;
    }
    return 100;
}

int Game::getTowerUpgradeCost(Tower t)
{
    if (t.type == TW_GUN) {
        return GUN_UPGRADE_COST[t.level];
    }
    return 5;
}

int Game::getTowerSellPrice(Tower t)
{
    if (t.type == TW_GUN) {
        return GUN_SELL_COST[t.level];
    }
    for (int i = 0; i < level.towerTypesCount; i++) {
        if (t.type == level.towerTypes[i].type) {
            return level.towerTypes[i].basePrice / 2;
        }
    }
}

int Game::getUnitPrice(Unit u)
{
    return calcReward(u.type, curWave, level.rewardK);
}

double Game::getUnitSpeed(Unit u)
{
    if (u.type == U_FAST) {
        return 0.2;
    }
    if (u.type == U_MEDIUM) {
        return 0.1;
    }
    if (u.type == U_STRONG) {
        return 0.07;
    }
    return 0.1;
}

Point Game::getCursorPos()
{
    POINT cursorPoint;
    GetCursorPos(&cursorPoint);
    ScreenToClient(hMainWnd, &cursorPoint);
    Point result;
    result.x = cursorPoint.x;
    result.y = cursorPoint.y;
    return result;
}

bool Game::intersects(Unit u, Shot shot)
{
    return (dist(u.pos, shot.pos) < UNIT_R + GUN_SHOT_R[towers[shot.tower].level]);
}

int Game::getButtonIndexById(int id)
{
    for (int i = 0; i < curButtonCount; i++)
    {
        if(buttons[i].id == id)
        {
            return i;
        }
    }
    return -1;
}

void Game::startPlaceNewTower(int towerTypeIndex)
{
    if (money >= level.towerTypes[towerTypeIndex].basePrice)
    {
        curControlState = NEW_TOWER;
        newTowerTypeIndex = towerTypeIndex;
    }
}

double Game::getMinDistFromPath(Point p)
{
    double minDist = 9999;

    for (int i = 0; i < level.path.length - 1; i++)
    {
        Point p1 = level.path.points[i];
        Point p2 = level.path.points[i + 1];
        Point p1p = minus_vect(p, p1);
        Point p2p = minus_vect(p, p2);
        Point p1p2 = minus_vect(p2, p1);
        if (smul(p1p, p1p2) * smul(p2p, p1p2) < 0)
        {
            double h = fabs(vmul(p1p, p1p2) / dist(p1, p2));
            minDist = min(minDist, h);
        }
        else
        {
            minDist = min(minDist, min(dist(p, p1), dist(p, p2)));
        }
    }

    return minDist;
}

void Game::setButtonActiveById(int id, bool active)
{
    int idx = getButtonIndexById(id);
    if (idx != -1)
    {
        buttons[idx].active = active;
    }
}

void Game::drawSelectedTowerInfo(HDC hdc)
{
    Tower t = towers[selectedTowerIndex];
    t.underCursor = false;
    t.selected = false;
    t.lastFocusX = -1;
    t.lastFocusY = -1;
    t.pos.x = 2 * TOWER_R;
    t.pos.y = WINDOW_H - BOTTOM_GAME_BLOCK_H / 2;
    drawTower(hdc, t);

    char levelStr[15];
    sprintf(levelStr, "Level %d", t.level);

    drawLabelText(levelStr, hdc, t.pos.x + 2 * TOWER_R, t.pos.y - TOWER_R, t.pos.x + 150, t.pos.y + TOWER_R, DT_LEFT | DT_VCENTER);
}

void Game::setButtonTextById(int id, char* text)
{
    int idx = getButtonIndexById(id);
    if (idx >= 0)
    {
        int i = 0;
        while (text[i] > 0)
        {
            buttons[idx].text[i] = text[i];
            i++;
        }
        buttons[idx].text[i] = 0;
    }
}

void Game::setDefaultControlState()
{
    curControlState = NONE;
    setButtonActiveById(BT_GAME_PAUSE_ID, true);
    setButtonActiveById(BT_GAME_SPEED_UP_ID, true);
    if (curWave != level.wavesCount - 1) {
        setButtonActiveById(BT_GAME_NEXT_WAVE_ID, true);
    } else {
        setButtonActiveById(BT_GAME_NEXT_WAVE_ID, false);
    }
    setButtonActiveById(BT_GAME_UPGRADE_ID, false);
    setButtonActiveById(BT_GAME_SELL_ID, false);
    if (selectedTowerIndex >= 0)
    {
        towers[selectedTowerIndex].selected = false;
        selectedTowerIndex = -1;
    }
}


void Game::setPanelVisible(bool visible)
{
    panelActive = visible;
}

void Game::configurePanelButton(int index, char* text, int id)
{
    panelButtons[index].id = id;
    sprintf(panelButtons[index].text, "%s", text);
}

void Game::drawPanel(HDC hdc)
{
    SelectObject(hdc, GetStockObject(WHITE_BRUSH));
    SelectObject(hdc, GetStockObject(BLACK_PEN));
    int x0 = (WINDOW_W - PANEL_W) / 2;
    int y0 = TOP_GAME_BLOCK_H + BUTTON_HEIGHT;
    //Rectangle(hdc, x0, y0, x0 + PANEL_W, y0 + PANEL_H);

    drawLabelText(panelText, hdc, x0, y0, x0 + PANEL_W, y0 + PANEL_TEXT_H, DT_CENTER | DT_VCENTER);

    for (int i = 0; i < panelButtonsCount; i++)
    {
        drawButton(hdc, panelButtons[i]);
    }
}

bool Game::checkLoose()
 {
     if (lifeCount <= 0)
     {
        showEndPanel(false);
        return true;
     } else
     {
         return false;
     }
 }

 bool Game::checkWin()
 {
     clearDeadUnits();
     if (curWave == level.wavesCount - 1)
     {
        if (curUnitCount != 0) {
            return false;
        }
        showEndPanel(true);
        return true;
     }
     return false;
 }

 void Game::showEndPanel(bool win)
 {
     panelButtonsCount = 2;
     configurePanelButton(0, "Retry", BT_GAME_RETRY_ID);
     configurePanelButton(1, "Select another level", BT_GAME_SELECT_LEVEL_ID);
     sprintf(panelText, win ? "You WIN!" : "You LOOSE!");
     setPanelVisible(true);
 }
