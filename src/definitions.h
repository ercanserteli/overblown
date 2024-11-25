#pragma once

#include <functional>
#include <stdint.h>
#include <array>
#include <cmath>
#include <algorithm>
#include <memory>
#include <random>
#include "SDL_FontCache.h"

// The logical screen width and height being rendered to
#define SCREEN_WIDTH 3840
#define SCREEN_HEIGHT 2160
#define LEVEL_SCALE 60

#define Pi32 3.14159265358979f

#define MIN(a, b) (((a) > (b)) ? (b) : (a))
#define MAX(a, b) (((a) > (b)) ? (a) : (b))

#define degToRad(angleInDegrees) ((angleInDegrees) * Pi32 / 180.0)
#define radToDeg(angleInRadians) ((angleInRadians) * 180.0 / Pi32)
#define SIGN(a) (((a) > 0) - ((a) < 0))

#define LEN(x)  (sizeof(x) / sizeof((x)[0]))

#define polarToCar(r, theta) {SCREEN_WIDTH / 2 + r * cos(degToRad(theta)), SCREEN_HEIGHT / 2 + r * sin(degToRad(theta))}

#define MAX_CONTROLLERS 4

#if DEBUG
    #define LogDebug(...) printf(__VA_ARGS__); printf("\n")
    #define LogInfo(...) printf(__VA_ARGS__); printf("\n")
#else
	#define LogDebug(...)
	#define LogInfo(...)
#endif

#define LogWarn(...) SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, __VA_ARGS__);
#define LogError(...) SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, __VA_ARGS__);

#define MUL_UP_1L 1.005f
#define MUL_UP_2L 1.01f
#define MUL_DOWN_1L 0.995025f
#define MUL_DOWN_2L 0.990099f

#define MUL_UP_1M 1.004f
#define MUL_UP_2M 1.008f
#define MUL_DOWN_1M 0.9960159f
#define MUL_DOWN_2M 0.99206349f

#define MUL_UP_1 1.0045f
#define MUL_UP_2 1.009f
#define MUL_DOWN_1 0.995520159f
#define MUL_DOWN_2 0.9910802775f

using int8 = int8_t;
using int16 = int16_t;
using int32 = int32_t;
using int64 = int64_t;

using uint8 = uint8_t;
using uint16 = uint16_t;
using uint32 = uint32_t;
using uint64 = uint64_t;

using real32 = float;
using real64 = double;

Uint32 getPixel(SDL_Surface* surface, int x, int y)
{
	int bpp = surface->format->BytesPerPixel;
	/* Here p is the address to the pixel we want to retrieve */
	Uint8* p = (Uint8*)surface->pixels + y * surface->pitch + x * bpp;

	switch (bpp)
	{
	case 1:
		return *p;
		break;

	case 2:
		return *(Uint16*)p;
		break;

	case 3:
		if (SDL_BYTEORDER == SDL_BIG_ENDIAN)
			return p[0] << 16 | p[1] << 8 | p[2];
		else
			return p[0] | p[1] << 8 | p[2] << 16;
		break;

	case 4:
		return *(Uint32*)p;
		break;

	default:
		return 0; /* shouldn't happen, but avoids warnings */
	}
}


struct Vector2f
{
	real32 x;
	real32 y;

	Vector2f(real32 x, real32 y) : x(x), y(y)
	{
	}

	Vector2f() : x(0), y(0)
	{
	}

	inline real32 getMagnitude() const
	{
		return sqrtf(x * x + y * y);
	}

	void normalize()
	{
		const real32 magnitude = sqrtf(x * x + y * y);
		if (magnitude > 0)
		{
			x = x / magnitude;
			y = y / magnitude;
		}
	}

	Vector2f getNormalized() const
	{
		Vector2f normalized = *this;
		normalized.normalize();
		return normalized;
	}

	inline bool isZero() const {
		return x == 0 && y == 0;
	}

	// With vectors

	Vector2f& operator+=(Vector2f b)
	{
		this->x += b.x;
		this->y += b.y;
		return *this;
	}

	Vector2f& operator-=(Vector2f b)
	{
		this->x -= b.x;
		this->y -= b.y;
		return *this;
	}

	// Hadamard product
	Vector2f& operator*=(Vector2f b)
	{
		this->x *= b.x;
		this->y *= b.y;
		return *this;
	}

	Vector2f& operator/=(Vector2f b)
	{
		this->x /= b.x;
		this->y /= b.y;
		return *this;
	}

	// Scaling
	Vector2f& operator*=(real32 b)
	{
		this->x = this->x * b;
		this->y = this->y * b;
		return *this;
	}

	Vector2f& operator/=(real32 b)
	{
		this->x = this->x / b;
		this->y = this->y / b;
		return *this;
	}

	bool operator==(Vector2f b) const
	{
		return (this->x == b.x && this->y == b.y);
	}

	Vector2f operator-() const {
        return Vector2f(-x, -y);
    }

	operator bool() const {
        return !isZero();
    }
};


// With vectors

inline Vector2f operator+(Vector2f a, Vector2f b)
{
	Vector2f result;
	result.x = a.x + b.x;
	result.y = a.y + b.y;
	return result;
}

inline Vector2f operator-(Vector2f a, Vector2f b)
{
	Vector2f result;
	result.x = a.x - b.x;
	result.y = a.y - b.y;
	return result;
}

// Hadamard product
inline Vector2f operator*(Vector2f a, Vector2f b)
{
	Vector2f result;
	result.x = a.x * b.x;
	result.y = a.y * b.y;
	return result;
}

inline Vector2f operator/(Vector2f a, Vector2f b)
{
	Vector2f result;
	result.x = a.x / b.x;
	result.y = a.y / b.y;
	return result;
}

// Dot product
inline real32 dot(Vector2f a, Vector2f b)
{
	return a.x * b.x + a.y * b.y;
}

// Scaling
inline Vector2f operator*(Vector2f a, real32 b)
{
	Vector2f result;
	result.x = a.x * b;
	result.y = a.y * b;
	return result;
}

inline Vector2f operator/(Vector2f a, real32 b)
{
	Vector2f result;
	result.x = a.x / b;
	result.y = a.y / b;
	return result;
}

// Specialize std::hash for Vector2f
namespace std {
    template <>
    struct hash<Vector2f> {
        std::size_t operator()(const Vector2f& v) const noexcept {
            std::hash<float> hasher;
            std::size_t h1 = hasher(v.x);
            std::size_t h2 = hasher(v.y);
            return h1 ^ (h2 << 1);
        }
    };
}

struct Rect2f
{
	real32 x;
	real32 y;
	real32 w;
	real32 h;

	Vector2f getCenter() const
	{
		Vector2f center;
		center.x = x + w / 2;
		center.y = y + h / 2;
		return center;
	}

	bool contains(Vector2f point) const
	{
		if (point.x >= x && point.x <= x + w && point.y >= y && point.y <= y + h)
		{
			return true;
		}
		return false;
	}

	bool collides(Rect2f other) const
	{
		return x < other.x + other.w &&
			x + w > other.x &&
			y < other.y + other.h &&
			y + h > other.y;
	}

	Vector2f collisionDepth(const Rect2f& other) const
	{
		Vector2f depth(0, 0);

		// Calculate the horizontal overlap
		if (x < other.x + other.w && x + w > other.x) {
			real32 overlapLeft = other.x + other.w - x;
			real32 overlapRight = x + w - other.x;
			depth.x = (overlapLeft < overlapRight) ? -overlapLeft : overlapRight;
		}

		// Calculate the vertical overlap
		if (y < other.y + other.h && y + h > other.y) {
			real32 overlapTop = other.y + other.h - y;
			real32 overlapBottom = y + h - other.y;
			depth.y = (overlapTop < overlapBottom) ? -overlapTop : overlapBottom;
		}

		return depth;
	}

	SDL_Rect toSDLRect() const {
		return {(int)x, (int)y, (int)w, (int)h};
	}
};

Vector2f getUnitVectorFromRadians(float angleInRadians) {
    Vector2f unitVector;
    unitVector.x = std::cos(angleInRadians);
    unitVector.y = std::sin(angleInRadians);
    return unitVector;
}

inline Vector2f getUnitVectorFromDegrees(float angleInDegrees) {
    return getUnitVectorFromRadians(angleInDegrees * M_PI / 180.0f);
}

Vector2f rotatePoint(const Vector2f& point, const Vector2f& pivot, float angleInDegrees) {
    float angleInRadians = angleInDegrees * M_PI / 180.0f;
    float translatedX = point.x - pivot.x;
    float translatedY = point.y - pivot.y;
    float rotatedX = translatedX * std::cos(angleInRadians) - translatedY * std::sin(angleInRadians);
    float rotatedY = translatedX * std::sin(angleInRadians) + translatedY * std::cos(angleInRadians);
    Vector2f rotatedPoint;
    rotatedPoint.x = rotatedX + pivot.x;
    rotatedPoint.y = rotatedY + pivot.y;
    return rotatedPoint;
}

/** Rect - line collision */
inline bool isPointInAABB(const Vector2f& p, const Rect2f& rect) {
	return (p.x >= rect.x && p.x <= rect.x + rect.w && p.y >= rect.y && p.y <= rect.y + rect.h);
}

inline bool checkLineSegmentIntersection(const Vector2f& p1, const Vector2f& p2, const Vector2f& q1, const Vector2f& q2) {
	auto orientation = [](const Vector2f& p, const Vector2f& q, const Vector2f& r) {
		return (q.y - p.y) * (r.x - q.x) - (q.x - p.x) * (r.y - q.y);
	};

	const real32 o1 = orientation(p1, p2, q1);
	const real32 o2 = orientation(p1, p2, q2);
	const real32 o3 = orientation(q1, q2, p1);
	const real32 o4 = orientation(q1, q2, p2);
	return o1 * o2 < 0 && o3 * o4 < 0;
}

bool isLineSegmentIntersectingAABB(const Vector2f& p1, const Vector2f& p2, const Rect2f& rect) {
	// Define the four edges of the AABB as line segments
	const Vector2f topLeft = {rect.x, rect.y};
	const Vector2f topRight = {rect.x + rect.w, rect.y};
	const Vector2f bottomLeft = {rect.x, rect.y + rect.h};
	const Vector2f bottomRight = {rect.x + rect.w, rect.y + rect.h};

	// Check if the line segment intersects any of the AABB's edges
	return checkLineSegmentIntersection(p1, p2, topLeft, topRight) || // Top edge
		checkLineSegmentIntersection(p1, p2, topRight, bottomRight) || // Right edge
		checkLineSegmentIntersection(p1, p2, bottomRight, bottomLeft) || // Bottom edge
		checkLineSegmentIntersection(p1, p2, bottomLeft, topLeft); // Left edge
}

Vector2f closestPointOnLineSegment(const Vector2f& p, const Vector2f& a, const Vector2f& b) {
	Vector2f ab = b - a;
	Vector2f ap = p - a;
	real32 t = (ap.x * ab.x + ap.y * ab.y) / (ab.x * ab.x + ab.y * ab.y);
	t = std::max(0.0f, std::min(1.0f, t));
	return a + ab * t;
}


bool checkAABBLineCollision(const Vector2f& p1, const Vector2f& p2, const Rect2f& rect, Vector2f * deltaPosition=NULL, Vector2f * normal=NULL) {
	if (isPointInAABB(p1, rect) && isPointInAABB(p2, rect)) {
		return true;
	}
	if (isLineSegmentIntersectingAABB(p1, p2, rect)) {
		if (deltaPosition && normal) {
			Vector2f rectCenter = rect.getCenter();
			Vector2f closestPoint = closestPointOnLineSegment(rectCenter, p1, p2);
			Vector2f penetrationVector = rectCenter - closestPoint;
			real32 penetrationDepth = sqrtf(rect.w*rect.w + rect.h*rect.h)/2.f - dot(penetrationVector, *normal);

			if (penetrationDepth > 0) {
				(*deltaPosition) = (*normal) * penetrationDepth;
				return true;
			}
		}
		else {
			return true;
		}
	}
	return false;
}

void renderOutlinedText(FC_Font* font, SDL_Renderer* renderer, float x, float y, const char* text, int outlineWidth=3, SDL_Color textColor = {255, 255, 255, 255}, SDL_Color outlineColor = {0, 0, 0, 255}) {
    FC_SetDefaultColor(font, outlineColor);

    for (int dx = -outlineWidth; dx <= outlineWidth; ++dx) {
        for (int dy = -outlineWidth; dy <= outlineWidth; ++dy) {
            if (dx != 0 || dy != 0) {
                FC_Draw(font, renderer, x + dx, y + dy, text);
            }
        }
    }

    FC_SetDefaultColor(font, textColor);
    FC_Draw(font, renderer, x, y, text);
}

template <typename T>
bool deleteFromVector(std::vector<T>& vec, const T& valueToRemove) {
    auto it = std::find(vec.begin(), vec.end(), valueToRemove);
    if (it != vec.end()) {
        vec.erase(it);
		return true;
    }
	return false;
}

template <typename T>
bool deleteUniqueFromVector(std::vector<std::unique_ptr<T>>& vec, const T* valueToRemove) {
    auto it = std::find_if(vec.begin(), vec.end(),
            [valueToRemove](const std::unique_ptr<T>& value) {
                return value.get() == valueToRemove;
            });
    if (it != vec.end()) {
        vec.erase(it);
		return true;
    }
	return false;
}

inline void playSound(Mix_Chunk* sound) {
	Mix_PlayChannel(-1, sound, 0);
}

std::mt19937 rng(std::random_device{}());


struct ControllerInput
{
	real32 dir_left = 0.f;
	real32 dir_right = 0.f;
	real32 dir_up = 0.f;
	real32 dir_down = 0.f;

	int32 mouseMoveX = 0;
	int32 mouseMoveY = 0;
	int32 mouseWheel = 0;
	bool button_mouse_l = false;
	bool button_mouse_r = false;
	bool button_mouse_m = false;

	bool button_a = false;
	bool button_b = false;
	bool button_c = false;
	bool button_d = false;
	bool button_l = false;
	bool button_r = false;
	bool button_l2 = false;
	bool button_r2 = false;
	bool button_select = false;
	bool button_start = false;
};

enum State
{
	MainMenu,
	Controls,
	Beginning,
	Playing,
	Victory,
	Dead,
	Paused,
	GameOver,
	Shaking,
	BossEntrance,
	Ending
};

FC_Font* speech_font;

SDL_Texture* player_texture_normal_idle = NULL;
SDL_Texture* player_texture_normal_swim = NULL;
SDL_Texture* player_texture_puffed_idle = NULL;
SDL_Texture* player_texture_puffed_swim = NULL;
SDL_Texture* player_texture_puffing = NULL;

SDL_Texture* enemy_fish_texture_idle = NULL;
SDL_Texture* enemy_fish_texture_swim = NULL;
SDL_Texture* enemy_fish_texture_chase = NULL;
SDL_Texture* enemy_shrimp_texture_main = NULL;
SDL_Texture* enemy_shrimp_texture_claw = NULL;
SDL_Texture* enemy_shrimp_texture_claw_attack = NULL;
SDL_Texture* enemy_bubble_texture = NULL;
SDL_Texture* enemy_bubble_big_texture = NULL;
SDL_Texture* enemy_jellyfish_texture_idle = NULL;
SDL_Texture* enemy_boss_texture_main_normal = NULL;
SDL_Texture* enemy_boss_texture_claw_normal = NULL;
SDL_Texture* enemy_boss_texture_main_crouched = NULL;
SDL_Texture* enemy_boss_texture_smallclaw_normal = NULL;
SDL_Texture* enemy_boss_texture_spit = NULL;

SDL_Texture* decor_texture_seaweed = NULL;
SDL_Texture* decor_texture_coral1 = NULL;
SDL_Texture* decor_texture_coral2 = NULL;
SDL_Texture* decor_texture_rock1 = NULL;
SDL_Texture* decor_texture_rock2 = NULL;
SDL_Texture* decor_texture_rock3 = NULL;
SDL_Texture* decor_texture_arrow_up = NULL;
SDL_Texture* decor_texture_arrow_up_right = NULL;
SDL_Texture* decor_texture_arrow_down_right = NULL;

SDL_Texture* diagonal_texture = NULL;
SDL_Texture* key_texture = NULL;
SDL_Texture* door_texture = NULL;
SDL_Texture* button_unpressed_texture = NULL;
SDL_Texture* button_pressed_texture = NULL;

SDL_Texture* tile1_texture_topleft = NULL;
SDL_Texture* tile1_texture_top = NULL;
SDL_Texture* tile1_texture_topright = NULL;
SDL_Texture* tile1_texture_midleft = NULL;
SDL_Texture* tile1_texture_mid = NULL;
SDL_Texture* tile1_texture_midright = NULL;
SDL_Texture* tile1_texture_botleft = NULL;
SDL_Texture* tile1_texture_bot = NULL;
SDL_Texture* tile1_texture_botright = NULL;
SDL_Texture* tile1_texture_breakable = NULL;

SDL_Texture* heart_texture = NULL;
SDL_Texture* grampa_texture = NULL;
SDL_Texture* stun_texture = NULL;

Mix_Music* title_music = NULL;
Mix_Music* level_music = NULL;
Mix_Music* boss_music = NULL;
Mix_Music* winscreen_music = NULL;

Mix_Chunk* shoot = NULL;
Mix_Chunk* popHurt = NULL;
Mix_Chunk* popHarmless = NULL;
Mix_Chunk* playerHurt = NULL;
Mix_Chunk* victory = NULL;
Mix_Chunk* inflate_sound = NULL;
Mix_Chunk* deflate_sound = NULL;
Mix_Chunk* block_break = NULL;
Mix_Chunk* block_build = NULL;
Mix_Chunk* heart_pickup = NULL;
Mix_Chunk* key_pickup = NULL;
Mix_Chunk* heart_popped = NULL;
Mix_Chunk* enter_butt = NULL;
Mix_Chunk* fish_hurt = NULL;
Mix_Chunk* fish_die = NULL;
Mix_Chunk* boss_hurt = NULL;

void renderTexture(SDL_Renderer* renderer, SDL_Texture* texture, const SDL_Rect* sourceRect, const SDL_FRect* destRect);
void renderTextureEx(SDL_Renderer* renderer, SDL_Texture* texture, const SDL_Rect* sourceRect, const SDL_FRect* destRect, const double angle, const SDL_FPoint* center, SDL_RendererFlip flip);
void changeCurrentState(State new_state);

enum Direction
{
	Left,
	Right
};

class Actor;
class Level;

class Solid
{
public:
	Solid(Vector2f position, real32 width, real32 height, SDL_Texture* texture, bool collidable = true, bool breakable=false, bool doesMove=false)
		: xRemainder(0), yRemainder(0), collidable(collidable), position(position), width(width), height(height),
		  texture(texture), breakable(breakable), doesMove(doesMove), orgPosition(position), sprite_rect({0, 0, static_cast<int>(width), static_cast<int>(height)})
	{
		dest_rect = {position.x, position.y, width, height};
	}

	void move(real32 x, real32 y);
	std::vector<Actor*> getAllRidingActors();
	bool overlapCheck(Actor* actor);

	real32 getLeft() const
	{
		return position.x;
	}

	real32 getRight() const
	{
		return position.x + width;
	}

	real32 getTop() const
	{
		return position.y;
	}

	real32 getBottom() const
	{
		return position.y + height;
	}

	inline Vector2f getCenter() const
	{
		return Vector2f(position.x + width/2, position.y + height/2);
	}


	inline void render(SDL_Renderer* renderer)
	{
		renderTexture(renderer, texture, &sprite_rect, &dest_rect);
	}

	void update(real32 time_delta)
	{
		if (doesMove)
		{
			if(position.y >= orgPosition.y - 200)
			{
				velocity.y -= 5;
			}
			else
			{
				velocity.y += 5;
			}
			move(velocity.x * time_delta, velocity.y * time_delta);
		}
	}

	/// Update the texture and properties according to the relative location to other solids
	void prepare(Level * level) ;

	bool operator==(const Solid& b) const
	{
		return this->position == b.position;
	}

	real32 xRemainder = 0;
	real32 yRemainder = 0;
	bool collidable;
	Vector2f orgPosition = {0, 0};
	Vector2f position = {0, 0};
	Vector2f velocity = {0, 0};
	real32 width = 0;
	real32 height = 0;
	SDL_Rect sprite_rect;
	SDL_FRect dest_rect;
	bool doesMove = false;
	bool breakable = false;
	SDL_Texture* texture = nullptr;
};

struct ActorTexture {
	SDL_Texture* texture = nullptr;
	SDL_Point size = {};
};

enum class TextureType {
	Idle, Swim, Puffing
};

class Actor
{
public:
	virtual void moveX(real32 amount, std::function<void()> on_collide = nullptr);
	virtual void moveY(real32 amount, std::function<void()> on_collide = nullptr);
	virtual void update(real32 time_delta, const ControllerInput* input);
	virtual void hurt(Actor* hurter, int32 damage=1);
	virtual void die();

	real32 xRemainder = 0;
	real32 yRemainder = 0;
	Vector2f position = {0, 0};
	Vector2f velocity = {0, 0};
	real32 width = 0;
	real32 height = 0;
	Rect2f hitRect = {0, 0, 0, 0};
	std::unordered_map<TextureType, ActorTexture> textures;
	bool visible = true;
	Direction facing = Direction::Right;
	uint32 currentFrame = 0;
	ActorTexture* currentTexture = nullptr;
	real32 lastAnimationTime = 0;
	real32 lastSwimSoundTime = 0;
	real32 angle = 0;
	bool isPlayer = false;
	bool isPuffed = false;
	real32 accConst = 0;
	real32 velocityLimit = 0;
	bool goingSlow = false;
	int health;
	int maxHealth;
	real32 dyingTime = 0;
	bool isDead = false;
	bool diesOnImpact = false;
	bool noClip = false;
	real32 movingAnimationDelay = 0.1f;
	real32 idleAnimationDelay = 0.8f;
	int32 puffingFrames = 0;

	virtual void swimSound()
	{

	}

	bool isDying() {
		return isDead || dyingTime;
	}

	virtual Rect2f getHitbox() const
	{
		return {hitRect.x + position.x, hitRect.y + position.y, hitRect.w, hitRect.h};
	}

	virtual Rect2f getHitbox(Vector2f customPos) const
	{
		return {hitRect.x + customPos.x, hitRect.y + customPos.y, hitRect.w, hitRect.h};
	}

	virtual void render(SDL_Renderer* renderer)
	{
		if (visible)
		{
			const SDL_Rect sprite_rect = {static_cast<int>(currentFrame * width), 0, static_cast<int>(width), static_cast<int>(height)};
			const SDL_FRect rect = {position.x, position.y, width, height};
			SDL_FPoint center(width/2, height/2);
			renderTextureEx(renderer, currentTexture->texture, &sprite_rect, &rect, angle, &center, facing == Right ? SDL_FLIP_NONE : SDL_FLIP_HORIZONTAL);
		}
	}

	const Solid* collideAt(const std::vector<Solid>& solids, Vector2f position) const;
	
	inline real32 getLeft() const
	{
		return position.x;
	}

	inline real32 getRight() const
	{
		return position.x + width;
	}

	inline real32 getTop() const
	{
		return position.y;
	}

	inline real32 getBottom() const
	{
		return position.y + height;
	}

	inline Vector2f getCenter() const
	{
		return Vector2f(position.x + width/2, position.y + height/2);
	}

	inline bool collidesWith(const Actor* actor) const
	{
		return actor->visible && this->visible && getHitbox().collides(actor->getHitbox());
	}

	void setTexture(SDL_Texture* texture, TextureType type)
	{
		ActorTexture actorTexture;
		actorTexture.texture = texture;
		SDL_QueryTexture(texture, NULL, NULL, &actorTexture.size.x, &actorTexture.size.y);
		this->textures[type] = actorTexture;

		if (type == TextureType::Idle) {
			this->currentTexture = &this->textures[type];
			this->currentFrame = 0;
		}
	}
};

class Player : public Actor
{
public:
	Player()
	{
		isPlayer = true;
		width = normalSize.x;
		height = normalSize.y;
		maxHealth = 3;
		health = 3;
		this->setTexture(player_texture_puffing, TextureType::Puffing);
		
		// acc_const = 700.f * 6.f;
		accConst = 600.f * 6.f;
		velocityLimit = 1000.f * 6.f;

		hitRects[0] = Rect2f(27/2, 0, 184/2, 91/2);
		hitRects[1] = Rect2f(218, 218, 222, 222);
		hitRects[2] = Rect2f(105, 105, 435, 417);
		hitRects[3] = Rect2f(77, 74, 499, 487);
	}

	bool tryHitRectChange(Vector2f deltaPos, const Rect2f& newHitRect);
	void puffUp();
	void puffDown()
	{
		puffingTime = 3*puffingTimeStep;
		puffingFrames = -2;
		hitRect = hitRects[2];
		setTexture(player_texture_puffing, TextureType::Idle);
		setTexture(player_texture_puffing, TextureType::Swim);
		currentFrame = 1;
		playSound(deflate_sound);
		resetPuffCooldown();
	}

	inline void resetPuffCooldown() {
		puffCooldown = puffingFrames < 0 ? puffMaxCooldown : puffMaxCooldown/2;
	}

	void swimSound() override
	{
		// if (isPuffed)
		// {
		// 	playSound(step);
		// }
		// else
		// {
		// 	playSound(mini_step);
		// }
	}

	virtual void update(real32 time_delta, const ControllerInput* input) override {
		Actor::update(time_delta, input);

		if (invulTime > 0) {			
			invulTime = MAX(invulTime - time_delta, 0);
			if (invulTime == 0) {
				visible = true;
			}
			else {
				visible = !visible;
			}
		}

		if (puffingTime > 0) {
			puffingTime = MAX(puffingTime - time_delta, 0);
		}
		
		if (puffingFrames > 0) {
			// Puffing
			this->currentTexture = &this->textures[TextureType::Puffing];
			switch(puffingFrames) {
				case 2:
					this->currentFrame = 0;
					break;
				case 1:
					this->currentFrame = 1;
					break;
			}

			if (puffingTime <= puffingFrames * puffingTimeStep) {
				puffingFrames--;
				Rect2f& newHitRect = hitRects[3 - puffingFrames];
				if (!this->tryHitRectChange({0, 0}, newHitRect)) {
					puffingFrames = -1;
					puffingTime = 2*puffingTimeStep;
					this->currentFrame = 0;
				}
				else {
					if (puffingFrames == 0) {
						isPuffed = true;
						setTexture(player_texture_puffed_idle, TextureType::Idle);
						setTexture(player_texture_puffed_swim, TextureType::Swim);
					}
				}
			}
		}
		else if(puffingFrames < 0) {
			// Unpuffing
			this->currentTexture = &this->textures[TextureType::Puffing];
			switch(puffingFrames) {
				case -2:
					this->currentFrame = 1;
					break;
				case -1:
					this->currentFrame = 0;
					break;
			}

			if (puffingTime <= -puffingFrames * puffingTimeStep) {
				puffingFrames++;
				hitRect = hitRects[-puffingFrames];
				if (puffingFrames == 0) {
					isPuffed = false;
					width = normalSize.x;
					height = normalSize.y;
					position += puffOffset;
					setTexture(player_texture_normal_idle, TextureType::Idle);
					setTexture(player_texture_normal_swim, TextureType::Swim);
				}
			}
		}
		else if (!isDying() && input->button_a && puffCooldown <= 0) {
			isPuffed ? puffDown() : puffUp();
		}

		if (puffCooldown > 0) {
			puffCooldown = MAX(puffCooldown - time_delta, 0);
		}
	}

	virtual void hurt(Actor* hurter, int32 damage=1) override;
	virtual void die() override;

	real32 puffingTime = 0;
	real32 puffCooldown = 0;
	real32 invulTime = 0;
	Rect2f hitRects[4];
	bool inButt = false;
	const real32 puffMaxCooldown = 2.0f;
	const real32 puffingTimeStep = 0.05f;
	const SDL_Point normalSize = {230/2, 91/2};
	const SDL_Point puffedSize = {639, 643};
	const Vector2f puffOffset = {236, 294};
};

class Decor : public Actor
{
	public:
	Decor(Vector2f startPos, Vector2f size, SDL_Texture * texture)
	{
		width = size.x;
		height = size.y;
		startPos.x -= width/2;
		startPos.y -= height - 60;
		position = startPos;
		hitRect = {0, 0, width, height};
		idleAnimationDelay = 0.1f;

		setTexture(texture, TextureType::Idle);
	}

	virtual void update(real32 time_delta, const ControllerInput* input) override;
};

class Key : public Actor
{
	public:
	Key()
	{
		width = 120;
		height = 120;
		hitRect = {0, 0, width, height};
		velocityLimit = 2000;
		accConst = 500.f * 6.f;
		noClip = true;

		setTexture(key_texture, TextureType::Idle);
	}

	void setStartPos(Vector2f startPos) {
		startPos.x -= width/2;
		startPos.y -= height/2;
		spawnPoint = position = startPos;
	}

	// void render(SDL_Renderer* renderer) override;
	virtual void update(real32 time_delta, const ControllerInput* input) override;

	real32 bobTimer = 0;
	Vector2f spawnPoint;
	Player * holder = nullptr;
	ControllerInput input;
};

class Door : public Actor
{
	public:
	Door()
	{
		width = 360;
		height = 360;
		hitRect = {0, 0, width, height};

		setTexture(door_texture, TextureType::Idle);
	}

	void setStartPos(Vector2f startPos) {
		startPos.x -= width/2;
		startPos.y -= height/2;
		position = startPos;
	}

	virtual void update(real32 time_delta, const ControllerInput* input) override;
};

class Button : public Actor
{
	public:
	Button(Vector2f startPos, bool isInverted): isInverted(isInverted)
	{
		width = 360;
		height = 60;
		hitRect = {0, 0, width, height};
		setStartPos(startPos);
		sprite_rect = {static_cast<int>(currentFrame * width), 0, static_cast<int>(width), static_cast<int>(height)};
		dest_rect = {position.x, position.y, width, height};

		setTexture(button_unpressed_texture, TextureType::Idle);
	}

	void setStartPos(Vector2f startPos) {
		startPos.x -= width/2;
		position = startPos;
	}

	void render(SDL_Renderer* renderer) override {
		renderTextureEx(renderer, currentTexture->texture, &sprite_rect, &dest_rect, 0, NULL, isInverted ? SDL_FLIP_VERTICAL : SDL_FLIP_NONE);
	}
	virtual void update(real32 time_delta, const ControllerInput* input) override;

	bool isPressed = false;
	bool isInverted = false;
	SDL_Rect sprite_rect;
	SDL_FRect dest_rect;
};

class Heart : public Actor
{
	public:
	Heart()
	{
		width = 100;
		height = 100;
		hitRect = {0, 0, width, height};
		visible = false;

		setTexture(heart_texture, TextureType::Idle);
	}

	void setStartPos(Vector2f startPos) {
		startPos.x -= width/2;
		startPos.y -= height/2;
		spawnPoint = position = startPos;
	}

	virtual void update(real32 time_delta, const ControllerInput* input) override;

	real32 bobTimer = 0;
	Vector2f spawnPoint;
};

class Grampa : public Actor
{
public:
	Grampa()
	{
		width = 319.f/2.f;
		height = 304.f/2.f;
		hitRect = {0, 0, width, height};
		idleAnimationDelay = 0.4f;

		setTexture(grampa_texture, TextureType::Idle);

		messages[0].push_back("Your adventure begins here, little fish");
		messages[0].push_back("Build up speed and puff up on the way");
		messages[0].push_back("Puff up at the right time\nand you can even reflect bubbles");
		messages[0].push_back("Now go and save the ocean!");

		messages[3].push_back("You have come a long way");
		messages[3].push_back("Forget not!");
		messages[3].push_back("Reflect the bubble\nand infiltrate the hole");
		messages[3].push_back("You must destroy the evil\nfrom the inside");
		messages[3].push_back("It is the only way...");
	}

	void setStartPos(Vector2f startPos) {
		startPos.x -= width/2;
		startPos.y -= height/2;
		spawnPoint = position = startPos;
	}

	void render(SDL_Renderer* renderer) override;
	virtual void update(real32 time_delta, const ControllerInput* input) override;

	bool seesPlayer = false;
	Vector2f spawnPoint;
	real32 bobTimer = 0;
	int32 grampaState = 0;
	int32 currentLine = 0;
	real32 speechTimer = 0;
	std::array<std::vector<std::string>, 4> messages;
};

enum class DiagDir {
	TopLeft, TopRight, BotLeft, BotRight
};

class Diagonal : public Actor
{
public:
	Diagonal(Vector2f startPos, DiagDir direction): direction(direction)
	{
		width = 480;
		height = 480;
		hitRect = {0, 0, width, height};

		switch (direction)
		{
		case DiagDir::TopLeft:
			flip = SDL_FLIP_NONE;
			p1 = {startPos.x, startPos.y + height};
			p2 = {startPos.x + width, startPos.y};
			normal = Vector2f(1, 1).getNormalized();
			break;
		case DiagDir::TopRight:
			flip = SDL_FLIP_HORIZONTAL;
			startPos.x -= width - 60;
			p1 = {startPos.x, startPos.y};
			p2 = {startPos.x + width, startPos.y + height};
			normal = Vector2f(-1, 1).getNormalized();
			break;
		case DiagDir::BotLeft:
			flip = SDL_FLIP_VERTICAL;
			startPos.y -= height - 60;
			p1 = {startPos.x, startPos.y + height};
			p2 = {startPos.x + width, startPos.y};
			normal = Vector2f(1, -1).getNormalized();
			break;
		default:
			flip = (SDL_RendererFlip)(SDL_FLIP_HORIZONTAL | SDL_FLIP_VERTICAL);
			startPos.y -= height - 60;
			startPos.x -= width - 60;
			p1 = {startPos.x, startPos.y + height};
			p2 = {startPos.x + width, startPos.y};
			normal = Vector2f(-1, -1).getNormalized();
			break;
		}
		position = startPos;
		sprite_rect = {0, 0, (int)width, (int)height};
		dest_rect = {position.x, position.y, width, height};

		setTexture(diagonal_texture, TextureType::Idle);
	}

	void render(SDL_Renderer* renderer) override;
	virtual void update(real32 time_delta, const ControllerInput* input) override;

	DiagDir direction;
	SDL_Rect sprite_rect;
	SDL_FRect dest_rect;
	SDL_RendererFlip flip;
	Vector2f p1;
	Vector2f p2;
	Vector2f normal;
};

class Enemy : public Actor
{
public:
	Enemy(Vector2f startPos, const real32 startWidth, const real32 startHeight) {
		width = startWidth;
		height = startHeight;
		startPos.x -= width/2;
		startPos.y -= height/2;
		
		spawnPoint = position = startPos;
	}

	Vector2f spawnPoint;
	real64 lastIdeaTime = 0;
	ControllerInput input;

	virtual void think(real32 time_delta) = 0;
	virtual void die() override;
};

class EnemyFish : public Enemy
{
public:
	EnemyFish(Vector2f startPos) : Enemy(startPos, 742, 444)
	{
		position.x += 100;
		hitRect = {150, 42, 468, 374};
		
		accConst = 550.f * 6.f;
		velocityLimit = 2000.f * 6.f;
		health = 5;
		maxHealth = 5;

		setTexture(enemy_fish_texture_idle, TextureType::Idle);
		setTexture(enemy_fish_texture_swim, TextureType::Swim);
	}

	bool chasingPlayer = false;
	bool seesPlayer = false;

	virtual void think(real32 time_delta) override;
	void chase(Vector2f target, ControllerInput &input);
	void render(SDL_Renderer* renderer) override;
	virtual void update(real32 time_delta, const ControllerInput* input) override;
};

class EnemyJelly : public Enemy
{
public:
	EnemyJelly(Vector2f startPos) : Enemy(startPos, 394, 620)
	{
		// hitRect = {67, 48, 270, 525};
		hitRect = {107, 47, 215, 490};
		
		accConst = 100.f * 6.f;
		velocityLimit = 100.f * 6.f;
		health = 1;
		maxHealth = 1;
		idleAnimationDelay = 0.075f;

		setTexture(enemy_jellyfish_texture_idle, TextureType::Idle);
	}

	real32 bobTimer = 0;

	virtual void think(real32 time_delta) override {};
	virtual void update(real32 time_delta, const ControllerInput* input) override;
};

class EnemyShrimp : public Enemy
{
public:
	EnemyShrimp(Vector2f startPos, bool isInverted) : Enemy(startPos, 353, 445), isInverted(isInverted)
	{
		accConst = 100.f * 6.f;
		velocityLimit = 100.f * 6.f;
		health = 1;
		maxHealth = 1;

		if (isInverted) {
			hitRect = {67, 0, 203, 153};
			claw_offset = {189, 148};
		}
		else {
			hitRect = {78, 293, 193, 143};
			claw_offset = {209, 305};
		}

		setTexture(enemy_shrimp_texture_main, TextureType::Idle);
		
		textureClaw = enemy_shrimp_texture_claw;
		SDL_QueryTexture(textureClaw, NULL, NULL, &textureClawSize.x, &textureClawSize.y);
		textureClawAttack = enemy_shrimp_texture_claw_attack;
		SDL_QueryTexture(textureClawAttack, NULL, NULL, &textureClawAttackSize.x, &textureClawAttackSize.y);
		
		currentClawTexture = textureClaw;
		currentClawTextureSize = textureClawSize;
	}

	virtual void think(real32 time_delta) override;
	void render(SDL_Renderer* renderer) override;
	virtual void update(real32 time_delta, const ControllerInput* input) override;

	SDL_Texture* textureClaw;
	SDL_Point textureClawSize = {0, 0};
	SDL_Texture* textureClawAttack;
	SDL_Point textureClawAttackSize = {0, 0};
	SDL_Texture* currentClawTexture;
	SDL_Point currentClawTextureSize = {0, 0};
	uint32 currentClawFrame = 0;
	real32 lastClawAnimationTime = 0;
	bool targetingPlayer = false;
	bool vigilant = false;
	real32 waveTimer = 0;
	real32 shootCooldown = 1.8f;
	const real32 shootPeriod = 1.8f;
	SDL_FPoint claw_offset;
	bool isInverted = false;
};

class EnemyBubble : public Enemy
{
public:
	EnemyBubble(Vector2f startPos, Vector2f target, Enemy* creator, real32 speed=2400.f, bool isBig=false, real32 lifespan=1.5f)
	: Enemy(startPos, isBig ? 267*3 : 267, isBig ? 203*3 : 203), isBig(isBig), lifespan(lifespan), creator(creator)
	{
		if (isBig) {
			hitRect = {69, 72, 552, 477};
		}
		else {
			hitRect = {23, 24, 184, 159};
		}
		
		accConst = 100.f * 6.f;
		velocityLimit = speed;
		health = 1;
		maxHealth = 1;
		diesOnImpact = true;
		
		velocity = target * velocityLimit;

		setTexture(isBig ? enemy_bubble_big_texture : enemy_bubble_texture, TextureType::Idle);
	}

	virtual void think(real32 time_delta) override {};
	virtual void update(real32 time_delta, const ControllerInput* input) override;

	real32 lifespan;
	bool isBig;
	bool bounced = false;
	Enemy* creator;
};

enum class BossState {
	Waiting, Idle, Bubbles, BigBubble, Sweep, Stunned, Hurt
};

enum class SweepState {
	Windup, BeforeSlash, Slash, AfterSlash, Bringback
};

enum class BigBubbleState {
	Windup, Shoot
};

class EnemyBoss : public Enemy
{
public:
	EnemyBoss(Vector2f startPos) : Enemy(startPos, 1800, 1800)
	{
		hitRect = {272, 639, 1369, 899};
		
		accConst = 100.f * 6.f;
		velocityLimit = 100.f * 6.f;
		health = 30;
		maxHealth = 30;

		setTexture(enemy_boss_texture_main_normal, TextureType::Idle);
		
		textureClaw = enemy_boss_texture_claw_normal;
		SDL_QueryTexture(textureClaw, NULL, NULL, &textureClawSize.x, &textureClawSize.y);
		textureSmallclaw = enemy_boss_texture_smallclaw_normal;
		SDL_QueryTexture(textureSmallclaw, NULL, NULL, &textureSmallclawSize.x, &textureSmallclawSize.y);
		textureMainStunned = enemy_boss_texture_main_crouched;
		SDL_QueryTexture(textureMainStunned, NULL, NULL, &textureMainStunnedSize.x, &textureMainStunnedSize.y);
	}

	virtual void think(real32 time_delta) override;
	void render(SDL_Renderer* renderer) override;
	virtual void update(real32 time_delta, const ControllerInput* input) override;
	void shootBubbles(real32 time_delta);
	void sweepAttack(real32 time_delta);
	void bigBubbleAttack(real32 time_delta);
	void changeState(BossState newState);
	virtual void die() override;

	SDL_Texture* textureClaw;
	SDL_Point textureClawSize = {0, 0};
	SDL_Texture* textureSmallclaw;
	SDL_Point textureSmallclawSize = {0, 0};
	SDL_Texture* textureMainStunned;
	SDL_Point textureMainStunnedSize = {0, 0};
	bool active = false;
	real32 shootCooldown = 1.f;
	real32 idleDelay = 0;
	real32 lastStateTime = 0;
	real32 lastSweepStateTime = 0;
	real32 lastBBStateTime = 0;
	int32 cycleCount = 0;
	int32 bubbleShootCount = 0;
	real32 clawRotationSpeed = 0;
	real32 clawAngle = -30;
	real32 clawAngleWave = 0;
	real32 clawPosYWave = 0;
	real32 smallclawAngle = 0;
	int32 stun_frame = 0;
	int32 clawFrame = 0;
	std::array<Rect2f, 3> clawHitRects = {Rect2f(673, 215, 548, 570), Rect2f(699, 702, 441, 305), Rect2f(765, 1014, 298, 260)};
	Rect2f buttRect = {129, 929, 434, 388};
	const real32 shootPeriod = 1.f;
	const SDL_FPoint claw_normal_offset = {-63, -493};
	const SDL_FPoint claw_joint_offset = {898, 1464};
	const SDL_FPoint smallclaw_joint_offset = {1220, 1279};
	Vector2f mouthOffset = {864, 1073};
	BossState bossState = BossState::Waiting;
	SweepState sweepState = SweepState::Windup;
	BigBubbleState bbState = BigBubbleState::Windup;
};

enum class EnemyType
{
	Fish, Shrimp, Jellyfish, Boss, ShrimpInverted
};

struct EnemySpawner 
{
	Vector2f spawnPoint;
	EnemyType enemyType;
};

struct DecorSpawner {
	Vector2f spawnPoint;
	Vector2f size;
	SDL_Texture * texture;
};

struct DiagSpawner {
	Vector2f spawnPoint;
	DiagDir direction;
};

struct ButtonSpawner {
	Vector2f spawnPoint;
	bool isInverted;
};

struct Level
{
	std::vector<Solid> solids;
	std::unordered_map<Vector2f, Solid*> solidMap;
	Vector2f playerStart;
	Vector2f keyStart;
	Vector2f doorStart;
	Vector2f heartStart;
	Vector2f grampaStart;
	std::vector<EnemySpawner> enemySpawners;
	std::vector<DecorSpawner> decorSpawners;
	std::vector<DiagSpawner> diagSpawners;
	std::vector<ButtonSpawner> buttonSpawners;
	uint32 width;
	uint32 height;
	bool heartTaken = false;

	void addSolid(Solid solid, int32 i, int32 j) {
		solids.emplace_back(solid);
		solidMap[{(float)(i * LEVEL_SCALE), (float)(j * LEVEL_SCALE)}] = &solids[solids.size()-1];
	}

	void load(const std::string& levelFilename)
	{
		SDL_Surface* surface = IMG_Load(("assets/" + levelFilename).c_str());
		if (!surface)
		{
			LogError("Failed to load level file: %s", levelFilename.c_str());
			return;
		}

		LogInfo("W: %d, H: %d\n", surface->w, surface->h);

		this->width = surface->w * LEVEL_SCALE;
		this->height = surface->h * LEVEL_SCALE;
		for (int i = 0; i < surface->w; i++)
		{
			// printf("\n");
			for (int j = 0; j < surface->h; j++)
			{
				uint32 p = getPixel(surface, i, j);
				// printf("%x", p);

				// Tiles
				if (p == 0xff000000)
				{
					addSolid(Solid({static_cast<float>(i * LEVEL_SCALE), static_cast<float>(j * LEVEL_SCALE)}, LEVEL_SCALE, LEVEL_SCALE, tile1_texture_top), i, j);
				}
				else if (p == 0xff808080)
				{
					addSolid(Solid({static_cast<float>(i * LEVEL_SCALE), static_cast<float>(j * LEVEL_SCALE)}, LEVEL_SCALE, LEVEL_SCALE, tile1_texture_mid), i, j);
				}
				else if (p == 0xff00337F)
				{
					addSolid(Solid({static_cast<float>(i * LEVEL_SCALE), static_cast<float>(j * LEVEL_SCALE)}, LEVEL_SCALE, LEVEL_SCALE, tile1_texture_breakable, true, true), i, j);
				}

				// Actors
				else if (p == 0xffff0000)
				{
					playerStart = {static_cast<float>(i * LEVEL_SCALE), static_cast<float>(j * LEVEL_SCALE)};
				}
				else if (p == 0xff0000ff)
				{
					enemySpawners.emplace_back(Vector2f(i * LEVEL_SCALE, j * LEVEL_SCALE), EnemyType::Fish);
				}
				else if (p == 0xffdc00ff)
				{
					enemySpawners.emplace_back(Vector2f(i * LEVEL_SCALE, j * LEVEL_SCALE), EnemyType::Jellyfish);
				}
				else if (p == 0xffffff00)
				{
					enemySpawners.emplace_back(Vector2f(i * LEVEL_SCALE, j * LEVEL_SCALE), EnemyType::Shrimp);
				}
				else if (p == 0xff898900)
				{
					enemySpawners.emplace_back(Vector2f(i * LEVEL_SCALE, j * LEVEL_SCALE), EnemyType::ShrimpInverted);
				}
				else if (p == 0xffFF007F)
				{
					enemySpawners.emplace_back(Vector2f(i * LEVEL_SCALE, j * LEVEL_SCALE), EnemyType::Boss);
				}
				else if (p == 0xff5BFCFF)
				{
					keyStart = {static_cast<float>(i * LEVEL_SCALE), static_cast<float>(j * LEVEL_SCALE)};
				}
				else if (p == 0xffC5FFAA)
				{
					doorStart = {static_cast<float>(i * LEVEL_SCALE), static_cast<float>(j * LEVEL_SCALE)};
				}
				else if (p == 0xff99BCFF)
				{
					buttonSpawners.emplace_back(Vector2f(static_cast<float>(i * LEVEL_SCALE), static_cast<float>(j * LEVEL_SCALE)), false);
				}
				else if (p == 0xff63B6FF)
				{
					buttonSpawners.emplace_back(Vector2f(static_cast<float>(i * LEVEL_SCALE), static_cast<float>(j * LEVEL_SCALE)), true);
				}
				else if (p == 0xffDAD6FF)
				{
					heartStart = {static_cast<float>(i * LEVEL_SCALE), static_cast<float>(j * LEVEL_SCALE)};
				}
				else if (p == 0xff63607C)
				{
					grampaStart = {static_cast<float>(i * LEVEL_SCALE), static_cast<float>(j * LEVEL_SCALE)};
				}

				// Decors
				else if (p == 0xff00FF7F)
				{
					decorSpawners.emplace_back(Vector2f(i * LEVEL_SCALE, j * LEVEL_SCALE), Vector2f(320, 747), decor_texture_seaweed);
				}
				else if (p == 0xff7FE9FF)
				{
					decorSpawners.emplace_back(Vector2f(i * LEVEL_SCALE, j * LEVEL_SCALE), Vector2f(170, 188), decor_texture_coral1);
				}
				else if (p == 0xff32d6ff)
				{
					decorSpawners.emplace_back(Vector2f(i * LEVEL_SCALE, j * LEVEL_SCALE), Vector2f(297, 368), decor_texture_coral2);
				}
				else if (p == 0xffffd1e1)
				{
					decorSpawners.emplace_back(Vector2f(i * LEVEL_SCALE, j * LEVEL_SCALE), Vector2f(895, 319), decor_texture_rock1);
				}
				else if (p == 0xffffb2ef)
				{
					decorSpawners.emplace_back(Vector2f(i * LEVEL_SCALE, j * LEVEL_SCALE), Vector2f(455, 273), decor_texture_rock2);
				}
				else if (p == 0xffdabaff)
				{
					decorSpawners.emplace_back(Vector2f(i * LEVEL_SCALE, j * LEVEL_SCALE), Vector2f(576, 590), decor_texture_rock3);
				}
				else if (p == 0xffC1FFBF)
				{
					decorSpawners.emplace_back(Vector2f(i * LEVEL_SCALE, j * LEVEL_SCALE), Vector2f(94, 124), decor_texture_arrow_up);
				}
				else if (p == 0xff70FF96)
				{
					decorSpawners.emplace_back(Vector2f(i * LEVEL_SCALE, j * LEVEL_SCALE), Vector2f(124, 124), decor_texture_arrow_up_right);
				}
				else if (p == 0xff88FF51)
				{
					decorSpawners.emplace_back(Vector2f(i * LEVEL_SCALE, j * LEVEL_SCALE), Vector2f(124, 124), decor_texture_arrow_down_right);
				}

				// Diagonal
				else if (p == 0xff0077FF)
				{
					diagSpawners.emplace_back(Vector2f(i * LEVEL_SCALE, j * LEVEL_SCALE), DiagDir::TopRight);
				}
				else if (p == 0xff0067FF)
				{
					diagSpawners.emplace_back(Vector2f(i * LEVEL_SCALE, j * LEVEL_SCALE), DiagDir::TopLeft);
				}
				else if (p == 0xff0057FF)
				{
					diagSpawners.emplace_back(Vector2f(i * LEVEL_SCALE, j * LEVEL_SCALE), DiagDir::BotRight);
				}
				else if (p == 0xff0047FF)
				{
					diagSpawners.emplace_back(Vector2f(i * LEVEL_SCALE, j * LEVEL_SCALE), DiagDir::BotLeft);
				}
			}
		}

		for (Solid& solid : solids) {
			solid.prepare(this);
		}
	}

	bool checkSolid(const Vector2f& pos) {
		if (pos.x < 0 || pos.x >= width || pos.y < 0 || pos.y >= height) {
			return true;
		}
		return solidMap.find(pos) != solidMap.end();
	}
};

struct GameState
{
	std::vector<Actor*> AllActors;

	Player player;
	Key key;
	Door door;
	Heart heart;
	Grampa grampa;
	std::vector<std::unique_ptr<Enemy>> enemies;
	std::vector<std::unique_ptr<Decor>> decors;
	std::vector<std::unique_ptr<Diagonal>> diagonals;
	std::vector<std::unique_ptr<Button>> buttons;

	std::vector<std::unique_ptr<Enemy>> newEnemiesQueue;
	Level* currentLevel;
	Level levels[4];
	State current_state = MainMenu;
	Rect2f camera = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};

	uint32 dead_frames = 0;
	uint32 controls_frames = 0;
	uint32 playing_frames = 0;
	uint32 gameover_frames = 0;
	uint32 shaking_frames = 0;
	uint32 main_menu_frames = 0;
	uint32 beginning_frames = 0;
	uint32 victory_frames = 0;
	real32 boss_entrance_time = 0;
	real32 ending_time = 0;
	int32 boss_brick_state = 0;
	const int32 boss_brick_total = 6;
	bool shaking_for_dead = false;
	real64 play_time_passed = 0;
	bool heartPopped = false;
	bool bossStarted = false;
	EnemyBoss* boss;

	void reset()
	{
		heartPopped = false;

		player.setTexture(player_texture_normal_idle, TextureType::Idle);
		player.setTexture(player_texture_normal_swim, TextureType::Swim);
		player.hitRect = player.hitRects[0];
		player.width = player.normalSize.x;
		player.height = player.normalSize.y;
		player.position = currentLevel->playerStart;
		player.velocity = { 0, 0 };
		player.visible = true;
		player.health = player.maxHealth;
		player.angle = 0;
		player.isDead = false;
		player.isPuffed = false;

		grampa.currentLine = 0;
		grampa.grampaState = 0;

		bossStarted = false;
		boss_brick_state = 0;
		boss_entrance_time = 0;
		if (currentLevel == levels + 3) {
			levels[3] = Level();
			levels[3].load("level4.png");
		}

		key.setStartPos(currentLevel->keyStart);
		if (currentLevel->keyStart.isZero()) {
			key.visible = false;
		}
		key.holder = nullptr;
		key.velocity = { 0, 0 };
		key.angle = 0;
		door.setStartPos(currentLevel->doorStart);
		if (currentLevel->doorStart.isZero()) {
			door.visible = false;
		}
		grampa.setStartPos(currentLevel->grampaStart);
		if (currentLevel->grampaStart.isZero()) {
			grampa.visible = false;
		}
		else {
			grampa.visible = true;
		}
		heart.setStartPos(currentLevel->heartStart);

		enemies.clear();
		decors.clear();
		diagonals.clear();
		buttons.clear();
		AllActors.clear();
		//todo replace breakable blocks?

		for (ButtonSpawner& spawner : currentLevel->buttonSpawners) {
			buttons.push_back(std::make_unique<Button>(spawner.spawnPoint, spawner.isInverted));
			AllActors.push_back(buttons[buttons.size()-1].get());
		}
		
		for (DecorSpawner& spawner : currentLevel->decorSpawners) {
			decors.push_back(std::make_unique<Decor>(spawner.spawnPoint, spawner.size, spawner.texture));
			AllActors.push_back(decors[decors.size()-1].get());
		}
		
		for (DiagSpawner& spawner : currentLevel->diagSpawners) {
			diagonals.push_back(std::make_unique<Diagonal>(spawner.spawnPoint, spawner.direction));
			AllActors.push_back(diagonals[diagonals.size()-1].get());
		}

		for (EnemySpawner& spawner : currentLevel->enemySpawners) {
			std::unique_ptr<Enemy> newEnemy;
			switch (spawner.enemyType)
			{
			case EnemyType::Fish:
				newEnemy = std::make_unique<EnemyFish>(spawner.spawnPoint);
				break;
			case EnemyType::Shrimp:
				newEnemy = std::make_unique<EnemyShrimp>(spawner.spawnPoint, false);
				break;
			case EnemyType::Jellyfish:
				newEnemy = std::make_unique<EnemyJelly>(spawner.spawnPoint);
				break;
			case EnemyType::ShrimpInverted:
				newEnemy = std::make_unique<EnemyShrimp>(spawner.spawnPoint, true);
				break;
			default:
				newEnemy = std::make_unique<EnemyBoss>(spawner.spawnPoint);
				boss = (EnemyBoss*)newEnemy.get();
				break;
			}
			enemies.push_back(std::move(newEnemy));
 			AllActors.push_back(enemies[enemies.size()-1].get());
		}
		
		AllActors.push_back(&key);
		AllActors.push_back(&door);
		AllActors.push_back(&player);
		AllActors.push_back(&heart);
		AllActors.push_back(&grampa);
	}

	GameState()
	{
		levels[0].load("level1.png");
		levels[1].load("level2.png");
		levels[2].load("level3.png");
		levels[3].load("level4.png");

		currentLevel = &levels[0];
		reset();
	}
};

extern GameState* state;

void Enemy::die() {
	Actor::die();
}

void Player::die() {
	Actor::die();

	changeCurrentState(Dead);
}

void Actor::die() {
	visible = false;
	isDead = true;
}

void Actor::hurt(Actor* hurter, const int32 damage) {
	health -= damage;
	if (health <= 0) {
		dyingTime = 1.0f;
	}
	// Knockback
	if (!dynamic_cast<EnemyBoss*>(this)) {
		const real32 knockAmount = 1500.0f;
		const Vector2f knockDir = (getCenter() - hurter->getCenter()).getNormalized();
		velocity = knockDir * knockAmount;
	}
}

void Player::hurt(Actor* hurter, const int32 damage) {
	Actor::hurt(hurter, damage);
	if (!isDying()) {
		invulTime = 1.75f;
	}
	changeCurrentState(Shaking);
	playSound(playerHurt);
}

void Actor::update(real32 time_delta, const ControllerInput* input)
{
	if (dyingTime > 0) {
		dyingTime -= time_delta;
		if (dyingTime <= 0) {
			dyingTime = 0;
			die();
		}
	}

	// const real32 water_friction = 0.7f;
	const real32 water_friction = 0.6f;
	const real32 velocity_deadzone = 3.f * 6.f;

	// Control
	real32 move_x = 0;
	real32 move_y = 0;
	if (input && !isPuffed && !isDying()) {
		move_x = input->dir_right - input->dir_left;
		move_y = input->dir_down - input->dir_up;
	}

	Vector2f acceleration = {move_x * accConst, move_y * accConst};
	if (goingSlow) {
		acceleration /= 4.0f;
	}

	velocity += acceleration * time_delta;

	velocity *= pow(1 - water_friction, time_delta);

	// Deadzone for velocity
	if (move_x == 0 && move_y == 0 && velocity.getMagnitude() < velocity_deadzone)
	{
		velocity.x = 0;
		velocity.y = 0;
	}

	// Top limit for velocity
	if (velocity.y < -velocityLimit)
	{
		velocity.y = -velocityLimit;
	}
	else if (velocity.y > velocityLimit)
	{
		velocity.y = velocityLimit;
	}
	else if (velocity.x < -velocityLimit)
	{
		velocity.x = -velocityLimit;
	}
	else if (velocity.x > velocityLimit)
	{
		velocity.x = velocityLimit;
	}

	// Set facing direction
	if (velocity.x > 0)
	{
		facing = Right;
	}
	else if (velocity.x < 0)
	{
		facing = Left;
	}

	real32 velocityMag = velocity.getMagnitude();

	// Set angle (shrimp is a special case)
	if (!dynamic_cast<EnemyShrimp*>(this)) {
		if (velocityMag > 0) {
			if (isPuffed) {
				angle = fmod(angle + time_delta * velocityMag / 6.f, 360.0);
			}
			else {
				Vector2f normVelocity = velocity.getNormalized();
				const real32 oldAngle = angle;
				angle = atan2(normVelocity.y, normVelocity.x) * (180.0 / Pi32);

				// const bool oldIsHorizontal = (oldAngle < 45 && oldAngle > -45) || (oldAngle > 135 || oldAngle < -135);
				// const bool newIsHorizontal = (angle < 45 && angle > -45) || (angle > 135 || angle < -135);
				// if (oldIsHorizontal != newIsHorizontal) {

				// }

				if (facing == Left) {
					angle -= 180;
				}
			}
		}
	}

	// Set animation frame
	if (puffingFrames != 0) {

	}
	else {
		real32 animationDelay;
		if (velocityMag > 0 && textures.contains(TextureType::Swim))
		{
			currentTexture = &textures[TextureType::Swim];
			animationDelay = movingAnimationDelay;
		}
		else
		{
			currentTexture = &textures[TextureType::Idle];
			animationDelay = idleAnimationDelay;
		}
		
		// Animation
		uint32 totalFrames = currentTexture->size.x / width;
		currentFrame = (currentFrame) % totalFrames;
		if (currentFrame == 0 && dynamic_cast<EnemyJelly*>(this)) {
			animationDelay = 2.5f;
		}

		if (state->play_time_passed - lastAnimationTime > animationDelay)
		{
			lastAnimationTime = state->play_time_passed;
			uint32 totalFrames = currentTexture->size.x / width;
			currentFrame = (currentFrame + 1) % totalFrames;
		}
	}

	// Apply velocity
	moveX(velocity.x * time_delta, [&](){
		if (diesOnImpact) {
			die();
		}

		if (isPuffed) {
			velocity.x = -velocity.x;
		}
		else{
			velocity.x = 0;
		}
	});
	moveY(velocity.y * time_delta, [&](){
		if (diesOnImpact) {
			die();
		}

		if (isPuffed) {
			velocity.y = -velocity.y;
		}
		else{
			velocity.y = 0;
		}
	});
}

inline const Solid* Actor::collideAt(const std::vector<Solid>& solids, Vector2f position) const
{
	Rect2f hitbox = getHitbox(position);
	for (const Solid& solid : solids)
	{
		if (solid.collidable)
		{
			const Rect2f solid_rect = {solid.position.x, solid.position.y, solid.width, solid.height};
			if (hitbox.collides(solid_rect))
			{
				LogWarn("Collided with solid in position %f, %f", position.x, position.y);
				return &solid;
			}
		}
	}
	return nullptr;
}

inline bool handleCollision(Vector2f position, std::function<void()> on_collide, int& move, int sign, Actor* actor, real32& coord) {
	const Solid * solid = actor->collideAt(state->currentLevel->solids, position);
	bool comingToBreak = false;
	if (solid) {
		comingToBreak = (actor->isPuffed || actor->puffingFrames > 0) && actor->velocity.getMagnitude() > 1200;
	}
	if (!solid || (solid->breakable && comingToBreak) || actor->noClip)
	{
		coord += sign;
		move -= sign;

		if (solid && solid->breakable && comingToBreak) {
			deleteFromVector(state->currentLevel->solids, *solid);
			playSound(block_break);
		}
	}
	else
	{
		if (on_collide != nullptr)
		{
			on_collide();
		}
		return true;
	}
	return false;
}

inline void Actor::moveX(real32 amount, std::function<void()> on_collide)
{
	//LogError("Actor moveX CALLED with amount: %f", amount);
	xRemainder += amount;
	int move = round(xRemainder);
	if (move != 0)
	{
		xRemainder -= move;
		int sign = SIGN(move);
		while (move != 0)
		{
			//LogError("moveX: %d", move);
			if (handleCollision(position + Vector2f(sign, 0), on_collide, move, sign, this, position.x)) {
				break;
			}
		}
	}
}

inline void Actor::moveY(real32 amount, std::function<void()> on_collide)
{
	//LogError("Actor moveY CALLED with amount: %f", amount);
	yRemainder += amount;
	int move = round(yRemainder);
	if (move != 0)
	{
		yRemainder -= move;
		int sign = SIGN(move);
		while (move != 0)
		{
			//LogError("moveY: %d, position y: %f", move, position.y);
			if (handleCollision(position + Vector2f(0, sign), on_collide, move, sign, this, position.y)) {
				break;
			}
		}
	}
}

inline Vector2f checkCollision(const std::vector<Solid>& solids, const Rect2f& hitbox)
{
	// Check out of bounds
	if (hitbox.x < 0){
		return {hitbox.x, 0};
	}
	if (hitbox.x + hitbox.w > state->currentLevel->width){
		return {state->currentLevel->width - hitbox.x - hitbox.w, 0};
	}
	if (hitbox.y < 0){
		return {0, hitbox.y};
	}
	if (hitbox.y + hitbox.h > state->currentLevel->height){
		return {0, state->currentLevel->height - hitbox.y - hitbox.h};
	}

	// Check solids
    for (const Solid& solid : solids)
    {
        if (solid.collidable)
        {
            const Rect2f solid_rect = {solid.position.x, solid.position.y, solid.width, solid.height};
            if (hitbox.collides(solid_rect))
            {
                return hitbox.collisionDepth(solid_rect);
            }
        }
    }
    return {0, 0};
}

void Player::puffUp()
{
	if (inButt && state->boss) {
		state->boss->hurt(this, 10);
		inButt = false;
		visible = true;
		position = state->boss->position + state->boss->buttRect.getCenter();
		velocity = {-5000.f, 300.f};
		state->boss->changeState(BossState::Hurt);
		playSound(inflate_sound);
		playSound(boss_hurt);
	}
	else {
		puffingTime = 3*puffingTimeStep;
		puffingFrames = 2;
		width = puffedSize.x;
		height = puffedSize.y;
		setTexture(player_texture_puffing, TextureType::Idle);
		setTexture(player_texture_puffing, TextureType::Swim);
		currentFrame = 0;

		if (!this->tryHitRectChange(-puffOffset, hitRects[1])) {
			puffingFrames = -1;
			puffingTime = 2*puffingTimeStep;
		}
		else {
			playSound(inflate_sound);
		}
	}
	
	resetPuffCooldown();
}

bool Player::tryHitRectChange(Vector2f deltaPos, const Rect2f& newHitRect) {
    // Calculate the total changes in position and size
	Vector2f orgPosition = position;
	Vector2f offsetChange = Vector2f(newHitRect.x - hitRect.x, newHitRect.y - hitRect.y);
    Vector2f totalDeltaPos = deltaPos + offsetChange;
    Vector2f totalSizeChange = {
        newHitRect.w - hitRect.w,
        newHitRect.h - hitRect.h
    };

    // Calculate the number of steps required
    int steps = std::max({
        ceil(abs(totalDeltaPos.x) / 30.f),
        ceil(abs(totalDeltaPos.y) / 30.f),
        ceil(abs(totalSizeChange.x) / 30.f),
        ceil(abs(totalSizeChange.y) / 30.f)
    });

    // Define the step increments
    Vector2f stepDeltaPos = deltaPos / float(steps);
    Vector2f stepOffsetChange = offsetChange / float(steps);
    Vector2f stepSizeChange = totalSizeChange / float(steps);

    for (int i = 0; i < steps; ++i) {
        // Calculate the target hitbox for this step
        Rect2f stepHitRect = {
            hitRect.x + stepOffsetChange.x,
            hitRect.y + stepOffsetChange.y,
            hitRect.w + stepSizeChange.x,
            hitRect.h + stepSizeChange.y
        };

        Rect2f targetHitBox = {
            position.x + stepDeltaPos.x + stepHitRect.x,
            position.y + stepDeltaPos.y + stepHitRect.y,
            stepHitRect.w,
            stepHitRect.h
        };

        std::vector<Solid>& solids = state->currentLevel->solids;

        Vector2f leftCollision = checkCollision(solids, {targetHitBox.x, targetHitBox.y, 1, targetHitBox.h});
        Vector2f rightCollision = checkCollision(solids, {targetHitBox.x + targetHitBox.w - 1, targetHitBox.y, 1, targetHitBox.h});
        Vector2f topCollision = checkCollision(solids, {targetHitBox.x, targetHitBox.y, targetHitBox.w, 1});
        Vector2f bottomCollision = checkCollision(solids, {targetHitBox.x, targetHitBox.y + targetHitBox.h - 1, targetHitBox.w, 1});

        if (leftCollision && rightCollision && topCollision && bottomCollision) {
            // Stop inflating if collisions on opposite sides
			position = orgPosition + deltaPos; // position will be taken back in deflation
            return false;
        } else {
            // Apply movement if there's a collision on one side but space on the other
			constexpr real32 push = 200.f;
            if (leftCollision && !rightCollision) {
                position.x += abs(leftCollision.x); // Move right
                if (velocity.x < 0) {
                    velocity.x = -velocity.x;
                }
                velocity.x += push;
            }
            if (rightCollision && !leftCollision) {
                position.x -= abs(rightCollision.x); // Move left
                if (velocity.x > 0) {
                    velocity.x = -velocity.x;
                }
                velocity.x -= push;
            }
            if (topCollision && !bottomCollision) {
                position.y += abs(topCollision.y); // Move down
                if (velocity.y < 0) {
                    velocity.y = -velocity.y;
                }
                velocity.y += push;
            }
            if (bottomCollision && !topCollision) {
                position.y -= abs(bottomCollision.y); // Move up
                if (velocity.y > 0) {
                    velocity.y = -velocity.y;
                }
                velocity.y -= push;
            }

            // Update position and hitbox after this step
            position += stepDeltaPos;
            hitRect = stepHitRect;
        }
    }

    return true;
}

void EnemyFish::chase(Vector2f target, ControllerInput& input)
{
	const real32 hDist = abs(target.x - position.x);
	const real32 vDist = abs(target.y - position.y);
	// if (abs(target.x - position.x) > abs(target.y - position.y)) {
		// Horizontal chase
		if (target.x < position.x)
		{
			input.dir_left = true;
		}
		else if (target.x > position.x)
		{
			input.dir_right = true;
		}
	// }
	// else {
		// Vertical chase
		if (target.y < position.y)
		{
			input.dir_up = true;
		}
		else if (target.y > position.y)
		{
			input.dir_down = true;
		}
	// }

	if (hDist < 200 && vDist > 200) {
		input.dir_left = input.dir_right = 0;
	}
	else if (vDist < 200 && hDist > 200) {
		input.dir_up = input.dir_down = 0;
	}
}

inline void EnemyFish::render(SDL_Renderer* renderer)
{
	Actor::render(renderer);
}

inline void EnemyShrimp::render(SDL_Renderer* renderer)
{
	if (visible)
	{
		SDL_RendererFlip flip = facing == Right ? SDL_FLIP_NONE : SDL_FLIP_HORIZONTAL;
		real32 drawAngle = angle;
		if (isInverted) {
			flip = (SDL_RendererFlip)(flip | SDL_FLIP_VERTICAL);
			if (targetingPlayer){
				drawAngle = drawAngle - 180;
			}
		}

		const SDL_Rect main_sprite_rect = {static_cast<int>(currentFrame * width), 0, static_cast<int>(width), static_cast<int>(height)};
		const SDL_Rect claw_sprite_rect = {static_cast<int>(currentClawFrame * width), 0, static_cast<int>(width), static_cast<int>(height)};
		const SDL_FRect dest_rect = {position.x, position.y, width, height};
		renderTextureEx(renderer, currentTexture->texture, &main_sprite_rect, &dest_rect, 0, NULL, flip);
		renderTextureEx(renderer, currentClawTexture, &claw_sprite_rect, &dest_rect, drawAngle, &claw_offset, flip);
	}
}

void EnemyFish::update(real32 time_delta, const ControllerInput* input)
{
	think(time_delta);
	Actor::update(time_delta, input);

	if (dyingTime) {
		visible = !visible;
	}
	else {
		// Collide with player
		if (!state->player.invulTime && !state->player.isDying()) {
			if (getHitbox().collides(state->player.getHitbox())) {
				if (state->player.isPuffed || state->player.puffingTime > 0) {
					const real32 speedDiff = (velocity - state->player.velocity).getMagnitude() ;
					const real32 hurtLimit = 1000.f;
					if (speedDiff > hurtLimit) {
						hurt(&state->player, speedDiff / hurtLimit);
						if (isDying()) {
							playSound(fish_die);
						}
						else {
							playSound(fish_hurt);
						}
					}
				}
				else {
					state->player.hurt(this);
				}
			}
		}
	}

	setTexture(chasingPlayer ? enemy_fish_texture_chase : enemy_fish_texture_swim, TextureType::Swim);
}

void EnemyFish::think(real32 time_delta)
{
	const real32 thinking_time = seesPlayer ? 0.03f : 0.8f;

	if (state->play_time_passed - lastIdeaTime > thinking_time)
	{
		lastIdeaTime = state->play_time_passed;

		input.dir_left = false;
		input.dir_right = false;
		input.dir_up = false;
		input.dir_down = false;

		chasingPlayer = false;
		const real32 distToPlayer = (state->player.position - position).getMagnitude();
		const real32 seeDistance = seesPlayer ? 1800 : 900;
		if (distToPlayer < seeDistance)
		{
			if (!seesPlayer) {
				// Just saw the player
				seesPlayer = true;
			}
			else {
				// Chasing started
				goingSlow = false;
				if (state->player.isPuffed) {
					const bool playerInTheWay = checkAABBLineCollision(getCenter(), spawnPoint, state->player.getHitbox());
					if (playerInTheWay) {
						// Reverse chase the player
						chase(state->player.position, input);
						input.dir_left = !input.dir_left;
						input.dir_right = !input.dir_right;
						input.dir_down = !input.dir_down;
						input.dir_up = !input.dir_up;
					}
					else {
						// Chase the spawn point like normal
						chase(spawnPoint, input);
					}
				}
				else {
					chasingPlayer = true;
					chase(state->player.position, input);
				}
			}
		}
		else {
			if (seesPlayer) {
				// Just lost the player
				seesPlayer = false;
			}
			else {
				goingSlow = true;
				chase(spawnPoint, input);
			}
		}
	}
}

void EnemyJelly::update(real32 time_delta, const ControllerInput* input)
{
	Actor::update(time_delta, input);

	// Bob
	const real32 bobPeriod = 5.f;
	const real32 bobAmount = 40.f;
	bobTimer = fmod(bobTimer + time_delta, bobPeriod);
	position.y = spawnPoint.y - bobAmount * sinf(2 * Pi32 * bobTimer/bobPeriod);

	// Collide with player
	if (!state->player.invulTime && !state->player.isDying()) {
		if (getHitbox().collides(state->player.getHitbox())) {
			state->player.hurt(this);
		}
	}
}

void EnemyShrimp::update(real32 time_delta, const ControllerInput* input)
{
	velocity.y += isInverted ? -100.f : 100.f;
	Actor::update(time_delta, input);
	
	if (isDying()) {
		visible = !visible;
	}
	else {
		// Collide with player
		if (!state->player.invulTime && !state->player.isDying()) {
			if (getHitbox().collides(state->player.getHitbox())) {
				if (state->player.isPuffed || state->player.puffingTime > 0) {
					const real32 speedDiff = (velocity - state->player.velocity).getMagnitude() ;
					const real32 hurtLimit = 1000.f;
					if (speedDiff > hurtLimit) {
						hurt(&state->player, speedDiff / hurtLimit);
					}
				}
				else {
					state->player.hurt(this);
				}
			}
		}

		if (shootCooldown > 0) {
			shootCooldown = MAX(shootCooldown - time_delta, 0);
		}
		else if (input->button_b && currentClawFrame == 4) {
			// Shoot a bubble
			const Vector2f targetVector = state->player.getCenter() - getCenter();
			Vector2f clawPos = {claw_offset.x, claw_offset.y};
			state->newEnemiesQueue.push_back(std::make_unique<EnemyBubble>(position + clawPos, targetVector.getNormalized(), this));
			shootCooldown = shootPeriod;
			playSound(shoot);
		}
	}

	// Claw animation
	real32 clawAnimationDelay;
	currentClawTexture = targetingPlayer ? textureClawAttack : textureClaw;
	currentClawTextureSize = targetingPlayer ? textureClawAttackSize : textureClawSize;
	
	clawAnimationDelay = 0.3f;
	
	// Animation
	uint32 totalFrames = currentClawTextureSize.x / width;
	currentClawFrame = (currentClawFrame) % totalFrames;
	if (state->play_time_passed - lastClawAnimationTime > clawAnimationDelay)
	{
		lastClawAnimationTime = state->play_time_passed;
		uint32 totalFrames = currentClawTextureSize.x / width;
		currentClawFrame = (currentClawFrame + 1) % totalFrames;
	}
}

void EnemyBubble::update(real32 time_delta, const ControllerInput* input)
{
	Actor::update(time_delta, input);

	lifespan -= time_delta;

	// Collide with player
	if (!bounced && !state->player.invulTime && !state->player.isDying()) {
		if (getHitbox().collides(state->player.getHitbox())) {
			if (state->player.puffingFrames > 0) {
				velocity = -velocity;
				bounced = true;
				if (isBig) {
					state->player.isPuffed = false;
					state->player.puffCooldown = 0;
				}
			}
			else {
				if (!state->player.isPuffed || isBig) {
					state->player.hurt(this);
					playSound(popHurt);
					die();
				}
				else {
					playSound(popHarmless);
					die();
				}
			}
		}
	}

	if (bounced && creator->getHitbox().collides(getHitbox())) {
		if (creator == state->boss) {
			if (isBig) {
				state->boss->changeState(BossState::Stunned);
			}
		}
		else {
			creator->hurt(this);
		}
		die();
		playSound(popHurt);
	}
	
	if(lifespan <= 0) {
		LogWarn("Bubble lifespan is over")
		die();
	}
}

void EnemyShrimp::think(real32 time_delta)
{	
	const real32 thinking_time = vigilant ? 0.03f : 0.75f;

	if (state->play_time_passed - lastIdeaTime > thinking_time)
	{
		input.button_b = false;

		const Vector2f targetVector = state->player.getCenter() - getCenter();
		const real32 distToPlayer = targetVector.getMagnitude();
		const real32 seeDistance = targetingPlayer ? 1600 : 800;
		if (distToPlayer < seeDistance) {
			if (!targetingPlayer) {
				targetingPlayer = true;
				vigilant = true;
			}
			else {
				angle = atan2(targetVector.y, targetVector.x) * (180.0 / Pi32) + 90;
				input.button_b = true;
			}
		}
		else {
			if (targetingPlayer) {
				targetingPlayer = false;
			}
			else {
				if (vigilant) {
					if (abs(angle) > 1.f) {
						angle += (-angle) * 0.02f;
					}
					else {
						vigilant = false;
					}
				}
				else {
					const real32 wavePeriod = 2.0f;
					const real32 waveAmount = 10.0f;
					waveTimer = fmod(waveTimer + time_delta, wavePeriod);
					angle = waveAmount * sinf(2 * Pi32 * (waveTimer/wavePeriod));
				}
			}
		}
	}
}

void Solid::prepare(Level* level) {
	if (texture == tile1_texture_top) {
		// Basic ground tile
		bool hasTop = level->checkSolid({position.x, position.y - height});
		bool hasLeft = level->checkSolid({position.x - width, position.y});
		bool hasBottom = level->checkSolid({position.x, position.y + height});
		bool hasRight = level->checkSolid({position.x + width, position.y});

		bool hasTopLeft = level->checkSolid({position.x - width, position.y - height});
		bool hasTopRight = level->checkSolid({position.x + width, position.y - height});
		bool hasBottomLeft = level->checkSolid({position.x - width, position.y + height});
		bool hasBottomRight = level->checkSolid({position.x + width, position.y + height});
		if (hasTop && hasBottom && hasLeft && hasRight) {
			if (hasTopLeft && hasTopRight  && hasBottomLeft  && hasBottomRight) {
				// If covered with two sets of tiles, set collision is not needed
				bool fullSolid = true;
				for (int32 i = -2; i <= 2; i++) {
					for (int32 j = -2; j <= 2; j++) {
						if (i == -2 || i == 2 || j == -2 || j == 2) {
							if (!level->checkSolid({position.x + i*width, position.y + j*height})) {
								fullSolid = false;
								goto after_loop;
							}
						}
					}
				}
				after_loop:
				if (fullSolid) {
					collidable = false;
				}
			}
			texture = tile1_texture_mid;
			return;
		}

		if (hasTop && hasBottom && !hasLeft) {
			texture = tile1_texture_midleft;
			return;
		}
		if (hasTop && hasBottom && !hasRight) {
			texture = tile1_texture_midright;
			return;
		}
		if (hasTop && hasLeft && !hasBottom && !hasRight) {
			texture = tile1_texture_botright;
			return;
		}
		if (hasTop && hasRight && !hasBottom && !hasLeft) {
			texture = tile1_texture_botleft;
			return;
		}
		if (hasBottom && hasLeft && !hasTop && !hasRight) {
			texture = tile1_texture_topright;
			return;
		}
		if (hasBottom && hasRight && !hasTop && !hasLeft) {
			texture = tile1_texture_topleft;
			return;
		}
		if (!hasBottom && hasRight && hasTop && hasLeft) {
			texture = tile1_texture_bot;
			return;
		}
	}
}


void EnemyBoss::think(real32 time_delta) {

}

void EnemyBoss::changeState(BossState newState) {
	lastStateTime = state->play_time_passed;
	bossState = newState;

	cycleCount++;
	bubbleShootCount = 0;
	sweepState = SweepState::Windup;
	lastSweepStateTime = state->play_time_passed;
	bbState = BigBubbleState::Windup;
	lastBBStateTime = state->play_time_passed;
	idleDelay = 1.0f;
	SDL_SetTextureColorMod(textureMainStunned, 255, 255, 255);
}

void EnemyBoss::shootBubbles(real32 time_delta) {
	const Vector2f mouthVector = position + mouthOffset;

	real32 bubbleSpeed = 3600.f;
	const real32 bubbleLife = 2.f;
	bool playerIsBehind = getCenter().x < state->player.getCenter().x;
	if (bubbleShootCount < 8) {
		bubbleSpeed = 3600.f;
		Vector2f targets[3];
		if (bubbleShootCount % 2 == 0) {
			targets[0] = Vector2f(-1, -1);
			targets[1] = Vector2f(-1, 0);
			targets[2] = Vector2f(-1, 1);
		}
		else {
			targets[0] = Vector2f(-0.924, -0.383);
			targets[1] = Vector2f(-0.924, 0.383);
			targets[2] = Vector2f(0, 0);
		}
		if (playerIsBehind) {
			targets[0].x = - targets[0].x;
			targets[1].x = - targets[1].x;
			targets[2].x = - targets[2].x;
		}
		state->newEnemiesQueue.push_back(std::make_unique<EnemyBubble>(mouthVector, (targets[0]).getNormalized(), this, bubbleSpeed, false, bubbleLife));
		state->newEnemiesQueue.push_back(std::make_unique<EnemyBubble>(mouthVector, (targets[1]).getNormalized(), this, bubbleSpeed, false, bubbleLife));
		if (targets[2]){
			state->newEnemiesQueue.push_back(std::make_unique<EnemyBubble>(mouthVector, (targets[2]).getNormalized(), this, bubbleSpeed, false, bubbleLife));
		}
		shootCooldown = shootPeriod * (1 - 0.1*bubbleShootCount);
	}
	else if (bubbleShootCount < 40) {
		bubbleSpeed = 2800.f;

		real32 step = -((bubbleShootCount - 8) % 20) * 5 + 10;
		real32 angle1 = 135;
		if (playerIsBehind) {
			angle1 = -45;
		}
		state->newEnemiesQueue.push_back(std::make_unique<EnemyBubble>(mouthVector, getUnitVectorFromDegrees(angle1 + step), this, bubbleSpeed, false, bubbleLife));
		state->newEnemiesQueue.push_back(std::make_unique<EnemyBubble>(mouthVector, getUnitVectorFromDegrees(angle1 + 45 + step), this, bubbleSpeed, false, bubbleLife));
		state->newEnemiesQueue.push_back(std::make_unique<EnemyBubble>(mouthVector, getUnitVectorFromDegrees(angle1 + 90 + step), this, bubbleSpeed, false, bubbleLife));
		shootCooldown = 0.1f;
	}
	else {
		changeState(BossState::BigBubble);
	}

	playSound(shoot);
	bubbleShootCount++;
}

void EnemyBoss::sweepAttack(real32 time_delta) {
	real32 timePassed;
	Vector2f posDiff;
	switch (sweepState)
	{
	case SweepState::Windup:
		if (clawAngle < 0) {
			clawRotationSpeed = 45;
			posDiff = (getCenter() - state->player.getCenter()).getNormalized();
			state->player.velocity += posDiff * 40.0f;
		}
		else {
			clawRotationSpeed = 0;
			sweepState = SweepState::BeforeSlash;
			lastSweepStateTime = state->play_time_passed;
		}
		break;
	case SweepState::BeforeSlash:
		clawRotationSpeed = 0;
		timePassed = state->play_time_passed - lastSweepStateTime;
		if (timePassed > 0.3f) {
			sweepState = SweepState::Slash;
			lastSweepStateTime = state->play_time_passed;
		} else if (timePassed < 0.15f) {
			clawFrame = int(timePassed/0.03f) % 5;
		}
		break;
	case SweepState::Slash:
		if (clawAngle > -160) {
			clawRotationSpeed = -600;
		}
		else {
			clawRotationSpeed = 0;
			sweepState = SweepState::AfterSlash;
			lastSweepStateTime = state->play_time_passed;
		}
		break;
	case SweepState::AfterSlash:
		clawRotationSpeed = 0;
		if (state->play_time_passed - lastSweepStateTime > 0.8f) {
			sweepState = SweepState::Bringback;
			lastSweepStateTime = state->play_time_passed;
		}
		break;
	case SweepState::Bringback:
		if (clawAngle < -30) {
			clawRotationSpeed = 150;
		}
		else {
			lastSweepStateTime = state->play_time_passed;
			changeState(BossState::Idle);
		}
		break;
	
	default:
		break;
	}
	clawAngle += clawRotationSpeed * time_delta;
}


void EnemyBoss::bigBubbleAttack(real32 time_delta) {
	real32 bubbleSpeed = 3800.f;
	Vector2f targetVector;
	switch (bbState)
	{
	case BigBubbleState::Windup:
		if (state->play_time_passed - lastBBStateTime > 3.f) {
			bbState = BigBubbleState::Shoot;
			lastBBStateTime = state->play_time_passed;
		}
		break;
	case BigBubbleState::Shoot:
		targetVector = state->player.getCenter() - getCenter();
		state->newEnemiesQueue.push_back(std::make_unique<EnemyBubble>(position + mouthOffset, targetVector.getNormalized(), this, bubbleSpeed, true));
		shootCooldown = shootPeriod;
		playSound(shoot);
		changeState(BossState::Idle);
		break;
	default:
		break;
	}
}

void EnemyBoss::die() {
	changeCurrentState(Ending);
}

void EnemyBoss::update(real32 time_delta, const ControllerInput* input)
{
	Actor::update(time_delta, input);
	real32 bobTimer;
	clawAngleWave = 3.f * sinf(2 * Pi32 * fmod(state->play_time_passed, 4.f)/4.f);
	// clawPosYWave = 30.f * sinf(2 * Pi32 * fmod(state->play_time_passed, 6.f)/6.f);

	if (!state->player.isDying() && !state->player.invulTime && bossState != BossState::Stunned && bossState != BossState::Hurt) {
		Rect2f playerHitbox = state->player.getHitbox();
		if (getHitbox().collides(playerHitbox)) {
			state->player.hurt(this);
		}
		else {
			for (Rect2f rect : clawHitRects) {
				Vector2f center = rect.getCenter();
				Vector2f rotated = rotatePoint({center.x+ claw_normal_offset.x + position.x, center.y + claw_normal_offset.y + position.y}, {claw_joint_offset.x + position.x, claw_joint_offset.y + position.y}, (clawAngle + clawAngleWave)*0.75);
				rect.x = rotated.x - rect.w/2;
				rect.y = rotated.y - rect.h/2;

				if (rect.collides(playerHitbox)) {
					state->player.hurt(this);
				}
			}
		}
	}

	std::uniform_int_distribution<int> rngDist(1, 3);

	switch (bossState)
	{
		case BossState::Waiting:
			if (state->bossStarted) {
				changeState(BossState::Idle);
				idleDelay = 2.0f;
			}
			break;
		case BossState::Idle:
			idleDelay -= time_delta;
			bobTimer = fmod(state->play_time_passed, 3.f);
			smallclawAngle = 10.f * sinf(2 * Pi32 * bobTimer/3.f);
			if (idleDelay <= 0) {
				if (cycleCount < 3) {
					changeState(BossState::Bubbles);
				}
				else {
					if (rngDist(rng) == 1) {
						changeState(BossState::Sweep);
					}
					else {
						changeState(BossState::Bubbles);
					}
				}
			}
			break;
		case BossState::Bubbles:
			if (shootCooldown <= 0) {
				if (state->player.isPuffed) {
					if (rngDist(rng) == 1) {
						shootBubbles(time_delta);
					}
					else {
						changeState(BossState::Sweep);
					}
				}
				else {
					shootBubbles(time_delta);
				}
			}
			shootCooldown -= time_delta;
			break;
		case BossState::BigBubble:
			bigBubbleAttack(time_delta);
			break;
		case BossState::Sweep:
			sweepAttack(time_delta);
			break;
		case BossState::Stunned:
			if (state->play_time_passed - lastStateTime > 10.0f) {
				if (state->player.inButt) {
					state->player.inButt = false;
					state->player.visible = true;
					state->player.position = position + buttRect.getCenter();
					state->player.velocity = {-5000.f, 300.f};
				}
				changeState(BossState::Idle);
			}
			else {
				Rect2f buttBox = buttRect;
				buttBox.x += position.x;
				buttBox.y += position.y;
				if (!state->player.inButt && buttBox.collides(state->player.getHitbox())) {
					if (state->player.isPuffed) {
						state->player.velocity = - state->player.velocity;
					}
					else {
						state->player.inButt = true;
						state->player.visible = false;
						playSound(enter_butt);
					}
				}
				stun_frame = int(fmod((state->play_time_passed - lastStateTime) * 10, 5.f));
			}
			break;
		case BossState::Hurt:
			if (state->play_time_passed - lastStateTime > 3.0f) {
				changeState(BossState::Idle);
			}
			else {
				
			}
			break;
		default:
			break;
	}
}

inline void EnemyBoss::render(SDL_Renderer* renderer)
{
	SDL_Rect stun_sprite_rect;
	SDL_FRect stun_dest_rect;
	int32 stun_width = 687;
	if (visible)
	{
		SDL_Rect main_sprite_rect;
		SDL_FRect main_dest_rect;
		SDL_Rect claw_sprite_rect;
		SDL_FRect claw_dest_rect;
		SDL_Rect smallclaw_sprite_rect;
		SDL_FRect smallclaw_dest_rect;
		switch (bossState)
		{
		case BossState::Waiting:
		case BossState::Idle:
		case BossState::Bubbles:
		case BossState::BigBubble:
		case BossState::Sweep:
			main_sprite_rect = {static_cast<int>(currentFrame * width), 0, static_cast<int>(width), static_cast<int>(height)};
			main_dest_rect = {position.x, position.y, width, height};
			claw_sprite_rect = {static_cast<int>(clawFrame * width), 0, static_cast<int>(width), static_cast<int>(height)};
			claw_dest_rect = {position.x + claw_normal_offset.x, position.y + claw_normal_offset.y + clawPosYWave, width, height};
			smallclaw_sprite_rect = {static_cast<int>(currentFrame * width), 0, static_cast<int>(width), static_cast<int>(height)};
			renderTextureEx(renderer, textureClaw, &claw_sprite_rect, &claw_dest_rect, clawAngle + clawAngleWave, &claw_joint_offset, SDL_FLIP_NONE);
			renderTextureEx(renderer, bossState == BossState::Bubbles ? enemy_boss_texture_spit : currentTexture->texture, &main_sprite_rect, &main_dest_rect, 0, NULL, SDL_FLIP_NONE);
			renderTextureEx(renderer, textureSmallclaw, &main_sprite_rect, &main_dest_rect, smallclawAngle, &smallclaw_joint_offset, SDL_FLIP_NONE);
			break;
		case BossState::Hurt:
			if (fmod(state->play_time_passed - lastStateTime, 1.f) < 0.5f) {
				SDL_SetTextureColorMod(textureMainStunned, 255, 0, 0);
			}
			else {
				SDL_SetTextureColorMod(textureMainStunned, 255, 255, 255);
			}
			main_sprite_rect = {static_cast<int>(currentFrame * width), 0, static_cast<int>(width), static_cast<int>(height)};
			main_dest_rect = {position.x, position.y, width, height};
			claw_sprite_rect = {static_cast<int>(currentFrame * width), 0, static_cast<int>(width), static_cast<int>(height)};
			claw_dest_rect = {position.x, position.y, width, height};
			renderTextureEx(renderer, textureMainStunned, &main_sprite_rect, &main_dest_rect, 0, NULL, SDL_FLIP_NONE);
			break;
		case BossState::Stunned:
			main_sprite_rect = {static_cast<int>(currentFrame * width), 0, static_cast<int>(width), static_cast<int>(height)};
			main_dest_rect = {position.x, position.y, width, height};
			claw_sprite_rect = {static_cast<int>(currentFrame * width), 0, static_cast<int>(width), static_cast<int>(height)};
			claw_dest_rect = {position.x, position.y, width, height};
			renderTextureEx(renderer, textureMainStunned, &main_sprite_rect, &main_dest_rect, 0, NULL, SDL_FLIP_NONE);
			
			stun_sprite_rect = {stun_frame*stun_width, 0, stun_width, 348};
			stun_dest_rect = {position.x + 884.f, position.y + 303.f, (real32)stun_width, 348.f};
			renderTextureEx(renderer, stun_texture, &stun_sprite_rect, &stun_dest_rect, 0, NULL, SDL_FLIP_NONE);
			break;
		default:
			break;
		}
	}
}

void Decor::update(real32 time_delta, const ControllerInput* input) 
{
	// currentTexture = textureIdle;todo delete
	uint32 totalFrames = currentTexture->size.x / width;
	if (totalFrames > 1) {
		real32 animationDelay = idleAnimationDelay;
		
		// Animation
		currentFrame = (currentFrame) % totalFrames;
		if (state->play_time_passed - lastAnimationTime > animationDelay)
		{
			lastAnimationTime = state->play_time_passed;
			uint32 totalFrames = currentTexture->size.x / width;
			currentFrame = (currentFrame + 1) % totalFrames;
		}
	}
}


void Diagonal::update(real32 time_delta, const ControllerInput* input) {
	Vector2f deltaPos = {0, 0};
	if (checkAABBLineCollision(p1, p2, state->player.getHitbox(), &deltaPos, &normal)) {
		state->player.position += deltaPos;
		Vector2f& vel = state->player.velocity;
		if (!state->player.isPuffed && state->player.puffingFrames == 0) {
			Vector2f perpendicular = normal * dot(vel, normal);
			vel -= perpendicular;
		}
		else {
			real32 absX = abs(vel.x);
			real32 absY = abs(vel.y);
			if (absY != 0 && absX / absY > 4) {
				vel.y = 0;
			}
			else if (absX != 0 && absY / absX > 4) {
				vel.x = 0;
			}

			const real32 temp = vel.y;
			vel.y = vel.x;
			vel.x = temp;
			if (direction == DiagDir::BotRight || direction == DiagDir::TopLeft) {
				vel = - vel;
			}
		}
	}
}

void Diagonal::render(SDL_Renderer* renderer) {
	renderTextureEx(renderer, currentTexture->texture, &sprite_rect, &dest_rect, 0, NULL, flip);
}

void Key::update(real32 time_delta, const ControllerInput* _input) {
	input.dir_right = false;
	input.dir_left = false;
	input.dir_up = false;
	input.dir_down = false;
	if (holder) {
		Vector2f posDiff = holder->position - position;
		real32 mag = posDiff.getMagnitude();
		if (mag > 150) {
			if (holder->position.x > position.x) {
				input.dir_right = true;
			}
			if (holder->position.x < position.x) {
				input.dir_left = true;
			}
			if (holder->position.y > position.y) {
				input.dir_down = true;
			}
			if (holder->position.y < position.y) {
				input.dir_up = true;
			}
		}
	}
	else {
		// Bob
		const real32 bobPeriod = 3.f;
		const real32 bobAmount = 10.f;
		bobTimer = fmod(bobTimer + time_delta, bobPeriod);
		position.y = spawnPoint.y - bobAmount * sinf(2 * Pi32 * bobTimer/bobPeriod);
		// Collide with player
		if (!state->player.isDying()) {
			if (getHitbox().collides(state->player.getHitbox())) {
				holder = &state->player;
				playSound(key_pickup);
			}
		}
	}
	Actor::update(time_delta, &input);
}

void Door::update(real32 time_delta, const ControllerInput* input) {
	if (!state->player.isDying() && state->key.holder && getHitbox().collides(state->key.getHitbox())) {
		changeCurrentState(State::Victory);
	}
}

void Button::update(real32 time_delta, const ControllerInput* input) {
	if (state->currentLevel->heartTaken || state->heartPopped) {
		isPressed = true;
		setTexture(button_pressed_texture, TextureType::Idle);
		return;
	}

	Player * player = &state->player;
	if (isPressed) {
		if (!player->isPuffed) {
			isPressed = false;
			setTexture(button_unpressed_texture, TextureType::Idle);
		}
	}
	else {
		if ((player->isPuffed || player->puffingFrames > 0) && getHitbox().collides(player->getHitbox())) {
			isPressed = true;
			setTexture(button_pressed_texture, TextureType::Idle);

			bool allPressed = true;
			for (auto& button : state->buttons) {
				if (!button->isPressed) {
					allPressed = false;
					break;
				}
			}

			if (allPressed) {
				state->heart.visible = true;
				playSound(heart_popped);
				state->heartPopped = true;
			}
		}
	}
}

void Heart::update(real32 time_delta, const ControllerInput* input) {
	if (state->currentLevel->heartTaken || !visible) {
		return;
	}
	else {
		// Bob
		const real32 bobPeriod = 4.f;
		const real32 bobAmount = 15.f;
		bobTimer = fmod(bobTimer + time_delta, bobPeriod);
		position.y = spawnPoint.y - bobAmount * sinf(2 * Pi32 * bobTimer/bobPeriod);

		// Collide with player
		if (!state->player.isDying()) {
			if (getHitbox().collides(state->player.getHitbox())) {
				playSound(heart_pickup);
				state->player.maxHealth++;
				state->player.health++;
				visible = false;
				state->currentLevel->heartTaken = true;
			}
		}
	}
}

void Grampa::update(real32 time_delta, const ControllerInput* input) {
	Actor::update(time_delta, input);
	
	// Bob
	const real32 bobPeriod = 5.f;
	const real32 bobAmount = 10.f;
	bobTimer = fmod(bobTimer + time_delta, bobPeriod);
	position.y = spawnPoint.y - bobAmount * sinf(2 * Pi32 * bobTimer/bobPeriod);

	int32 currentLevelId = state->currentLevel - state->levels;
	if (grampaState == 0) {
		if ((getCenter() - state->player.getCenter()).getMagnitude() < 300.f) {
			speechTimer = 0;
			grampaState = 1;
		}
	}
	else if (grampaState == 1) {
		if (currentLine < messages[currentLevelId].size()) {
			if (speechTimer > messages[currentLevelId][currentLine].size() * 0.12f) {
				speechTimer = 0;
				currentLine++;
				grampaState = 3;
			}
		}
		else {
			grampaState = 2;
		}
		speechTimer += time_delta;
	}
	else if (grampaState == 3) {
		if (speechTimer > 1.f) {
			speechTimer = 0;
			grampaState = 1;
		}
		else {
			speechTimer += time_delta;
		}
	}
}

inline void Grampa::render(SDL_Renderer* renderer)
{
	if (!speech_font) {
		// c++ broke and now I have to do it this way
		speech_font = FC_CreateFont();
		FC_LoadFont(speech_font, renderer, "assets/Action_Man.ttf", 20*6, FC_MakeColor(255, 255, 255, 255), TTF_STYLE_NORMAL);
	}
	int32 currentLevelId = state->currentLevel - state->levels;
	Actor::render(renderer);
	if (grampaState == 1 && currentLine < messages[currentLevelId].size()) {
		int32 width = FC_GetWidth(speech_font, messages[currentLevelId][currentLine].c_str());
		int32 height = FC_GetHeight(speech_font, messages[currentLevelId][currentLine].c_str());
		Vector2f textPos = position;
		textPos.y -= height + 20;
		textPos.x -= width/2;
		textPos.x -= state->camera.x;
		textPos.y -= state->camera.y;
		renderOutlinedText(speech_font, renderer, textPos.x, textPos.y, messages[currentLevelId][currentLine].c_str());
	}
}


void renderTexture(SDL_Renderer* renderer, SDL_Texture* texture, const SDL_Rect* sourceRect, const SDL_FRect* destRect) {
    SDL_FRect renderDestRect = { destRect->x - state->camera.x, destRect->y - state->camera.y, destRect->w, destRect->h };
    SDL_RenderCopyF(renderer, texture, sourceRect, &renderDestRect);
}
void renderTextureEx(SDL_Renderer* renderer, SDL_Texture* texture, const SDL_Rect* sourceRect, const SDL_FRect* destRect, const double angle, const SDL_FPoint* center, SDL_RendererFlip flip) {
    SDL_FRect renderDestRect = { destRect->x - state->camera.x, destRect->y - state->camera.y, destRect->w, destRect->h };
    SDL_RenderCopyExF(renderer, texture, sourceRect, &renderDestRect, angle, center, flip);
}