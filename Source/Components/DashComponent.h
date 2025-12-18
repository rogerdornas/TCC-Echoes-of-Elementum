//
// Created by roger on 25/04/2025.
//

#pragma once

#include "Component.h"

class DashComponent : public Component
{
public:
    DashComponent(class Actor* owner, float dashSpeed, float dashDuration, float dashCooldown);

    bool UseDash(bool isOnGround);
    bool GetIsDashing() const { return mIsDashing; }
    void SetHasDashedInAir(bool hasDashedInAir) { mHasDashedInAir = hasDashedInAir; }

    void StopDash();

    void SetDashSpeed(float dashSpeed) { mDashSpeed = dashSpeed; }
    float GetDashSpeed() const { return mDashSpeed; }
    void SetDashDuration(float dashDuration) { mDashDuration = dashDuration; }
    void SetDashCooldown(float dashCooldown) { mDashCooldown = dashCooldown; }

    void Update(float deltaTime) override;

    void InitDashEffect();

private:
    bool mIsDashing;
    float mDashSpeed;
    float mDashDuration;
    float mDashCooldown;
    float mDashTimer;
    float mDashCooldownTimer;
    bool mHasDashedInAir;

    class DashEffect* mDashEffect;
};
