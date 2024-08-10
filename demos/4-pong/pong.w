: void ClearBackground(Color color);
: void BeginDrawing(void);
: void DrawRectangle(int posX, int posY, int width, int height, Color color);
: void DrawCircle(int centerX, int centerY, float radius, Color color);
: void DrawText(const char *text, int posX, int posY, int fontSize, Color color);
: void EndDrawing(void);
: void InitWindow(int width, int height, const char *title);
: bool WindowShouldClose(void);
: void CloseWindow(void);

#def SCREEN_WIDTH 1600
#def SCREEN_HEIGHT 900

#def PADDLE_WIDTH 20
#def PADDLE_HEIGHT 100

#def BALL_SIZE 10
#def PADDLE_SPEED 1
#def BALL_SPEED_X 1
#def BALL_SPEED_Y 1

#def KEY_UP 265
#def KEY_DOWN 264
#def KEY_W 87
#def KEY_S 83

!void& create_color_struct(!int <64> r, !int <64> g, !int <64> b, !int <64> a) 
{
    : // Color, 4 components, R8G8B8A8 (32bit)
    : typedef struct Color {
    :     unsigned char r;        // Color red value
    :     unsigned char g;        // Color green value
    :     unsigned char b;        // Color blue value
    :     unsigned char a;        // Color alpha value
    : } Color;

    !void& <64> color_ptr = malloc(4);
    ^(color_ptr + 0) = r;
    ^(color_ptr + 1) = g;
    ^(color_ptr + 2) = b;
    ^(color_ptr + 3) = a;

    ret color_ptr; 
}

init_game(!int& <64> ball_pos_x, !int& <64> ball_pos_y, !int& <64> ball_speed_x, !int& <64> ball_speed_y, !int& <64> left_paddle_y, !int& <64> right_paddle_y)
{    
    : reset ball position to center
    ^(ball_pos_x) = SCREEN_WIDTH / 2;
    ^(ball_pos_y) = SCREEN_HEIGHT / 2;

    : reset ball speed
    ^(ball_speed_x) = BALL_SPEED_X;
    ^(ball_speed_y) = BALL_SPEED_Y;

    : reset paddle positions
    ^(left_paddle_y) = SCREEN_HEIGHT / 2 - PADDLE_HEIGHT / 2;
    ^(right_paddle_y) = SCREEN_HEIGHT / 2 - PADDLE_HEIGHT / 2;
}


update_paddles(!int& <64> left_paddle_y, !int& <64> right_paddle_y)
{
    : move left paddle up
    if (@is_key_down(KEY_W) && ^(left_paddle_y) > 0) {
        ^(left_paddle_y) = ^(left_paddle_y) - PADDLE_SPEED;
    } 
    else {
        : move left paddle down
        if (@is_key_down(KEY_S) && ^(left_paddle_y) < SCREEN_HEIGHT - PADDLE_HEIGHT) {
            ^(left_paddle_y) = ^(left_paddle_y) + PADDLE_SPEED;
        }
    }

    : move right paddle up
    if (@is_key_down(KEY_UP) && ^(right_paddle_y) > 0) {
        ^(right_paddle_y) = ^(right_paddle_y) - PADDLE_SPEED;
    } 
    else {
        : move right paddle down
        if (@is_key_down(KEY_DOWN) && ^(right_paddle_y) < SCREEN_HEIGHT - PADDLE_HEIGHT) {
            ^(right_paddle_y) = ^(right_paddle_y) + PADDLE_SPEED;
        }
    }
}

update_ball(!int& <64> ball_pos_x, !int& <64> ball_pos_y, !int& <64> ball_speed_x, !int& <64> ball_speed_y, !int& <64> left_paddle_y, !int& <64> right_paddle_y, !int& <64> left_score, !int& <64> right_score)
{
    : update ball position
    ^(ball_pos_x) = ^(ball_pos_x) + ^(ball_speed_x);
    ^(ball_pos_y) = ^(ball_pos_y) + ^(ball_speed_y);

    : check for collision with top/bottom walls
    if (^(ball_pos_y) <= 0 || ^(ball_pos_y) >= SCREEN_HEIGHT - BALL_SIZE) {
        ^(ball_speed_y) = -^(ball_speed_y);  
    }

    : check for collision with left paddle
    if (^(ball_pos_x) <= PADDLE_WIDTH && ^(ball_pos_y) >= ^(left_paddle_y) && ^(ball_pos_y) <= ^(left_paddle_y) + PADDLE_HEIGHT) {
        ^(ball_speed_x) = -^(ball_speed_x); 
    } 
    else {
        : check for collision with right paddle
        if (^(ball_pos_x) >= SCREEN_WIDTH - PADDLE_WIDTH - BALL_SIZE && ^(ball_pos_y) >= ^(right_paddle_y) && ^(ball_pos_y) <= ^(right_paddle_y) + PADDLE_HEIGHT) {
            ^(ball_speed_x) = -^(ball_speed_x); 
        }
    }

    : check if ball has passed left edge
    if (^(ball_pos_x) <= 0) {
        ^(right_score) = ^(right_score) + 1;  
        init_game(ball_pos_x, ball_pos_y, ball_speed_x, ball_speed_y, left_paddle_y, right_paddle_y);  : reset game
    } 
    else {
        : check if ball has passed right edge
        if (^(ball_pos_x) >= SCREEN_WIDTH) {
            ^(left_score) = ^(left_score) + 1; 
            init_game(ball_pos_x, ball_pos_y, ball_speed_x, ball_speed_y, left_paddle_y, right_paddle_y);  : reset game
        }
    }
}

draw_game(<64> ball_pos_x, <64> ball_pos_y, <64> left_paddle_y, <64> right_paddle_y, <64> left_score, <64> right_score)
{
    : create colors
    !color_struct& <64> white_color = create_color_struct(255, 255, 255, 255);  : white
    !color_struct& <64> background_color = create_color_struct(10, 61, 139, 255);  : blue

    !wstr& <64> score_text[2];  

    @BeginDrawing();
    @ClearBackground(^background_color);  : clear the background with blue color

    @DrawRectangle(10, left_paddle_y, PADDLE_WIDTH, PADDLE_HEIGHT, ^white_color);  : draw left paddle
    @DrawRectangle(SCREEN_WIDTH - PADDLE_WIDTH - 10, right_paddle_y, PADDLE_WIDTH, PADDLE_HEIGHT, ^white_color);  : draw right paddle
    @DrawRectangle(ball_pos_x, ball_pos_y, BALL_SIZE, BALL_SIZE, ^white_color);  : draw ball

    ^(score_text - 1*8) = left_score + 48;  : convert left score to ASCII
    ^(score_text - 2*8)  = 0;

    !cstr& <64> score_text_cstr = wstr_to_cstr(score_text, 2); 
    @DrawText(score_text_cstr, SCREEN_WIDTH / 4, 20, 40, ^white_color);  : draw left player's score
    
    ^(score_text - 1*8) = right_score + 48;  : convert right score to ASCII

    score_text_cstr = wstr_to_cstr(score_text, 2); 
    @DrawText(score_text_cstr, SCREEN_WIDTH * 3 / 4 - 40, 20, 40, ^white_color);  : draw right player's score

    @EndDrawing();
}

glb <64> main()
{
    : initialize game variables
    <64> ball_pos_x;
    <64> ball_pos_y;
    <64> ball_speed_x;
    <64> ball_speed_y;
    <64> left_paddle_y;
    <64> right_paddle_y;
    <64> left_score = 0;
    <64> right_score = 0;

    : initialize window
    !wstr& <64> window_title[10] = "Pong Game\0";
    !cstr& <64> window_tile_cstr = wstr_to_cstr(window_title, 10); 
    @InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, window_tile_cstr);

    init_game(&ball_pos_x, &ball_pos_y, &ball_speed_x, &ball_speed_y, &left_paddle_y, &right_paddle_y); 

    : game loop
    loop {
        if (@WindowShouldClose()) {
            break;
        }

        update_paddles(&left_paddle_y, &right_paddle_y); 
        update_ball(&ball_pos_x, &ball_pos_y, &ball_speed_x, &ball_speed_y, &left_paddle_y, &right_paddle_y, &left_score, &right_score); 
        draw_game(ball_pos_x, ball_pos_y, left_paddle_y, right_paddle_y, left_score, right_score); 

        sleep(2);  : wait 2ms
    }

    @CloseWindow(); 

    ret 0;
}