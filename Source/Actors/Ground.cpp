//
// Created by roger on 22/04/2025.
//

#include "Ground.h"
#include "../Game.h"
#include "../Components/RigidBodyComponent.h"
#include "../Components/AABBComponent.h"
#include "../Components/Drawing/RectComponent.h"
#include "../Components/Drawing/TileMapComponent.h"
#include <unordered_map>

#include "ParticleSystem.h"
#include "../Components/Drawing/AnimatorComponent.h"

Ground::Ground(Game* game, float width, float height, bool isSpike, bool isMoving, float movingDuration, Vector2 velocity, bool groundBehindPlayer, bool usePadding)
    :Actor(game)
    ,mStartingPosition(Vector2::Zero)
    ,mRespawnPosition(Vector2::Zero)
    ,mHeight(height)
    ,mWidth(width)
    ,mIsSpike(isSpike)
    ,mIsMoving(isMoving)
    ,mMovingDuration(movingDuration)
    ,mMovingTimer(movingDuration)
    ,mIsBreakable(false)
    ,mVelocity(velocity)
    ,mRectComponent(nullptr)
    ,mDrawComponent(nullptr)
{
    Vector2 v1(-mWidth / 2, -mHeight / 2);
    Vector2 v2(mWidth / 2, -mHeight / 2);
    Vector2 v3(mWidth / 2, mHeight / 2);
    Vector2 v4(-mWidth / 2, mHeight / 2);

    std::vector<Vector2> vertices;
    vertices.emplace_back(v1);
    vertices.emplace_back(v2);
    vertices.emplace_back(v3);
    vertices.emplace_back(v4);

    float paddingTop = 0;
    float paddingBottom = 0;
    float paddingLeft = 0;
    float paddingRight = 0;

    if (usePadding) {
        paddingTop = 10.0f;    // Ignora a altura da grama
        paddingBottom = 8.0f;  // Ignora as raizes
        paddingLeft = 8.0f;    // Ignora folhas nas laterais
        paddingRight = 8.0f;   // Ignora folhas nas laterais
    }

    Vector2 physicsV1(-mWidth / 2 + paddingLeft, -mHeight / 2 + paddingTop);
    Vector2 physicsV3(mWidth / 2 - paddingRight, mHeight / 2 - paddingBottom);

    SDL_Color color;
    if (mIsSpike)
        color = SDL_Color{255, 0, 0, 255};

    else
        color = SDL_Color{0, 255, 0, 255};

    // mRectComponent = new RectComponent(this, mWidth, mHeight, RendererMode::LINES);
    // mRectComponent->SetColor(Vector3(color.r, color.g, color.b));

    // mDrawPolygonComponent = new DrawPolygonComponent(this, vertices, color);

    mRigidBodyComponent = new RigidBodyComponent(this, 1);
    mAABBComponent = new AABBComponent(this, physicsV1, physicsV3);
    // mAABBComponent->SetDebugDraw(true);

    // mDrawGroundSpritesComponent = new DrawGroundSpritesComponent(this, mGame->GetTileSize(), mGame->GetTileSize(), 99);
    int drawOrder;
    if (groundBehindPlayer) {
        drawOrder = 99;
    }
    else {
        drawOrder = 1003;
    }
    mDrawComponent = new TileMapComponent(this, drawOrder);

    if (mIsMoving) {
        mRigidBodyComponent->SetVelocity(mVelocity);
    }

    mGame->AddGround(this);
}

Ground::~Ground() { mGame->RemoveGround(this); }

void Ground::OnUpdate(float deltaTime) {
    if (mIsMoving) {
        mMovingTimer += deltaTime;
        if (mMovingTimer > mMovingDuration) {
            mVelocity = mVelocity * -1.0f;
            mMovingTimer -= mMovingDuration;
        }
        float progress = mMovingTimer / mMovingDuration;
        float smoothMultiplier = Math::PiOver2 * sinf(Math::Pi * progress);

        mRigidBodyComponent->SetVelocity(mVelocity * smoothMultiplier);
    }
}

void Ground::SetIsMoving(bool isMoving) {
    mIsMoving = isMoving;
    if (!mIsMoving) {
        mRigidBodyComponent->SetVelocity(Vector2::Zero);
    }
}

void Ground::SetSprites() {
    int rows = mHeight / mGame->GetTileSize();
    int cols = mWidth / mGame->GetTileSize();

    int topLeftX = mStartingPosition.x - mWidth / 2;
    int topLeftY = mStartingPosition.y - mHeight / 2;

    int minRow = topLeftY / mGame->GetTileSize();
    int maxRow = minRow + rows;

    int minCol = topLeftX / mGame->GetTileSize();
    int maxCol = minCol + cols;

    std::unordered_map<int, std::vector<Vector2> > spriteOffsetMap;

    int** levelData = mGame->GetLevelData();

    for (int row = minRow; row < maxRow; ++row) {
        for (int col = minCol; col < maxCol; ++col) {
            int tile = levelData[row][col];

            int tileX = col * mGame->GetTileSize();
            int tileY = row * mGame->GetTileSize();

            Vector2 offset = Vector2(tileX, tileY) - mStartingPosition;

            if (tile >= 0) {
                spriteOffsetMap[tile].emplace_back(offset);
            }
        }
    }
    // mDrawGroundSpritesComponent->SetSpriteOffsetMap(spriteOffsetMap);
    // mDrawGroundSpritesComponent->SetWidth(mGame->GetTileSize());
    // mDrawGroundSpritesComponent->SetHeight(mGame->GetTileSize());
}

void Ground::SetTilesIndex(float width, float height, float x, float y) {
    int tileSize = mGame->GetOriginalTileSize();

    int rows = static_cast<int>(height / tileSize);
    int cols = static_cast<int>(width / tileSize);
    mTilesIndex.assign(rows, std::vector<int>(cols, 0));

    // Convertendo coordenadas de mundo (em pixels) para índices de tile
    int minCol = static_cast<int>(x / tileSize);
    int minRow = static_cast<int>(y / tileSize);

    int** levelData = mGame->GetLevelData();

    for (int row = 0; row < rows; ++row)
    {
        for (int col = 0; col < cols; ++col)
        {
            int srcRow = minRow + row;
            int srcCol = minCol + col;

            mTilesIndex[row][col] = levelData[srcRow][srcCol];
        }
    }

    // Define e bakeia os tiles
    if (mDrawComponent) {
        mDrawComponent->SetTilesIndex(mTilesIndex);
        mDrawComponent->BakeTilesToTexture(GetGame()->GetRenderer());
    }
}

void Ground::DestroyGround() {
    DestroyEffects();
    SetState(ActorState::Destroy);
}

void Ground::DestroyEffects() {
    auto* smoke = new ParticleSystem(mGame, Particle::ParticleType::BlurParticle, 80.0f, 25.0f, 0.35f, 0.3f);
    smoke->SetParticleColor(SDL_Color{130, 130, 130, 50});
    smoke->SetConeSpread(360.0f);
    smoke->SetParticleSpeedScale(0.2f);
    smoke->SetParticleGravity(false);
    smoke->SetEmitDirection(Vector2::Zero);
    smoke->SetPosition(GetPosition());
    smoke->SetGroundCollision(false);
}
