/*
*	@file	GLFWGW.cpp
*/
#include "GLFWEW.h"
#include <iostream>


/// GLFW と GLEW をラップする為の名前空間
namespace GLFWEW{

	/**
	*	GLFW からのエラー報告を処理する
	*
	*	@param	error	エラー番号
	*	@param	desc	エラー内容
	*/
	void ErrorCallback(int error, const char* desc)
	{
		std::cerr << "ERROR: " << desc << std::endl;
	}



	/*
	*	シングルトンインスタンスを取得する
	*	@return Windowのシングルトンインスタンス
	*/
	Window& Window::Instance()
	{
		static Window instance;
		return instance;
	}

	/*
	*	コンストラクタ
	*/
	Window::Window() : isGLFWInitialized(false),isInitialized(false),window(nullptr)
	{
	}


	/*
	*	デストラクタ
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
			std::cerr << "ERROR: GLFWEW はすでに初期化されています。" << std::endl;
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
			std::cerr << "ERROR: GLEWの初期化に失敗しました。" << std::endl;
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
	*	ウィンドウを閉じるべきか調べる
	*
	*	@retval	true	閉じる
	*	@retvval false	閉じない	
	*/
	bool Window::ShouldClose() const
	{
		return glfwWindowShouldClose(window) != 0;
	}

	/**
	*	フロントバッファとバックバッファを切り替える
	*/
	void Window::SwapBuffers() const
	{
		glfwPollEvents();
		glfwSwapBuffers(window);
	}



}// namescapse GLFWEW

