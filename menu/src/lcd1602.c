#include "lcd1602.h"
#include "menu.h"

typedef struct lcd1602_handle {
    char display[2][17];
    int cursor_x;
    int cursor_y;
    bool backlight;
    bool blink;
    int contrast;
    
    get_str_title_fn_t get_title_fn;
    get_str_value_fn_t get_value_fn;
    
    rotary_encoder_callback_t position_cb;
    push_button_callback_t push_button_cb; 
    long_push_buttont_callback_t long_push_button_cb;

    int pos;

} lcd1602_handle_t;

static void s_lcd_init(
    lcd1602_handle_t *lcd,
    get_str_title_fn_t title_fn,
    get_str_value_fn_t value_fn,
    rotary_encoder_callback_t position_cb, 
    push_button_callback_t push_button_cb, 
    long_push_buttont_callback_t long_push_button_cb);
static void s_handle_key_press(SDL_Keycode key, bool long_press, lcd1602_handle_t *lcd);
static void s_lcd_render(SDL_Renderer *renderer, TTF_Font *font, lcd1602_handle_t *lcd);

static void s_update_menu(lcd1602_handle_t *lcd);

bool lcd1602_init(
    get_str_title_fn_t title_fn,
    get_str_value_fn_t value_fn,
    rotary_encoder_callback_t position_cb, 
    push_button_callback_t push_button_cb, 
    long_push_buttont_callback_t long_push_button_cb)
{
    // const char defaultLcdFont[] = "/home/yevst/Projects/CCPP/lcd1602/resources/lcd_font.ttf";
    const char defaultLcdFont[] = "/usr/share/fonts/truetype/freefont/FreeMono.ttf";

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("SDL initialization failed: %s\n", SDL_GetError());
        return false;
    }
    
    if (TTF_Init() < 0) {
        printf("TTF initialization failed: %s\n", TTF_GetError());
        SDL_Quit();
        return false;
    }
    
    SDL_Window *window = SDL_CreateWindow("LCD1602 Simulator with Menu", 
                                         SDL_WINDOWPOS_UNDEFINED, 
                                         SDL_WINDOWPOS_UNDEFINED, 
                                         360, 150, 
                                         SDL_WINDOW_SHOWN);
    
    if (!window) {
        printf("Window creation failed: %s\n", SDL_GetError());
        TTF_Quit();
        SDL_Quit();
        return false;
    }
    
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    TTF_Font *font = TTF_OpenFont(defaultLcdFont, 22);
    
    if (!font) {
        // Попробуем найти стандартный шрифт
        font = TTF_OpenFont("/usr/share/fonts/truetype/dejavu/DejaVuSansMono.ttf", 20);
        if (!font) {
            font = TTF_OpenFont("/usr/share/fonts/truetype/freefont/FreeMono.ttf", 20);
            if (!font) {
                printf("Using default SDL font\n");
            }
        }
    }
    
    lcd1602_handle_t lcd;
    s_lcd_init(&lcd, title_fn, value_fn, position_cb, push_button_cb, long_push_button_cb);
    s_update_menu(&lcd);
    
    bool running = true;
    SDL_Event event;
    Uint32 last_blink = SDL_GetTicks();
    Uint32 key_press_time = 0;
    SDL_Keycode last_key = SDLK_UNKNOWN;
    bool long_press_detected = false;
    
    while (running) {
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_QUIT:
                    running = false;
                    break;
                    
                case SDL_KEYDOWN:
                    if (event.key.keysym.sym != SDLK_UNKNOWN) {
                        last_key = event.key.keysym.sym;
                        key_press_time = SDL_GetTicks();
                        long_press_detected = false;
                    }
                    break;
                    
                case SDL_KEYUP:
                    if (!long_press_detected) {
                        s_handle_key_press(last_key, false, &lcd);
                    }
                    last_key = SDLK_UNKNOWN;
                    break;
            }
        }
        
        // Проверка долгого нажатия
        if (last_key != SDLK_UNKNOWN && SDL_GetTicks() - key_press_time > 800) {
            s_handle_key_press(last_key, true, &lcd);
            long_press_detected = true;
            last_key = SDLK_UNKNOWN;
        }
        
        // Мигание курсора
        if (SDL_GetTicks() - last_blink > 500) {
            lcd.blink = !lcd.blink;
            last_blink = SDL_GetTicks();
        }
        
        s_lcd_render(renderer, font, &lcd);
        SDL_Delay(16);
    }
    
    if (font) TTF_CloseFont(font);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_Quit();
    SDL_Quit();
}

static void s_lcd_init(lcd1602_handle_t *lcd, 
    get_str_title_fn_t title_fn,
    get_str_value_fn_t value_fn,
    rotary_encoder_callback_t position_cb, 
    push_button_callback_t push_button_cb, 
    long_push_buttont_callback_t long_push_button_cb) {
    for (int y = 0; y < 2; y++) {
        for (int x = 0; x < 16; x++) {
            lcd->display[y][x] = ' ';
        }
        lcd->display[y][16] = '\0';
    }
    lcd->cursor_x = 0;
    lcd->cursor_y = 0;
    lcd->backlight = true;
    lcd->blink = false;
    lcd->contrast = 50;
    lcd->pos = 0;
    lcd->get_title_fn = title_fn;
    lcd->get_value_fn = value_fn;
    lcd->position_cb = position_cb;
    lcd->push_button_cb = push_button_cb;
    lcd->long_push_button_cb = long_push_button_cb;
}

void lcd_clear(lcd1602_handle_t *lcd) {
    for (int y = 0; y < 2; y++) {
        for (int x = 0; x < 16; x++) {
            lcd->display[y][x] = ' ';
        }
        lcd->display[y][16] = '\0';
    }
    lcd->cursor_x = 0;
    lcd->cursor_y = 0;
}

void lcd_set_cursor(lcd1602_handle_t *lcd, int x, int y) {
    if (x >= 0 && x < 16 && y >= 0 && y < 2) {
        lcd->cursor_x = x;
        lcd->cursor_y = y;
    }
}

void lcd_print_char(lcd1602_handle_t *lcd, char ch) {
    if (lcd->cursor_x < 16) {
        lcd->display[lcd->cursor_y][lcd->cursor_x] = ch;
        lcd->cursor_x++;
    }
}

void lcd_print_str(lcd1602_handle_t *lcd, const char *str) {
    for (int i = 0; str[i] != '\0' && lcd->cursor_x < 16; i++) {
        lcd_print_char(lcd, str[i]);
    }
}

// Функции для работы с меню
static void s_update_menu(lcd1602_handle_t *lcd) {
    lcd_clear(lcd);
    lcd_set_cursor(lcd, 0, 0);
    lcd_print_str(lcd, lcd->get_title_fn());
    lcd_set_cursor(lcd, 0, 1);
    lcd_print_str(lcd, lcd->get_value_fn());
}

static void s_handle_key_press(SDL_Keycode key, bool long_press, lcd1602_handle_t *lcd) {
    switch (key) {
        case SDLK_UP:
            lcd->pos += 2;
            lcd->position_cb(lcd->pos);
            break;
            
        case SDLK_DOWN:
            lcd->pos -= 2;
            lcd->position_cb(lcd->pos);
            break;
            
        case SDLK_RETURN:
            lcd->push_button_cb();
            break;
            
        case SDLK_d:
            lcd->long_push_button_cb();
            break;
            
        case SDLK_ESCAPE:
            break;
    }
    
    s_update_menu(lcd);
}

static void s_lcd_render(SDL_Renderer *renderer, TTF_Font *font, lcd1602_handle_t *lcd) {
    // Фон
    SDL_SetRenderDrawColor(renderer, 40, 40, 40, 255);
    SDL_RenderClear(renderer);
    
    // Дисплей LCD
    SDL_Rect lcd_rect = {20, 20, 320, 80};
    SDL_SetRenderDrawColor(renderer, 0, 80, 0, 255);
    SDL_RenderFillRect(renderer, &lcd_rect);
    SDL_SetRenderDrawColor(renderer, 0, 60, 0, 255);
    SDL_RenderDrawRect(renderer, &lcd_rect);
    
    // Текст на дисплее
    SDL_Color text_color = {200, 200, 200, 255};
    for (int y = 0; y < 2; y++) {
        SDL_Surface *surface = TTF_RenderText_Solid(font, lcd->display[y], text_color);
        if (surface) {
            SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
            SDL_Rect text_rect = {30, 30 + y * 40, surface->w, surface->h};
            SDL_RenderCopy(renderer, texture, NULL, &text_rect);
            SDL_FreeSurface(surface);
            SDL_DestroyTexture(texture);
        }
    }
    
    // Курсор
    if (lcd->blink) {
        SDL_Rect cursor_rect = {30 + lcd->cursor_x * 19, 30 + lcd->cursor_y * 40, 2, 20};
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderFillRect(renderer, &cursor_rect);
    }
    
    // Подсказки управления
    SDL_Color hint_color = {150, 150, 150, 255};
    const char *hints = "UP/DOWN: Navigate  ENTER: Select  D: Long press";
    SDL_Surface *hint_surface = TTF_RenderText_Solid(font, hints, hint_color);
    if (hint_surface) {
        SDL_Texture *hint_texture = SDL_CreateTextureFromSurface(renderer, hint_surface);
        SDL_Rect hint_rect = {20, 110, hint_surface->w, hint_surface->h};
        SDL_RenderCopy(renderer, hint_texture, NULL, &hint_rect);
        SDL_FreeSurface(hint_surface);
        SDL_DestroyTexture(hint_texture);
    }
    
    SDL_RenderPresent(renderer);
}
