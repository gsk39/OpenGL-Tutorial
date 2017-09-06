/*
*	@file	GLFWGW.cpp
*/
#include "GLFWEW.h"
#include <iostream>


/// GLFW �� GLEW �����b�v����ׂ̖��O���
namespace GLFWEW{

	/**
	*	GLFW ����̃G���[�񍐂���������
	*
	*	@param	error	�G���[�ԍ�
	*	@param	desc	�G���[���e
	*/
	void ErrorCallback(int error, const char* desc)
	{
		std::cerr << "ERROR: " << desc << std::endl;
	}



	/*
	*	�V���O���g���C���X�^���X���擾����
	*	@return Window�̃V���O���g���C���X�^���X
	*/
	Window& Window::Instance()
	{
		static Window instance;
		return instance;
	}

	/*
	*	�R���X�g���N�^
	*/
	Window::Window() : isGLFWInitialized(false),isInitialized(false),window(nullptr)
	{
	}


	/*
	*	�f�X�g���N�^
	*/
	Window::~Window() 
	{
		if (isGLFWInitialized) {
			glfwTerminate();
		}
	}

	/*
	*
	*
	*/
	bool Window::Init(int w, int h, const char* title)
	{
		if (isInitialized) {
			std::cerr << "ERROR: GLFWEW �͂��łɏ���������Ă��܂��B" << std::endl;
			return false;
		}
		if (!isGLFWInitialized) {
			glfwSetErrorCallback(ErrorCallback);
			if (glfwInit() != GL_TRUE) {
				return false;
			}
			isGLFWInitialized = true;
		}

		if (!window) {
			window = glfwCreateWindow(w, h, title, nullptr, nullptr);
			if (!window) {
				return false;
			}
			glfwMakeContextCurrent(window);
		}
		if (glewInit() != GLEW_OK) {
			std::cerr << "ERROR: GLEW�̏������Ɏ��s���܂����B" << std::endl;
			glfwTerminate();
			return false;
		}

		const GLubyte* renderer = glGetString(GL_RENDERER);
		std::cout << "Remderer: " << renderer << std::endl;
		const GLubyte* version = glGetString(GL_VERSION);
		std::cout << "Version: " << version << std::endl;
		isInitialized = true;
		return true;
	}


	/**
	*	�E�B���h�E�����ׂ������ׂ�
	*
	*	@retval	true	����
	*	@retvval false	���Ȃ�	
	*/
	bool Window::ShouldClose() const
	{
		return glfwWindowShouldClose(window) != 0;
	}

	/**
	*	�t�����g�o�b�t�@�ƃo�b�N�o�b�t�@��؂�ւ���
	*/
	void Window::SwapBuffers() const
	{
		glfwPollEvents();
		glfwSwapBuffers(window);
	}



}// namescapse GLFWEW

