: void ClearBackground(Color color);
: void BeginDrawing(void);
: void DrawRectangle(int posX, int posY, int width, int height, Color color);
: void DrawCircle(int centerX, int centerY, float radius, Color color);
: void DrawText(const char *text, int posX, int posY, int fontSize, Color color);
: void EndDrawing(void);
: void InitWindow(int width, int height, const char *title);
: bool WindowShouldClose(void);
: void CloseWindow(void);

#def SCREEN_WIDTH 800
#def SCREEN_HEIGHT 450

#def PADDLE_WIDTH 20
#def PADDLE_HEIGHT 100
#def BALL_SIZE 10

#def PADDLE_SPEED 5
#def BALL_SPEED_X 4
#def BALL_SPEED_Y 4

#def KEY_UP 265
#def KEY_DOWN 264
#def KEY_W 119
#def KEY_S 115

glb !void update_paddles(!int& <64> left_paddle_y, !int& <64> right_paddle_y)
{
    if (IsKeyDown(KEY_W) && left_paddle_y > 0) {
        left_paddle_y = left_paddle_y - PADDLE_SPEED;
    } else if (IsKeyDown(KEY_S) && left_paddle_y < SCREEN_HEIGHT - PADDLE_HEIGHT) {
        left_paddle_y = left_paddle_y + PADDLE_SPEED;
    }

    if (IsKeyDown(KEY_UP) && right_paddle_y > 0) {
        right_paddle_y = right_paddle_y - PADDLE_SPEED;
    } else if (IsKeyDown(KEY_DOWN) && right_paddle_y < SCREEN_HEIGHT - PADDLE_HEIGHT) {
        right_paddle_y = right_paddle_y + PADDLE_SPEED;
    }
}

glb !void update_ball(!int& <64> ball_pos_x, !int& <64> ball_pos_y, !int& <64> ball_speed_x, !int& <64> ball_speed_y, !int <64> left_paddle_y, !int <64> right_paddle_y, !int& <64> left_score, !int& <64> right_score)
{
    ball_pos_x = ball_pos_x + ball_speed_x;
    ball_pos_y = ball_pos_y + ball_speed_y;

    if (ball_pos_y <= 0 || ball_pos_y >= SCREEN_HEIGHT - BALL_SIZE) {
        ball_speed_y = -ball_speed_y;
    }

    if (ball_pos_x <= PADDLE_WIDTH && ball_pos_y >= left_paddle_y && ball_pos_y <= left_paddle_y + PADDLE_HEIGHT) {
        ball_speed_x = -ball_speed_x;
    } else if (ball_pos_x >= SCREEN_WIDTH - PADDLE_WIDTH - BALL_SIZE && ball_pos_y >= right_paddle_y && ball_pos_y <= right_paddle_y + PADDLE_HEIGHT) {
        ball_speed_x = -ball_speed_x;
    }

    if (ball_pos_x <= 0) {
        right_score = right_score + 1;
        InitGame(ball_pos_x, ball_pos_y, ball_speed_x, ball_speed_y, left_paddle_y, right_paddle_y);
    } else if (ball_pos_x >= SCREEN_WIDTH) {
        left_score = left_score + 1;
        InitGame(ball_pos_x, ball_pos_y, ball_speed_x, ball_speed_y, left_paddle_y, right_paddle_y);
    }
}

glb !void draw_game(!int <64> ball_pos_x, !int <64> ball_pos_y, !int <64> left_paddle_y, !int <64> right_paddle_y, !int <64> left_score, !int <64> right_score)
{
    !ascii <64> score_text[2];

    @BeginDrawing();
    @ClearBackground(0x0C0C0CFF);

    @DrawRectangle(10, left_paddle_y, PADDLE_WIDTH, PADDLE_HEIGHT, 0xFFFFFFFF);  
    @DrawRectangle(SCREEN_WIDTH - PADDLE_WIDTH - 10, right_paddle_y, PADDLE_WIDTH, PADDLE_HEIGHT, 0xFFFFFFFF);  
    @DrawCircle(ball_pos_x, ball_pos_y, BALL_SIZE, 0xFFFFFFFF);  

    score_text[0] = left_score + 48;  : Convertir en code ASCII
    score_text[1] = 0;
    @DrawText(score_text, SCREEN_WIDTH / 4, 20, 40, 0xFFFFFFFF);
    
    score_text[0] = right_score + 48;  : Convertir en code ASCII
    @DrawText(score_text, SCREEN_WIDTH * 3 / 4 - 40, 20, 40, 0xFFFFFFFF);

    @EndDrawing();
}

glb !int <64> main()
{
    : init game
    !int <64> ball_pos_x = SCREEN_WIDTH / 2;
    !int <64> ball_pos_y = SCREEN_HEIGHT / 2;
    !int <64> ball_speed_x = BALL_SPEED_X;
    !int <64> ball_speed_y = BALL_SPEED_Y;
    !int <64> left_paddle_y = SCREEN_HEIGHT / 2 - PADDLE_HEIGHT / 2;
    !int <64> right_paddle_y = SCREEN_HEIGHT / 2 - PADDLE_HEIGHT / 2;
    !int <64> left_score = 0;
    !int <64> right_score = 0;

    !ascii <64> window_title[11] = "Pong Game";
    @InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, window_title);

    : game loop
    loop {
        if (@WindowShouldClose()) {
            break;
        }

        update_paddles(left_paddle_y, right_paddle_y);
        update_ball(ball_pos_x, ball_pos_y, ball_speed_x, ball_speed_y, left_paddle_y, right_paddle_y, left_score, right_score);
        draw_game(ball_pos_x, ball_pos_y, left_paddle_y, right_paddle_y, left_score, right_score);
    }

    @CloseWindow();

    ret 0;
}
