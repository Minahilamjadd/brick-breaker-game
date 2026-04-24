#include <iostream>
#include "C:\raylib\raylib\src\raylib.h"

#include <vector>
#include <string>

using namespace std;

// Define constants
const int screenWidth = 865;
const int screenHeight = 600;
const int initialPaddleWidth = 200; // Initial paddle width
const int paddleHeight = 20;
const int brickWidth = 80;
const int brickHeight = 30;
const int rows = 5;
const int cols = 10;
const int maxMisses = 5;
const float ballRadius = 10.0f;

struct Brick {
    Rectangle rect;
    bool active;
};

void ResetGame(Rectangle &paddle, Vector2 &ball, Vector2 &ballSpeed, vector<Brick> &bricks, int &misses, bool &gameOver, bool &win) {
    paddle = {screenWidth / 2 - initialPaddleWidth / 2, screenHeight - 40, initialPaddleWidth, paddleHeight};
    // Position ball just above the paddle and set the speed to go upwards
    ball = {screenWidth / 2, screenHeight - paddleHeight - ballRadius - 20};  // Ball starts right above the paddle
    ballSpeed = {300.0f, -300.0f};  // Ensure the ball is moving upwards (negative y velocity)
    bricks.clear();
    for (int row = 0; row < rows; row++) {
        for (int col = 0; col < cols; col++) {
            Brick brick = {
                {(float)(col * (brickWidth + 5) + 10), (float)(row * (brickHeight + 5) + 10), brickWidth, brickHeight},
                true};
            bricks.push_back(brick);
        }
    }
    misses = 0;  // Make sure misses is reset to 0
    gameOver = false;
    win = false;
}

int main() {
    // Initialization
    InitWindow(screenWidth, screenHeight, "Brick Breaker");
    InitAudioDevice(); // Initialize the audio device
    SetTargetFPS(60);

    // Load sounds
    Sound gameOverSound = LoadSound("C://Cainoosh//adat.wav");  // Make sure this file exists in your project folder
    Sound gameSound = LoadSound("C://Cainoosh//theme.mp3");

    // Paddle
    Rectangle paddle = {screenWidth / 2 - initialPaddleWidth / 2, screenHeight - 40, initialPaddleWidth, paddleHeight};
    const float paddleSpeed = 500.0f;

    // Ball
    Vector2 ball = {screenWidth / 2, screenHeight - paddleHeight - ballRadius - 30};
    Vector2 ballSpeed = {300.0f, -300.0f};

    // Bricks
    vector<Brick> bricks;
    for (int row = 0; row < rows; row++) {
        for (int col = 0; col < cols; col++) {
            Brick brick = {
                {(float)(col * (brickWidth + 5) + 10), (float)(row * (brickHeight + 5) + 10), brickWidth, brickHeight},
                true};
            bricks.push_back(brick);
        }
    }

    int misses = 0; // Track missed balls
    bool gameOver = false;
    bool win = false;
    int hitCounter = 0; // Counter for paddle hits

    bool gameStarted = false;  // Track whether the game has started

    bool gameOverSoundPlayed = false;  // Flag to track if the game over sound has been played

    // Game loop
    while (!WindowShouldClose()) {
        // Update
        if (!gameOver && !win) {
            if (!gameStarted) {
                // Wait for the user to press space to start the game
                if (IsKeyPressed(KEY_SPACE)) {
                    gameStarted = true;  // Start the game
                    PlaySound(gameSound);
                }
            }
            else {
                float deltaTime = GetFrameTime();

                // Paddle movement
                if (IsKeyDown(KEY_LEFT) && paddle.x > 0)
                    paddle.x -= paddleSpeed * deltaTime;
                if (IsKeyDown(KEY_RIGHT) && paddle.x + paddle.width < screenWidth)
                    paddle.x += paddleSpeed * deltaTime;

                // Ball movement
                ball.x += ballSpeed.x * deltaTime;
                ball.y += ballSpeed.y * deltaTime;

                // Ball collision with screen edges
                if (ball.x <= ballRadius || ball.x >= screenWidth - ballRadius)
                    ballSpeed.x *= -1;
                if (ball.y <= ballRadius)
                    ballSpeed.y *= -1;

                // Ball collision with paddle
                if (CheckCollisionCircleRec(ball, ballRadius, paddle)) {
                    ballSpeed.y *= -1; // Reverse the ball's vertical direction
                    hitCounter++; // Increment hit counter

                    // Increase speed every 5 hits
                    if (hitCounter >= 5) {
                        ballSpeed.x *= 1.08f; // Increase speed by 5%
                        ballSpeed.y *= 1.08f; // Increase speed by 5%
                        hitCounter = 0; // Reset hit counter

                        // Decrease paddle width every 5 hits
                        if (paddle.width > 50) {
                            paddle.width *= 0.90f; // Decrease width by 10%
                        }
                    }
                }

                // Ball collision with bricks
                for (auto &brick : bricks) {
                    if (brick.active && CheckCollisionCircleRec(ball, ballRadius, brick.rect)) {
                        brick.active = false;
                        ballSpeed.y *= -1;
                        break;
                    }
                }

                // Ball reset on falling below screen
                if (ball.y > screenHeight) {
                    ball = {paddle.x + paddle.width / 2, screenHeight - paddleHeight - ballRadius - 20};
                    ballSpeed.y *= -1;
                    misses++;
                    if (misses >= maxMisses)
                        gameOver = true;
                }

                // Check if all bricks are destroyed
                win = true;
                for (const auto &brick : bricks) {
                    if (brick.active) {
                        win = false;
                        break;
                    }
                }
            }
        }

        // Draw
        BeginDrawing();
        ClearBackground(BLACK);

        if (!gameStarted) {
            // Draw the "Press Space to Start" message
            DrawText("Press SPACE to Start", screenWidth / 2 - 140, screenHeight / 2 - 30, 20, WHITE);
        } else {
            // Draw paddle
            DrawRectangleRec(paddle, VIOLET);

            // Draw ball
            DrawCircleV(ball, ballRadius, YELLOW);

            // Draw bricks
            for (const auto &brick : bricks) {
                if (brick.active)
                    DrawRectangleRec(brick.rect, VIOLET);
            }

            // Display game over or win message
            if (gameOver) {
                StopSound(gameSound);
                if (!gameOverSoundPlayed) {
                    PlaySound(gameOverSound);  // Play sound only once
                    gameOverSoundPlayed = true; // Set flag to true
                }

                DrawText("LOSERRR HEHEHEH!!!!", screenWidth / 2 - 110, screenHeight / 2 - 30, 20, RED);
                DrawText("Press R to Restart", screenWidth / 2 - 100, screenHeight / 2 + 10, 20, WHITE);
                if (IsKeyPressed(KEY_R)) {
                    StopSound(gameOverSound);  // Stop sound before restarting
                    ResetGame(paddle, ball, ballSpeed, bricks, misses, gameOver, win);
                    gameStarted = false; // Reset the game state to waiting
                    gameOverSoundPlayed = false; // Reset the sound flag for next game
                }
            } else if (win) {
                DrawText("GOOD JOB NERD :)", screenWidth / 2 - 90, screenHeight / 2 - 30, 20, GREEN);
                DrawText("Press R to Restart", screenWidth / 2 - 100, screenHeight / 2 + 10, 20, WHITE);
                if (IsKeyPressed(KEY_R)) {
                    ResetGame(paddle, ball, ballSpeed, bricks, misses, gameOver, win);
                    gameStarted = false; // Reset the game state to waiting
                }
            } else {
                // Display misses
                DrawText(("Misses: " + to_string(misses)).c_str(), 10, screenHeight - 20, 20, WHITE);
            }
        }

        EndDrawing();
    }

    // De-Initialization
    CloseWindow(); // Close window and OpenGL context

    // Unload sound
    UnloadSound(gameOverSound);

    return 0;
}
