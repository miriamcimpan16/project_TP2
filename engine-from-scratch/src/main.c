#include "engine/array_list.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <glad/glad.h>
#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>

#include <SDL2/SDL_mixer.h>
#include "engine/global.h"
#include "engine/config.h"
#include "engine/input.h"
#include "engine/time.h"
#include "engine/physics.h"
#include "engine/util.h"
#include "engine/entity.h"
#include "engine/render/render.h"
#include "engine/animation.h"
#include "engine/audio.h"
#include<ft2build.h>
#include FT_FREETYPE_H


void reset(void);
static int score = 0;
static const f32 SPEED_GROUND = 200.0f;  
static f32 ground_offset = 0.0f;         // Offset pe axa X
static bool is_game_over = false;
static const f32 GROUNDED_TIME = 0.1f;
static const f32 SPEED_PLAYER = 250;
static const f32 JUMP_VELOCITY = 1200;
static const f32 SPEED_ENEMY_LARGE = 80;
static const f32 SPEED_ENEMY_SMALL = 100;
static const f32 HEALTH_ENEMY_LARGE = 7;
static const f32 HEALTH_ENEMY_SMALL = 3;
static Mix_Chunk *SOUND_PLAYER_DEATH;
static Mix_Chunk *SOUND_JUMP;
static Mix_Music *MUSIC_STAGE_1;
static f32 total_time = 0.0f;
static bool is_night = false;


typedef enum collision_layer {
	COLLISION_LAYER_PLAYER = 1,
	COLLISION_LAYER_ENEMY = 1 << 1,
	COLLISION_LAYER_TERRAIN = 1 << 2,
	COLLISION_LAYER_ENEMY_PASSTHROUGH = 1 << 3,
    
} Collision_Layer;








static f32 render_width;
static f32 render_height;
static u32 texture_slots[8] = {0};


static bool should_quit = false;
static bool player_is_grounded = false;
static usize anim_player_walk_id;
static usize anim_player_idle_id;
static usize anim_enemy_small_id;
static usize anim_enemy_large_id;
static usize anim_enemy_small_enraged_id;
static usize anim_enemy_large_enraged_id;


static usize player_id;

static f32 ground_timer = 0;

static f32 spawn_timer = 0;

static u8 enemy_mask = COLLISION_LAYER_PLAYER | COLLISION_LAYER_TERRAIN;
static u8 player_mask = COLLISION_LAYER_ENEMY | COLLISION_LAYER_TERRAIN | COLLISION_LAYER_ENEMY_PASSTHROUGH;









static void input_handle(Body *body_player) {
	if (global.input.escape) {
		should_quit = true;
	}

    Animation *walk_anim = animation_get(anim_player_walk_id);
    Animation *idle_anim = animation_get(anim_player_idle_id);

	f32 velx = 0;
	f32 vely = body_player->velocity[1];


	if (global.input.up && player_is_grounded) {
		player_is_grounded = false;
		vely = JUMP_VELOCITY;
		audio_sound_play(SOUND_JUMP);
		
	}

	body_player->velocity[0] = velx;
	body_player->velocity[1] = vely;

    
}

void player_on_hit(Body *self, Body *other, Hit hit) {
	if (other->collision_layer == COLLISION_LAYER_ENEMY) {
		is_game_over = true;
	}
}
#define PLAYER_VISUAL_OFFSET_Y 4.0f 


		void player_on_hit_static(Body *self, Static_Body *other, Hit hit) {
			if (hit.normal[1] > 0) {
				player_is_grounded = true;
		
				f32 platform_top = other->aabb.position[1] + other->aabb.half_size[1];
				f32 player_height = self->aabb.half_size[1] * 2.0f;
		
				// Setăm centrul jucătorului să fie chiar deasupra platformei
				self->aabb.position[1] = platform_top + self->aabb.half_size[1];
		
				self->velocity[1] = 0;
			}
		}



		
	






void spawn_cactus() {
    // Coordonatele pentru platformă
    f32 platform_y = 155.0f; // Y-ul centrului platformei (unde se află dinozaurul)
	
    
    f32 spawn_x = render_width;  
	
	
    vec2 size = {10, 10};  // Dimensiunile cactusului
   
   vec2 position = {render_width-90, platform_y - 30/ 2.0f};



    vec2 velocity = {-(SPEED_GROUND + (score/50)), 0};
	// Înainte să creezi entitatea:
	usize animation_id;
if (rand() % 2 == 0) {
	
    animation_id = anim_enemy_small_id; // cactus mic
} else {
	
    animation_id = anim_enemy_large_id; // cactus mare
}
 // Viteza cactusului (se mișcă spre stânga)
   

    // Creează cactusul ca entitate
    entity_create(
        position,
        size,
        (vec2){0, 10},  // Offsetul sprite-ului
        velocity,
        COLLISION_LAYER_ENEMY,  // Layer-ul pe care apare cactusul
        enemy_mask,
        false,
        animation_id,
        NULL,
        NULL
    );
	
}





void reset(void) {
	   audio_music_play(MUSIC_STAGE_1);
   
    input_update();
    physics_reset();
    entity_reset();
	score = 0;

    ground_timer = 0;
    spawn_timer = 0;
    f32 platform_top = 133 + 40 / 2.0f;  
f32 player_height = 24;

f32 player_y = platform_top + player_height / 2.0f;  

player_id = entity_create((vec2){100, player_y}, (vec2){24, 24}, (vec2){0, 0}, (vec2){0, 0}, COLLISION_LAYER_PLAYER, player_mask, false, (usize)-1, player_on_hit, player_on_hit_static);


	
    // Init level.
	{
		
		

       
		physics_static_body_create((vec2){200, 133}, (vec2){800, 40}, COLLISION_LAYER_TERRAIN);
        
        
        
	}

    
}



int main(int argc, char *argv[]) {
	
	time_init(60);
	SDL_Window *window = render_init();
	config_init();
	physics_init();
	entity_init();
	animation_init();
	audio_init();
	




	

	
    audio_sound_load(&SOUND_JUMP,"C:/Users/Deborah/OneDrive/Desktop/project/engine-from-scratch/src/engine/assets/jump.wav");
	audio_sound_load(&SOUND_PLAYER_DEATH, "C:/Users/Deborah/OneDrive/Desktop/project/engine-from-scratch/src/engine/assets/player_death.wav");
	audio_music_load(&MUSIC_STAGE_1, "C:/Users/Deborah/OneDrive/Desktop/project/engine-from-scratch/src/engine/assets/breezys_mega_quest_2_stage_1.mp3");
	
	

	i32 window_width, window_height;
	SDL_GetWindowSize(window, &window_width, &window_height);
	render_width = window_width / render_get_scale();
	render_height = window_height / render_get_scale();

	Sprite_Sheet sprite_sheet_player;
	Sprite_Sheet sprite_sheet_map;
	Sprite_Sheet sprite_sheet_enemy_small;
	Sprite_Sheet sprite_sheet_enemy_large;
    Sprite_Sheet gv_sprite_sheet;
    Sprite_Sheet start_sprite_sheet;
   
	render_sprite_sheet_init(&sprite_sheet_player, "C:/Users/Deborah/OneDrive/Desktop/project/engine-from-scratch/src/engine/assets/dino5.png", 33, 36);
    render_sprite_sheet_init(&sprite_sheet_map, "C:/Users/Deborah/OneDrive/Desktop/project/engine-from-scratch/src/engine/assets/ground4.png", window_width, window_height);
    render_sprite_sheet_init(&sprite_sheet_enemy_small, "C:/Users/Deborah/OneDrive/Desktop/project/engine-from-scratch/src/engine/assets/cactus4.png", 26, 40);
    render_sprite_sheet_init(&sprite_sheet_enemy_large, "C:/Users/Deborah/OneDrive/Desktop/project/engine-from-scratch/src/engine/assets/cactus12.png", 20, 40);
    render_sprite_sheet_init(&gv_sprite_sheet, "C:/Users/Deborah/OneDrive/Desktop/project/engine-from-scratch/src/engine/assets/gameover2.png", 400, 272);
    render_sprite_sheet_init(&start_sprite_sheet, "C:/Users/Deborah/OneDrive/Desktop/project/engine-from-scratch/src/engine/assets/start2.png",400,262);
	
	
    

	usize adef_player_walk_id = animation_definition_create(&sprite_sheet_player, 0.1, 0, (u8[]){0,1,2}, 3);
	usize adef_player_idle_id = animation_definition_create(&sprite_sheet_player, 0, 0, (u8[]){0}, 1);
	anim_player_walk_id = animation_create(adef_player_walk_id, true);
	anim_player_idle_id = animation_create(adef_player_idle_id, false);

    usize adef_enemy_small_id = animation_definition_create(&sprite_sheet_enemy_small, 0.0, 0, (u8[]){0}, 1);
    usize adef_enemy_large_id = animation_definition_create(&sprite_sheet_enemy_large, 0.0, 0, (u8[]){0}, 1);
    usize adef_enemy_small_enraged_id = animation_definition_create(&sprite_sheet_enemy_small, 0.0, 0, (u8[]){0}, 1);
    usize adef_enemy_large_enraged_id = animation_definition_create(&sprite_sheet_enemy_large, 0.1, 0, (u8[]){0}, 1);
    anim_enemy_small_id = animation_create(adef_enemy_small_id, true);
    anim_enemy_large_id = animation_create(adef_enemy_large_id, true);
    anim_enemy_small_enraged_id = animation_create(adef_enemy_small_enraged_id, true);
    anim_enemy_large_enraged_id = animation_create(adef_enemy_large_enraged_id, true);
    // start
    usize adef_start_animation_id = animation_definition_create(&start_sprite_sheet, 0.5, 0, (u8[]){0}, 1);
    usize anim_start_screen_id = animation_create(adef_start_animation_id, true);

	//game over
	 usize adef_gv_animation_id = animation_definition_create(&gv_sprite_sheet, 0.5, 0, (u8[]){0}, 1);
    usize anim_gv_screen_id = animation_create(adef_gv_animation_id, true);


   

    bool waiting_to_start = true;
	f32 spawn_timer_cactus = 0;
    reset();
	srand(time(NULL));


	while (!should_quit) {
		time_update();
		total_time += global.time.delta;
        // Fiecare 40 de secunde schimbă starea zi/noapte
        if (((int)(total_time / 20)) % 2 == 0) {
        is_night = false;  // zi
        } else {
        is_night = true;   // noapte
        }


		SDL_Event event;

		while (SDL_PollEvent(&event)) {
			switch (event.type) {
			case SDL_QUIT:
				should_quit = true;
				break;
			default:
				break;
			}
		}
        if(waiting_to_start == true){
            input_update();
			if (global.input.up) {
				waiting_to_start = false;
				
			}
			
            animation_update(global.time.delta);
			render_begin();
			
            glClearColor(1.0f, 1.0f, 1.0f, 1.0f);


			
            
            glClear(GL_COLOR_BUFFER_BIT);
			

			
           vec2 center = {render_width / 2.0f, render_height / 2.0f};

         render_sprite_sheet_frame(&start_sprite_sheet, 0, 0, center, false, WHITE, texture_slots);
         animation_render(animation_get(anim_start_screen_id), center, WHITE, texture_slots);


           
	       render_end(window, texture_slots);
          
           time_update_late();
            continue;

        }
        
        spawn_timer -= global.time.delta;
        ground_timer -= global.time.delta;
 
		Entity *player = entity_get(player_id);
		Body *body_player = physics_body_get(player->body_id);
        
		if (player_is_grounded) {
			player->animation_id = anim_player_walk_id;  // Setează animația de mers tot timpul
		} else {
			player->animation_id = anim_player_idle_id; 
		}
		
        
		
		if (is_game_over) {
			
			
			input_update();
			

			if (global.input.escape) {
				should_quit = true;
				
			}
			if(global.input.up){
				is_game_over = false;
                reset();
                continue;

			}
			player->animation_id = anim_player_idle_id;
			
			render_begin();
			animation_update(global.time.delta);
			render_begin();
			
            glClearColor(1.0f, 1.0f, 1.0f, 1.0f);


			
            
            glClear(GL_COLOR_BUFFER_BIT);
			

			
           vec2 center = {render_width / 2.0f, render_height / 2.0f};

           render_sprite_sheet_frame(&gv_sprite_sheet, 0, 0, center, false, WHITE, texture_slots);
            animation_render(animation_get(anim_gv_screen_id), center, WHITE, texture_slots);

			
			
			
			
			
	       render_end(window, texture_slots);
           time_update_late();
            continue;
			
		}else{
		
	   
		input_update();
		input_handle(body_player);
		physics_update();

		animation_update(global.time.delta);

		// Spawn enemies.
		{
			
			if (spawn_timer <= 0) {
				spawn_timer = (f32)((rand() % 200) + 100) / 100.f;

				
				
				if(spawn_timer<1.0f){
					spawn_timer=1.0f;
				}

                spawn_cactus();
			}
		}
	}
        
		render_begin();
		if (is_night) {
    // Culoare închisă pentru noapte (de exemplu un albastru închis)
    glClearColor(0.05f, 0.05f, 0.2f, 1.0f);
} else {
    // Culoarea de zi actuală (aer liber, cyan deschis)
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
}
        glClear(GL_COLOR_BUFFER_BIT);
		

		// Viteza la care se mișcă fundalul

  
 ground_offset -= (SPEED_GROUND+(score/50)) * global.time.delta; 

// Când fundalul iese din fereastră, îl resetăm
if (ground_offset <= -render_width) {
    ground_offset = 0.0f;  // Resetăm fundalul la început
}

// În timpul renderizării, afisează fundalul
render_sprite_sheet_frame(&sprite_sheet_map, 0, 0, (vec2){ground_offset, render_height - 64}, false,WHITE, texture_slots);
render_sprite_sheet_frame(&sprite_sheet_map, 0, 0, (vec2){ground_offset + render_width, render_height - 64}, false, WHITE, texture_slots);



        // Debug render bounding boxes.
        {
            for (usize i = 0; i < entity_count(); ++i) {
                Entity *entity = entity_get(i);
                Body *body = physics_body_get(entity->body_id);

                if (body->is_active) {
                    render_aabb((f32*)body, TURQUOISE);
                } else {
                    render_aabb((f32*)body, RED);
                }
            }

            for (usize i = 0; i < physics_static_body_count(); ++i) {
                render_aabb((f32*)physics_static_body_get(i), WHITE);
            }
        }

		// Render animated entities...
		for (usize i = 0; i < entity_count(); ++i) {
			Entity *entity = entity_get(i);
			if (!entity->is_active || entity->animation_id == (usize)-1) {
				continue;
			}

			Body *body = physics_body_get(entity->body_id);
			Animation *anim = animation_get(entity->animation_id);

			if (body->velocity[0] < 0) {
				anim->is_flipped = true;
			} else if (body->velocity[0] > 0) {
				anim->is_flipped = false;
			}

            vec2 pos;

            vec2_add(pos, body->aabb.position, entity->sprite_offset);
            animation_render(anim, pos, WHITE, texture_slots);
		}
		

        


        
        render_end(window, texture_slots);

		time_update_late();
		    
	
	}
	
	
	return 0;
		

		}
