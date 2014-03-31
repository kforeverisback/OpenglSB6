// OpenglSB6.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <sb6.h>

const char* CHAPTER_TITLE = "OpenGL SuperBible 6: Chapter 3";

#define DefineObjectAs(_T_, _X_)	typedef _T_ _X_
DefineObjectAs(GLuint, VertexBufferObject);
DefineObjectAs(GLuint, VertexArrayObject);
DefineObjectAs(GLuint, ShaderId);
DefineObjectAs(GLuint, ShaderProgramId);

const char* VS = "#version 430 core\n\
layout(location = 0) in vec4 offset;\n\
layout(location = 1) in vec4 color;\n\
out vec4 vs_color;\n\
void main(void)\n\
{\n\
	vs_color = color;\n\
	const vec4 vertices[] = vec4[](vec4(0.25, -0.25, 0.5, 1.0),\n\
		vec4(-0.25, -0.25, 0.5, 1.0),\n\
		vec4(0.25, 0.25, 0.5, 1.0));\n\
			gl_Position = vertices[gl_VertexID] + offset;\n\
}";

const char* FS = "#version 430 core\n\
				 in vec4 vs_color;\n\
				 out vec4 colorOut;\n\
				 void main(void)\n\
				 {colorOut = vs_color;}";

class my_application : public sb6::application
{
public:
	void render(double currenttime) override
	{
		const GLfloat red[] = { .2f, .2f, 0, 1 };
		GLfloat colors[] = { (float)sin(currenttime)*0.5f,
			(float)cos(currenttime) * 0.5f ,
			0,
			1 };
		glClearBufferfv(GL_COLOR, 0, colors);
		const GLfloat attrib[] = { (float)sin(currenttime)*0.5f,
									(float)cos(currenttime) * 0.5f,
									0,
									0 };
		glVertexAttrib4fv(0, attrib);
		colors[1] += 0.1f;
		colors[3] += 0.3f;
		colors[0] += 0.5f;
		colors[2] += 0.2f;
		glVertexAttrib4fv(1, colors);
		glUseProgram(m_shaderProgram);
		glDrawArrays(GL_TRIANGLES, 0, 3);
	}

#pragma region Initialization, Startup and Shutdown

	void startup() override
	{
		CompileShaders(VS, FS);
		glGenVertexArrays(1, &m_vertex_array_object);
		glBindVertexArray(m_vertex_array_object);
	}

	void init() override
	{
		sb6::application::init();
		memset(info.title, 0, ARRAYSIZE(info.title));
		memcpy_s(info.title, ARRAYSIZE(info.title), CHAPTER_TITLE, strlen(CHAPTER_TITLE));
	}

	void shutdown() override
	{
		glDeleteProgram(m_shaderProgram);
		glDeleteVertexArrays(1, &m_vertex_array_object);
	}
#pragma endregion

#pragma region GL Debug Callback
	void onDebugMessage(GLenum source,
		GLenum type,
		GLuint id,
		GLenum severity,
		GLsizei length,
		const GLchar* message) override
	{
#ifdef _WIN32
		OutputDebugStringA(message);
		OutputDebugStringA("\n");
#endif /* _WIN32 */

		FILE_LOG(message << endl);
	}
#pragma endregion

	void CompileShaders(const char* vert, const char* frag)
	{
		//Create and Compile vertex shader
		ShaderId vertShader = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(vertShader, 1, &vert, nullptr);
		glCompileShader(vertShader);

		//Create and Compile fragment shader
		ShaderId fragShader = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(fragShader, 1, &frag, nullptr);
		glCompileShader(fragShader);

		//Create and Link Program with the pipeline
		m_shaderProgram =  glCreateProgram();
		glAttachShader(m_shaderProgram, vertShader);
		glAttachShader(m_shaderProgram, fragShader);
		glLinkProgram(m_shaderProgram);

		//Delete Shader object as the shaders are integrated in the Program
		glDeleteShader(vertShader);
		glDeleteShader(fragShader);

	}

private:
	ShaderProgramId m_shaderProgram = 0;
	VertexArrayObject m_vertex_array_object = 0;
};

//DECLARE_MAIN(my_application);
int _tmain(int argc, _TCHAR* argv[])
{
	if (!start_klog())
	{
		FILE_LOG("Failed to start KLog");
	}
	my_application* app = new my_application();
	app->run(app);
	delete app;
	app = nullptr;
	return 0;
}

