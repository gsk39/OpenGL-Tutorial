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

/// 3D�@�x�N�^�[�^
struct Vector3
{
	float x, y, z;
};

/// RGBA �J���[
struct  Color
{
	float r, g, b, a;
};


struct Vertex
{
	glm::vec3 position; ///< ���W
	glm::vec4 color; ///< �F
	glm::vec2 texCoord; ///< �e�N�X�`�����W.
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

// �C���f�b�N�X�Ł\t��
const GLuint indices[] = {
	0,1,2,2,3,0,	// �l�p�`
	4,5,6,7,8,9,

	10, 11, 12, 12, 13, 10,
};




// ���_�V�F�[�_�̃p�����[�^�^.
struct VertexData
{
	glm::mat4 matMVP;
	glm::vec4 lightPosition;
	glm::vec4 lightColor;
	glm::vec4 ambientColor;
};


/**
* ���C�g�f�[�^(�_����).
*/
struct PointLight
{
	glm::vec4 position; ///< ���W(���[���h���W�n).
	glm::vec4 color; ///< ���邳.
};

const int maxLightCount = 4; ///< ���C�g�̐�.

/**
* ���C�e�B���O�p�����[�^.
*/
struct LightData
{
	glm::vec4 ambientColor; ///< ����.
	PointLight light[maxLightCount]; ///< ���C�g�̃��X�g.
};

/**
* �|�X�g�G�t�F�N�g�f�[�^.
*/
struct PostEffectData
{
	glm::mat4x4 matColor; ///< �F�ϊ��s��.
};



/**
* �����`��f�[�^.
*/
struct RenderingPart
{
	GLsizei size; ///< �`�悷��C���f�b�N�X��.
	GLvoid* offset; ///< �`��J�n�C���f�b�N�X�̃o�C�g�I�t�Z�b�g.
};

/**
* RenderingPart���쐬����.
*
* @param size �`�悷��C���f�b�N�X��.
* @param offset �`��J�n�C���f�b�N�X�̃I�t�Z�b�g(�C���f�b�N�X�P��).
*
* @return �쐬���������`��I�u�W�F�N�g.
*/
constexpr RenderingPart MakeRenderingPart(GLsizei size, GLsizei offset) {
	return{ size, reinterpret_cast<GLvoid*>(offset * sizeof(GLuint)) };
}

/**
* �����`��f�[�^���X�g.
*/
static const RenderingPart renderingParts[] = {
	MakeRenderingPart(12, 0),
	MakeRenderingPart(6, 12),
};


// ���_�V�F�[�_
static const char* vsCode;

// �t���O�����g�V�F�[�_
static const char* fsCode;




/**
*	Vertex Buffer Object ���쐬����
*
*	@param	size	���_�f�[�^�̃T�C�Y
*	@param	data	���_�f�[�^�ւ̃|�C���^
*
*	@return	�쐬����VBO
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
*	Vertex Buffer Object ���쐬����
*
*	@param	size	���_�f�[�^�̃T�C�Y
*	@param	data	���_�f�[�^�ւ̃|�C���^
*
*	@return	�쐬����IBO
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
*	���_�A�g���r���[�g
*
*	@param	index	���_�A�g���r���[�g�̃C���f�b�N�X
*	@param	cls		���_�f�[�^�^��
*	@param	mbr		���_�A�g���r���[�g�ɐݒ� cls �̃����o�ϐ���
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
*	Vertex Array Object ���쐬����
*
*	@param	vbo	VAO�Ɋ֘A�t������VBO
*
*	@retrun �쐬����VAO
*/
GLuint CreateVAO(GLuint vbo,GLuint ibo)
{
	GLuint vao = 0;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	// ����Ƃ�����
	// vbo���蓖�Ă���Ă�ꍇ�́@vbo�Ɋ��蓖�Ă���
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	SetVertexAttribPointer(0, Vertex, position);
	SetVertexAttribPointer(1, Vertex, color);
	SetVertexAttribPointer(2, Vertex, texCoord);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	glBindVertexArray(0);
	return vao;
}



/**
* Uniform Block Object���쐬����.
*
* @param size Uniform Block�̃T�C�Y.
* @param data Uniform Block�ɓ]������f�[�^�ւ̃|�C���^.
*
* @return �쐬����UBO.
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




// �G���g���[�|�C���g
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

	// ���C�g�f�[�^�p��UBO���쐬
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
	                                           
	// �e�N�X�`���f�[�^.
	static const uint32_t textureData[] = {
		0xffffffff, 0xffcccccc, 0xffffffff, 0xffcccccc, 0xffffffff,
		0xff888888, 0xffffffff, 0xff888888, 0xffffffff, 0xff888888,
		0xffffffff, 0xff444444, 0xffffffff, 0xff444444, 0xffffffff,
		0xff000000, 0xffffffff, 0xff000000, 0xffffffff, 0xff000000,
		0xffffffff, 0xff000000, 0xffffffff, 0xff000000, 0xffffffff,
	};

	// BMP�t�@�C������ǂݍ���
	TexturePtr tex = Texture::LoadFromFile("Res/hart.bmp");
	TexturePtr texToroid = Texture::LoadFromFile("Res/Toroid.bmp"); 
	if (!tex || !texToroid) {                                     
		return 1;
	}
	Mesh::BufferPtr meshBuffer = Mesh::Buffer::Create(10 * 1024, 30 * 1024); 
	meshBuffer->LoadMeshFromFile("Res/Toroid.fbx");


	// �[�x�o�b�t�@���p����
	glEnable(GL_DEPTH_TEST);

	// ���ʃ|���S����\��
	//glEnable(GL_CULL_FACE);

	const OffscreenBufferPtr offscreen = OffscreenBuffer::Create(800, 600);

	while (!window.ShouldClose()) {
		glBindFramebuffer(GL_FRAMEBUFFER, offscreen->GetFramebuffer());

		glClearColor(0.1f, 0.3f, 0.5f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// ��]
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

		// UBO�փf�[�^��]��
		VertexData vertexData;
		vertexData.matMVP = matProj * matView;
		uboVertex->BufferSubData(&vertexData);
		// ���C�e�B���O�p�f�[�^���쐬
		LightData lightData;                                            //<-New!
		lightData.ambientColor = glm::vec4(0.05f, 0.1f, 0.2f, 1);        //<-New!
		lightData.light[0].position = glm::vec4(1, 1, 1, 1);            //<-New!
		lightData.light[0].color = glm::vec4(2, 2, 2, 1);               //<-New!
		lightData.light[1].position = glm::vec4(-0.2f, 0, 0.6f, 1);     //<-New!
		lightData.light[1].color = glm::vec4(0.125f, 0.125f, 0.05f, 1); //<-New!
		uboLight->BufferSubData(&lightData);

		// �e�N�X�`���ݒ�
		progTutorial->BindTexture(GL_TEXTURE0, GL_TEXTURE_2D, tex->Id());

		glBindVertexArray(vao);

		// �`��
		glDrawElements(
			GL_TRIANGLES, renderingParts[0].size, GL_UNSIGNED_INT, renderingParts[0].offset);
		progTutorial->BindTexture(GL_TEXTURE0, GL_TEXTURE_2D, texToroid->Id());
		meshBuffer->BindVAO();
		meshBuffer->GetMesh("Toroid")->Draw(meshBuffer);
		glBindVertexArray(vao);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glClearColor(0.5f, 0.3f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		// ���m�g�[���i�����j�V�F�[�_�[���g��
		progColorFilter->UseProgram();

		// �I�t�X�N���[���o�b�t�@
		progTutorial->BindTexture(GL_TEXTURE0, GL_TEXTURE_2D, offscreen->GetTexutre());

		/*
		// UBO�Ƀf�[�^��]�����郉�C�e�B���O�p�f�[�^���쐬
		vertexData = {};
		uboVertex->BufferSubData(&vertexData);
		lightData = {};                       //<-New!
		lightData.ambientColor = glm::vec4(1); //<-New!
		uboLight->BufferSubData(&lightData);
		*/


		// UBO�Ƀf�[�^��]��
		uboVertex->BufferSubData(&vertexData);

		PostEffectData postEffect;                                     
		postEffect.matColor[0] = glm::vec4(0.393f, 0.349f, 0.272f, 0); 
		postEffect.matColor[1] = glm::vec4(0.769f, 0.686f, 0.534f, 0); 
		postEffect.matColor[2] = glm::vec4(0.189f, 0.168f, 0.131f, 0); 
		postEffect.matColor[3] = glm::vec4(0, 0, 0, 1);                
		uboPostEffect->BufferSubData(&postEffect);                     



		// ���ڂ̕`��
		glDrawElements(
			GL_TRIANGLES, renderingParts[1].size, GL_UNSIGNED_INT, renderingParts[1].offset);


		window.SwapBuffers();
		R += 0.1f;
		R= sin(R);
	}


	// �I�u�W�F�N�g�폜
	glDeleteVertexArrays(1, &vao);
	glDeleteBuffers(1, &vbo);

	glfwTerminate();

	return 0;
}




