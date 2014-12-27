// OpenglSB6.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <sb6.h>

const char CHAPTER_TITLE[]{"OpenGL SuperBible 6: Chapter 3"};

Typedef(GLuint, VertexBufferObject);
Typedef(GLuint, VertexArrayObject);
Typedef(GLuint, ShaderId);
Typedef(GLuint, ShaderProgramId);

class my_application : public sb6::application
{
public:
	void render(double currenttime) override
	{
		const GLfloat red[] = { .2f, .2f, 0, 1 };
		GLfloat colors[] = { (float)sin(currenttime)*0.5f,
			(float)cos(currenttime) * 0.5f,
			0,
			1 };
		glClearBufferfv(GL_COLOR, 0, colors);
		const GLfloat attrib[] = { (float)sin(currenttime)*0.5f,
			(float)cos(currenttime) * 0.5f,
			0,
			0 };
		glVertexAttrib4fv(0, attrib);
		colors[0] = 1 - colors[0];
		colors[2] = 1 - colors[2];
		colors[1] = 1 - colors[1];
		colors[3] = 1 - colors[3];
		glVertexAttrib4fv(1, colors);
		glUseProgram(m_shaderProgram);
		glDrawArrays(GL_TRIANGLES, 0, 3);
	}

#pragma region Initialization, Startup and Shutdown

	void startup() override
	{
		const char *vert = nullptr, *frag = nullptr;
		try
		{
			vert = m_cfg.getRoot()["shaders"]["vs"].c_str();
			frag = m_cfg.getRoot()["shaders"]["ps"].c_str();
		}
		catch (const libconfig::SettingNotFoundException &nfex)
		{
			LOG_E("Cannot read the setting from config file\nException:\n" << nfex.what());
			return;
		}
		CompileShaders(vert, frag);
		//CompileShaders(VS, FS);
		glGenVertexArrays(1, &m_vertex_array_object);
		glBindVertexArray(m_vertex_array_object);
	}

	void init() override
	{
		try
		{
			m_cfg.readFile("inputs.cfg");
		}
		catch (const libconfig::FileIOException &fioex)
		{
			LOG_E("I/O error while reading file.");
			return;
		}
		catch (const libconfig::ParseException &pex)
		{
			/*g_flog_obj.getLogger<ige::FileLogger::e_logType::DBG>() << "Parse error at " << pex.getFile() << ":" << pex.getLine()
				<< " - " << pex.getError();
				g_flog_obj << ige::FileLogger::e_logType::DBG << "Parse error at " << pex.getFile() << ":" << pex.getLine()
				<< " - " << pex.getError();*/
			LOG_E("Parse error at " << pex.getFile() << ":" << pex.getLine()
				<< " - " << pex.getError());
			return;
		}

		LOG_D("Read inputs.cfg successfully");
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

		LOG_D(message << endl);
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
		m_shaderProgram = glCreateProgram();
		glAttachShader(m_shaderProgram, vertShader);
		glAttachShader(m_shaderProgram, fragShader);
		glLinkProgram(m_shaderProgram);

		//Delete Shader object as the shaders are integrated in the Program through the Program Object
		glDeleteShader(vertShader);
		glDeleteShader(fragShader);

	}

private:
	libconfig::Config m_cfg;
	ShaderProgramId m_shaderProgram = 0;
	VertexArrayObject m_vertex_array_object = 0;
};

ige::FileLogger g_flog_obj(GL_LOG_FILE);
//DECLARE_MAIN(my_application);
int _tmain(int argc, const char* argv[])
{
	///////////////////////////////
	//////////KLOG////////////////
	/*if (!start_klog())
	{
	LOG_E("Failed to start KLog");
	}
	else
	{
	LOG_D("Successfully started KLog");
	}*/
	/////////END KLOG//////////////
	//////////////////////////////


	////////////////////////////////
	////////ige::FileLogger/////////
	LOG_D("This is a logger " << 10 << "," << " TEsting");
	////////ige::FileLogger/////////
	///////////////////////////////

	/////////////////////////////
	//////////G2GLOG Init////////
	/////////////////////////////
	/**
	g2LogWorker worker("Sb6_log", "");
	//g2logWorker g2log(argv[0], "/tmp/whatever-directory-path-you-want/");
	g2::initializeLogging(&worker);
	LOG(INFO) << "This is a logging" << 20 << 2.444;
	**/
	/////////////////////////////
	//////////G2GLOG Init End////
	/////////////////////////////
	my_application* app = new my_application();
	app->run(app);
	SafeDelete(app);
	return 0;
}

