#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include "../Math.h"
#include "UIText.h"
#include "UIButton.h"
#include "UIImage.h"
#include "UISlider.h"
#include "UISkillNodeButton.h"

class UIScreen
{
public:
    // Tracks if the UI is active or closing
    enum class UIState
    {
        Active,
        Closing
    };

	UIScreen(class Game* game, const std::string& fontName, bool isClosable = true);
	virtual ~UIScreen();

	// UIScreen subclasses can override these
	virtual void Update(float deltaTime);
	virtual void Draw(class Renderer *renderer);
	virtual void ProcessInput(const uint8_t* keys);
	virtual void HandleKeyPress(int key, int controllerButton, int leftControllerAxisY, int leftControllerAxisX, int rightControllerAxisY, int rightControllerAxisX);
	virtual void HandleKeyRelease(int key, int controllerButton);
	virtual void HandleMousePress(const Vector2& virtualMousePos);
	virtual void HandleMouseRelease(const Vector2& virtualMousePos);
	virtual void HandleMouseMotion(const Vector2& virtualMousePos);

	void SetPosition(Vector2 position) { mPos = position; }
	void SetSize(Vector2 size) { mSize = size; }
	Vector2 GetPosition() const { return mPos; }
	Vector2 GetSize() const { return mSize; }

    // Set state to closing
	virtual void Close();

    // Get state of UI screen
	UIState GetState() const { return mState; }

	void SetIsVisible(bool isVisible) { mIsVisible = isVisible; }

    // Game getter
    class Game* GetGame() const { return mGame; }

	// Buttons getter
	std::vector<UIButton *> GetButtons() { return mButtons; }

	// Texts getter
	std::vector<UIText *> GetTexts() { return mTexts; }
	virtual void RefreshTexts() {}

    // Add a button to this screen
	UIButton* AddButton(const std::string& name, const Vector2& pos, const Vector2& dims, const int pointSize, UIButton::TextPos alignText, std::function<void()> onClick, bool useTextSize = false, Vector2 textPos = Vector2::Zero, Vector3 textColor = Color::White, unsigned textWrapLength = 1024);
    UIText* AddText(const std::string& name, const Vector2& pos = Vector2::Zero, const int pointSize = 40, Vector3 color = Color::White, const int unsigned wrapLength = 1024);
    UIImage* AddImage(const std::string& imagePath, const Vector2& pos, const Vector2& dims, const Vector3& color = Color::White);
	UISlider* AddSlider(const std::string& name, const Vector2& pos, const Vector2& size, const Vector2& sliderOffset, const Vector2& sliderSize, float minVal, float maxVal, float initialVal, int textPointSize, int valuePointSize, std::function<void(float)> onValueChanged);
	UISkillNodeButton* AddSkillNodeButton(const std::string& skillId, UISkillNodeButton::NodeType nodeType, const Vector2& pos, std::function<void()> onClick, std::function<void(const std::string&)> onSelect);

	bool IsClosable() { return mIsClosable; }

	virtual UIButton* FindNeighbor(UIButton* current, const Vector2& dir);

protected:
    // Sets the mouse mode to relative or not
	class Game* mGame;
	class UIFont* mFont;

	// Configure positions
	Vector2 mPos;
	Vector2 mSize;

	// State
	UIState mState;

	bool mIsVisible;
	bool mIsClosable;

	UIButton* mDraggedButton;

	// List of buttons, texts, and images
    int mSelectedButtonIndex;
	std::vector<UIButton *> mButtons;
    std::vector<UIText *> mTexts;
    std::vector<UIImage *> mImages;
};
