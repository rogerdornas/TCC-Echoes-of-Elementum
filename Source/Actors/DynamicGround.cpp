//
// Created by roger on 25/05/2025.
//

#include "DynamicGround.h"
#include <unordered_map>
#include "../Game.h"
#include "../Components/RigidBodyComponent.h"
#include "../Components/AABBComponent.h"
#include "../Components/Drawing/AnimatorComponent.h"
#include "../Components/Drawing/RectComponent.h"
#include "../Components/Drawing/TileMapComponent.h"

DynamicGround::DynamicGround(Game* game, float width, float height, bool isSpike, bool isMoving,
                             float movingDuration, Vector2 velocity, bool groundBehindPlayer, bool usePadding)
    :Ground(game, width, height, isSpike, isMoving, movingDuration, velocity, groundBehindPlayer, usePadding)
    ,mMaxWidth(width)
    ,mMaxHeight(height)
    ,mGrowSpeed(Vector2::Zero)
    ,mIsGrowing(false)
    ,mIsDecreasing(false)
    ,mIsOscillating(false)
    ,mGrowthDirection(GrowthDirection::Centered)
{
    if (mDrawComponent) {
        int drawOrder;
        if (groundBehindPlayer) {
            drawOrder = 100;
        }
        else {
            drawOrder = 1004;
        }
        mDrawComponent->SetDrawOrder(drawOrder);
    }
}

void DynamicGround::OnUpdate(float deltaTime) {
    if ((mWidth == 0 || mHeight == 0)) {
        if (mRectComponent) {
            mRectComponent->SetVisible(false);
        }
        if (mDrawComponent) {
            mDrawComponent->SetVisible(false);
        }
        if (mAABBComponent) {
            mAABBComponent->SetActive(false);
        }
    }
    else {
        if (mRectComponent) {
            mRectComponent->SetVisible(true);
        }
        if (mDrawComponent) {
            mDrawComponent->SetVisible(true);
        }
        if (mAABBComponent) {
            mAABBComponent->SetActive(true);
        }
    }
    if (mIsMoving) {
        mMovingTimer += deltaTime;
        if (mMovingTimer > mMovingDuration) {
            mRigidBodyComponent->SetVelocity(mRigidBodyComponent->GetVelocity() * -1);
            mMovingTimer = 0;
        }
    }
    if (mIsOscillating) {
        if (mWidth >= mMaxWidth && mHeight >= mMaxHeight) {
            mIsGrowing = false;
            mIsDecreasing = true;
        }
        if (mWidth <= mMinWidth && mHeight <= mMinHeight) {
            mIsDecreasing = false;
            mIsGrowing = true;
        }
    }
    float growX = 0;
    float growY = 0;
    if (mIsGrowing) {
        // fazer isso para não ficar tremendo os chãos
        growX = std::ceil(mGrowSpeed.x * deltaTime);
        growY = std::ceil(mGrowSpeed.y * deltaTime);

        if (mWidth + growX > mMaxWidth) {
            growX = mMaxWidth - mWidth;
        }
        if (mHeight + growY > mMaxHeight) {
            growY = mMaxHeight - mHeight;
        }
    }
    if (mIsDecreasing) {
        // fazer isso para não ficar tremendo os chãos
        growX = std::floor(-mGrowSpeed.x * deltaTime);
        growY = std::floor(-mGrowSpeed.y * deltaTime);

        if (mWidth + growX < mMinWidth) {
            growX = mMinWidth - mWidth;
        }
        if (mHeight + growY < mMinHeight) {
            growY = mMinHeight - mHeight;
        }
    }

    mWidth += growX;
    mHeight += growY;

    if (mWidth >= mMaxWidth && mHeight >= mMaxHeight) {
        mIsGrowing = false;
    }
    if (mWidth <= mMinWidth && mHeight <= mMinHeight) {
        mIsDecreasing = false;
    }

    Vector2 v1(-mWidth / 2, -mHeight / 2);
    Vector2 v2(mWidth / 2, -mHeight / 2);
    Vector2 v3(mWidth / 2, mHeight / 2);
    Vector2 v4(-mWidth / 2, mHeight / 2);

    std::vector<Vector2> vertices;
    vertices.emplace_back(v1);
    vertices.emplace_back(v2);
    vertices.emplace_back(v3);
    vertices.emplace_back(v4);

    if (auto* aabb = dynamic_cast<AABBComponent*>(mAABBComponent)) {
        aabb->SetMin(v1);
        aabb->SetMax(v3);
    }
    if (mRectComponent) {
        // mDrawPolygonComponent->SetVertices(vertices);
        mRectComponent->SetWidth(mWidth);
        mRectComponent->SetHeight(mHeight);
    }

    if (mDrawComponent) {
        mDrawComponent->SetCurrentWidth(mWidth);
        mDrawComponent->SetCurrentHeight(mHeight);
    }

    switch (mGrowthDirection) {
        case GrowthDirection::Right:
            SetPosition(Vector2(GetPosition().x + growX / 2.0f, GetPosition().y));
            break;
        case GrowthDirection::Left:
            SetPosition(Vector2(GetPosition().x - growX / 2.0f, GetPosition().y));
            break;
        case GrowthDirection::Up:
            SetPosition(Vector2(GetPosition().x, GetPosition().y - growY / 2.0f));
            break;
        case GrowthDirection::Down:
            SetPosition(Vector2(GetPosition().x, GetPosition().y + growY / 2.0f));
            break;
        default:
            break;
    }
}

void DynamicGround::SetSprites() {
    int rows = mMaxHeight / mGame->GetTileSize();
    int cols = mMaxWidth / mGame->GetTileSize();

    float topLeftX = mStartingPosition.x - mMaxWidth / 2;
    float topLeftY = mStartingPosition.y - mMaxHeight / 2;

    int minRow = topLeftY / mGame->GetTileSize();
    int maxRow = minRow + rows;

    int minCol = topLeftX / mGame->GetTileSize();
    int maxCol = minCol + cols;

    std::unordered_map<int, std::vector<Vector2> > spriteOffsetMap;

    int** levelData = mGame->GetLevelDataDynamicGrounds();

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
    // mDrawDynamicGroundSpritesComponent->SetSpriteOffsetMap(spriteOffsetMap);
    // mDrawDynamicGroundSpritesComponent->SetWidth(mGame->GetTileSize());
    // mDrawDynamicGroundSpritesComponent->SetHeight(mGame->GetTileSize());
}

void DynamicGround::SetTilesIndex(float width, float height, float x, float y) {
    int rows = height / mGame->GetOriginalTileSize();
    int cols = width / mGame->GetOriginalTileSize();
    mTilesIndex.resize(rows, std::vector<int>(cols));

    float topLeftX = x;
    float topLeftY = y;

    int minRow = topLeftY / mGame->GetOriginalTileSize();
    int maxRow = minRow + rows;

    int minCol = topLeftX / mGame->GetOriginalTileSize();
    int maxCol = minCol + cols;

    int** levelData = mGame->GetLevelDataDynamicGrounds();

    for (int row = 0; row < rows; ++row) {
        for (int col = 0; col < cols; ++col) {
            int tile = levelData[row + minRow][col + minCol];
            mTilesIndex[row][col] = tile;
        }
    }

    // Define e bakeia os tiles
    if (mDrawComponent) {
        mDrawComponent->SetTilesIndex(mTilesIndex);
        mDrawComponent->BakeTilesToTexture(GetGame()->GetRenderer());
        mDrawComponent->SetGrowDirection(mGrowthDirection);
    }
}
