// DX Ball Game with Menu + Music + Restart System
// Developed by: Mahmuda Afrin

#include <GL/glut.h>
#include <cmath>
#include <vector>
#include <string>
#include <fstream>

#ifdef _WIN32
#include <windows.h>
#include <mmsystem.h>
#pragma comment(lib, "winmm.lib")
#endif

// ---------------- GAME STATES ----------------
enum GameState {
    MENU,
    PLAYING,
    CREDITS,
    HIGHSCORE,
    DIFFICULTY,
    GAME_OVER,
    GAME_WIN
};

GameState state = MENU;

// ---------------- GAME VARIABLES ----------------
float paddleX = 0.0f;
float ballX = 0.0f, ballY = -0.3f;
float ballDX = 0.003f, ballDY = 0.004f;

int score = 0;
int highScore = 0;

bool musicOn = true;
int difficulty = 2;

// ---------------- BRICKS ----------------
struct Brick {
    float x, y;
    bool active;
};

std::vector<Brick> bricks;

// ---------------- MUSIC ----------------
// Set your WAV file path manually here
std::string arcadeMusicPath = "F:\\Final_Lab_Project\\arcade.wav";
// Music functions
void playMusic(){
    if(musicOn){
        PlaySound(arcadeMusicPath.c_str(), NULL, SND_ASYNC | SND_LOOP);
    }
}

// ---------------- HIGH SCORE ----------------
void loadHighScore() {
    std::ifstream file("score.txt");
    if (file.is_open()) file >> highScore;
}

void saveHighScore() {
    std::ofstream file("score.txt");
    file << highScore;
}

// ---------------- RESET GAME ----------------
void resetGame() {
    paddleX = 0;
    ballX = 0;
    ballY = -0.3f;

    score = 0;
    ballDX = 0.003f;
    ballDY = 0.004f;

    bricks.clear();

    int rows = 5, cols = 10;
    float startX = -0.9f, startY = 0.8f;

    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            Brick b;
            b.x = startX + j * 0.18f;
            b.y = startY - i * 0.08f;
            b.active = true;
            bricks.push_back(b);
        }
    }

    bool gameOver = false;
    bool gameWin = false;
}

// ---------------- FLAGS ----------------
bool gameOver = false;
bool gameWin = false;

// ---------------- DRAW HELPERS ----------------
void drawRect(float x, float y, float w, float h) {
    glBegin(GL_QUADS);
    glVertex2f(x, y);
    glVertex2f(x + w, y);
    glVertex2f(x + w, y + h);
    glVertex2f(x, y + h);
    glEnd();
}

void text(float x, float y, std::string s) {
    glRasterPos2f(x, y);
    for (char c : s)
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c);
}

// ---------------- MENU ----------------
void drawMenu() {
    text(-0.2f, 0.60f, "DX BALL GAME");

    text(-0.2f, 0.40f, "1. Play");
    text(-0.2f, 0.32f, "2. Difficulty");
    text(-0.2f, 0.24f, "3. High Score");
    text(-0.2f, 0.16f, "4. Credits");
    text(-0.2f, 0.08f, "5. Exit");
}

// ---------------- GAME DRAW ----------------
void drawGame() {
    glClear(GL_COLOR_BUFFER_BIT);

    // Paddle
    glColor3f(0.2f, 0.8f, 1.0f);
    drawRect(paddleX - 0.15f, -0.9f, 0.3f, 0.03f);

    // Ball
    glColor3f(1, 0.9f, 0.2f);
    glBegin(GL_POLYGON);
    for (int i = 0; i < 360; i += 20) {
        float t = i * 3.14159f / 180;
        glVertex2f(ballX + 0.02f * cos(t),
                   ballY + 0.02f * sin(t));
    }
    glEnd();

    // Bricks
    glColor3f(1, 0.3f, 0.3f);
    for (auto &b : bricks)
        if (b.active)
            drawRect(b.x, b.y, 0.16f, 0.06f);

    text(-0.95f, 0.95f, "Score: " + std::to_string(score));
}

// ---------------- COLLISION ----------------
void collision() {

    if (ballX < -1 || ballX > 1) ballDX = -ballDX;
    if (ballY > 1) ballDY = -ballDY;

    if (ballY < -1) {
        gameOver = true;
        state = GAME_OVER;
        playMusic();
    }

    if (ballY <= -0.85f &&
        ballX >= paddleX - 0.15f &&
        ballX <= paddleX + 0.15f) {
        ballDY = fabs(ballDY);
    }

    for (auto &b : bricks) {
        if (!b.active) continue;

        if (ballX > b.x && ballX < b.x + 0.16f &&
            ballY > b.y && ballY < b.y + 0.06f) {
            b.active = false;
            ballDY = -ballDY;
            score += 10;
        }
    }

    bool win = true;
    for (auto &b : bricks)
        if (b.active) win = false;

    if (win) {
        gameWin = true;
        state = GAME_WIN;
        playMusic();
    }

    if (score > highScore) {
        highScore = score;
        saveHighScore();
    }
}

// ---------------- UPDATE ----------------
void update(int) {

    if (state == PLAYING) {
        ballX += ballDX * difficulty;
        ballY += ballDY * difficulty;
        collision();
    }

    glutPostRedisplay();
    glutTimerFunc(16, update, 0);
}

// ---------------- DISPLAY ----------------
void display() {
    glClear(GL_COLOR_BUFFER_BIT);

    if (state == MENU) drawMenu();

    else if (state == PLAYING) drawGame();

    else if (state == CREDITS) {
        text(-0.3f, 0.2f, "DX Ball Project");
        text(-0.3f, 0.1f, "Developed by: Mahmuda Afrin Chaity & Sathi Rani Das");
        text(-0.3f, -0.1f, "Press any key to return");
    }

    else if (state == HIGHSCORE) {
        text(-0.3f, 0.2f, "High Score:");
        text(-0.3f, 0.0f, std::to_string(highScore));
        text(-0.3f, -0.2f, "Press any key to return");
    }

    else if (state == DIFFICULTY) {
        text(-0.3f, 0.2f, "Difficulty: 1 Easy 2 Medium 3 Hard");
        text(-0.3f, 0.0f, "Current: " + std::to_string(difficulty));
        text(-0.3f, -0.2f, "Press any key to return");
    }

    else if (state == GAME_OVER) {
        text(-0.2f, 0.2f, "GAME OVER");
        text(-0.2f, 0.0f, "Press R to Restart");
        text(-0.2f, -0.2f, "ESC -> Menu");
    }

    else if (state == GAME_WIN) {
        text(-0.2f, 0.2f, "YOU WIN!");
        text(-0.2f, 0.0f, "Press R to Restart");
        text(-0.2f, -0.2f, "ESC -> Menu");
    }

    glFlush();
}

// ---------------- KEYBOARD ----------------
void keyboard(unsigned char key, int, int) {

    // -------- MENU --------
    if (state == MENU) {
        switch (key) {
            case '1':
                resetGame();
                state = PLAYING;
                playMusic();
                break;

            case '2':
                state = DIFFICULTY;
                break;

            //case '3':
                //musicOn = !musicOn;
                //break;

            case '3':
                state = HIGHSCORE;
                break;

            case '4':
                state = CREDITS;
                break;

            case '5':
                exit(0);
        }
    }

    // -------- RESTART --------
    else if (state == GAME_OVER || state == GAME_WIN) {
        if (key == 'r' || key == 'R') {
            resetGame();
            state = PLAYING;
            playMusic();
        }
        if (key == 27) state = MENU;
    }

    // -------- RETURN TO MENU FROM SUBMENUS --------
    else {
        state = MENU;
        playMusic();
    }
}

// ---------------- SPECIAL KEYS ----------------
void special(int key, int, int) {
    if (state == PLAYING) {
        if (key == GLUT_KEY_LEFT) paddleX -= 0.05f;
        if (key == GLUT_KEY_RIGHT) paddleX += 0.05f;

        if (paddleX < -0.85f) paddleX = -0.85f;
        if (paddleX > 0.85f) paddleX = 0.85f;
    }
}

// ---------------- INIT ----------------
void init() {
    glClearColor(0, 0, 0, 1);
    loadHighScore();
    resetGame();
}

// ---------------- MAIN ----------------
int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    glutInitWindowSize(800, 600);
    glutCreateWindow("DX Ball - Full Version");

    init();

    glutDisplayFunc(display);
    glutKeyboardFunc(keyboard);
    glutSpecialFunc(special);
    glutTimerFunc(0, update, 0);

    glutMainLoop();
}
