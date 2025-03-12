#include <SDL2/SDL.h>
#include <stdbool.h>

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 400
#define GROUND_HEIGHT 300
#define DINO_WIDTH 50
#define DINO_HEIGHT 50
#define OBSTACLE_WIDTH 40
#define OBSTACLE_HEIGHT 50
#define GRAVITY 1
#define JUMP_STRENGTH -15
#define OBSTACLE_SPEED 5

typedef struct {
    int x, y, width, height, velocityY;
    bool isJumping;
} Dino;

typedef struct {
    int x, y, width, height;
} Obstacle;

void handleEvents(bool *running, Dino *dino) {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            *running = false;
        } else if (event.type == SDL_KEYDOWN) {
            if (event.key.keysym.sym == SDLK_SPACE && !dino->isJumping) {
                dino->velocityY = JUMP_STRENGTH;
                dino->isJumping = true;
            }
        }
    }
}

void update(Dino *dino, Obstacle *obstacle) {
    // Apply gravity
    dino->y += dino->velocityY;
    dino->velocityY += GRAVITY;

    // Keep dino on the ground
    if (dino->y >= GROUND_HEIGHT - DINO_HEIGHT) {
        dino->y = GROUND_HEIGHT - DINO_HEIGHT;
        dino->isJumping = false;
    }

    // Move obstacle
    obstacle->x -= OBSTACLE_SPEED;
    if (obstacle->x < -OBSTACLE_WIDTH) {
        obstacle->x = WINDOW_WIDTH;
    }

    // Check for collision
    if (dino->x < obstacle->x + OBSTACLE_WIDTH &&
        dino->x + DINO_WIDTH > obstacle->x &&
        dino->y < obstacle->y + OBSTACLE_HEIGHT &&
        dino->y + DINO_HEIGHT > obstacle->y) {
        printf("Game Over!\n");
        SDL_Delay(1000);
        exit(0);
    }
}

void render(SDL_Renderer *renderer, Dino *dino, Obstacle *obstacle) {
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderClear(renderer);

    // Draw dino
    SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
    SDL_Rect dinoRect = {dino->x, dino->y, DINO_WIDTH, DINO_HEIGHT};
    SDL_RenderFillRect(renderer, &dinoRect);

    // Draw obstacle
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    SDL_Rect obstacleRect = {obstacle->x, obstacle->y, OBSTACLE_WIDTH, OBSTACLE_HEIGHT};
    SDL_RenderFillRect(renderer, &obstacleRect);

    SDL_RenderPresent(renderer);
}

int main() {
    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window *window = SDL_CreateWindow("Dino Game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    Dino dino = {50, GROUND_HEIGHT - DINO_HEIGHT, DINO_WIDTH, DINO_HEIGHT, 0, false};
    Obstacle obstacle = {WINDOW_WIDTH, GROUND_HEIGHT - OBSTACLE_HEIGHT, OBSTACLE_WIDTH, OBSTACLE_HEIGHT};

    bool running = true;
    while (running) {
        handleEvents(&running, &dino);
        update(&dino, &obstacle);
        render(renderer, &dino, &obstacle);
        SDL_Delay(16);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
