#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <SDL.h>
#include "../Math.h"
#include "VertexArray.h"
#include "Texture.h"

enum class RendererMode
{
    TRIANGLES,
    LINES
};

enum class PostProcessEffect
{
	Blur = 0,
	Grayscale = 1,
	DamageFlash = 2,
	ChromaticAberration = 3,
	Count
};

class Renderer
{
public:
	Renderer(SDL_Window* window, class Game* game);
	~Renderer();

	bool Initialize(float width, float height);
	void InitializeFadeQuad();
	void Shutdown();
	void UnloadUnusedTextures(const std::vector<std::string>& keepTextures);

    void DrawRect(const Vector2 &position, const Vector2 &size,  float rotation,
                  const Vector3 &color, const Vector2 &cameraPos, RendererMode mode, float alpha = 1.0f);

    void DrawTexture(const Vector2 &position, const Vector2 &size,  float rotation,
                     const Vector3 &color, Texture *texture,
                     const Vector4 &textureRect = Vector4::UnitRect,
                     const Vector2 &cameraPos = Vector2::Zero, Vector2 scale = Vector2::One,
                     float textureFactor = 1.0f, float alpha = 1.0f, float freezeLevel = 0.0f,
                     bool additiveBlending = false);

    void DrawGeometry(const Vector2 &position, const Vector2 &size,  float rotation,
                      const Vector3 &color, const Vector2 &cameraPos, VertexArray *vertexArray, RendererMode mode);

	void DrawLine(const Vector2 &start, const Vector2 &end, const Vector3 &color,
						float thickness, const Vector2 &cameraPos, float alpha, bool additiveBlending = false);

	void DrawCircularBar(const Vector2& position, float radius, float thickness, float ratio,
					 const Vector3& fillColor, const Vector3& bgColor, float alpha);

	void DrawFade(float alpha);

    void Clear();
    void Present();

	void BeginGameDraw();
	void BeginUIDraw();

	// Nova função para ser chamada quando a janela for redimensionada
	void OnWindowResize(float newWidth, float newHeight);

	void SetZoom(float zoom);
	// Getter para o zoom atual
	float GetZoom() const { return mZoom; }
	// Getters para a resolução virtual "zoomada" (útil para a Câmera)
	float GetZoomedWidth() const { return mZoomedWidth; }
	float GetZoomedHeight() const { return mZoomedHeight; }

    // Getters
    class Texture* GetTexture(const std::string& fileName, bool smooth = false);
	class Shader* GetBaseShader() const { return mBaseShader; }
	class VertexArray* GetSpriteVerts() const { return mSpriteVerts; }
	// Getters para a resolução virtual (útil para o Game)
	float GetVirtualWidth() const { return mVirtualWidth; }
	float GetVirtualHeight() const { return mVirtualHeight; }

	Vector2 ScreenToVirtual(const Vector2& screenPoint) const;

	// Nova função helper para calcular e definir o viewport
	void UpdateViewport();

	bool CreateRenderTarget(int width, int height);
	Vector2 GetResolution() const { return Vector2(mRenderWidth, mRenderHeight); }

	// Iluminação
	void SetAmbientLight(const Vector3& color, float intensity);
	void AddLight(class Light* light);
	void RemoveLight(class Light* light);
	void ClearLights();

	// Pos Processamento
	void SetEffectIntensity(PostProcessEffect effect, float intensity);
	void SetAberrationAngle(float angle) { mAberrationAngle = angle; }
	void DeactivateAllEffects();

	// Map
	// Cria um FBO e uma textura específicos para a máscara de uma sala
	void CreateMaskFBO(int width, int height, unsigned int& outFBO, unsigned int& outTexture);

	// Muda o alvo da renderização para podermos "pintar" na máscara
	void BindFramebuffer(unsigned int fboID, int width, int height);

	// Restaura o alvo para o FBO da UI ou do Jogo
	void RestoreDefaultFramebuffer();

	void CreateColorFBO(int width, int height, unsigned int& outFBO, unsigned int& outTexture);

	void DrawMapRoom(const Vector2& position, const Vector2& size, class Texture* mapTexture, unsigned int maskTextureID, float viewWidth, float viewHeight);

	// Função para desenhar a textura final do FBO
	void DrawTextureByID(const Vector2 &position, const Vector2 &size, float rotation,
						const Vector3 &color, unsigned int textureID, const Vector4 &textureRect = Vector4::UnitRect,
						const Vector2 &cameraPos = Vector2::Zero, Vector2 scale = Vector2::One, float textureFactor = 1.0f,
						float alpha = 1.0f);

	void DrawFogBrush(const Vector2& centerPos, float radius, class Texture* brush, const Vector2& fboSize);

	void PrintUsedTextures();

private:
    void Draw(RendererMode mode, const Matrix4 &modelMatrix, const Vector2 &cameraPos, VertexArray *vertices,
              const Vector3 &color, float alpha = 1.0f, Texture *texture = nullptr, const Vector4 &textureRect = Vector4::UnitRect,
              float textureFactor = 1.0f);

	bool LoadShaders();
    void CreateSpriteVerts();
	void UploadLightingUniforms(); // envia luzes ao shader
	void DeactivateLighting();

	// Game
	class Game* mGame;

	// shader
	class Shader* mBaseShader;
	class Shader* mFadeShader;
	class Shader* mScreenShader;
	class Shader* mPostProcessShader;
	class Shader* mCircleShader;
	class Shader* mMapShader;
	class Shader* mBrushShader;

    // Sprite vertex array
    class VertexArray *mSpriteVerts;
	class VertexArray* mFadeVAO;
	class VertexArray* mScreenQuad;

	// Window
	SDL_Window* mWindow;

	// OpenGL context
	SDL_GLContext mContext;

	// Ortho projection for 2D shaders
	Matrix4 mOrthoProjection;

    // Map of textures loaded
    std::unordered_map<std::string, class Texture*> mTextures;

	// Controle de Resolução Interna
	float mRenderWidth;
	float mRenderHeight;

	// FBO 1: Onde o jogo e o HUD são desenhados
	unsigned int mGameFBO;
	unsigned int mGameFBOTexture;

	// FBO 2: Onde os efeitos são aplicados e a UI Limpa é desenhada
	unsigned int mUIFBO;
	unsigned int mUIFBOTexture;

	// === Dados de iluminação ===
	Vector3 mAmbientColor;
	float mAmbientIntensity;
	std::vector<Light*> mLights;

	// Pos processamento
	float mEffectIntensities[static_cast<int>(PostProcessEffect::Count)];
	float mAberrationAngle;

	// === Novas variáveis para controle de resolução ===
	float mVirtualWidth;  // Largura da nossa cena (ex: 1920)
	float mVirtualHeight; // Altura da nossa cena (ex: 1080)
	float mWindowWidth;   // Largura atual da janela
	float mWindowHeight;  // Altura atual da janela

	float mZoom;          // Nível de zoom atual
	float mZoomedWidth;   // mVirtualWidth / mZoom
	float mZoomedHeight;  // mVirtualHeight / mZoom

	int mViewportX;
	int mViewportY;
	int mViewportWidth;
	int mViewportHeight;

	bool mDrawingUI;
};