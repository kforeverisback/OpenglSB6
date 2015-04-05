// OpenglSB6.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <sb6.h>

const char CHAPTER_TITLE[]{"OpenGL SuperBible 6 - Chapter 5: Data"};

Typedef(GLuint, VertexBufferObject);
Typedef(GLuint, VertexArrayObject);
Typedef(GLuint, ShaderId);
Typedef(GLuint, ShaderProgramId);

#define SATURATE_TO_1(_X_) (_X_ - (int)_X_)
class my_application : public sb6::application
{
public:
	my_application() : sb6::application(){};
	my_application(const my_application&) = delete;
	my_application& operator=(const my_application&) = delete;

	///3. Called Third
	void render(double currenttime) override
	{
		////////////////////////////////////////////////////////
		//SIMPLE Back Colorization//////////////////////////////
		GLfloat colors[] = { (float)sin(currenttime)*0.5f + 0.5,
			(float)cos(currenttime) * 0.5f + 0.5,
			0,
			1 };
		glClearBufferfv(GL_COLOR, 0, colors);
		////////////////////////////////////////////////////////

		////////////////////////////////////////////////////////
		//Fragment Shader data pass/////////////////////////////
		colors[0] = 1 - colors[0];
		colors[2] = 1 - colors[2];
		colors[1] = 1 - colors[1];
		colors[3] = 1 - colors[3];
		glVertexAttrib4fv(1, colors);
		///////////////////////////////////////////////////////


		///////////////////////////////////////////////
		//Testing to write the buffer at runtime :D
		if (0 == (((int)currenttime) % 5))
		{
			glBindBuffer(GL_ARRAY_BUFFER, vbo_vertex_offset);
			float* vbo_vertex_offset_data = (float*)glMapBuffer(GL_ARRAY_BUFFER, GL_READ_WRITE);
			vbo_vertex_offset_data[0] += .05;
			glUnmapBuffer(GL_ARRAY_BUFFER);
			vbo_vertex_offset_data[0] = vbo_vertex_offset_data[0] - (int)vbo_vertex_offset_data[0];
			LOG_D(vbo_vertex_offset_data[0]);
		}
		//Test End
		///////////////////////////////////////////

		glUseProgram(m_shaderProgram);
		glDrawArrays(GL_TRIANGLES, 0, 3);
	}

	VertexBufferObject vbo_vertex_offset = -1;
#pragma region Initialization, Startup and Shutdown

	///2. Called Second
	void startup() override
	{
		const char *vert = nullptr, *frag = nullptr;
		try
		{
			vert = (*res_data)["data_shader"]["vs"].c_str();
			frag = (*res_data)["data_shader"]["ps"].c_str();
			libconfig::Setting &data_offset_settings = (*res_data)["data_offset"];
			if (data_offset_settings.isArray())
			{
				int ll = data_offset_settings.getLength();
				vector<float> data_offset(ll);
				for (int i = data_offset_settings.getLength() - 1;
					i >= 0;
					i--)
				{
					float f = data_offset_settings[i];
					data_offset[i] =f;
				}
				vbo_vertex_offset = CreateBuffers(data_offset);
				//vbo_color = CreateBuffers(data_color);

			}
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

		glBindBuffer(GL_ARRAY_BUFFER, vbo_vertex_offset);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

		glEnableVertexAttribArray(0);
	}

	///1. Called First
	void init() override
	{
		try
		{
			m_cfg.readFile("inputs.cfg");
			res_data = &m_cfg.getRoot()["data"];
		}
		catch (const libconfig::FileIOException &fioex)
		{
			LOG_E("I/O error while reading file.");
			return;
		}
		catch (const libconfig::ParseException &pex)
		{
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

	VertexBufferObject CreateBuffers(const vector<float>& vals)
	{
		size_t sizeOfBuffer = vals.size() * sizeof(float);
		GLboolean unmapped = 0;
		VertexBufferObject vbo = -1;
		glGenBuffers(1, &vbo);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, sizeOfBuffer, vals.data(), GL_STATIC_DRAW);
		return vbo;
		/*void* ptr = glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
		if (nullptr == ptr)
		goto FINISH;

		memcpy_s(ptr, sizeOfBuffer, vals.data(), vals.size() * sizeof(float));
		unmapped = glUnmapBuffer(GL_ARRAY_BUFFER);*/
	FINISH:
		return (unmapped == 1) ? vbo : -1;
	}

private:
	////Resources
	libconfig::Config m_cfg;
	libconfig::Setting* res_data;
	////Resources End


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

