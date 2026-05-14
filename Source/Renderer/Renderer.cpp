#include <GL/glew.h>
#include "Renderer.h"
#include "Shader.h"
#include "VertexArray.h"
#include "Texture.h"
#include "../Game.h"
#include "../Actors/Light.h"

Renderer::Renderer(SDL_Window *window)
    :mBaseShader(nullptr)
    ,mFadeShader(nullptr)
    ,mScreenShader(nullptr)
    ,mPostProcessShader(nullptr)
    ,mWindow(window)
    ,mContext(nullptr)
    ,mOrthoProjection(Matrix4::Identity)
    ,mAmbientColor(Vector3(1.0f, 1.0f, 1.0f))
    ,mAmbientIntensity(0.8f)
    ,mAberrationAngle(0.0f)
    ,mVirtualWidth(1920.0f) // <-- Defina sua resolução virtual 16:9 aqui
    ,mVirtualHeight(1080.0f) // <-- Defina sua resolução virtual 16:9 aqui
    ,mWindowWidth(0.0f)
    ,mWindowHeight(0.0f)
    ,mZoom(1.0f) // <-- Inicializa o zoom como 1.0 (normal)
    ,mZoomedWidth(1920.0f) // <-- Inicializa com o valor base
    ,mZoomedHeight(1080.0f) // <-- Inicializa com o valor base
    ,mDrawingUI(false)
{
    for (int i = 0; i < static_cast<int>(PostProcessEffect::Count); ++i) {
        mEffectIntensities[i] = 0.0f;
    }
}

Renderer::~Renderer()
{
}

bool Renderer::Initialize(float width, float height)
{
    // Specify version 3.3 (core profile)
	// SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	// SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    // SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    // Enable double buffering
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

    // Force OpenGL to use hardware acceleration
    SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);

    // Create an OpenGL context
    mContext = SDL_GL_CreateContext(mWindow);

    if (!mContext) {
        SDL_Log("Falha ao criar o contexto OpenGL: %s", SDL_GetError());
        return false;
    }

    // Turn on vsync
    SDL_GL_SetSwapInterval(1);

    // Initialize GLEW
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        SDL_Log("Failed to initialize GLEW.");
        return false;
    }

    mGameFBO = 0;
    mGameFBOTexture = 0;

    mUIFBO = 0;
    mUIFBOTexture = 0;

    // Define a resolução de renderização inicial
    CreateRenderTarget(1920, 1080);

    // O mesmo quad usado no Fade serve aqui (espaço NDC -1 a 1)
    float vertices[] = {
        -1.0f,  1.0f,   0.0f, 1.0f,
        -1.0f, -1.0f,   0.0f, 0.0f,
         1.0f, -1.0f,   1.0f, 0.0f,
         1.0f,  1.0f,   1.0f, 1.0f
    };
    unsigned int indices[] = { 0, 1, 2, 0, 2, 3 };
    mScreenQuad = new VertexArray(vertices, 4, indices, 6);

    // Você precisará de um shader MUITO SIMPLES para isso
    mScreenShader = new Shader();
    mScreenShader->Load("../Shaders/Screen");

    mPostProcessShader = new Shader();
    mPostProcessShader->Load("../Shaders/PostProcess");

    // Armazena o tamanho inicial da janela
    // mWindowWidth = width;
    // mWindowHeight = height;

    int pixelWidth, pixelHeight;
    SDL_GL_GetDrawableSize(mWindow, &pixelWidth, &pixelHeight);
    // Armazena o tamanho inicial da janela (em pixels)
    mWindowWidth = static_cast<float>(pixelWidth);
    mWindowHeight = static_cast<float>(pixelHeight);

    mZoomedWidth = mVirtualWidth;
    mZoomedHeight = mVirtualHeight;

	// Make sure we can create/compile shaders
	if (!LoadShaders()) {
		SDL_Log("Failed to load shaders.");
		return false;
	}

    // Create quad for drawing sprites
    CreateSpriteVerts();

    InitializeFadeQuad();

    // Set the clear color to light grey
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    // Enable alpha blending on textures
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Create orthografic projection matrix
    mOrthoProjection = Matrix4::CreateOrtho(0.0f, mZoomedWidth, mZoomedHeight, 0.0f, -1.0f, 1.0f);
    mBaseShader->SetMatrixUniform("uOrthoProj", mOrthoProjection);

    // Set texture unit to zero (all textures will be binded to this one)
    mBaseShader->SetTextureUniform("uTexture", 0);

    // Activate shader
    mBaseShader->SetActive();

    // Calcule e defina o viewport inicial
    UpdateViewport(); // <-- ADICIONADO

    return true;
}

void Renderer::InitializeFadeQuad()
{
    // Quad em tela cheia no espaço NDC (-1 a 1)
    float vertices[] = {
        -1.0f,  1.0f,   0.0f, 1.0f,
        -1.0f, -1.0f,   0.0f, 0.0f,
         1.0f, -1.0f,   1.0f, 0.0f,
         1.0f,  1.0f,   1.0f, 1.0f
    };
    unsigned int indices[] = { 0, 1, 2, 0, 2, 3 };

    mFadeVAO = new VertexArray(vertices, 4, indices, 6);
    mFadeShader = new Shader();
    mFadeShader->Load("../Shaders/Fade"); // fade.vert + fade.frag
}

void Renderer::Shutdown()
{
    // Destroy textures
    for (auto i : mTextures)
    {
        i.second->Unload();
        delete i.second;
    }
    mTextures.clear();

    // Destroy Shaders
    if (mBaseShader) { mBaseShader->Unload(); delete mBaseShader; }
    if (mFadeShader) { mFadeShader->Unload(); delete mFadeShader; }
    if (mScreenShader) { mScreenShader->Unload(); delete mScreenShader; }
    if (mPostProcessShader) { mPostProcessShader->Unload(); delete mPostProcessShader; }
    if (mCircleShader) { mCircleShader->Unload(); delete mCircleShader; }

    // Destroy Vertex Arrays
    delete mSpriteVerts;
    delete mFadeVAO;
    delete mScreenQuad;

    SDL_GL_DeleteContext(mContext);
	SDL_DestroyWindow(mWindow);
}

void Renderer::UnloadUnusedTextures(const std::vector<std::string>& keepTextures)
{
    for (auto it = mTextures.begin(); it != mTextures.end(); ) {
        // Verifica se o caminho atual está na lista de texturas para manter
        bool shouldKeep = false;
        for (const auto& path : keepTextures) {
            if (it->first == path) {
                shouldKeep = true;
                break;
            }
        }

        if (!shouldKeep) {
            it->second->Unload();
            delete it->second;
            it = mTextures.erase(it); // Remove do map e avança
        }
        else {
            ++it; // Apenas avança, mantendo a textura viva
        }
    }
}

void Renderer::Clear()
{
    // Diz ao OpenGL para desenhar na nossa textura de resolução interna
    glBindFramebuffer(GL_FRAMEBUFFER, mGameFBO);

    // Define o Viewport para a resolução do jogo
    glViewport(0, 0, static_cast<int>(mRenderWidth), static_cast<int>(mRenderHeight));

    // Limpa o fundo do jogo
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // Cor de fundo do cenário
    glClear(GL_COLOR_BUFFER_BIT);
}

void Renderer::Draw(RendererMode mode, const Matrix4 &modelMatrix, const Vector2 &cameraPos, VertexArray *vertices,
                    const Vector3 &color, float alpha, Texture *texture, const Vector4 &textureRect,
                    float textureFactor)
{
    // Transforma posição da câmera em int para não ter tremor
    Vector2 cameraInt(std::floor(cameraPos.x), std::floor(cameraPos.y));

    mBaseShader->SetMatrixUniform("uOrthoProj", mOrthoProjection);
    mBaseShader->SetMatrixUniform("uWorldTransform", modelMatrix);
    mBaseShader->SetVectorUniform("uColor", color);
    mBaseShader->SetVectorUniform("uTexRect", textureRect);
    mBaseShader->SetVectorUniform("uCameraPos", cameraInt);
    mBaseShader->SetFloatUniform("uAlpha", alpha);

    if (mDrawingUI) {
        mOrthoProjection = Matrix4::CreateOrtho(0.0f, mVirtualWidth, mVirtualHeight, 0.0f, -1.0f, 1.0f);
        mBaseShader->SetMatrixUniform("uOrthoProj", mOrthoProjection);
        DeactivateLighting();

    }
    else {
        mOrthoProjection = Matrix4::CreateOrtho(0.0f, mZoomedWidth, mZoomedHeight, 0.0f, -1.0f, 1.0f);
        mBaseShader->SetMatrixUniform("uOrthoProj", mOrthoProjection);
        UploadLightingUniforms();
    }

    if(vertices)
    {
        vertices->SetActive();
    }

    if(texture)
    {
        texture->SetActive();
        mBaseShader->SetFloatUniform("uTextureFactor", textureFactor);
    }
    else {
        glBindTexture(GL_TEXTURE_2D, 0); // desativa qualquer textura anterior
        mBaseShader->SetFloatUniform("uTextureFactor", 0.0f);
    }

    if (mode == RendererMode::LINES)
    {
        glDrawElements(GL_LINE_LOOP, vertices->GetNumIndices(), GL_UNSIGNED_INT,nullptr);
    }
    else if(mode == RendererMode::TRIANGLES)
    {
        glDrawElements(GL_TRIANGLES, vertices->GetNumIndices(), GL_UNSIGNED_INT,nullptr);
    }
}

void Renderer::DrawRect(const Vector2 &position, const Vector2 &size, float rotation, const Vector3 &color,
                        const Vector2 &cameraPos, RendererMode mode, float alpha)
{
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    Matrix4 model = Matrix4::CreateScale(Vector3(size.x, size.y, 1.0f)) *
                    Matrix4::CreateRotationZ(rotation) *
                    Matrix4::CreateTranslation(Vector3(position.x, position.y, 0.0f));

    Draw(mode, model, cameraPos, mSpriteVerts, color, alpha);
}

void Renderer::DrawTexture(const Vector2 &position, const Vector2 &size, float rotation, const Vector3 &color,
                           Texture *texture, const Vector4 &textureRect, const Vector2 &cameraPos, Vector2 scale,
                           float textureFactor, float alpha, float freezeLevel, bool additiveBlending)
{
    if (additiveBlending) {
        glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    }
    else {
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    }
    // Transforma para int a posição para não ter tremor
    Vector3 pos(std::floor(position.x), std::floor(position.y), std::floor(0.0f));

    Matrix4 model = Matrix4::CreateScale(Vector3(size.x * scale.x, size.y * scale.y, 1.0f)) *
                    Matrix4::CreateRotationZ(rotation) *
                    Matrix4::CreateTranslation(pos);

    mBaseShader->SetFloatUniform("uFreezeLevel", freezeLevel);

    Draw(RendererMode::TRIANGLES, model, cameraPos, mSpriteVerts, color, alpha, texture, textureRect, textureFactor);
}

void Renderer::DrawGeometry(const Vector2 &position, const Vector2 &size, float rotation, const Vector3 &color,
                            const Vector2 &cameraPos, VertexArray *vertexArray, RendererMode mode)
{
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    Matrix4 model = Matrix4::CreateScale(Vector3(size.x, size.y, 1.0f)) *
                    Matrix4::CreateRotationZ(rotation) *
                    Matrix4::CreateTranslation(Vector3(position.x, position.y, 0.0f));

    Draw(mode, model, cameraPos, vertexArray, color);
}

void Renderer::DrawLine(const Vector2 &start, const Vector2 &end, const Vector3 &color,
                        float thickness, const Vector2 &cameraPos, float alpha, bool additiveBlending)
{
    if (additiveBlending) {
        glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    }
    else {
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    }
    Vector2 lineVector = end - start;
    float lineLength = lineVector.Length();

    if (lineLength < 0.01f) {
        return;
    }

    Vector2 center = (start + end) / 2.0f;
    float rotation = std::atan2(lineVector.y, lineVector.x);

    Matrix4 model = Matrix4::CreateScale(Vector3(lineLength, thickness, 1.0f)) *
                    Matrix4::CreateRotationZ(rotation) *
                    Matrix4::CreateTranslation(Vector3(center.x, center.y, 0.0f));

    Draw(RendererMode::TRIANGLES, model, cameraPos, mSpriteVerts, color, alpha);
}

void Renderer::DrawCircularBar(const Vector2& position, float radius, float thickness, float ratio,
                               const Vector3& fillColor, const Vector3& bgColor, float alpha)
{
    // Garante que o blend está ativado para as bordas suaves e transparência funcionarem
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Como mSpriteVerts vai de -0.5 a 0.5 (tamanho 1), escalar pelo diâmetro (raio * 2)
    // faz o quad ter exatamente o tamanho do círculo na tela.
    float diameter = radius * 2.0f;
    Matrix4 model = Matrix4::CreateScale(Vector3(diameter, diameter, 1.0f)) *
                    Matrix4::CreateTranslation(Vector3(position.x, position.y, 0.0f));

    // Ativa nosso novo shader de círculo
    mCircleShader->SetActive();
    if (mDrawingUI) {
        mOrthoProjection = Matrix4::CreateOrtho(0.0f, mVirtualWidth, mVirtualHeight, 0.0f, -1.0f, 1.0f);
    }
    else {
        mOrthoProjection = Matrix4::CreateOrtho(0.0f, mZoomedWidth, mZoomedHeight, 0.0f, -1.0f, 1.0f);
    }
    mCircleShader->SetMatrixUniform("uOrthoProj", mOrthoProjection);
    mCircleShader->SetMatrixUniform("uWorldTransform", model);

    // Passa os uniformes da barra
    mCircleShader->SetFloatUniform("uRatio", ratio);
    mCircleShader->SetVectorUniform("uFillColor", fillColor);
    mCircleShader->SetVectorUniform("uBgColor", bgColor);
    mCircleShader->SetFloatUniform("uAlpha", alpha);

    // A espessura no shader precisa ser uma porcentagem do raio (0.0 a 1.0)
    mCircleShader->SetFloatUniform("uThickness", thickness / radius);

    mSpriteVerts->SetActive();
    glDrawElements(GL_TRIANGLES, mSpriteVerts->GetNumIndices(), GL_UNSIGNED_INT, nullptr);

    mBaseShader->SetActive();
}

void Renderer::DrawFade(float alpha)
{
    if (!mFadeShader || !mFadeVAO)
        return;

    // Ativa blending (já ativo em geral, mas garantimos)
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Usa o shader de fade
    mFadeShader->SetActive();
    mFadeShader->SetFloatUniform("uAlpha", alpha);

    // Desenha o quad
    mFadeVAO->SetActive();
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

    // Restaura o shader principal
    mBaseShader->SetActive();
}

void Renderer::Present()
{
    // Volta para o monitor (janela padrão do SO)
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // Limpa a janela real (as bordas pretas)
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    // Define onde a tela será desenhada na janela (usando suas variáveis calculadas no UpdateViewport)
    glViewport(mViewportX, mViewportY, mViewportWidth, mViewportHeight);

    // Desenha o Quad com a textura do jogo
    glDisable(GL_BLEND); // Desativa blend para desenhar a tela mais rápido

    mScreenShader->SetActive();
    mScreenQuad->SetActive();

    glBindTexture(GL_TEXTURE_2D, mUIFBOTexture);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

    glEnable(GL_BLEND); // Reativa o blend

    // Restaura o shader principal
    mBaseShader->SetActive();

    // Swap the buffers
    SDL_GL_SwapWindow(mWindow);
}

void Renderer::SetAmbientLight(const Vector3& color, float intensity)
{
    mAmbientColor = color;
    mAmbientIntensity = intensity;
}

void Renderer::AddLight(Light* light)
{
    if (mLights.size() < 80) // mesmo limite que o shader
        mLights.push_back(light);
}

void Renderer::RemoveLight(Light* light) {
    auto iter = std::find(mLights.begin(), mLights.end(), light);
    if (iter != mLights.end()) {
        mLights.erase(iter);
    }
}

void Renderer::ClearLights()
{
    mLights.clear();
}

bool Renderer::LoadShaders()
{
	// Create sprite shader
	mBaseShader = new Shader();
	if (!mBaseShader->Load("../Shaders/Base")) {
		return false;
	}

    mBaseShader->SetActive(); // Ativa para configurar
    mBaseShader->SetMatrixUniform("uOrthoProj", mOrthoProjection);
    mBaseShader->SetTextureUniform("uTexture", 0);

    mCircleShader = new Shader();
    if (!mCircleShader->Load("../Shaders/Circle")) {
        SDL_Log("Falha ao carregar Circle shader.");
        return false;
    }

    return true;
}

void Renderer::CreateSpriteVerts()
{
    // Create rect vertex data to draw sprite
    float vertexData[] = {
            -0.5f, 0.5f, 0.0f, 1.0f,
            0.5f, 0.5f, 1.0f, 1.0f,
            0.5f, -0.5f, 1.0f, 0.0f,
            -0.5f, -0.5f, 0.0f, 0.0f
    };

    unsigned int indexData[] = {0, 1, 2, 2, 3, 0};

    mSpriteVerts = new VertexArray(vertexData, 4, indexData, 6);
}

void Renderer::UploadLightingUniforms()
{
    mBaseShader->SetVectorUniform("uAmbientColor", mAmbientColor);
    mBaseShader->SetFloatUniform("uAmbientIntensity", mAmbientIntensity);
    mBaseShader->SetIntUniform("uNumLights", static_cast<int>(mLights.size()));

    for (int i = 0; i < static_cast<int>(mLights.size()); ++i)
    {
        std::string prefix = "uLights[" + std::to_string(i) + "]";
        if (mLights[i]->IsActivate()) {
            mBaseShader->SetVectorUniform((prefix + ".position").c_str(), mLights[i]->GetPosition());
            mBaseShader->SetVectorUniform((prefix + ".color").c_str(), mLights[i]->GetColor());
            mBaseShader->SetFloatUniform((prefix + ".intensity").c_str(), mLights[i]->GetIntensity());
            mBaseShader->SetFloatUniform((prefix + ".radius").c_str(), mLights[i]->GetRadius());
        }
        else {
            mBaseShader->SetFloatUniform((prefix + ".intensity").c_str(), 0);
            mBaseShader->SetFloatUniform((prefix + ".radius").c_str(), 0);
        }
    }
}

void Renderer::DeactivateLighting() {
    mBaseShader->SetVectorUniform("uAmbientColor", Vector3(1.0f, 1.0f, 1.0f));
    mBaseShader->SetFloatUniform("uAmbientIntensity", 1.0f);
    mBaseShader->SetIntUniform("uNumLights", 0);
}

bool Renderer::CreateRenderTarget(int width, int height)
{
    mRenderWidth = static_cast<float>(width);
    mRenderHeight = static_cast<float>(height);

    // Limpa se já existirem
    if (mGameFBO != 0) {
        glDeleteFramebuffers(1, &mGameFBO);
        glDeleteTextures(1, &mGameFBOTexture);
        glDeleteFramebuffers(1, &mUIFBO);
        glDeleteTextures(1, &mUIFBOTexture);
    }

    // Gera o FBO do jogo
    glGenFramebuffers(1, &mGameFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, mGameFBO);

    // Gera a Textura que servirá de tela
    glGenTextures(1, &mGameFBOTexture);
    glBindTexture(GL_TEXTURE_2D, mGameFBOTexture);

    // Aloca a memória da textura com a resolução desejada
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);

    // Filtro de interpolação:
    // Use GL_NEAREST se for Pixel Art. Use GL_LINEAR se for HD/Suave.
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    // Anexa a textura ao FBO
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mGameFBOTexture, 0);

    // --- CRIA O FBO DA UI (FBO 2) ---
    glGenFramebuffers(1, &mUIFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, mUIFBO);
    glGenTextures(1, &mUIFBOTexture);
    glBindTexture(GL_TEXTURE_2D, mUIFBOTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mUIFBOTexture, 0);

    // Desvincula o FBO para voltar à tela normal
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    return true;
}

Texture* Renderer::GetTexture(const std::string& fileName, bool smooth)
{
    Texture* tex = nullptr;
    auto iter = mTextures.find(fileName);
    if (iter != mTextures.end())
    {
        tex = iter->second;
    }
    else
    {
        tex = new Texture();
        if (tex->Load(fileName, smooth))
        {
            mTextures.emplace(fileName, tex);
            return tex;
        }
        else
        {
            delete tex;
            return nullptr;
        }
    }
    return tex;
}

void Renderer::OnWindowResize(float newWidth, float newHeight)
{
    // Atualiza as dimensões da janela
    mWindowWidth = newWidth;
    mWindowHeight = newHeight;

    // Recalcula o viewport
    UpdateViewport();
}

void Renderer::SetZoom(float zoom)
{
    // Limita o zoom para evitar valores estranhos (ex: divisão por zero)
    if (zoom < 0.01f)
    {
        zoom = 0.01f;
    }

    mZoom = zoom;

    // Recalcula as dimensões da "janela" virtual
    mZoomedWidth = mVirtualWidth / mZoom;
    mZoomedHeight = mVirtualHeight / mZoom;

    // Recalcula a matriz de projeção com base nas novas dimensões
    mOrthoProjection = Matrix4::CreateOrtho(0.0f, mZoomedWidth, mZoomedHeight, 0.0f, -1.0f, 1.0f);
}

void Renderer::UpdateViewport()
{
    if (mWindowWidth == 0.0f || mWindowHeight == 0.0f)
    {
        return;
    }

    // 1. Calcula a proporção da janela e a proporção desejada
    float windowAspect = mWindowWidth / mWindowHeight;
    float targetAspect = mVirtualWidth / mVirtualHeight; // (16.0f / 9.0f)

    int newWidth = static_cast<int>(mWindowWidth);
    int newHeight = static_cast<int>(mWindowHeight);

    int xOffset = 0;
    int yOffset = 0;

    // 2. Compara as proporções para decidir sobre letterbox (barras em cima/baixo) ou pillarbox (barras laterais)

    if (windowAspect > targetAspect)
    {
        // Pillarbox (janela mais larga que 16:9)
        newWidth = static_cast<int>(mWindowHeight * targetAspect);
        xOffset = (static_cast<int>(mWindowWidth) - newWidth) / 2;
    }
    else if (windowAspect < targetAspect)
    {
        // Letterbox (janela mais alta que 16:9)
        newHeight = static_cast<int>(mWindowWidth / targetAspect);
        yOffset = (static_cast<int>(mWindowHeight) - newHeight) / 2;
    }
    // else: A proporção é exata, xOffset e yOffset permanecem 0.

    // Armazene os valores calculados nos membros da classe
    mViewportX = xOffset;
    mViewportY = yOffset;
    mViewportWidth = newWidth;
    mViewportHeight = newHeight;

    // 3. Define o viewport do OpenGL
    // Isso diz ao OpenGL para desenhar apenas nesta área retangular da janela.
    // O `Renderer::Clear()` (com glClearColor(0,0,0,1)) limpará a janela inteira,
    // criando automaticamente as barras pretas nas áreas fora do viewport.
    glViewport(xOffset, yOffset, newWidth, newHeight);
}

Vector2 Renderer::ScreenToVirtual(const Vector2& screenPoint) const
{
    // Evita divisão por zero se a janela for minimizada
    if (mViewportWidth == 0 || mViewportHeight == 0)
    {
        return Vector2(0.0f, 0.0f);
    }

    Vector2 virtualPoint;

    // 1. Remove o offset das barras pretas (traduz para coordenadas relativas ao viewport)
    virtualPoint.x = screenPoint.x - mViewportX;
    virtualPoint.y = screenPoint.y - mViewportY;

    // 2. Escala as coordenadas do viewport para as coordenadas virtuais
    // (mouse.x / viewportWidth) * virtualWidth
    virtualPoint.x *= (mVirtualWidth / static_cast<float>(mViewportWidth));
    virtualPoint.y *= (mVirtualHeight / static_cast<float>(mViewportHeight));

    return virtualPoint;
}

void Renderer::SetEffectIntensity(PostProcessEffect effect, float intensity) {
    // Grava a intensidade no slot específico do efeito
    mEffectIntensities[static_cast<int>(effect)] = intensity;

    // Checa se algum efeito que deforma o UV (Spatial) está ativo
    bool needsLinear = (mEffectIntensities[static_cast<int>(PostProcessEffect::Blur)] > 0.0f ||
                        mEffectIntensities[static_cast<int>(PostProcessEffect::ChromaticAberration)] > 0.0f);

    // Ajusta o filtro da textura de acordo
    glBindTexture(GL_TEXTURE_2D, mGameFBOTexture);
    if (needsLinear) {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    } else {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    }
    glBindTexture(GL_TEXTURE_2D, 0);
}

void Renderer::DeactivateAllEffects() {
    mEffectIntensities[static_cast<int>(PostProcessEffect::Blur)] = 0.0f;
    mEffectIntensities[static_cast<int>(PostProcessEffect::Grayscale)] = 0.0f;
    mEffectIntensities[static_cast<int>(PostProcessEffect::DamageFlash)] = 0.0f;
    mEffectIntensities[static_cast<int>(PostProcessEffect::ChromaticAberration)] = 0.0f;
}

void Renderer::BeginGameDraw()
{
    mDrawingUI = false;
}

void Renderer::BeginUIDraw()
{
    mDrawingUI = true;

    // 1. Muda o alvo para o FBO da UI (FBO 2). AINDA estamos na resolução interna!
    glBindFramebuffer(GL_FRAMEBUFFER, mUIFBO);

    // Opcional: Não precisa limpar (Clear) o FBO 2, porque vamos desenhar um Quad gigante cobrindo tudo
    glDisable(GL_BLEND);

    // 2. Prepara o shader de pós-processamento
    mPostProcessShader->SetActive();
    mPostProcessShader->SetFloatUniform("uBlurIntensity", mEffectIntensities[static_cast<int>(PostProcessEffect::Blur)]);
    mPostProcessShader->SetFloatUniform("uGrayscaleIntensity", mEffectIntensities[static_cast<int>(PostProcessEffect::Grayscale)]);
    mPostProcessShader->SetFloatUniform("uDamageIntensity", mEffectIntensities[static_cast<int>(PostProcessEffect::DamageFlash)]);
    mPostProcessShader->SetFloatUniform("uAberrationIntensity", mEffectIntensities[static_cast<int>(PostProcessEffect::ChromaticAberration)]);
    mPostProcessShader->SetFloatUniform("uAberrationAngle", mAberrationAngle);

    mPostProcessShader->SetVectorUniform("uResolution", GetResolution());

    mScreenQuad->SetActive();

    // 3. Pega a textura do FBO 1 (Jogo) como entrada
    glBindTexture(GL_TEXTURE_2D, mGameFBOTexture);

    // 4. Desenha! Agora o FBO 2 contém o jogo borrado.
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

    glEnable(GL_BLEND);

    // 5. Devolve o controle para o BaseShader.
    // Como o mUIFBO ainda está "bindado", qualquer menu desenhado a partir de
    // agora cairá no FBO 2, nítido, sobrepondo o fundo embaçado!
    mBaseShader->SetActive();
}