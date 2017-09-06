/*
*	@file Main.cpp
*/

#include "GLFWEW.h"
#include "glm/gtc/matrix_transform.hpp"
#include "Texture.h"
#include "Shader.h"
#include "OffscreenBuffer.h"
#include "UniformBuffer.h"
#include "Mesh.h"
#include <iostream>
#include <vector>

/// 3D　ベクター型
struct Vector3
{
	float x, y, z;
};

/// RGBA カラー
struct  Color
{
	float r, g, b, a;
};


struct Vertex
{
	glm::vec3 position; ///< 座標
	glm::vec4 color; ///< 色
	glm::vec2 texCoord; ///< テクスチャ座標.
};


const Vertex vertices[] = {
	{ { -0.5f, -0.3f, 0.5f },{ 0.0f, 0.0f, 1.0f, 1.0f },{ 0.0f, 0.0f } },
	{ { 0.3f, -0.3f, 0.5f },{ 0.0f, 1.0f, 0.0f, 1.0f },{ 1.0f, 0.0f } },
	{ { 0.3f,  0.5f, 0.5f },{ 0.0f, 0.0f, 1.0f, 1.0f },{ 1.0f, 1.0f } },
	{ { -0.5f,  0.5f, 0.5f },{ 1.0f, 0.0f, 0.0f, 1.0f },{ 0.0f, 1.0f } },

	{ { -0.3f,  0.3f, 0.1f },{ 0.0f, 0.0f, 1.0f, 1.0f },{ 0.0f, 1.0f } },
	{ { -0.3f, -0.5f, 0.1f },{ 0.0f, 1.0f, 1.0f, 1.0f },{ 0.0f, 0.0f } },
	{ { 0.5f, -0.5f, 0.1f },{ 0.0f, 0.0f, 1.0f, 1.0f },{ 1.0f, 0.0f } },
	{ { 0.5f, -0.5f, 0.1f },{ 1.0f, 0.0f, 0.0f, 1.0f },{ 1.0f, 0.0f } },
	{ { 0.5f,  0.3f, 0.1f },{ 1.0f, 1.0f, 0.0f, 1.0f },{ 1.0f, 1.0f } },
	{ { -0.3f,  0.3f, 0.1f },{ 1.0f, 0.0f, 0.0f, 1.0f },{ 0.0f, 1.0f } },

	{ { -1.0f,-1.0f, 0.5f },{ 1.0f, 1.0f, 1.0f, 1.0f },{ 1.0f, 0.0f } },
	{ { 1.0f,-1.0f, 0.5f },{ 1.0f, 1.0f, 1.0f, 1.0f },{ 0.0f, 0.0f } },
	{ { 1.0f, 1.0f, 0.5f },{ 1.0f, 1.0f, 1.0f, 1.0f },{ 0.0f, 1.0f } },
	{ { -1.0f, 1.0f, 0.5f },{ 1.0f, 1.0f, 1.0f, 1.0f },{ 1.0f, 1.0f } },
};

// インデックスで―tあ
const GLuint indices[] = {
	0,1,2,2,3,0,	// 四角形
	4,5,6,7,8,9,

	10, 11, 12, 12, 13, 10,
};




// 頂点シェーダのパラメータ型.
struct VertexData
{
	glm::mat4 matMVP;
	glm::vec4 lightPosition;
	glm::vec4 lightColor;
	glm::vec4 ambientColor;
};


/**
* ライトデータ(点光源).
*/
struct PointLight
{
	glm::vec4 position; ///< 座標(ワールド座標系).
	glm::vec4 color; ///< 明るさ.
};

const int maxLightCount = 4; ///< ライトの数.

/**
* ライティングパラメータ.
*/
struct LightData
{
	glm::vec4 ambientColor; ///< 環境光.
	PointLight light[maxLightCount]; ///< ライトのリスト.
};

/**
* ポストエフェクトデータ.
*/
struct PostEffectData
{
	glm::mat4x4 matColor; ///< 色変換行列.
};



/**
* 部分描画データ.
*/
struct RenderingPart
{
	GLsizei size; ///< 描画するインデックス数.
	GLvoid* offset; ///< 描画開始インデックスのバイトオフセット.
};

/**
* RenderingPartを作成する.
*
* @param size 描画するインデックス数.
* @param offset 描画開始インデックスのオフセット(インデックス単位).
*
* @return 作成した部分描画オブジェクト.
*/
constexpr RenderingPart MakeRenderingPart(GLsizei size, GLsizei offset) {
	return{ size, reinterpret_cast<GLvoid*>(offset * sizeof(GLuint)) };
}

/**
* 部分描画データリスト.
*/
static const RenderingPart renderingParts[] = {
	MakeRenderingPart(12, 0),
	MakeRenderingPart(6, 12),
};


// 頂点シェーダ
static const char* vsCode;

// フラグメントシェーダ
static const char* fsCode;




/**
*	Vertex Buffer Object を作成する
*
*	@param	size	頂点データのサイズ
*	@param	data	頂点データへのポインタ
*
*	@return	作成したVBO
*/
GLuint CreateVBO(GLsizeiptr size, const GLvoid* data)
{
	GLuint vbo = 0;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	return vbo;
}




/**
*	Vertex Buffer Object を作成する
*
*	@param	size	頂点データのサイズ
*	@param	data	頂点データへのポインタ
*
*	@return	作成したIBO
*/
GLuint CreateIBO(GLsizeiptr size, const GLvoid* data)
{
	GLuint ibo = 0;
	glGenBuffers(1, &ibo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	return ibo;
}



/**
*	頂点アトリビュート
*
*	@param	index	頂点アトリビュートのインデックス
*	@param	cls		頂点データ型名
*	@param	mbr		頂点アトリビュートに設定 cls のメンバ変数名
*/
#define SetVertexAttribPointer(index, cls , mbr) SetVertexAttribPointerI( \
	index, \
	sizeof(cls::mbr) / sizeof(float), \
	sizeof(cls),\
	reinterpret_cast<GLvoid*>(offsetof(cls,mbr)))

void SetVertexAttribPointerI(
	GLuint index, GLint size, GLsizei stride, const GLvoid* pointer)
{
	glEnableVertexAttribArray(index);
	glVertexAttribPointer(index, size, GL_FLOAT, GL_FALSE, stride, pointer);
}


/**
*	Vertex Array Object を作成する
*
*	@param	vbo	VAOに関連付けられるVBO
*
*	@retrun 作成したVAO
*/
GLuint CreateVAO(GLuint vbo,GLuint ibo)
{
	GLuint vao = 0;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	// ちょとちがう
	// vbo割り当てされてる場合は　vboに割り当てられる
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	SetVertexAttribPointer(0, Vertex, position);
	SetVertexAttribPointer(1, Vertex, color);
	SetVertexAttribPointer(2, Vertex, texCoord);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	glBindVertexArray(0);
	return vao;
}



/**
* Uniform Block Objectを作成する.
*
* @param size Uniform Blockのサイズ.
* @param data Uniform Blockに転送するデータへのポインタ.
*
* @return 作成したUBO.
*/
GLuint CreateUBO(GLsizeiptr size, const GLvoid* data = nullptr)
{
	GLuint ubo;
	glGenBuffers(1, &ubo);
	glBindBuffer(GL_UNIFORM_BUFFER, ubo);
	glBufferData(GL_UNIFORM_BUFFER, size, data, GL_STREAM_DRAW);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
	return ubo;
}




// エントリーポイント
int main()
{
	GLFWEW::Window& window = GLFWEW::Window::Instance();
	if (!window.Init(800, 600, "OpenGL Tutorial")) {
		return 1;
	}

	float R = 0.0f;
	float B = 0.0f;
	float G = 0.0f;
	float A = 1.0f;  

	const GLuint vbo = CreateVBO(sizeof(vertices), vertices);
	const GLuint ibo = CreateIBO(sizeof(indices), indices);
	const GLuint vao = CreateVAO(vbo, ibo);

	// ライトデータ用のUBOを作成
	const UniformBufferPtr uboVertex = UniformBuffer::Create(  
		sizeof(VertexData), 0, "VertexData");
	const UniformBufferPtr uboLight = UniformBuffer::Create( 
			sizeof(LightData), 1, "LightData");
	const UniformBufferPtr uboPostEffect =
		UniformBuffer::Create(sizeof(PostEffectData), 2, "PostEffectData");
	const Shader::ProgramPtr progTutorial =
		Shader::Program::Create("Res/Tutorial.vert", "Res/Tutorial.frag");

	const Shader::ProgramPtr progColorFilter =                           
		Shader::Program::Create("Res/ColorFilter.vert", "Res/ColorFilter.frag"); 
	if (!vbo || !ibo || !vao || !uboVertex || !uboLight || !progTutorial || !progColorFilter) {
		return 1;
	}
	progTutorial->UniformBlockBinding("VertexData", 0);
	progTutorial->UniformBlockBinding("LightData", 1);
	                                           
	// テクスチャデータ.
	static const uint32_t textureData[] = {
		0xffffffff, 0xffcccccc, 0xffffffff, 0xffcccccc, 0xffffffff,
		0xff888888, 0xffffffff, 0xff888888, 0xffffffff, 0xff888888,
		0xffffffff, 0xff444444, 0xffffffff, 0xff444444, 0xffffffff,
		0xff000000, 0xffffffff, 0xff000000, 0xffffffff, 0xff000000,
		0xffffffff, 0xff000000, 0xffffffff, 0xff000000, 0xffffffff,
	};

	// BMPファイルから読み込む
	TexturePtr tex = Texture::LoadFromFile("Res/hart.bmp");
	TexturePtr texToroid = Texture::LoadFromFile("Res/Toroid.bmp"); 
	if (!tex || !texToroid) {                                     
		return 1;
	}
	Mesh::BufferPtr meshBuffer = Mesh::Buffer::Create(10 * 1024, 30 * 1024); 
	meshBuffer->LoadMeshFromFile("Res/Toroid.fbx");


	// 深度バッファ利用許可
	glEnable(GL_DEPTH_TEST);

	// 裏面ポリゴン非表示
	//glEnable(GL_CULL_FACE);

	const OffscreenBufferPtr offscreen = OffscreenBuffer::Create(800, 600);

	while (!window.ShouldClose()) {
		glBindFramebuffer(GL_FRAMEBUFFER, offscreen->GetFramebuffer());

		glClearColor(0.1f, 0.3f, 0.5f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// 回転
		static float degree = 0.0f;
		degree += 0.1f;
		if (degree >= 360.0f) { degree -= 360.0f; }
		const glm::vec3 viewPos = glm::rotate(
			glm::mat4(), glm::radians(degree), glm::vec3(0, 1, 0)) * glm::vec4(2, 3, 3, 1);

		progTutorial->UseProgram();
		const glm::mat4x4 matProj =
			glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 100.0f);
		const glm::mat4x4 matView =
			glm::lookAt(viewPos, glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));

		// UBOへデータを転送
		VertexData vertexData;
		vertexData.matMVP = matProj * matView;
		uboVertex->BufferSubData(&vertexData);
		// ライティング用データを作成
		LightData lightData;                                            //<-New!
		lightData.ambientColor = glm::vec4(0.05f, 0.1f, 0.2f, 1);        //<-New!
		lightData.light[0].position = glm::vec4(1, 1, 1, 1);            //<-New!
		lightData.light[0].color = glm::vec4(2, 2, 2, 1);               //<-New!
		lightData.light[1].position = glm::vec4(-0.2f, 0, 0.6f, 1);     //<-New!
		lightData.light[1].color = glm::vec4(0.125f, 0.125f, 0.05f, 1); //<-New!
		uboLight->BufferSubData(&lightData);

		// テクスチャ設定
		progTutorial->BindTexture(GL_TEXTURE0, GL_TEXTURE_2D, tex->Id());

		glBindVertexArray(vao);

		// 描画
		glDrawElements(
			GL_TRIANGLES, renderingParts[0].size, GL_UNSIGNED_INT, renderingParts[0].offset);
		progTutorial->BindTexture(GL_TEXTURE0, GL_TEXTURE_2D, texToroid->Id());
		meshBuffer->BindVAO();
		meshBuffer->GetMesh("Toroid")->Draw(meshBuffer);
		glBindVertexArray(vao);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glClearColor(0.5f, 0.3f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		// モノトーン（白黒）シェーダーを使う
		progColorFilter->UseProgram();

		// オフスクリーンバッファ
		progTutorial->BindTexture(GL_TEXTURE0, GL_TEXTURE_2D, offscreen->GetTexutre());

		/*
		// UBOにデータを転送するライティング用データを作成
		vertexData = {};
		uboVertex->BufferSubData(&vertexData);
		lightData = {};                       //<-New!
		lightData.ambientColor = glm::vec4(1); //<-New!
		uboLight->BufferSubData(&lightData);
		*/


		// UBOにデータを転送
		uboVertex->BufferSubData(&vertexData);

		PostEffectData postEffect;                                     
		postEffect.matColor[0] = glm::vec4(0.393f, 0.349f, 0.272f, 0); 
		postEffect.matColor[1] = glm::vec4(0.769f, 0.686f, 0.534f, 0); 
		postEffect.matColor[2] = glm::vec4(0.189f, 0.168f, 0.131f, 0); 
		postEffect.matColor[3] = glm::vec4(0, 0, 0, 1);                
		uboPostEffect->BufferSubData(&postEffect);                     



		// 二回目の描画
		glDrawElements(
			GL_TRIANGLES, renderingParts[1].size, GL_UNSIGNED_INT, renderingParts[1].offset);


		window.SwapBuffers();
		R += 0.1f;
		R= sin(R);
	}


	// オブジェクト削除
	glDeleteVertexArrays(1, &vao);
	glDeleteBuffers(1, &vbo);

	glfwTerminate();

	return 0;
}




