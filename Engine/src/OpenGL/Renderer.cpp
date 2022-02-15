
#include "Renderer.h"
#include "Texture.h"
#include "Mesh.h"
#include <algorithm>
#include "Shader.h"
#include "VertexArray.h"
#include "SpriteComponent.h"
#include "MeshComponent.h"
#include <GL\glew.h>

namespace Engine
{
	Renderer::Renderer(Game* game) :
		m_Game(game),
		m_SpriteShader(nullptr),
		m_MeshShader(nullptr)
	{
	}

	Renderer::~Renderer()
	{
	}

	bool Renderer::Initialize(float screenWidth, float screenHeight)
	{
		m_ScreenWidth = screenWidth;
		m_ScreenHeight = screenHeight;

		// Set OpenGL attributes
		// Use the core OpenGL profile
		// Available profiles: Core, Compatibility (deprecated features available), ES (mobile)
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK,
			SDL_GL_CONTEXT_PROFILE_CORE);
		// Specify version 3.3
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
		// Request a color buffer with 8-bits per RGBA channel
		SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
		SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
		SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
		SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
		// 24 bits for the z-buffering
		SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
		// Enable double buffering
		SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
		// Run on GPU
		SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);

		m_Window = SDL_CreateWindow(
			"Simple Game Engine",
			100,
			100,
			1024,
			768,
			SDL_WINDOW_OPENGL
		);

		if (!m_Window)
		{
			SDL_Log("Failed to create window: % s", SDL_GetError());
			return false;
		}

		m_Context = SDL_GL_CreateContext(m_Window);

		// Prevents initialization errors
		glewExperimental = GL_TRUE;
		if (glewInit() != GLEW_OK)
		{
			SDL_Log("Failed to initialize GLEW");
			return false;
		}

		// Clear benign error on some platforms
		glGetError();

		if (!LoadShaders())
		{
			SDL_Log("Failed to load shaders.");
			return false;
		}

		// Create quad for drawing sprites
		CreateSpriteVerts();

		return true;
	}

	void Renderer::Shutdown()
	{
		delete m_SpriteVerts;
		m_SpriteShader->Unload();
		delete m_SpriteShader;
		m_MeshShader->Unload();
		delete m_MeshShader;
		SDL_GL_DeleteContext(m_Context);
		SDL_DestroyWindow(m_Window);
	}

	void Renderer::UnloadData()
	{
		for (auto i : m_Textures)
		{
			i.second->Unload();
			delete i.second;
		}
		m_Textures.clear();

		for (auto i : m_Meshes)
		{
			i.second->Unload();
			delete i.second;
		}
		m_Meshes.clear();
	}

	void Renderer::Draw()
	{
		glClearColor(0.86f, 0.86f, 0.86f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Draw Mesh Components
		// Enable depth buffering/disable alpha blend
		glEnable(GL_DEPTH_TEST);
		glDisable(GL_BLEND);

		m_MeshShader->SetActive();
		m_MeshShader->SetMatrixUniform("uViewProj", m_View * m_Projection);

		for (auto mc : m_MeshComps)
		{
			mc->Draw(m_MeshShader);
		}

		// Draw all Sprite Components
		// Enable alpha blending on the color buffer
		glDisable(GL_DEPTH_TEST);
		glEnable(GL_BLEND);
		glBlendEquationSeparate(GL_FUNC_ADD, GL_FUNC_ADD);
		glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ZERO);

		m_SpriteShader->SetActive();
		m_SpriteVerts->SetActive();

		for (auto sprite : m_Sprites)
		{
			sprite->Draw(m_SpriteShader);
		}

		SDL_GL_SwapWindow(m_Window);
	}

	void Renderer::AddSprite(SpriteComponent* sprite)
	{
		// Find the insertion point in the sorted vector
		// (The first element with a higher draw order than me)
		int myDrawOrder = sprite->GetDrawOrder();
		auto iter = m_Sprites.begin();
		for (; iter != m_Sprites.end(); ++iter)
		{
			if (myDrawOrder < (*iter)->GetDrawOrder())
			{
				break;
			}
		}

		m_Sprites.insert(iter, sprite);
	}

	void Renderer::RemoveSprite(SpriteComponent* sprite)
	{
		// We can't swap because it ruins ordering
		auto iter = std::find(m_Sprites.begin(), m_Sprites.end(), sprite);
		m_Sprites.erase(iter);
	}

	void Renderer::AddMeshComp(MeshComponent* mesh)
	{
		m_MeshComps.emplace_back(mesh);
	}

	void Renderer::RemoveMeshComp(MeshComponent* mesh)
	{
		auto iter = std::find(m_MeshComps.begin(), m_MeshComps.end(), mesh);
		m_MeshComps.erase(iter);
	}

	Texture* Renderer::GetTexture(const std::string& fileName)
	{
		Texture* tex = nullptr;
		auto iter = m_Textures.find(fileName);
		if (iter != m_Textures.end())
		{
			tex = iter->second;
		}
		else
		{
			tex = new Texture();
			if (tex->Load(fileName))
			{
				m_Textures.emplace(fileName, tex);
			}
			else
			{
				delete tex;
				tex = nullptr;
			}
		}

		return tex;
	}

	Mesh* Renderer::GetMesh(const std::string& fileName)
	{
		Mesh* m = nullptr;
		auto iter = m_Meshes.find(fileName);
		if (iter != m_Meshes.end())
		{
			m = iter->second;
		}
		else
		{
			m = new Mesh();
			if (m->Load(fileName, this))
			{
				m_Meshes.emplace(fileName, m);
			}
			else
			{
				delete m;
				m = nullptr;
			}
		}
		return m;
	}

	bool Renderer::LoadShaders()
	{
		m_SpriteShader = new Shader();
		if (!m_SpriteShader->Load("src/Shaders/Sprite.vert", "src/Shaders/Sprite.frag"))
		{
			return false;
		}

		m_SpriteShader->SetActive();
		// Set the view-projection matrix
		Matrix4 viewProj = Matrix4::CreateSimpleViewProj(1024.f, 768.f);
		m_SpriteShader->SetMatrixUniform("uViewProj", viewProj);

		m_MeshShader = new Shader();
		if (!m_MeshShader->Load("src/Shaders/BasicMesh.vert", "src/Shaders/BasicMesh.frag"))
		{
			return false;
		}

		m_MeshShader->SetActive();
		m_View = Matrix4::CreateLookAt(Vector3::Zero, Vector3::UnitX, Vector3::UnitZ);
		m_Projection = Matrix4::CreatePerspectiveFOV(
			CustomMath::ToRadians(70.0f),		// Horizontal Field of View
			m_ScreenWidth,			
			m_ScreenHeight, 
			25.0f,								// Near plane
			10000.0f);							// Far plane
		m_MeshShader->SetMatrixUniform("uViewProj", m_View * m_Projection);

		return true;
	}

	void Renderer::CreateSpriteVerts()
	{
		float vertices[] = {
			-0.5f,  0.5f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, // top left
			 0.5f,  0.5f, 0.f, 0.f, 0.f, 0.f, 1.f, 0.f, // top right	
			 0.5f, -0.5f, 0.f, 0.f, 0.f, 0.f, 1.f, 1.f, // bottom right
			-0.5f, -0.5f, 0.f, 0.f, 0.f, 0.f, 0.f, 1.f  // bottom left
		};

		unsigned int indices[] = {
			0, 1, 2,
			2, 3, 0
		};

		m_SpriteVerts = new VertexArray(vertices, 4, indices, 6);
	}
}