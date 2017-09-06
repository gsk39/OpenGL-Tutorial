/*
*	@file	GLFEW.h
*/
#ifndef GLFWEW_INCLUDE
#define GLFWEW_INCLUDE
#include <GL/glew.h>
#include <GLFW/glfw3.h>

namespace GLFWEW {

	/*
	*
	*	GLFW と GLEW のラッパークラス
	*/
	class Window
	{
	public:
		static Window& Instance();
		bool Init(int w, int h, const char* title);
		bool ShouldClose() const;
		void SwapBuffers() const;

	private:
		Window();
		~Window();
		Window(const Window&) = delete;
		Window& operator = (const Window&) = delete;

		bool isGLFWInitialized;
		bool isInitialized;
		GLFWwindow* window;
	};

}// namespase	GLFWEW

#endif // GLFWEW_INCLUDE