//
// Created by roger on 16/08/2025.
//

#include "OBBComponent.h"
#include <cfloat>
#include "../Actors/Actor.h"
#include "../Game.h"
#include "../Math.h"
#include "RigidBodyComponent.h"

OBBComponent::OBBComponent(class Actor *owner, Vector2 halfSize, Vector2 offset, bool autoRegister)
    :ColliderComponent(owner, ColliderType::OBB, offset, autoRegister)
    ,mHalfSize(halfSize)
{
    float c = cosf(mOwner->GetRotation());
    float s = sinf(mOwner->GetRotation());
    mAxis[0] = Vector2(c, s);
    mAxis[1] = Vector2(-s, c);
}

void OBBComponent::Update(float deltaTime) {
    // Atualiza eixos quando a rotação muda
    float c = cosf(mOwner->GetRotation());
    float s = sinf(mOwner->GetRotation());
    mAxis[0] = Vector2(c, s);
    mAxis[1] = Vector2(-s, c);
}

void OBBComponent::Draw(Renderer* renderer) {
    if (!mDebugDraw || !mIsActive) {
        return;
    }

    // 1. Calcula o tamanho do retângulo
    float width = mHalfSize.x * 2.0f;
    float height = mHalfSize.y * 2.0f;
    Vector2 size(width, height);

    // 2. Calcula o deslocamento do centro da caixa em relação ao Actor
    Vector2 drawPos = GetOwner()->GetPosition() + mOffset;

    // 3. Desenha
    GetGame()->GetRenderer()->DrawRect(
        drawPos,
        size,
        mOwner->GetRotation(),
        mColor,
        GetGame()->GetCamera()->GetPosCamera(),
        RendererMode::LINES
    );
}

std::vector<Vector2> OBBComponent::GetVertices() {
    std::vector<Vector2> verts;
    Vector2 right = mAxis[0] * mHalfSize.x;
    Vector2 up    = mAxis[1] * mHalfSize.y;

    verts.emplace_back(-1 * right - up);
    verts.emplace_back(right - up);
    verts.emplace_back(right + up);
    verts.emplace_back(-1 * right + up);

    return verts;
}


bool OBBComponent::Intersect(ColliderComponent &other) {
    if (!mIsActive || !other.IsActive()) {
        return false;
    }

    switch (other.GetType()) {
        case ColliderType::AABB:
            return IntersectWithAABB(*dynamic_cast<AABBComponent*>(&other));
        break;
        case ColliderType::OBB:
            return IntersectWithOBB(*dynamic_cast<OBBComponent*>(&other));
        break;
    }
    return false;
}

bool OBBComponent::IntersectWithAABB(AABBComponent& other) {
    // Obtém vértices do AABB
    Vector2 AABBPosition = other.GetOwner()->GetPosition() + other.GetOffset();
    std::vector<Vector2> vertsAABB = {
        Vector2(other.GetMin().x + AABBPosition.x, other.GetMin().y + AABBPosition.y),
        Vector2(other.GetMax().x + AABBPosition.x, other.GetMin().y + AABBPosition.y),
        Vector2(other.GetMax().x + AABBPosition.x, other.GetMax().y + AABBPosition.y),
        Vector2(other.GetMin().x + AABBPosition.x, other.GetMax().y + AABBPosition.y)
    };

    // Obtém vértices do OBB
    auto vertsOBB = GetVertices();
    for (int i = 0; i < vertsOBB.size(); i++) {
        vertsOBB[i] += GetOwner()->GetPosition() + mOffset;
    }

    // Eixos para testar (SAT)
    Vector2 axes[4] = {
        Vector2(1, 0),      // eixo X global (AABB)
        Vector2(0, 1),      // eixo Y global (AABB)
        GetAxis()[0],               // eixo X do OBB
        GetAxis()[1]                // eixo Y do OBB
    };

    // Função auxiliar para projetar pontos num eixo
    auto projectOntoAxis = [](const std::vector<Vector2>& verts, const Vector2& axis, float& min, float& max) {
        min = max = Vector2::Dot(verts[0],axis);
        for (int i = 1; i < 4; i++) {
            float p = Vector2::Dot(verts[i],axis);
            if (p < min) min = p;
            if (p > max) max = p;
        }
    };

    // SAT: testa todos os eixos
    for (int i = 0; i < 4; i++) {
        float minA, maxA, minB, maxB;
        projectOntoAxis(vertsAABB, axes[i], minA, maxA);
        projectOntoAxis(vertsOBB,  axes[i], minB, maxB);

        if (maxA < minB || maxB < minA) {
            return false; // separação → sem colisão
        }
    }

    return true; // colidiu
}

bool OBBComponent::IntersectWithOBB(OBBComponent &other) {
    // Obtém vértices do OBB1
    auto vertsOBB1 = GetVertices();
    for (int i = 0; i < vertsOBB1.size(); i++) {
        vertsOBB1[i] += GetOwner()->GetPosition() + mOffset;
    }

    // Obtém vértices do OBB2
    auto vertsOBB2 = other.GetVertices();
    for (int i = 0; i < vertsOBB2.size(); i++) {
        vertsOBB2[i] += other.GetOwner()->GetPosition() + other.GetOffset();
    }

    // Eixos para testar (SAT)
    Vector2 axes[4] = {
        GetAxis()[0],               // eixo X do OBB1
        GetAxis()[1],               // eixo Y do OBB2
        other.GetAxis()[0],         // eixo X do OBB2
        other.GetAxis()[1]          // eixo Y do OBB2
    };

    // Função auxiliar para projetar pontos num eixo
    auto projectOntoAxis = [](const std::vector<Vector2>& verts, const Vector2& axis, float& min, float& max) {
        min = max = Vector2::Dot(verts[0],axis);
        for (int i = 1; i < 4; i++) {
            float p = Vector2::Dot(verts[i],axis);
            if (p < min) min = p;
            if (p > max) max = p;
        }
    };

    // SAT: testa todos os eixos
    for (int i = 0; i < 4; i++) {
        float minA, maxA, minB, maxB;
        projectOntoAxis(vertsOBB1, axes[i], minA, maxA);
        projectOntoAxis(vertsOBB2,  axes[i], minB, maxB);

        if (maxA < minB || maxB < minA) {
            return false; // separação → sem colisão
        }
    }

    return true; // colidiu
}

Vector2 OBBComponent::ResolveCollision(ColliderComponent &other) {
    switch (other.GetType()) {
        case ColliderType::AABB:
            return ResolveCollisionWithAABB(*dynamic_cast<AABBComponent*>(&other));
        break;
        case ColliderType::OBB:
            return ResolveCollisionWithOBB(*dynamic_cast<OBBComponent*>(&other));
        break;
    }
    return Vector2::Zero;
}

Vector2 OBBComponent::ResolveCollisionWithAABB(AABBComponent &other) {
    // Obtém vértices do AABB
    Vector2 AABBPosition = other.GetOwner()->GetPosition() + other.GetOffset();
    std::vector<Vector2> vertsAABB = {
        Vector2(other.GetMin().x + AABBPosition.x, other.GetMin().y + AABBPosition.y),
        Vector2(other.GetMax().x + AABBPosition.x, other.GetMin().y + AABBPosition.y),
        Vector2(other.GetMax().x + AABBPosition.x, other.GetMax().y + AABBPosition.y),
        Vector2(other.GetMin().x + AABBPosition.x, other.GetMax().y + AABBPosition.y)
    };

    // Obtém vértices do OBB
    auto vertsOBB = GetVertices();
    for (int i = 0; i < vertsOBB.size(); i++) {
        vertsOBB[i] += GetOwner()->GetPosition() + mOffset;
    }

    // Eixos para testar (SAT)
    Vector2 axes[4] = {
        Vector2(1, 0),      // eixo X global (AABB)
        Vector2(0, 1),      // eixo Y global (AABB)
        GetAxis()[0],               // eixo X do OBB
        GetAxis()[1]                // eixo Y do OBB
    };

    // Função auxiliar para projetar pontos num eixo
    auto projectOntoAxis = [](const std::vector<Vector2>& verts, const Vector2& axis, float& min, float& max) {
        min = max = Vector2::Dot(verts[0],axis);
        for (int i = 1; i < 4; i++) {
            float p = Vector2::Dot(verts[i],axis);
            if (p < min) min = p;
            if (p > max) max = p;
        }
    };

    Vector2 normal;
    float penetration = FLT_MAX;
    // SAT: testa todos os eixos
    for (int i = 0; i < 4; i++) {
        float minA, maxA, minB, maxB;
        projectOntoAxis(vertsOBB, axes[i], minA, maxA);
        projectOntoAxis(vertsAABB,  axes[i], minB, maxB);

        if (maxA < minB || maxB < minA) {
            return Vector2::Zero; // não colidiu
        }

        // Calcula quanto penetrou neste eixo
        float overlap = std::min(maxA, maxB) - std::max(minA, minB);
        if (overlap < penetration) {
            penetration = overlap;
            normal = axes[i];
        }
    }

    // Decide direção correta do vetor normal
    Vector2 centerA = GetOwner()->GetPosition() + mOffset;
    Vector2 dir = other.GetOwner()->GetPosition() + other.GetOffset() - centerA;
    if (Vector2::Dot(dir, normal) > 0) {
        normal = normal * -1.0f;
    }

    // Vetor de correção
    Vector2 correction = normal * penetration;

    // Move o AABB para fora do OBB
    GetOwner()->SetPosition(centerA + correction);

    return normal;
}

Vector2 OBBComponent::ResolveCollisionWithOBB(OBBComponent &other) {
     // Obtém vértices do OBB1
    auto vertsOBB1 = GetVertices();
    for (int i = 0; i < vertsOBB1.size(); i++) {
        vertsOBB1[i] += GetOwner()->GetPosition() + mOffset;
    }

    // Obtém vértices do OBB2
    auto vertsOBB2 = other.GetVertices();
    for (int i = 0; i < vertsOBB2.size(); i++) {
        vertsOBB2[i] += other.GetOwner()->GetPosition() + other.GetOffset();
    }

    // Eixos para testar (SAT)
    Vector2 axes[4] = {
        GetAxis()[0],               // eixo X do OBB1
        GetAxis()[1],               // eixo Y do OBB2
        other.GetAxis()[0],         // eixo X do OBB2
        other.GetAxis()[1]          // eixo Y do OBB2
    };

    // Função auxiliar para projetar pontos num eixo
    auto projectOntoAxis = [](const std::vector<Vector2>& verts, const Vector2& axis, float& min, float& max) {
        min = max = Vector2::Dot(verts[0],axis);
        for (int i = 1; i < 4; i++) {
            float p = Vector2::Dot(verts[i],axis);
            if (p < min) min = p;
            if (p > max) max = p;
        }
    };

    Vector2 normal;
    float penetration = FLT_MAX;
    // SAT: testa todos os eixos
    for (int i = 0; i < 4; i++) {
        float minA, maxA, minB, maxB;
        projectOntoAxis(vertsOBB1, axes[i], minA, maxA);
        projectOntoAxis(vertsOBB2,  axes[i], minB, maxB);

        if (maxA < minB || maxB < minA) {
            return Vector2::Zero; // não colidiu
        }

        // Calcula quanto penetrou neste eixo
        float overlap = std::min(maxA, maxB) - std::max(minA, minB);
        if (overlap < penetration) {
            penetration = overlap;
            normal = axes[i];
        }
    }

    // Decide direção correta do vetor normal
    Vector2 centerA = GetOwner()->GetPosition() + mOffset;
    Vector2 dir = other.GetOwner()->GetPosition() + other.GetOffset() - centerA;
    if (Vector2::Dot(dir, normal) > 0) {
        normal = normal * -1.0f;
    }

    // Vetor de correção
    Vector2 correction = normal * penetration;

    // Move o AABB para fora do OBB
    GetOwner()->SetPosition(centerA + correction);

    return normal;
}

Vector2 OBBComponent::CollisionSide(ColliderComponent &other) {
    switch (other.GetType()) {
        case ColliderType::AABB:
            return CollisionSideWithAABB(*dynamic_cast<AABBComponent*>(&other));
        break;
        case ColliderType::OBB:
            return CollisionSideWithOBB(*dynamic_cast<OBBComponent*>(&other));
        break;
    }
    return Vector2::Zero;
}

Vector2 OBBComponent::CollisionSideWithAABB(AABBComponent &other) {
// Obtém vértices do AABB
    Vector2 AABBPosition = other.GetOwner()->GetPosition() + other.GetOffset();
    std::vector<Vector2> vertsAABB = {
        Vector2(other.GetMin().x + AABBPosition.x, other.GetMin().y + AABBPosition.y),
        Vector2(other.GetMax().x + AABBPosition.x, other.GetMin().y + AABBPosition.y),
        Vector2(other.GetMax().x + AABBPosition.x, other.GetMax().y + AABBPosition.y),
        Vector2(other.GetMin().x + AABBPosition.x, other.GetMax().y + AABBPosition.y)
    };

    // Obtém vértices do OBB
    auto vertsOBB = GetVertices();
    for (int i = 0; i < vertsOBB.size(); i++) {
        vertsOBB[i] += GetOwner()->GetPosition() + mOffset;
    }

    // Eixos para testar (SAT)
    Vector2 axes[4] = {
        Vector2(1, 0),      // [0] eixo X global (AABB)
        Vector2(0, 1),      // [1] eixo Y global (AABB)
        GetAxis()[0],               // [2] eixo X do OBB
        GetAxis()[1]                // [3] eixo Y do OBB
    };

    // Função auxiliar para projetar pontos num eixo
    auto projectOntoAxis = [](const std::vector<Vector2>& verts, const Vector2& axis, float& min, float& max) {
        min = max = Vector2::Dot(verts[0], axis);
        for (int i = 1; i < 4; i++) {
            float p = Vector2::Dot(verts[i], axis);
            if (p < min) min = p;
            if (p > max) max = p;
        }
    };

    Vector2 normal;
    float minAABBOverlap = FLT_MAX; // Variável dedicada apenas para o AABB

    // SAT: testa todos os eixos
    for (int i = 0; i < 4; i++) {
        float minA, maxA, minB, maxB;
        projectOntoAxis(vertsOBB, axes[i], minA, maxA);
        projectOntoAxis(vertsAABB, axes[i], minB, maxB);

        if (maxA < minB || maxB < minA) {
            return Vector2::Zero; // separação → não colidiu
        }

        // Se chegou aqui, os eixos se sobrepõem. Calcula quanto penetrou
        float overlap = std::min(maxA, maxB) - std::max(minA, minB);

        // Só guardamos a normal e a penetração se o eixo pertencer ao AABB (índices 0 e 1)
        if (i < 2 && overlap < minAABBOverlap) {
            minAABBOverlap = overlap;
            normal = axes[i];
        }
    }

    // Decide direção correta do vetor normal (tem que apontar para FORA do AABB)
    Vector2 centerA = GetOwner()->GetPosition() + mOffset;
    Vector2 dir = other.GetOwner()->GetPosition() + other.GetOffset() - centerA;

    // Se o produto escalar for positivo, a normal tá apontando pro AABB. Invertemos ela.
    if (Vector2::Dot(dir, normal) > 0) {
        normal = normal * -1.0f;
    }

    return normal;
}

Vector2 OBBComponent::CollisionSideWithOBB(OBBComponent &other) {
     // Obtém vértices do OBB1
    auto vertsOBB1 = GetVertices();
    for (int i = 0; i < vertsOBB1.size(); i++) {
        vertsOBB1[i] += GetOwner()->GetPosition() + mOffset;
    }

    // Obtém vértices do OBB2
    auto vertsOBB2 = other.GetVertices();
    for (int i = 0; i < vertsOBB2.size(); i++) {
        vertsOBB2[i] += other.GetOwner()->GetPosition() + other.GetOffset();
    }

    // Eixos para testar (SAT)
    Vector2 axes[4] = {
        GetAxis()[0],               // eixo X do OBB1
        GetAxis()[1],               // eixo Y do OBB2
        other.GetAxis()[0],         // eixo X do OBB2
        other.GetAxis()[1]          // eixo Y do OBB2
    };

    // Função auxiliar para projetar pontos num eixo
    auto projectOntoAxis = [](const std::vector<Vector2>& verts, const Vector2& axis, float& min, float& max) {
        min = max = Vector2::Dot(verts[0],axis);
        for (int i = 1; i < 4; i++) {
            float p = Vector2::Dot(verts[i],axis);
            if (p < min) min = p;
            if (p > max) max = p;
        }
    };

    Vector2 normal;
    float penetration = FLT_MAX;
    // SAT: testa todos os eixos
    for (int i = 0; i < 4; i++) {
        float minA, maxA, minB, maxB;
        projectOntoAxis(vertsOBB1, axes[i], minA, maxA);
        projectOntoAxis(vertsOBB2,  axes[i], minB, maxB);

        if (maxA < minB || maxB < minA) {
            return Vector2::Zero; // não colidiu
        }

        // Calcula quanto penetrou neste eixo
        float overlap = std::min(maxA, maxB) - std::max(minA, minB);
        if (overlap < penetration) {
            penetration = overlap;
            normal = axes[i];
        }
    }

    // Decide direção correta do vetor normal
    Vector2 centerA = GetOwner()->GetPosition() + mOffset;
    Vector2 dir = other.GetOwner()->GetPosition() + other.GetOffset() - centerA;
    if (Vector2::Dot(dir, normal) > 0) {
        normal = normal * -1.0f;
    }

    return normal;
}
