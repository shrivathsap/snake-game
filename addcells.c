//compile with gcc snake.c -o snake -I/usr/local/include -L/usr/local/lib -lSDL2 -lSDL2_image -lSDL2_ttf -lSDL2_mixer

//https://archive.org/details/BeautifulJapaneseMusicZenGarden/Beautiful+Japanese+Music+-+Cherry+Blossoms.mp3
//https://ia800809.us.archive.org/6/items/various-sound-effects/lid.ogg
//https://ia802701.us.archive.org/9/items/BeautifulJapaneseMusicZenGarden/Beautiful%20Japanese%20Music%20-%20Cherry%20Blossoms.ogg

#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include<SDL2/SDL.h>
#include<SDL2/SDL_timer.h>
#include<SDL2/SDL_image.h>
#include<SDL2/SDL_ttf.h>
#include<SDL2/SDL_mixer.h>

#define WINDOW_WIDTH (640)
#define GAME_WIDTH (480)
#define WINDOW_HEIGHT (480)
#define SIZE (32)
#define FPS (12)
#define FONTSIZE (42)
#define GAP (10)
//gap between button text and button tick

#define AGAIN (250)

//score holder
#define score_x (480)
#define score_y (10)
#define score_w (150)
#define score_h (50)

//border button holder
#define border_x (480)
#define border_y (70)
#define border_w (100)
#define border_h (50)

//sound button holder
#define sound_x (480)
#define sound_y (140)
#define sound_w (100)
#define sound_h (50)

//music button holder
#define music_x (480)
#define music_y (210)
#define music_w (100)
#define music_h (50)

struct snake{
    SDL_Rect * cell;
    SDL_Texture * tex;
    struct snake * prev;
}snake;

struct buttons{
    //int pressed;
    SDL_Rect * button_text;
    SDL_Rect * button_tick;
    SDL_Texture * clicked;
    SDL_Texture * unclicked;
    SDL_Texture * text;
}buttons;

const char* path_head_u = "/home/shrivathsa/reC/graphics/resources/head_u.png";
const char* path_head_l = "/home/shrivathsa/reC/graphics/resources/head_l.png";
const char* path_head_d = "/home/shrivathsa/reC/graphics/resources/head_d.png";
const char* path_head_r = "/home/shrivathsa/reC/graphics/resources/head_r.png";
const char* path_body_sv = "/home/shrivathsa/reC/graphics/resources/body_sv.png";
const char* path_body_sh = "/home/shrivathsa/reC/graphics/resources/body_sh.png";
const char* path_body_ur = "/home/shrivathsa/reC/graphics/resources/body_ur.png";
const char* path_body_ul = "/home/shrivathsa/reC/graphics/resources/body_ul.png";
const char* path_body_dr = "/home/shrivathsa/reC/graphics/resources/body_dr.png";
const char* path_body_dl = "/home/shrivathsa/reC/graphics/resources/body_dl.png";
const char* path_tail_ul = "/home/shrivathsa/reC/graphics/resources/tail_ul.png";
const char* path_tail_ur = "/home/shrivathsa/reC/graphics/resources/tail_ur.png";
const char* path_tail_dl = "/home/shrivathsa/reC/graphics/resources/tail_dl.png";
const char* path_tail_dr = "/home/shrivathsa/reC/graphics/resources/tail_dr.png";
const char* path_tail_ll = "/home/shrivathsa/reC/graphics/resources/tail_ll.png";
const char* path_tail_lr = "/home/shrivathsa/reC/graphics/resources/tail_lr.png";
const char* path_tail_rl = "/home/shrivathsa/reC/graphics/resources/tail_rl.png";
const char* path_tail_rr = "/home/shrivathsa/reC/graphics/resources/tail_rr.png";
const char* path_food = "/home/shrivathsa/reC/graphics/resources/apple.png";
const char* path_bite = "/home/shrivathsa/reC/graphics/resources/bite.ogg";
const char* path_bgm = "/home/shrivathsa/reC/graphics/resources/zen.ogg";
const char* path_tick= "/home/shrivathsa/reC/graphics/resources/tick.png";
const char* path_no_tick = "/home/shrivathsa/reC/graphics/resources/no_tick.png";
const char* path_font = "/home/shrivathsa/reC/graphics/resources/DejaVuSansCondensed.ttf";


struct snake *temp;//a dummy to hold the tail when spawning new food

int food_x, food_y, conflict, score=0;//conflict to see if food spawns over snake's body

//global variables
int close_requested = 0; int up = 0; int left = 0; int down = 0; int right = 0; int is_running = 0;
int state_changed = 0;//to see if direction changed
char previous_direction;

struct buttons border_button, sound_button, music_button;
SDL_Rect border_button_text, border_button_tick, sound_button_text, sound_button_tick, music_button_text, music_button_tick;

//keeping track of settings across games (i.e., after restarting etc)
int border_button_pressed = 1; int sound_button_pressed = 1; int music_button_pressed = 1;

SDL_Rect head, tail, food, right_edge, score_rect;
struct snake first_cell, last_cell, food_cell;
struct snake * new_last_cell;//pointer for holding newly added cells
struct snake * current;//pointer tracking which cell is being rendered
SDL_Texture * food_texture;
SDL_Texture * tick_texture; SDL_Texture * no_tick_texture;
SDL_Texture * head_texture_u; SDL_Texture * head_texture_l; SDL_Texture * head_texture_d; SDL_Texture * head_texture_r;
SDL_Texture * body_texture_sh; SDL_Texture * body_texture_sv; SDL_Texture * body_texture_dl;
SDL_Texture * body_texture_dr; SDL_Texture * body_texture_ul; SDL_Texture * body_texture_ur;
// SDL_Texture * tail_texture;//_l; SDL_Texture * tail_texture_r;

SDL_Texture * tail_texture_ul; SDL_Texture * tail_texture_ur; SDL_Texture * tail_texture_dl; SDL_Texture * tail_texture_dr;
SDL_Texture * tail_texture_ll; SDL_Texture * tail_texture_lr; SDL_Texture * tail_texture_rl; SDL_Texture * tail_texture_rr;

SDL_Texture * up_tail_textures[2]; SDL_Texture * down_tail_textures[2];
SDL_Texture * left_tail_textures[2]; SDL_Texture * right_tail_textures[2];

int tail_state = 1;//for turning the tail

//restart_screen stuff
SDL_Rect play_again, yes, no;
SDL_Texture * play_again_tex; SDL_Texture * yes_tex; SDL_Texture * no_tex;

//sounds
Mix_Chunk * bite; Mix_Music * bgm;

struct snake * add_cell(){
    struct snake * new_tail = malloc(sizeof(snake));//create a memory space for the tail
    SDL_Rect * new_cell = malloc(sizeof(SDL_Rect));
    new_cell->w = SIZE;
    new_cell->h = SIZE;
    new_tail->cell = new_cell;
    new_tail->prev = current;
    return new_tail;
}

void spawn_food(){
    temp = current;
    conflict = 1;
    while (conflict){
        food_x = rand()%GAME_WIDTH;
        food_y = rand()%WINDOW_HEIGHT;
        food_x -= food_x%SIZE;
        food_y -= food_y%SIZE;
        while (temp->prev!= NULL){
            if ((food_x == temp->cell->x)&&(food_y == temp->cell->y)){
                food_x = rand()%GAME_WIDTH;
                food_y = rand()%WINDOW_HEIGHT;
                food_x -= food_x%SIZE;
                food_y -= food_y%SIZE;
                temp = current;
            }
            temp = temp->prev;
        }
        //tail is now the head of the snake, final check
        if ((food_x != temp->cell->x)||(food_y != temp->cell->y)){
            conflict = 0;
        }
    }

    food_cell.cell->x = food_x;
    food_cell.cell->y = food_y;
}

int has_eaten(){
    if ((first_cell.cell->x == food_cell.cell->x)&&(first_cell.cell->y == food_cell.cell->y)){
        return 1;
    }
    return 0;
}

int self_collide(){
    //when collision is tested, current points to the head of the snake (having shifted cells along the body of the snake)
    temp = new_last_cell;
    while (temp->prev != NULL){
        if ((temp->cell->x == first_cell.cell->x) && (temp->cell->y == first_cell.cell->y)){
            return 1;
        }
        temp = temp->prev;
    }
    return 0;
}

void draw_edge(SDL_Renderer * rend){
    SDL_SetRenderDrawColor(rend, 255, 255, 255, SDL_ALPHA_OPAQUE);
    SDL_RenderFillRect(rend, &right_edge);
    SDL_SetRenderDrawColor(rend, 0, 0, 0, 0);
}

int collision(int boundary){
    if (self_collide()) return 1;
    if (boundary){
        if (first_cell.cell->x < 0) return 1;
        if (first_cell.cell->x > GAME_WIDTH-SIZE) return 1;
        if (first_cell.cell->y < 0) return 1;
        if (first_cell.cell->y > WINDOW_HEIGHT-SIZE) return 1;
    }
    if (first_cell.cell->x < 0) first_cell.cell->x = GAME_WIDTH-SIZE;
    if (first_cell.cell->x > GAME_WIDTH-SIZE) first_cell.cell->x = 0;
    if (first_cell.cell->y < 0) first_cell.cell->y = WINDOW_HEIGHT-SIZE;
    if (first_cell.cell->y > WINDOW_HEIGHT-SIZE) first_cell.cell->y = 0;
    return 0;
}

void load_textures(SDL_Renderer * rend){
    SDL_Surface * surface = IMG_Load(path_food);
    food_texture = SDL_CreateTextureFromSurface(rend, surface);
    {surface = IMG_Load(path_head_d);
    head_texture_d = SDL_CreateTextureFromSurface(rend, surface);
    surface = IMG_Load(path_head_u);
    head_texture_u = SDL_CreateTextureFromSurface(rend, surface);
    surface = IMG_Load(path_head_r);
    head_texture_r = SDL_CreateTextureFromSurface(rend, surface);
    surface = IMG_Load(path_head_l);
    head_texture_l = SDL_CreateTextureFromSurface(rend, surface);}
    {surface = IMG_Load(path_body_sv);
    body_texture_sv = SDL_CreateTextureFromSurface(rend, surface);
    surface = IMG_Load(path_body_sh);
    body_texture_sh = SDL_CreateTextureFromSurface(rend, surface);
    surface = IMG_Load(path_body_ur);
    body_texture_ur = SDL_CreateTextureFromSurface(rend, surface);
    surface = IMG_Load(path_body_ul);
    body_texture_ul = SDL_CreateTextureFromSurface(rend, surface);
    surface = IMG_Load(path_body_dr);
    body_texture_dr = SDL_CreateTextureFromSurface(rend, surface);
    surface = IMG_Load(path_body_dl);
    body_texture_dl = SDL_CreateTextureFromSurface(rend, surface);}
    {surface = IMG_Load(path_tail_ul);
    tail_texture_ul = SDL_CreateTextureFromSurface(rend, surface);
    surface = IMG_Load(path_tail_ur);
    tail_texture_ur = SDL_CreateTextureFromSurface(rend, surface);
    surface = IMG_Load(path_tail_dl);
    tail_texture_dl = SDL_CreateTextureFromSurface(rend, surface);
    surface = IMG_Load(path_tail_dr);
    tail_texture_dr = SDL_CreateTextureFromSurface(rend, surface);
    surface = IMG_Load(path_tail_ll);
    tail_texture_ll = SDL_CreateTextureFromSurface(rend, surface);
    surface = IMG_Load(path_tail_lr);
    tail_texture_lr = SDL_CreateTextureFromSurface(rend, surface);
    surface = IMG_Load(path_tail_rl);
    tail_texture_rl = SDL_CreateTextureFromSurface(rend, surface);
    surface = IMG_Load(path_tail_rr);
    tail_texture_rr = SDL_CreateTextureFromSurface(rend, surface);}
}

SDL_Texture * load_no_tick(SDL_Renderer *rend){
    SDL_Surface* surface1 = IMG_Load(path_no_tick);
    if (!surface1){
        printf("Error loading image %s\n", SDL_GetError());
        return NULL;
    }
    SDL_Texture* tex = SDL_CreateTextureFromSurface(rend, surface1);
    SDL_FreeSurface(surface1);
    if(!tex){
        printf("error creating texture %s\n", SDL_GetError());
        return NULL;
    }
    return tex;
}
SDL_Texture * load_tick(SDL_Renderer *rend){
    SDL_Surface* surface1 = IMG_Load(path_tick);
    if (!surface1){
        printf("Error loading image %s\n", SDL_GetError());
        return NULL;
    }
    SDL_Texture* tex = SDL_CreateTextureFromSurface(rend, surface1);
    SDL_FreeSurface(surface1);
    if(!tex){
        printf("error creating texture %s\n", SDL_GetError());
        return NULL;
    }
    return tex;
}
SDL_Texture * load_message(SDL_Renderer *rend, char * message, TTF_Font * font, SDL_Color color){
    SDL_Surface *message_converted = TTF_RenderText_Solid(font, message, color);
    SDL_Texture *message_texture = SDL_CreateTextureFromSurface(rend, message_converted);
    if (!message_converted) printf("%s\n", SDL_GetError());
    SDL_FreeSurface(message_converted);
    return message_texture;
}

void update_score(SDL_Renderer * rend, TTF_Font * font, SDL_Color color){
    char message[10];
    int j = snprintf(message, 10, "Score: %d", score);//magik
    SDL_Surface * score_board = TTF_RenderText_Solid(font, message, color);
    SDL_Texture * score_message = SDL_CreateTextureFromSurface(rend, score_board);
    SDL_RenderCopy(rend, score_message, NULL, &score_rect);
    SDL_FreeSurface(score_board);
    SDL_DestroyTexture(score_message);
}

void create_buttons(SDL_Renderer * rend, TTF_Font *font, SDL_Color color){
    tick_texture = load_tick(rend);
    no_tick_texture = load_no_tick(rend);

    //border_button
    //border_button_pressed = 1;
    border_button.button_text = &border_button_text;
    border_button.button_tick = &border_button_tick;
    border_button.clicked = tick_texture;
    border_button.unclicked = no_tick_texture;
    border_button_text.x = border_x; border_button_text.y = border_y; border_button_text.w = border_w; border_button_text.h = border_h;
    border_button_tick.x = border_x+border_w+GAP; border_button_tick.y = border_y+GAP;
    border_button_tick.w = SIZE; border_button_tick.h = SIZE;
    border_button.text = load_message(rend, "Boundary", font, color);

    //sound_button
    //sound_button_pressed = 1;
    sound_button.button_text = &sound_button_text;
    sound_button.button_tick = &sound_button_tick;
    sound_button.clicked = tick_texture;
    sound_button.unclicked = no_tick_texture;
    sound_button_text.x = sound_x; sound_button_text.y = sound_y; sound_button_text.w = sound_w; sound_button_text.h = sound_h;
    sound_button_tick.x = sound_x+sound_w+GAP; sound_button_tick.y = sound_y+GAP;
    sound_button_tick.w = SIZE; sound_button_tick.h = SIZE;
    sound_button.text = load_message(rend, "Sound", font, color);

    //music_button
    //music_button_pressed = 1;
    music_button.button_text = &music_button_text;
    music_button.button_tick = &music_button_tick;
    music_button.clicked = tick_texture;
    music_button.unclicked = no_tick_texture;
    music_button_text.x = music_x; music_button_text.y = music_y; music_button_text.w = music_w; music_button_text.h = music_h;
    music_button_tick.x = music_x+music_w+GAP; music_button_tick.y = music_y+GAP;
    music_button_tick.w = SIZE; music_button_tick.h = SIZE;
    music_button.text = load_message(rend, "Music", font, color);
}

void change_button_state(int mouse_x, int mouse_y, int is_paused){//shouldn't change border settings while running
    if (mouse_x >= border_button_tick.x && mouse_x <= border_button_tick.x+border_button_tick.w && !is_running){
        if (mouse_y >= border_button_tick.y && mouse_y <= border_button_tick.y+border_button_tick.h)
            border_button_pressed = !border_button_pressed;
    }

    if (mouse_x >= music_button_tick.x && mouse_x <= music_button_tick.x+music_button_tick.w){
        if (mouse_y >= music_button_tick.y && mouse_y <= music_button_tick.y+music_button_tick.h){
            if(!is_paused){
                (music_button_pressed)? Mix_PauseMusic():Mix_ResumeMusic();
            }
            music_button_pressed = !music_button_pressed;
        }
    }

    if (mouse_x >= sound_button_tick.x && mouse_x <= sound_button_tick.x+sound_button_tick.w){
        if (mouse_y >= sound_button_tick.y && mouse_y <= sound_button_tick.y+sound_button_tick.h)
            sound_button_pressed = !sound_button_pressed;
    }
}

void set_previous_direction(){
    if (up) previous_direction = 'u';
    else if (down) previous_direction = 'd';
    else if (left) previous_direction = 'l';
    else previous_direction = 'r';
}

SDL_Texture * set_tail_texture(int s){//score
    if(score == 0){
            if(up) return up_tail_textures[tail_state];
            else if (down) return down_tail_textures[tail_state];
            else if(left) return left_tail_textures[tail_state];
            else return right_tail_textures[tail_state];
    }
    else{
        if (current->prev->tex == body_texture_dl){
            if (current->cell->x == current->prev->cell->x) return left_tail_textures[tail_state];
            else return down_tail_textures[tail_state];
        }
        else if (current->prev->tex == body_texture_dr){
            if (current->cell->x == current->prev->cell->x) return right_tail_textures[tail_state];
            else return down_tail_textures[tail_state];
        }
        else if (current->prev->tex == body_texture_ul){
            if (current->cell->x == current->prev->cell->x) return left_tail_textures[tail_state];
            else return up_tail_textures[tail_state];
        }
        else if (current->prev->tex == body_texture_ur){
            if (current->cell->x == current->prev->cell->x) return right_tail_textures[tail_state];
            else return up_tail_textures[tail_state];
        }
        else if (current->prev->tex == body_texture_sv){
            if (current->tex == tail_texture_ul||current->tex == tail_texture_ur) return up_tail_textures[tail_state];
            else return down_tail_textures[tail_state];
        }
        else if (current->prev->tex == body_texture_sh){
            if (current->tex == tail_texture_ll||current->tex == tail_texture_lr) return left_tail_textures[tail_state];
            else return right_tail_textures[tail_state];
        }
        //if adding a new cell, then the previous is the previous tail cell
        else if (current->prev->tex == tail_texture_dl) return tail_texture_dr;
        else if (current->prev->tex == tail_texture_dr) return tail_texture_dl;
        else if (current->prev->tex == tail_texture_ul) return tail_texture_ur;
        else if (current->prev->tex == tail_texture_ur) return tail_texture_ul;
        else if (current->prev->tex == tail_texture_ll) return tail_texture_lr;
        else if (current->prev->tex == tail_texture_lr) return tail_texture_ll;
        else if (current->prev->tex == tail_texture_rl) return tail_texture_rr;
        else return tail_texture_rl;
    }
}

int restart_screen(SDL_Renderer * rend, TTF_Font *font, SDL_Color color){

    Mix_PauseMusic();//can't resume after halt or something, so pause instead; doesn't matter
    // score = 0; up = 0; left = 0; right = 0; down = 0;

    play_again.w = AGAIN; play_again.h = 2*SIZE;
    play_again.x = WINDOW_WIDTH/2-AGAIN/2; play_again.y = WINDOW_HEIGHT/2 - 2*SIZE;
    yes.w = (int) AGAIN/3, yes.h = 2*SIZE; no.w = (int) AGAIN/3; no.h = 2*SIZE;
    yes.x = play_again.x; no.x = yes.x+2*yes.w; yes.y = play_again.y+play_again.h; no.y = play_again.y+play_again.h;

    play_again_tex = load_message(rend, "Play Again?", font, color);
    yes_tex = load_message(rend, "Yes", font, color);
    no_tex = load_message(rend, "No", font, color);

    SDL_RenderClear(rend);
    SDL_RenderCopy(rend, play_again_tex, NULL, &play_again);
    SDL_RenderCopy(rend, yes_tex, NULL, &yes);
    SDL_RenderCopy(rend, no_tex, NULL, &no);
    SDL_RenderPresent(rend);

    while (!close_requested){
        SDL_Event event;//is a union of things
        while (SDL_PollEvent(&event)){
            switch(event.type){
                case SDL_QUIT:
                    close_requested = 1;
                    break;
                case SDL_KEYDOWN:
                    if (event.key.keysym.scancode == SDL_SCANCODE_Y) return 1;
                    else if (event.key.keysym.scancode == SDL_SCANCODE_N) return 0;
                    break;//very important to break out of this case
                case SDL_MOUSEBUTTONDOWN:
                    if (event.button.button == SDL_BUTTON_LEFT && event.button.state == SDL_PRESSED){
                        int mouse_x = event.button.x; int mouse_y = event.button.y;
                        if (mouse_y <= yes.y+yes.h && mouse_y >= yes.y){
                            if (mouse_x <= yes.x+yes.w && mouse_x >= yes.x) return 1;
                            else if (mouse_x <=no.x+no.w && mouse_x >= no.x) return 0;
                        }
                    }
                    break;
            }
        }
    }
    return 0;
}

int load_data(SDL_Renderer * rend){
    //set score = 0
    score = 0;
    //load image into memory
    load_textures(rend);
    up_tail_textures[0] = tail_texture_ul; up_tail_textures[1] = tail_texture_ur;
    down_tail_textures[0] = tail_texture_dl; down_tail_textures[1] = tail_texture_dr;
    left_tail_textures[0] = tail_texture_ll; left_tail_textures[1] = tail_texture_lr;
    right_tail_textures[0] = tail_texture_rl; right_tail_textures[1] = tail_texture_rr;

    //scale down texture to have wasd movement
    head.w = SIZE; head.h = SIZE; tail.w = SIZE; tail.h = SIZE; food.w = SIZE; food.h = SIZE;
    right_edge.h = WINDOW_HEIGHT; right_edge.w = 1; right_edge.x = GAME_WIDTH; right_edge.y = 0;
    score_rect.h = score_h, score_rect.w = score_w, score_rect.x = score_x, score_rect.y = score_y;

    first_cell.cell = &head;
    first_cell.prev = NULL;
    first_cell.tex = head_texture_r;

    last_cell.cell = &tail;
    last_cell.prev = &first_cell;
    last_cell.tex = tail_texture_rl;

    food_cell.cell = &food;
    food_cell.prev = NULL;
    food_cell.tex = food_texture;

    current = &last_cell;
    new_last_cell = &last_cell;

    bite = Mix_LoadWAV(path_bite);
    bgm = Mix_LoadMUS(path_bgm);
    if (music_button_pressed) Mix_PlayMusic(bgm, -1);

    return 1;
}

void load_start_screen(SDL_Renderer * rend, TTF_Font * font, SDL_Color color){
    SDL_RenderClear(rend);
    SDL_RenderCopy(rend, first_cell.tex, NULL, first_cell.cell);
    SDL_RenderCopy(rend, last_cell.tex, NULL, last_cell.cell);
    SDL_RenderCopy(rend, food_cell.tex, NULL, food_cell.cell);
    SDL_RenderCopy(rend, border_button.text, NULL, border_button.button_text);
    if (border_button_pressed)
        SDL_RenderCopy(rend, border_button.clicked, NULL, border_button.button_tick);
    else
        SDL_RenderCopy(rend, border_button.unclicked, NULL, border_button.button_tick);

    SDL_RenderCopy(rend, music_button.text, NULL, music_button.button_text);
    if (music_button_pressed)
        SDL_RenderCopy(rend, music_button.clicked, NULL, music_button.button_tick);
    else
        SDL_RenderCopy(rend, music_button.unclicked, NULL, music_button.button_tick);

    SDL_RenderCopy(rend, sound_button.text, NULL, sound_button.button_text);
    if (sound_button_pressed)
        SDL_RenderCopy(rend, sound_button.clicked, NULL, sound_button.button_tick);
    else
        SDL_RenderCopy(rend, sound_button.unclicked, NULL, sound_button.button_tick);

    draw_edge(rend);
    update_score(rend, font, color);
}

int start_screen(SDL_Renderer *rend, TTF_Font * font, SDL_Color color){
    if (!load_data(rend)) return 0;
    create_buttons(rend, font, color);
    int x_pos = GAME_WIDTH/2;
    int y_pos = WINDOW_HEIGHT/2;
    first_cell.cell->x = x_pos - x_pos%SIZE;
    first_cell.cell->y = y_pos - y_pos%SIZE;
    last_cell.cell->x = x_pos - x_pos%SIZE - SIZE;
    last_cell.cell->y = y_pos - y_pos%SIZE;
    spawn_food();
    load_start_screen(rend, font, color);

    SDL_RenderPresent(rend);

    //animation before player starts
    while (!close_requested && !is_running){
        SDL_Event event;//is a union of things
        while (SDL_PollEvent(&event)){
            switch(event.type){
                case SDL_QUIT:
                    close_requested = 1;
                    break;
                case SDL_KEYDOWN:
                    switch (event.key.keysym.scancode){//event.key.keysymbol.associated scan code
                        case SDL_SCANCODE_W:
                        case SDL_SCANCODE_UP:
                            is_running = 1;
                            left = 0; right = 0;
                            if (!down) up = 1;
                            break;
                        case SDL_SCANCODE_A:
                        case SDL_SCANCODE_LEFT://no is_running for left because that's where the body is
                            up = 0; down = 0;
                            if (!right) left = 1;
                            break;
                        case SDL_SCANCODE_S:
                        case SDL_SCANCODE_DOWN:
                            is_running = 1;
                            left = 0; right = 0;
                            if (!up) down = 1;
                            break;
                        case SDL_SCANCODE_D:
                        case SDL_SCANCODE_RIGHT:
                            is_running = 1;
                            up = 0; down = 0;
                            if (!left) right = 1;
                            break;
                        case SDL_SCANCODE_B:
                            border_button_pressed = !border_button_pressed;
                            break;
                        case SDL_SCANCODE_C:
                            sound_button_pressed = !sound_button_pressed;
                            break;
                        case SDL_SCANCODE_M:
                            (music_button_pressed) ? Mix_PauseMusic():Mix_ResumeMusic();
                            music_button_pressed = !music_button_pressed;
                            break;
                        }
                    break;//very important to break out of this case
                case SDL_MOUSEBUTTONDOWN:
                    if (event.button.button == SDL_BUTTON_LEFT && event.button.state == SDL_PRESSED){
                        change_button_state(event.button.x, event.button.y, 0);
                    }
                    break;
            }
        }

        load_start_screen(rend, font, color);
        SDL_RenderPresent(rend);
        SDL_Delay(1000/FPS);

    }
    //if (close_requested) return 0;
    set_previous_direction();
    return 1;
}

int pause_game(SDL_Renderer * rend, TTF_Font * font, SDL_Color color){
    Mix_PauseMusic();
    while(!close_requested){
        SDL_Event event;//is a union of things
        while (SDL_PollEvent(&event)){
            switch(event.type){
                case SDL_QUIT:
                    close_requested = 1;
                    break;
                case SDL_KEYDOWN:
                    switch (event.key.keysym.scancode){//event.key.keysymbol.associated scan code
                        case SDL_SCANCODE_C:
                            sound_button_pressed = !sound_button_pressed;
                            break;
                        case SDL_SCANCODE_M:
                            music_button_pressed = !music_button_pressed;
                            break;
                        case SDL_SCANCODE_SPACE:
                            return 1;
                        }
                    break;//very important to break out of this case
                case SDL_MOUSEBUTTONDOWN:
                    if (event.button.button == SDL_BUTTON_LEFT && event.button.state == SDL_PRESSED){
                        change_button_state(event.button.x, event.button.y, 1);
                    }
                    break;
            }
        }
        SDL_RenderClear(rend);
        //draw right edge
        draw_edge(rend);
        SDL_RenderCopy(rend, food_cell.tex, NULL, food_cell.cell);

        while(current->prev != NULL){
            SDL_RenderCopy(rend, current->tex, NULL, current->cell);
            current = current->prev;
        }
        SDL_RenderCopy(rend, current->tex, NULL, current->cell);
        current = new_last_cell;

        {SDL_RenderCopy(rend, border_button.text, NULL, border_button.button_text);
        if (border_button_pressed)
            SDL_RenderCopy(rend, border_button.clicked, NULL, border_button.button_tick);
        else
            SDL_RenderCopy(rend, border_button.unclicked, NULL, border_button.button_tick);

        SDL_RenderCopy(rend, music_button.text, NULL, music_button.button_text);
        if (music_button_pressed)
            SDL_RenderCopy(rend, music_button.clicked, NULL, music_button.button_tick);
        else
            SDL_RenderCopy(rend, music_button.unclicked, NULL, music_button.button_tick);

        SDL_RenderCopy(rend, sound_button.text, NULL, sound_button.button_text);
        if (sound_button_pressed)
            SDL_RenderCopy(rend, sound_button.clicked, NULL, sound_button.button_tick);
        else
            SDL_RenderCopy(rend, sound_button.unclicked, NULL, sound_button.button_tick);}

        update_score(rend, font, color);
        SDL_RenderPresent(rend);
        SDL_Delay(1000/FPS);
    }
    return 0;
}

int game_play(SDL_Renderer *rend, TTF_Font * font, SDL_Color color){
    while (!close_requested){
        //process events
        state_changed = 0;
        SDL_Event event;//is a union of things
        set_previous_direction();
        while (SDL_PollEvent(&event) && !state_changed){
            switch(event.type){
                case SDL_QUIT:
                    close_requested = 1;
                    break;
                case SDL_KEYDOWN:
                    switch (event.key.keysym.scancode){//event.key.keysymbol.associated scan code
                        case SDL_SCANCODE_W:
                        case SDL_SCANCODE_UP:
                            left = 0; right = 0;
                            if (!down){up = 1; state_changed = 1;}
                            break;
                        case SDL_SCANCODE_A:
                        case SDL_SCANCODE_LEFT:
                            up = 0; down = 0;
                            if (!right){left = 1; state_changed = 1;}
                            break;
                        case SDL_SCANCODE_S:
                        case SDL_SCANCODE_DOWN:
                            left = 0; right = 0;
                            if (!up){down = 1; state_changed = 1;}
                            break;
                        case SDL_SCANCODE_D:
                        case SDL_SCANCODE_RIGHT:
                            up = 0; down = 0;
                            if (!left){right = 1; state_changed = 1;}
                            break;
                        case SDL_SCANCODE_C:
                            sound_button_pressed = !sound_button_pressed;
                            break;
                        case SDL_SCANCODE_M:
                            (music_button_pressed) ? Mix_PauseMusic():Mix_ResumeMusic();
                            music_button_pressed = !music_button_pressed;
                            break;
                        case SDL_SCANCODE_SPACE:
                            if(!pause_game(rend, font, color))return 0;
                            if(music_button_pressed) Mix_ResumeMusic();
                            break;
                        }
                    break;//very important to break out of this case
                case SDL_MOUSEBUTTONDOWN:
                    if (event.button.button == SDL_BUTTON_LEFT && event.button.state == SDL_PRESSED){
                        change_button_state(event.button.x, event.button.y, 0);
                    }
                    break;
            }
        }
        //draw stuff
        SDL_RenderClear(rend);
        //draw right edge
        draw_edge(rend);
        SDL_RenderCopy(rend, food_cell.tex, NULL, food_cell.cell);

        //current is the last cell
        current->tex = set_tail_texture(score);


        tail_state = !tail_state;

        while(current->prev != NULL){
            //logic for curved body;

            if(current->prev->prev == &first_cell){
                if (score != 1) {current->tex = current->prev->tex;}

                if (previous_direction == 'u'){
                    if(up) current->prev->tex = body_texture_sv;
                    if(left) current->prev->tex = body_texture_dl;
                    if(right) current->prev->tex = body_texture_dr;
                }
                else if (previous_direction == 'l'){
                    if(up) current->prev->tex = body_texture_ur;
                    if(left) current->prev->tex = body_texture_sh;
                    if(down) current->prev->tex = body_texture_dr;
                }
                else if (previous_direction == 'r'){
                    if(up) current->prev->tex = body_texture_ul;
                    if(down) current->prev->tex = body_texture_dl;
                    if(right) current->prev->tex = body_texture_sh;
                }
                else if (previous_direction == 'd'){
                    if(down) current->prev->tex = body_texture_sv;
                    if(left) current->prev->tex = body_texture_ul;
                    if(right) current->prev->tex = body_texture_ur;
                }

                current->cell->x = current->prev->cell->x;
                current->cell->y = current->prev->cell->y;
                SDL_RenderCopy(rend, current->tex, NULL, current->cell);
                current = current->prev;
                current->cell->x = current->prev->cell->x;
                current->cell->y = current->prev->cell->y;
                SDL_RenderCopy(rend, current->tex, NULL, current->cell);
                current = current->prev;
            }
            else{
                current->cell->x = current->prev->cell->x;
                current->cell->y = current->prev->cell->y;
                if(!(current == new_last_cell)) current->tex = current->prev->tex;
                SDL_RenderCopy(rend, current->tex, NULL, current->cell);
                current = current->prev;
            }

        }
        if (up) {current->cell->y -= SIZE; current->tex = head_texture_u;}
        if (down) {current->cell->y += SIZE; current->tex = head_texture_d;}
        if (left) {current->cell->x -= SIZE; current->tex = head_texture_l;}
        if (right) {current->cell->x += SIZE; current->tex = head_texture_r;}


        //collision before drawing head; else will have a problem in case of passing through right edge
        //only reason to pass new_last_cell is to test for self_collision, which requires the tail cell
        //after eating, the new last cell changes, but in this case, because the head is near a food cell, self collision is impossible!
        if (collision(border_button_pressed)) break;

        SDL_RenderCopy(rend, current->tex, NULL, current->cell);
        current = new_last_cell;

        if (has_eaten()){
            if(sound_button_pressed){
                Mix_PlayChannel(-1, bite, 0);
            }
            score += 1;
            spawn_food();
            SDL_RenderCopy(rend, food_cell.tex, NULL, food_cell.cell);
            new_last_cell = add_cell();
            current = new_last_cell;
        }

        {SDL_RenderCopy(rend, border_button.text, NULL, border_button.button_text);
        if (border_button_pressed)
            SDL_RenderCopy(rend, border_button.clicked, NULL, border_button.button_tick);
        else
            SDL_RenderCopy(rend, border_button.unclicked, NULL, border_button.button_tick);

        SDL_RenderCopy(rend, music_button.text, NULL, music_button.button_text);
        if (music_button_pressed)
            SDL_RenderCopy(rend, music_button.clicked, NULL, music_button.button_tick);
        else
            SDL_RenderCopy(rend, music_button.unclicked, NULL, music_button.button_tick);

        SDL_RenderCopy(rend, sound_button.text, NULL, sound_button.button_text);
        if (sound_button_pressed)
            SDL_RenderCopy(rend, sound_button.clicked, NULL, sound_button.button_tick);
        else
            SDL_RenderCopy(rend, sound_button.unclicked, NULL, sound_button.button_tick);}

        update_score(rend, font, color);
        SDL_RenderPresent(rend);
        SDL_Delay(1000/FPS);
    }
    if (close_requested) return 0;
    return 1;
}

int main(void){
    //seed rng, needs to happen inside a function(?)
    time_t t;//for seeding srand
    srand((unsigned) time(&t));//initialize rng; some magic happens

    if (SDL_Init(SDL_INIT_VIDEO|SDL_INIT_TIMER) != 0){
         printf("error %s\n", SDL_GetError());
         close_requested = 1;
     }
     //pointer to the window
    SDL_Window * win = SDL_CreateWindow("Snake",
                                        SDL_WINDOWPOS_CENTERED,
                                        SDL_WINDOWPOS_CENTERED,
                                        WINDOW_WIDTH, WINDOW_HEIGHT, 0);
    if (!win){
        printf("Something went wrong");
        close_requested = 1;
        SDL_Quit();
    }
    //create renderer
    Uint32 render_flags = SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC;
    SDL_Renderer* rend = SDL_CreateRenderer(win, -1, render_flags);//which window, which graphics driver (-1 if that doesn't matter)
    if (!rend){
        printf("Error creating renderer %s\n", SDL_GetError());
        close_requested = 1;
        SDL_DestroyWindow(win);
        SDL_Quit();
    }

    //fonts
    if (TTF_Init() < 0){
        printf("error : %s\n", SDL_GetError());
        close_requested = 1;
        int exit_status = system("gnome-terminal");
    };

    Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, 2, 4096);

    TTF_Font * font = TTF_OpenFont(path_font, FONTSIZE);
    if (!font){
        printf("Couldn't load font:%s\n", SDL_GetError());
    }
    SDL_Color font_colour = {150, 255, 150};//font color

    while(!close_requested){
        if (!start_screen(rend, font, font_colour)) break;
        if (game_play(rend, font, font_colour)){
            if (!restart_screen(rend, font, font_colour)) break;
            else{
                score = 0; up = 0; left = 0; right = 0; down = 0; is_running = 0;
                if(music_button_pressed) Mix_PlayMusic(bgm, -1);
            }
        }
    }


    TTF_CloseFont(font);
    SDL_DestroyTexture(food_texture);
    SDL_DestroyTexture(head_texture_u);SDL_DestroyTexture(head_texture_l);
    SDL_DestroyTexture(head_texture_d);SDL_DestroyTexture(head_texture_r);
    SDL_DestroyTexture(body_texture_sh); SDL_DestroyTexture(body_texture_sv); SDL_DestroyTexture(body_texture_ur);
    SDL_DestroyTexture(body_texture_ul); SDL_DestroyTexture(body_texture_dr); SDL_DestroyTexture(body_texture_dl);
    SDL_DestroyTexture(tail_texture_dl); SDL_DestroyTexture(tail_texture_dr); SDL_DestroyTexture(tail_texture_ll);
    SDL_DestroyTexture(tail_texture_lr); SDL_DestroyTexture(tail_texture_ul); SDL_DestroyTexture(tail_texture_ur);
    SDL_DestroyTexture(tail_texture_rl); SDL_DestroyTexture(tail_texture_rr);
    SDL_DestroyTexture(tick_texture);
    SDL_DestroyTexture(no_tick_texture);
    SDL_DestroyTexture(border_button.text);
    SDL_DestroyTexture(sound_button.text);
    SDL_DestroyTexture(music_button.text);
    SDL_DestroyRenderer(rend);
    SDL_DestroyWindow(win);

    // Mix_FreeChunk(sound);
    // sound = NULL;//to be safe??
    Mix_FreeChunk(bite);
    Mix_FreeMusic(bgm);

    Mix_CloseAudio();
    SDL_Quit();
    return 0;
}
