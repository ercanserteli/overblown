#include <vector>
#include <string>
#include <sstream>
#include <iostream>
#include <SDL.h>
#include <SDL_mixer.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include "definitions.h"
#include "SDL_FontCache.h"
#ifdef __EMSCRIPTEN__
#include <emscripten/emscripten.h>
#endif

GameState* state;
static int screen_physical_width = SCREEN_WIDTH;
static int screen_physical_height = SCREEN_HEIGHT;
static bool closing = false;
static uint64 frame_count = 0;
static bool last_pause_press = false;
bool draw_debug = false;
SDL_GameController* gamepad_handles[MAX_CONTROLLERS];
int32 music_volume = 0;//MIX_MAX_VOLUME / 8;

constexpr real32 game_update_hz = 60;
constexpr real32 target_seconds_per_frame = 1.0f / game_update_hz;
uint64 perf_frequency = SDL_GetPerformanceFrequency();
SDL_Surface* screen_surface = NULL;
SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;

FC_Font* medium_font;
FC_Font* large_font;
FC_Font* large_blue_font;
FC_Font* xlarge_font;
static int32 gamepad_index = 0;
const int8 shake_xs[] = { -6, 3, 5, 2, -3, 2, -2, 0 };
const int8 shake_ys[] = { 3, -6, 2, 4, -2, 3, 1, -1 };

static void SDLInitGamepads()
{
	const int32 max_joysticks = SDL_NumJoysticks();
	for (int32 joystick_index = 0; joystick_index < max_joysticks; ++joystick_index)
	{
		if (!SDL_IsGameController(joystick_index))
		{
			continue;
		}
		if (gamepad_index >= MAX_CONTROLLERS)
		{
			break;
		}
		gamepad_handles[gamepad_index] = SDL_GameControllerOpen(joystick_index);

		char* mapping = SDL_GameControllerMapping(gamepad_handles[gamepad_index]);
		SDL_Log("Controller %i is mapped as \"%s\".", joystick_index, mapping);

		gamepad_index++;
	}
}


void handleEvents(ControllerInput& controller)
{
	SDL_Event event;
	while (SDL_PollEvent(&event))
	{
		if (event.type == SDL_QUIT)
		{
			closing = true;
		}
		else if (event.type == SDL_CONTROLLERDEVICEADDED)
		{
			LogInfo("Controller added: %d\n", event.cdevice.which);
			const int32 device_index = event.cdevice.which;
			if (SDL_IsGameController(device_index))
			{
				SDL_GameController* game_controller = SDL_GameControllerOpen(device_index);
			}
		}
		else if (event.type == SDL_CONTROLLERDEVICEREMOVED)
		{
			LogInfo("Controller removed: %d\n", event.cdevice.which);
			int32 instance_id = event.cdevice.which;
		}
		else if ((event.type == SDL_KEYDOWN || event.type == SDL_KEYUP) && !event.key.repeat)
		{
			const bool is_down = event.type == SDL_KEYDOWN;

			switch (event.key.keysym.sym)
			{
			//case SDLK_w:
			case SDLK_UP:
				controller.dir_up = is_down ? 1.0f : 0;
				break;
			//case SDLK_s:
			case SDLK_DOWN:
				controller.dir_down = is_down ? 1.0f : 0;
				break;
			//case SDLK_a:
			case SDLK_LEFT:
				controller.dir_left = is_down ? 1.0f : 0;
				break;
			//case SDLK_d:
			case SDLK_RIGHT:
				controller.dir_right = is_down ? 1.0f : 0;
				break;
			case SDLK_SPACE:
			case SDLK_p:
				controller.button_start = is_down;
				break;
			case SDLK_ESCAPE:
				controller.button_select = is_down;
				break;
			case SDLK_x:
				controller.button_a = is_down;
				break;
			case SDLK_n:
				controller.button_l = is_down;
				break;
			case SDLK_m:
				draw_debug = !draw_debug;
				break;
			// case SDLK_w://todo remove
			// 	if (state->currentLevel == state->levels + 3) {
			// 		changeCurrentState(Ending);
			// 	}
			// 	else {
			// 		changeCurrentState(Victory);
			// 	}
			// 	break;
			}
		}
		else if (event.type == SDL_CONTROLLERAXISMOTION)
		{
			real32 value;
			if (event.caxis.value > 0)
			{
				value = ((real32)event.caxis.value) / 32767.f;
			}
			else
			{
				value = ((real32)event.caxis.value) / 32768.f;
			}

			switch (event.caxis.axis)
			{
			case SDL_CONTROLLER_AXIS_LEFTX:
				if (value < 0)
				{
					controller.dir_left = -value;
					controller.dir_right = 0;
				}
				else
				{
					controller.dir_right = value;
					controller.dir_left = 0;
				}
				break;
			case SDL_CONTROLLER_AXIS_LEFTY:
				if (value < 0)
				{
					controller.dir_up = -value;
					controller.dir_down = 0;
				}
				else
				{
					controller.dir_down = value;
					controller.dir_up = 0;
				}
				break;
			}
		}
		else if (event.type == SDL_CONTROLLERBUTTONDOWN || event.type == SDL_CONTROLLERBUTTONUP)
		{
			const bool is_pressed = event.cbutton.state == SDL_PRESSED;

			switch (event.cbutton.button)
			{
			case SDL_CONTROLLER_BUTTON_DPAD_UP:
				controller.dir_up = is_pressed ? 1.0f : 0;
				break;
			case SDL_CONTROLLER_BUTTON_DPAD_DOWN:
				controller.dir_down = is_pressed ? 1.0f : 0;
				break;
			case SDL_CONTROLLER_BUTTON_DPAD_LEFT:
				controller.dir_left = is_pressed ? 1.0f : 0;
				break;
			case SDL_CONTROLLER_BUTTON_DPAD_RIGHT:
				controller.dir_right = is_pressed ? 1.0f : 0;
				break;
			case SDL_CONTROLLER_BUTTON_A:
				controller.button_a = is_pressed;
				break;
			case SDL_CONTROLLER_BUTTON_B:
				controller.button_b = is_pressed;
				break;
			case SDL_CONTROLLER_BUTTON_X:
				controller.button_c = is_pressed;
				break;
			case SDL_CONTROLLER_BUTTON_Y:
				controller.button_d = is_pressed;
				break;
			case SDL_CONTROLLER_BUTTON_LEFTSHOULDER:
				controller.button_l = is_pressed;
				break;
			case SDL_CONTROLLER_BUTTON_RIGHTSHOULDER:
				controller.button_r = is_pressed;
				break;
			case SDL_CONTROLLER_BUTTON_START:
				controller.button_start = is_pressed;
				break;
			case SDL_CONTROLLER_BUTTON_BACK:
				controller.button_select = is_pressed;
				break;
			default:
				LogWarn("Unknown controller button pressed");
			}
		}
		else if (event.type == SDL_MOUSEMOTION)
		{
			controller.mouseMoveX += event.motion.xrel;
			controller.mouseMoveY += event.motion.yrel;
		}
		else if (event.type == SDL_MOUSEBUTTONDOWN || event.type == SDL_MOUSEBUTTONUP)
		{
			switch (event.button.button)
			{
			case SDL_BUTTON_LEFT:
				controller.button_mouse_l = (event.button.state == SDL_PRESSED);
				break;
			case SDL_BUTTON_RIGHT:
				controller.button_mouse_r = (event.button.state == SDL_PRESSED);
				break;
			case SDL_BUTTON_MIDDLE:
				controller.button_mouse_m = (event.button.state == SDL_PRESSED);
				break;
			default:
				break;
			}
		}
		else if (event.type == SDL_MOUSEWHEEL)
		{
			controller.mouseWheel += event.wheel.y;
		}
	}
}

static real32 SDLGetSecondsElapsed(uint64 old_counter, uint64 current_counter, uint64 perf_frequency)
{
	return ((real32)(current_counter - old_counter) / (real32)(perf_frequency));
}

SDL_Texture* loadTexture(SDL_Renderer* renderer, std::string filepath, SDL_Surface** surfacePtr = NULL)
{
	SDL_Surface* surface = IMG_Load(("assets/" + filepath).c_str());
	if (surface == NULL)
	{
		LogError("Failed to load image %s", filepath.c_str());
		return NULL;
	}
	SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
	if (surfacePtr == NULL)
	{
		SDL_FreeSurface(surface);
	}
	else
	{
		*surfacePtr = surface;
	}
	return texture;
}

SDL_Texture* frozen_texture = NULL;
SDL_Texture* overlay_texture = NULL;
SDL_Texture* controls_texture = NULL;
SDL_Texture* title_bg_texture = NULL;
SDL_Texture* level_bg_texture = NULL;
SDL_Texture* win_screen_texture = NULL;


void changeCurrentState(State new_state)
{
	LogDebug("Changing state from %d to %d", state->current_state, new_state);
	if (state->current_state == Playing)
	{
		if (new_state == Paused)
		{
			Mix_VolumeMusic(music_volume / 2);
		}
		else if (new_state == Shaking)
		{
			state->shaking_frames = 0;
		}
		else if (new_state == Victory)
		{
			Mix_HaltMusic();
			playSound(victory);
		}
		else if (new_state == MainMenu) {
			Mix_HaltMusic();
			Mix_PlayMusic(title_music, -1);
			
			delete state;
			state = new GameState();
		}
		else if (new_state == BossEntrance) {
			Mix_HaltMusic();
		}
		else if (new_state == Ending) {
			Mix_HaltMusic();
			Mix_PlayMusic(winscreen_music, -1);
		}
	}
	else if (state->current_state == Victory)
	{
		if (new_state == MainMenu)
		{
			Mix_PlayMusic(title_music, -1);
		}
		else if (new_state == Playing)
		{
			Mix_PlayMusic(level_music, -1);
		}
	}
	else if (state->current_state == Dead)
	{
		if (new_state == Playing)
		{
			state->dead_frames = 0;
			state->reset();
		}
	}
	else if (state->current_state == Paused)
	{
		if (new_state == Playing)
		{
			Mix_VolumeMusic(music_volume);
		}
	}
	else if (state->current_state == MainMenu)
	{
		if (new_state == Controls)
		{
			Mix_HaltMusic();
		}
	}
	else if (state->current_state == Controls)
	{
		if (new_state == Beginning)
		{
			Mix_PlayMusic(level_music, -1);
		}
	}
	else if (state->current_state == GameOver)
	{
		if (new_state == MainMenu)
		{
			Mix_PlayMusic(title_music, -1);
		}
	}
	else if (state->current_state == Shaking)
	{
		if (new_state == Dead)
		{
			state->dead_frames = 0;
		}
	}
	else if (state->current_state == Ending)
	{
		if (new_state == MainMenu)
		{
			Mix_HaltMusic();
			Mix_PlayMusic(title_music, -1);
			delete state;
			state = new GameState();
		}
	}
	else if (state->current_state == BossEntrance)
	{
		if (new_state == Playing)
		{
			Mix_PlayMusic(boss_music, -1);
			state->player.velocity = {0, 0};
		}
	}

	state->current_state = new_state;
}

void initialize(SDL_Renderer* renderer)
{
	frozen_texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_TARGET, SCREEN_WIDTH, SCREEN_HEIGHT);
	overlay_texture = loadTexture(renderer, "overlay.png");
	controls_texture = loadTexture(renderer, "controls.png");
	title_bg_texture = loadTexture(renderer, "title_bg.png");
	level_bg_texture = loadTexture(renderer, "level_bg.png");
	win_screen_texture = loadTexture(renderer, "win_screen.png");

	player_texture_normal_idle = loadTexture(renderer, "player_idle.png");
	player_texture_normal_swim = loadTexture(renderer, "player_swim.png");
	player_texture_puffed_idle = loadTexture(renderer, "player_puffed.png");
	player_texture_puffed_swim = loadTexture(renderer, "player_puffed_flail.png");
	player_texture_puffing = loadTexture(renderer, "player_puffing.png");

	enemy_fish_texture_idle = loadTexture(renderer, "badfish_idle.png");
	enemy_fish_texture_swim = loadTexture(renderer, "badfish_swim.png");
	enemy_fish_texture_chase = loadTexture(renderer, "badfish_chase.png");
	enemy_shrimp_texture_main = loadTexture(renderer, "badshrimp_main.png");
	enemy_shrimp_texture_claw = loadTexture(renderer, "badshrimp_claw.png");
	enemy_shrimp_texture_claw_attack = loadTexture(renderer, "badshrimp_claw_attack.png");
	enemy_bubble_texture = loadTexture(renderer, "projectile_bubble.png");
	enemy_bubble_big_texture = loadTexture(renderer, "projectile_bubble_big.png");
	enemy_jellyfish_texture_idle = loadTexture(renderer, "badjelly.png");
	enemy_boss_texture_main_normal = loadTexture(renderer, "boss_main_normal.png");
	enemy_boss_texture_claw_normal = loadTexture(renderer, "boss_claw_normal.png");
	enemy_boss_texture_main_crouched = loadTexture(renderer, "boss_main_back.png");
	enemy_boss_texture_smallclaw_normal = loadTexture(renderer, "boss_main_smallclaw.png");
	enemy_boss_texture_spit = loadTexture(renderer, "boss_main_spit.png");

	decor_texture_seaweed = loadTexture(renderer, "seaweed.png");
	decor_texture_coral1 = loadTexture(renderer, "deco_coral1.png");
	decor_texture_coral2 = loadTexture(renderer, "deco_coral2.png");
	decor_texture_rock1 = loadTexture(renderer, "deco_rock1.png");
	decor_texture_rock2 = loadTexture(renderer, "deco_rock2.png");
	decor_texture_rock3 = loadTexture(renderer, "deco_rock3.png");
	decor_texture_arrow_up = loadTexture(renderer, "arrow_up.png");
	decor_texture_arrow_up_right = loadTexture(renderer, "arrow_up_right.png");
	decor_texture_arrow_down_right = loadTexture(renderer, "arrow_down_right.png");

	diagonal_texture = loadTexture(renderer, "diagonal.png");
	key_texture = loadTexture(renderer, "key.png");
	door_texture = loadTexture(renderer, "door.png");
	button_unpressed_texture = loadTexture(renderer, "button_unpressed.png");
	button_pressed_texture = loadTexture(renderer, "button_pressed.png");

	tile1_texture_topleft = loadTexture(renderer, "tile_top_left.png");
	tile1_texture_top = loadTexture(renderer, "tile_top.png");
	tile1_texture_topright = loadTexture(renderer, "tile_top_right.png");
	tile1_texture_midleft = loadTexture(renderer, "tile_mid_left.png");
	tile1_texture_mid = loadTexture(renderer, "tile_mid.png");
	tile1_texture_midright = loadTexture(renderer, "tile_mid_right.png");
	tile1_texture_botleft = loadTexture(renderer, "tile_bot_left.png");
	tile1_texture_bot = loadTexture(renderer, "tile_bot.png");
	tile1_texture_botright = loadTexture(renderer, "tile_bot_right.png");
	tile1_texture_breakable = loadTexture(renderer, "tile_breakable.png");

	heart_texture = loadTexture(renderer, "heart.png");
	grampa_texture = loadTexture(renderer, "grampa_puffer.png");
	stun_texture = loadTexture(renderer, "stun.png");

	//Load music
	title_music = Mix_LoadMUS("assets/menu.ogg");
	level_music = Mix_LoadMUS("assets/level.ogg");
	boss_music = Mix_LoadMUS("assets/boss.ogg");
	winscreen_music = Mix_LoadMUS("assets/win_screen.ogg");
	if (title_music == NULL)
	{
		LogError("Failed to load music! SDL_mixer Error: %s\n", Mix_GetError());
	}

	//Load sound effects
	shoot = Mix_LoadWAV("assets/shoot.wav");
	popHurt = Mix_LoadWAV("assets/pop_hurt.wav");
	popHarmless = Mix_LoadWAV("assets/pop_harmless.wav");
	playerHurt = Mix_LoadWAV("assets/player_hurt.wav");
	victory = Mix_LoadWAV("assets/victory.wav");
	inflate_sound = Mix_LoadWAV("assets/inflate.wav");
	deflate_sound = Mix_LoadWAV("assets/deflate.wav");
	block_break = Mix_LoadWAV("assets/block_break.wav");
	block_build = Mix_LoadWAV("assets/block_build.wav");
	heart_pickup = Mix_LoadWAV("assets/heart_pickup.wav");
	key_pickup = Mix_LoadWAV("assets/key_pickup.wav");
	heart_popped = Mix_LoadWAV("assets/heart_popped.wav");
	enter_butt = Mix_LoadWAV("assets/enter_butt.wav");
	fish_hurt = Mix_LoadWAV("assets/fish_hurt.wav");
	fish_die = Mix_LoadWAV("assets/fish_die.wav");
	boss_hurt = Mix_LoadWAV("assets/boss_hurt.wav");

	if (shoot == NULL)
	{
		LogError("Failed to load sound effect! SDL_mixer Error: %s\n", Mix_GetError());
	}

	SDL_RenderSetLogicalSize(renderer, SCREEN_WIDTH, SCREEN_HEIGHT);

	Mix_VolumeMusic(music_volume);
	Mix_PlayMusic(title_music, -1);

	// Text
	medium_font = FC_CreateFont();
	large_font = FC_CreateFont();
	large_blue_font = FC_CreateFont();
	xlarge_font = FC_CreateFont();
	// speech_font = FC_CreateFont();
	FC_LoadFont(medium_font, renderer, "assets/Action_Man.ttf", 24*6, FC_MakeColor(255, 255, 255, 255),
	            TTF_STYLE_NORMAL);
	// FC_LoadFont(speech_font, renderer, "assets/Action_Man.ttf", 24*6, FC_MakeColor(255, 255, 255, 255),
	//             TTF_STYLE_NORMAL);
	FC_LoadFont(large_font, renderer, "assets/Action_Man.ttf", 48*6, FC_MakeColor(255, 255, 255, 255),
	            TTF_STYLE_NORMAL);
	FC_LoadFont(large_blue_font, renderer, "assets/Action_Man.ttf", 48*6, FC_MakeColor(57, 59, 116, 255),
	            TTF_STYLE_NORMAL);
	FC_LoadFont(xlarge_font, renderer, "assets/Action_Man.ttf", 72*6, FC_MakeColor(116, 0, 32, 255),
	            TTF_STYLE_NORMAL);
}

inline bool handlePause(const ControllerInput* controller) {
	const bool pausePress = controller->button_select || controller->button_start;
	if (!last_pause_press && pausePress)
	{
		changeCurrentState(Paused);
		last_pause_press = pausePress;
		return true;
	}
	last_pause_press = pausePress;
	return false;
}

void playingUpdate(const ControllerInput* controller, real32 time_delta)
{
	if (handlePause(controller)){
		return;
	}

	Player* player = &state->player;
	
	player->update(time_delta, controller);
#if DEBUG
	if (controller->button_l) {
		player->noClip = !player->noClip;
	}
#endif

	// Center the camera over the player
	Rect2f* camera = &state->camera;
	if (!state->bossStarted) {
		Vector2f playerCenter = state->player.getCenter();
		camera->x = playerCenter.x - camera->w / 2;
		camera->y = playerCenter.y - camera->h / 2;
	}

    // Keep the camera in bounds of the level
    if(camera->x < 0) {
        camera->x = 0;
    }
    if(camera->y < 0) {
        camera->y = 0;
    }
    if(camera->x > state->currentLevel->width - camera->w) {
        camera->x = state->currentLevel->width - camera->w;
    }
    if(camera->y > state->currentLevel->height - camera->h) {
        camera->y = state->currentLevel->height - camera->h;
    }

	// Update stuff
	Rect2f extendedCamera = *camera;
	extendedCamera.x -= 200;
	extendedCamera.y -= 200;
	extendedCamera.w += 400;
	extendedCamera.h += 400;
	for (auto& enemy : state->enemies) {
		if (!enemy->isDead && enemy->getHitbox().collides(extendedCamera)) {
			enemy->think(time_delta);
			enemy->update(time_delta, &enemy->input);
		}
	}
	for (auto& decor : state->decors) {
		if (decor->getHitbox().collides(extendedCamera)) {
			decor->update(time_delta, 0);
		}
	}
	for (auto& diagonal : state->diagonals) {
		if (diagonal->getHitbox().collides(extendedCamera)) {
			diagonal->update(time_delta, 0);
		}
	}
	for (auto& button : state->buttons) {
		if (button->getHitbox().collides(extendedCamera)) {
			button->update(time_delta, 0);
		}
	}
	state->key.update(time_delta, 0);
	state->door.update(time_delta, 0);
	state->heart.update(time_delta, 0);
	state->grampa.update(time_delta, 0);

	if (!state->bossStarted && state->currentLevel == state->levels + 3 && state->player.position.x > 80 * 60) {
		state->bossStarted = true;
		changeCurrentState(State::BossEntrance);
	}
	
	// Clean dead bodies
	for (int32 i=state->enemies.size()-1; i>=0; i--) {
		auto& enemy = state->enemies[i];
		if (enemy->isDead){
			LogWarn("Cleaning dead enemy");
			deleteFromVector(state->AllActors, (Actor*)enemy.get());
			state->enemies.erase(state->enemies.begin() + i);
		}
	}

	// Add queued enemies
	if (state->newEnemiesQueue.size() > 0) {
		for (auto& enemy : state->newEnemiesQueue) {
			state->AllActors.push_back(enemy.get());
			state->enemies.push_back(std::move(enemy));
		}
		state->newEnemiesQueue.clear();
	}

	state->play_time_passed += time_delta;
}

inline void drawHUD() {
	// Hearts
	const SDL_Point heartPos = {50, 35};
	for (int32 i=0; i<state->player.health; i++) {
		SDL_Rect dstRect = {heartPos.x + i*135, heartPos.y, 100, 100};
		SDL_RenderCopy(renderer, heart_texture, 0, &dstRect);
	}

	// Cooldown
	const int32 cooldownLength = 370;
	SDL_Rect rectBg = {50, 150, cooldownLength, 50};
	SDL_Rect rectFg = {50, 150, (int)(cooldownLength * (state->player.puffMaxCooldown - state->player.puffCooldown) / state->player.puffMaxCooldown), 50};
	SDL_SetRenderDrawColor(renderer, 70, 0, 0, 255);
	SDL_RenderFillRect(renderer, &rectBg);
	if (rectFg.w > 0) {
		SDL_SetRenderDrawColor(renderer, 150, 255, 30, 255);
		SDL_RenderFillRect(renderer, &rectFg);
	}

	// Pause overlay
	if (state->current_state == Paused)
	{
		SDL_RenderCopy(renderer, overlay_texture, 0, 0);
		SDL_Rect controls_rect = {140*6, 100*6, 360*6, 180*6};
		SDL_RenderCopy(renderer, controls_texture, 0, &controls_rect);
		SDL_RenderPresent(renderer);
	}
	else if (state->current_state == Dead) {
		FC_Draw(xlarge_font, renderer, 300, 150, "You Sleep");
		FC_Draw(xlarge_font, renderer, 200, 600, "with the Fishes");
	}
	
#if DEBUG
		// Draw speed
		// const real32 speed = state->player.velocity.getMagnitude();
		// FC_Draw(medium_font, renderer, 3100, 50, std::to_string((int)roundf(speed)).c_str());
#endif
}

void playingDraw()
{
	SDL_RenderCopy(renderer, level_bg_texture, 0, 0);

	for (Solid& solid : state->currentLevel->solids) {
		solid.render(renderer);
	}

	Rect2f extendedCamera = state->camera;
	extendedCamera.x -= 200;
	extendedCamera.y -= 200;
	extendedCamera.w += 400;
	extendedCamera.h += 400;
	for (Actor* actor : state->AllActors) {
		if (actor->getHitbox().collides(extendedCamera)) {
			actor->render(renderer);
#if DEBUG
			if (draw_debug) {
				// Draw the hitboxes
				SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
				SDL_Rect rect = actor->getHitbox().toSDLRect();
				rect.x -= state->camera.x;
				rect.y -= state->camera.y;
				SDL_SetRenderDrawColor(renderer, 255, 0, 0, 85);
				SDL_RenderFillRect(renderer, &rect);

				EnemyBoss* boss = dynamic_cast<EnemyBoss*>(actor);
				if (boss) {
					for (Rect2f rect : boss->clawHitRects) {
						Vector2f center = rect.getCenter();
						Vector2f rotated = rotatePoint({center.x+ boss->claw_normal_offset.x + boss->position.x, center.y + boss->claw_normal_offset.y + boss->position.y}, {boss->claw_joint_offset.x + boss->position.x, boss->claw_joint_offset.y + boss->position.y}, (boss->clawAngle + boss->clawAngleWave)*0.75);
						rect.x = rotated.x - rect.w/2 - state->camera.x;
						rect.y = rotated.y - rect.h/2 - state->camera.y;
						SDL_Rect sdl_rect = rect.toSDLRect();
						SDL_SetRenderDrawColor(renderer, 255, 0, 0, 85);
						SDL_RenderFillRect(renderer, &sdl_rect);
					}
				}

				SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
			}
#endif
		}
	}

	drawHUD();
}

void update(const ControllerInput* controller, real32 time_delta) {
	bool pausePress = false;
	real32 speedMult = 1.f;
	switch (state->current_state)
	{
	case MainMenu:
		if (controller->button_start)
		{
			if (!last_pause_press){
				last_pause_press = true;
				changeCurrentState(Controls);
			}
		}
		else {
			last_pause_press = false;
		}
#ifndef __EMSCRIPTEN__
		if (controller->button_select)
		{
			closing = true;
		}
#endif
		state->main_menu_frames++;
		break;
	case Controls:
		if (controller->button_start && state->controls_frames > 10)
		{
			last_pause_press = true;
			changeCurrentState(Beginning);
		}
		if (controller->button_select)
		{
			closing = true;
		}
		state->controls_frames++;
		break;
	case Beginning:
		if (state->beginning_frames > 1)
		{
			changeCurrentState(Playing);
		}
		state->beginning_frames++;
		break;
	case Playing:
		playingUpdate(controller, time_delta);
		break;
	case Dead:
		if (state->dead_frames > 120)
		{
			changeCurrentState(Playing);
		}
		state->dead_frames++;
		break;
	case Paused:
		pausePress = controller->button_start || controller->button_select;
		if (!last_pause_press && pausePress)
		{
			changeCurrentState(Playing);
		}
		if (!last_pause_press && controller->button_select)
		{
			closing = true;
		}
		last_pause_press = pausePress;
		break;
	case GameOver:

		state->gameover_frames++;
		break;
	case Shaking:
		if (state->shaking_for_dead) {
			if (state->shaking_frames > 7) {
				changeCurrentState(Dead);
				state->shaking_for_dead = false;
			}
		}
		else if (state->shaking_frames > 2) {
			changeCurrentState(Playing);
		}
		break;
	case Victory:
		if (state->victory_frames > 150)
		{
			state->victory_frames = 0;
			if(state->currentLevel == &state->levels[0])
			{
				state->currentLevel = &state->levels[1];
				state->reset();
				changeCurrentState(Playing);
			}
			else if (state->currentLevel == &state->levels[1])
			{
				state->currentLevel = &state->levels[2];
				state->reset();
				changeCurrentState(Playing);
			}
			else if (state->currentLevel == &state->levels[2])
			{
				state->currentLevel = &state->levels[3];
				state->reset();
				changeCurrentState(Playing);
			}
			else
			{
				changeCurrentState(Ending);
			}
		}
		state->victory_frames++;
		break;
	case Ending:
		if (state->ending_time > 7.f) {
			if (controller->button_select || controller->button_start) {
				last_pause_press = true;
				
				changeCurrentState(MainMenu);
			}
		}
		state->ending_time += time_delta;
		break;
	case BossEntrance:
		#if DEBUG
		speedMult = 10.f;
		#endif
		if (state->camera.x + state->camera.w < state->currentLevel->width) {
			// Move the camera into place
			state->camera.x += 350.f * time_delta * speedMult;
		}
		else {
			int32& brickState = state->boss_brick_state;
			if (brickState < 6) {
				if (state->boss_entrance_time > 0.5f * (brickState + 1) / speedMult) {
					for (int i=0; i < 4; i++) {
						for (int j=1; j < 3 * (brickState + 1); j++) {
							Vector2f coords = {(float)((i + 64) * LEVEL_SCALE), (float)(j * LEVEL_SCALE)};
							if (!state->currentLevel->checkSolid(coords)) {
								state->currentLevel->addSolid(Solid(coords, LEVEL_SCALE, LEVEL_SCALE, tile1_texture_mid), i+64, j);
							}
						}
						for (int j=35; j > 35 - 3 * (brickState + 1); j--) {
							Vector2f coords = {(float)((i + 64) * LEVEL_SCALE), (float)(j * LEVEL_SCALE)};
							if (!state->currentLevel->checkSolid(coords)) {
								state->currentLevel->addSolid(Solid(coords, LEVEL_SCALE, LEVEL_SCALE, tile1_texture_mid), i+64, j);
							}
						}
					}
					brickState++;
					playSound(block_build);
				}
			}
			else {
				changeCurrentState(State::Playing);
			}
			state->boss_entrance_time += time_delta;
		}
		break;
	default:
		break;
	}
}

void draw() {
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
	SDL_RenderClear(renderer);
	if (state->current_state == Shaking)
	{
		SDL_SetRenderTarget(renderer, frozen_texture);
	}

	if (state->current_state == MainMenu)
	{
		// Main menu
		SDL_RenderCopy(renderer, title_bg_texture, 0, 0);
		if (state->main_menu_frames/30 % 2 == 0)
		{
			FC_Draw(medium_font, renderer, 180*6, 275*6, "Press Space to Start");
		}
	}
	else if (state->current_state == Controls)
	{
		// Controls
		SDL_Rect controls_rect = {140*6, 100*6, 360*6, 180*6};
		SDL_RenderCopy(renderer, controls_texture, 0, &controls_rect);
	}
	else
	{
		playingDraw();
	}
	
	if (state->current_state == Ending) {
		SDL_Rect rect = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};
		if (state->ending_time <= 3.0f) {
			SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
			real32 fadeAmount = MIN(state->ending_time/3.0f, 1.f);
			SDL_SetRenderDrawColor(renderer, 255, 255, 255, static_cast<Uint8>(fadeAmount * 255.f));
			SDL_RenderFillRect(renderer, &rect);
		}
		else {
			SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
			SDL_RenderCopyF(renderer, win_screen_texture, 0, 0);
		}
		if (state->ending_time > 5.f) {
			renderOutlinedText(large_blue_font, renderer, 200, 300, "You have saved the ocean!", 5, {57, 59, 116, 255}, {255, 255, 255, 255});
		}
		if (state->ending_time > 7.f) {
			renderOutlinedText(large_blue_font, renderer, 300, 1700, "Thank you for playing!", 5, {57, 59, 116, 255}, {255, 255, 255, 255});
		}
	}

	// Screen Shake
	if (state->current_state == Shaking)
	{
		SDL_SetRenderTarget(renderer, 0);
		const SDL_Rect frame_rect = { shake_xs[state->shaking_frames], shake_ys[state->shaking_frames], SCREEN_WIDTH, SCREEN_HEIGHT};
		SDL_RenderCopy(renderer, frozen_texture, 0, &frame_rect);
		SDL_RenderPresent(renderer);

		state->shaking_frames++;
	}
	else
	{
		SDL_RenderPresent(renderer);
	}

	if (state->current_state != Paused && state->current_state != Shaking)
	{
		static uint32 non_paused_frame_count = 0;
		non_paused_frame_count++;
	}
}

void updateAndDraw(const ControllerInput* controller, real32 time_delta)
{
	update(controller, time_delta);
	draw();
}

void main_loop() {
	static uint64 last_counter = SDL_GetPerformanceCounter();
	static uint64 update_counter = last_counter;
	static ControllerInput controller = {};

	handleEvents(controller);

	uint64 new_update_counter = SDL_GetPerformanceCounter();
	const real32 time_delta = SDLGetSecondsElapsed(update_counter, new_update_counter, perf_frequency);
	update_counter = new_update_counter;

	updateAndDraw(&controller, time_delta);

#ifndef __EMSCRIPTEN__
	const real32 seconds_elapsed = SDLGetSecondsElapsed(last_counter, SDL_GetPerformanceCounter(), perf_frequency);
	if (seconds_elapsed < target_seconds_per_frame)
	{
		const int32 time_to_sleep = (int32)((target_seconds_per_frame - seconds_elapsed) * 1000) - 1;
		if (time_to_sleep > 0)
		{
			SDL_Delay(time_to_sleep);
		}
		//SDL_assert(SDLGetSecondsElapsed(last_counter, SDL_GetPerformanceCounter(), perf_frequency) < target_seconds_per_frame);
		while (SDLGetSecondsElapsed(last_counter, SDL_GetPerformanceCounter(), perf_frequency) <
			target_seconds_per_frame)
		{
			// Waiting...
		}
	}
#endif

	uint64 end_counter = SDL_GetPerformanceCounter();

#if DEBUG
	if (frame_count % 256 == 0)
	{
		uint64 counter_elapsed = end_counter - last_counter;
		real64 ms_per_frame = (((1000.0f * (real64)counter_elapsed) / (real64)perf_frequency));
		real64 fps = (real64)perf_frequency / (real64)counter_elapsed;
		printf("%.02f ms/f, %.02f f/s \n", ms_per_frame, fps);
	}
#endif
	last_counter = end_counter;

	if (closing) {
#ifdef __EMSCRIPTEN__
		changeCurrentState(MainMenu);
		closing = false;
#else
		exit(0);
#endif
	}

	frame_count++;
}

int main(int argc, char** argv) {
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_GAMECONTROLLER) != 0) {
		std::cout << "SDL_Init Error: " << SDL_GetError() << std::endl;
		return 1;
	}
	atexit(SDL_Quit);

	uint32 window_properties = 0;

#if (!defined(DEBUG) || DEBUG == 0) || defined(__EMSCRIPTEN__)
	SDL_DisplayMode display_mode;
	int32 should_be_zero = SDL_GetCurrentDisplayMode(0, &display_mode);
	window_properties |= SDL_WINDOW_BORDERLESS | SDL_WINDOW_RESIZABLE;
	if (should_be_zero != 0) {
		LogError("Could not get display mode");
	}
	else {
		screen_physical_width = display_mode.w;
		screen_physical_height = display_mode.h;
	}
#else
screen_physical_width = 1920;
screen_physical_height = 1080;
#endif

	window = SDL_CreateWindow("Overblown", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, screen_physical_width, screen_physical_height, window_properties);
	if (!window) {
		LogError("Window could not be created! SDL_Error: %s\n", SDL_GetError());
		return 1;
	}
	LogInfo("Window is created");

	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

	SDLInitGamepads();

	if (IMG_Init(IMG_INIT_PNG) == 0) {
		LogError("SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError());
	}
	if (Mix_Init(MIX_INIT_OGG) == 0) {
		LogError("SDL_mixer could not initialize! SDL_mixer Error: %s\n", Mix_GetError());
	}
	if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
		LogError("SDL_mixer could not initialize! SDL_mixer Error: %s\n", Mix_GetError());
	}

	SDL_ShowCursor(SDL_DISABLE);

	initialize(renderer);
	state = new GameState();
#ifdef __EMSCRIPTEN__
    emscripten_set_main_loop(main_loop, 0, 1);
#else
    while (true) { main_loop(); }
#endif

	return 0;
}
