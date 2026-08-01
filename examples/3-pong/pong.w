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

glb !void* create_color_struct(!int <64> r, !int <64> g, !int <64> b, !int <64> a) 
{
    : // Color, 4 components, R8G8B8A8 (32bit)
    : typedef struct Color {
    :     unsigned char r;        // Color red value
    :     unsigned char g;        // Color green value
    :     unsigned char b;        // Color blue value
    :     unsigned char a;        // Color alpha value
    : } Color;

    !void* <64> color_ptr = @malloc(4);
    ^(color_ptr + 0) = r;
    ^(color_ptr + 1) = g;
    ^(color_ptr + 2) = b;
    ^(color_ptr + 3) = a;

    ret color_ptr; 
}

glb !void free_color(!void* <64> color_ptr) {
    @free(color_ptr);  : Libérer la mémoire allouée pour la couleur
}



glb !void init_game(!int& <64> ball_pos_x, !int& <64> ball_pos_y, !int& <64> ball_speed_x, !int& <64> ball_speed_y, !int& <64> left_paddle_y, !int& <64> right_paddle_y)
{
    : Réinitialise les positions de la balle et des paddles
    ^(ball_pos_x) = SCREEN_WIDTH / 2;
    ^(ball_pos_y) = SCREEN_HEIGHT / 2;

    : Réinitialise la vitesse de la balle
    ^(ball_speed_x) = BALL_SPEED_X;
    ^(ball_speed_y) = BALL_SPEED_Y;

    : Réinitialise les positions des paddles
    ^(left_paddle_y) = SCREEN_HEIGHT / 2 - PADDLE_HEIGHT / 2;
    ^(right_paddle_y) = SCREEN_HEIGHT / 2 - PADDLE_HEIGHT / 2;
}


!void update_paddles(!int& <64> left_paddle_y, !int& <64> right_paddle_y)
{
    if (@is_key_down(KEY_W) && ^(left_paddle_y) > 0) {
        ^(left_paddle_y) = ^(left_paddle_y) - PADDLE_SPEED;
    } 
    else {
        if (@is_key_down(KEY_S) && ^(left_paddle_y) < SCREEN_HEIGHT - PADDLE_HEIGHT) {
            ^(left_paddle_y) = ^(left_paddle_y) + PADDLE_SPEED;
        }
    }
    
    if (@is_key_down(KEY_UP) && ^(right_paddle_y) > 0) {
        ^(right_paddle_y) = ^(right_paddle_y) - PADDLE_SPEED;
    } 
    else {
        if (@is_key_down(KEY_DOWN) && ^(right_paddle_y) < SCREEN_HEIGHT - PADDLE_HEIGHT) {
            ^(right_paddle_y) = ^(right_paddle_y) + PADDLE_SPEED;
        }
    }
}

glb !void update_ball(!int& <64> ball_pos_x, !int& <64> ball_pos_y, !int& <64> ball_speed_x, !int& <64> ball_speed_y, !int& <64> left_paddle_y, !int& <64> right_paddle_y, !int& <64> left_score, !int& <64> right_score)
{
    ^(ball_pos_x) = ^(ball_pos_x) + ^(ball_speed_x);
    ^(ball_pos_y) = ^(ball_pos_y) + ^(ball_speed_y);

    if (^(ball_pos_y) <= 0 || ^(ball_pos_y) >= SCREEN_HEIGHT - BALL_SIZE) {
        ^(ball_speed_y) = -^(ball_speed_y);
    }

    if (^(ball_pos_x) <= PADDLE_WIDTH && ^(ball_pos_y) >= ^(left_paddle_y) && ^(ball_pos_y) <= ^(left_paddle_y) + PADDLE_HEIGHT) {
        ^(ball_speed_x) = -^(ball_speed_x);
    } 
    else {
        if (^(ball_pos_x) >= SCREEN_WIDTH - PADDLE_WIDTH - BALL_SIZE && ^(ball_pos_y) >= ^(right_paddle_y) && ^(ball_pos_y) <= ^(right_paddle_y) + PADDLE_HEIGHT) {
            ^(ball_speed_x) = -^(ball_speed_x);
        }
    }

    if (^(ball_pos_x) <= 0) {
        ^(right_score) = ^(right_score) + 1;
        init_game(ball_pos_x, ball_pos_y, ball_speed_x, ball_speed_y, left_paddle_y, right_paddle_y);
    } 
    else {
        if (^(ball_pos_x) >= SCREEN_WIDTH) {
            ^(left_score) = ^(left_score) + 1;
            init_game(ball_pos_x, ball_pos_y, ball_speed_x, ball_speed_y, left_paddle_y, right_paddle_y);
        }
    }
}

glb !void draw_game(!int <64> ball_pos_x, !int <64> ball_pos_y, !int <64> left_paddle_y, !int <64> right_paddle_y, !int <64> left_score, !int <64> right_score)
{
    : create colors
    !void* <64> white_color = create_color_struct(255, 255, 255, 255);  : Blanc
    !void* <64> background_color = create_color_struct(10, 61, 139, 255);  : Bleu

    !ascii <64> score_text[2];

    @BeginDrawing();
    @ClearBackground(^background_color);

    @DrawRectangle(10, left_paddle_y, PADDLE_WIDTH, PADDLE_HEIGHT, ^white_color);  
    @DrawRectangle(SCREEN_WIDTH - PADDLE_WIDTH - 10, right_paddle_y, PADDLE_WIDTH, PADDLE_HEIGHT, ^white_color);  
    @DrawRectangle(ball_pos_x, ball_pos_y, BALL_SIZE, BALL_SIZE, ^white_color);  

    ^(score_text - 1*8) = left_score + 48;  : Convertir en code ASCII
    ^(score_text - 2*8)  = 0;

    !ascii& <64> score_text_char_ptr = convert_ascii_array_to_char_ptr(score_text, 2); 
    @DrawText(score_text_char_ptr, SCREEN_WIDTH / 4, 20, 40, ^white_color);
    
    ^(score_text - 1*8) = right_score + 48;  : Convertir en code ASCII

    score_text_char_ptr = convert_ascii_array_to_char_ptr(score_text, 2); 
    @DrawText(score_text_char_ptr, SCREEN_WIDTH * 3 / 4 - 40, 20, 40, ^white_color);

    @EndDrawing();
}
glb !int <64> main()
{
    : init game
    !int <64> ball_pos_x;
    !int <64> ball_pos_y;
    !int <64> ball_speed_x;
    !int <64> ball_speed_y;
    !int <64> left_paddle_y;
    !int <64> right_paddle_y;
    !int <64> left_score = 0;
    !int <64> right_score = 0;

    !ascii <64> window_title[10] = "Pong Game\0";
    !ascii& <64> window_tile_char_ptr = convert_ascii_array_to_char_ptr(window_title, 10); 
    @InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT);

    init_game(&ball_pos_x, &ball_pos_y, &ball_speed_x, &ball_speed_y, &left_paddle_y, &right_paddle_y);

    : game loop
    loop {
        if (@WindowShouldClose()) {
            break;
        }

        update_paddles(&left_paddle_y, &right_paddle_y);
        update_ball(&ball_pos_x, &ball_pos_y, &ball_speed_x, &ball_speed_y, &left_paddle_y, &right_paddle_y, &left_score, &right_score);
        draw_game(ball_pos_x, ball_pos_y, left_paddle_y, right_paddle_y, left_score, right_score);

        @usleep(2000);
    }

    @CloseWindow();


    ret 0;
}
