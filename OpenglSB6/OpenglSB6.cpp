// OpenglSB6.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <sb6.h>

class my_application : public sb6::application
{
public:
	void render(double currenttime) override
	{
		const GLfloat red[] = { 1, 0, 0, 1 };
#if defined(USE_OPENGL_3_1)
		glClear(GL_COLOR_BUFFER_BIT);
#else
		glClearBufferfv(GL_COLOR_BUFFER_BIT, 0, red);
#endif
	}

};

//DECLARE_MAIN(my_application);
int _tmain(int argc, _TCHAR* argv[])
{
	my_application a;
	a.run(&a);
	return 0;
}

