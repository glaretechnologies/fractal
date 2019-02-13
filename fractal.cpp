/*
Copyright (c) 2019 Nicholas Chapman

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include <SDL.h>
#include "GL\gl3w.h"
#include <SDL_opengl.h>
#include <iostream>
#include <fstream>
#include <string>


// Load the contents of a file into a string and return the string.
std::string loadFile(const std::string& path)
{
	std::ifstream t(path.c_str());
	if(!t)
	{
		std::cerr << "Failed to open file " << path << std::endl;
		exit(1);
	}
	t.seekg(0, std::ios::end);
	size_t size = (size_t)t.tellg();
	std::string buffer(size, ' ');
	t.seekg(0);
	t.read(&buffer[0], size); 
	return buffer;
}


// Check for an OpenGL error, and print the error code and exit the program if we encountered one.
void checkForError()
{
	const GLenum res = glGetError();
	if(res != GL_NO_ERROR)
	{
		std::cout << "Encountered a GL error: " << res << std::endl;
		exit(3);
	}
}


// Get a build/compile log from OpenGL.
static const std::string getLog(GLuint shader)
{
	// Get log length including null terminator
	GLint log_length = 0;
	glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &log_length);

	std::string log;
	if(log_length > 0)
	{
		log.resize(log_length - 1);
		glGetShaderInfoLog(shader, log_length, NULL, &log[0]);
	}
	return log;
}


// The following array holds vec3 data of 
// three vertex positions
static const GLfloat vertices[] = {
	-10.0f, -10.0f, 0.f,
	10.0f, -10.0f, 0.f,
	0.0f,  10.0f, 0.f,
};


static double getCurTimeRealSec()
{
	return SDL_GetPerformanceCounter() / (double)SDL_GetPerformanceFrequency(); // Is this Windows only?
	// (double)SDL_GetTicks() * 0.001;
}


struct vec3
{
	vec3(float x_, float y_, float z_) : x(x_), y(y_), z(z_) {}
	vec3(){}

	float x, y, z;
};


int main(int, char**)
{
	if(SDL_Init(SDL_INIT_VIDEO) != 0)
	{
		std::cout << "SDL_Init Error: " << SDL_GetError() << std::endl;
		return 1;
	}

	SDL_Window *win = SDL_CreateWindow("Fractals", 100, 100, 600, 600, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
	if(win == nullptr)
	{
		std::cout << "SDL_CreateWindow Error: " << SDL_GetError() << std::endl;
		return 1;
	}

	if(SDL_SetWindowFullscreen(win, SDL_WINDOW_FULLSCREEN_DESKTOP) != 0)
	{
		std::cout << "SDL_SetWindowFullscreen failed: " << SDL_GetError() << std::endl;
		return 1;
	}

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

	SDL_GLContext gl_context = SDL_GL_CreateContext(win);
	if(!gl_context)
	{
		std::cout << "OpenGL context could not be created! SDL Error: " << SDL_GetError() << std::endl;
		return 1;
	}

	gl3wInit();

	// Load shaders
	const std::string shader_dir = "D:/programming/fractal/trunk";
	const std::string vertexShaderCode   = loadFile(shader_dir + "/vert.glsl");
	const std::string fragmentShaderCode = loadFile(shader_dir + "/frag.glsl");

	// Compile shaders
	GLuint program = glCreateProgram();
	checkForError();
	checkForError();
	GLuint vert_shader = glCreateShader(GL_VERTEX_SHADER);
	checkForError();
	const char* vert_source[] = { vertexShaderCode.c_str(), NULL };
	glShaderSource(vert_shader, 1, vert_source, NULL);
	checkForError();
	glCompileShader(vert_shader);
	checkForError();


	const std::string vert_shader_log = getLog(vert_shader);
	std::cout << "vert_shader_log: " << vert_shader_log << std::endl;

	GLint shader_ok;
	glGetShaderiv(vert_shader, GL_COMPILE_STATUS, &shader_ok);
	if(!shader_ok)
		std::cout << "Failed to compile vert shader" << std::endl;

	GLuint frag_shader = glCreateShader(GL_FRAGMENT_SHADER);
	const char* frag_source[] = { fragmentShaderCode.c_str(), NULL };
	glShaderSource(frag_shader, 1, frag_source, NULL);
	checkForError();
	glCompileShader(frag_shader);
	checkForError();

	
	const std::string frag_shader_log = getLog(frag_shader);
	std::cout << "frag_shader_log: " << frag_shader_log << std::endl;

	glAttachShader(program, frag_shader);
	glAttachShader(program, vert_shader);
	checkForError();
	glLinkProgram(program);
	checkForError();
	glUseProgram(program);
	checkForError();

	std::cout << "Finished compiling and linking program." << std::endl;


	// Get vertex attribute and uniform locations
	GLint posLoc  = glGetAttribLocation(program, "pos");
	GLint timeLoc = glGetUniformLocation(program, "time");
	GLint col0Loc = glGetUniformLocation(program, "col0");
	GLint col1Loc = glGetUniformLocation(program, "col1");
	GLint col2Loc = glGetUniformLocation(program, "col2");
	GLint cLoc =	glGetUniformLocation(program, "c");
	GLint trapLoc = glGetUniformLocation(program, "trap");
	checkForError();

	//std::cout << "posLoc: "<<  posLoc << std::endl;
	//std::cout << "timeLoc: "<< timeLoc << std::endl;

	// Create Vertex Buffer Object
	GLuint vbo;
	glGenBuffers(1, &vbo);
	checkForError();
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	checkForError();
	glBufferData(GL_ARRAY_BUFFER, 9 * sizeof(float), vertices, GL_STATIC_DRAW);
	checkForError();


	// Set our vertex data
	glEnableVertexAttribArray(posLoc);
	checkForError();
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	checkForError();
	glVertexAttribPointer(posLoc, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	checkForError();

	glDisable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);

	checkForError();

	std::cout << "Finished init!" << std::endl;

	const double start_time = getCurTimeRealSec();
	int frame = 0;

	bool quit = false;
	while(!quit)
	{
		// Set time uniform
		const double t = (getCurTimeRealSec() /* - start_time*/) * 3.0f;
		glUniform1f(timeLoc, (float)t);

		//double phase0 = 0.2354;// t * 0.2;
		//double phase1 =  1454.34543f;// 1.5 + t * 0.2;
		//double phase2 = 334.976363;// 3.0 + t * 0.2;
		double col_change_speed = 0.05;
		double phase0 = t * col_change_speed;
		double phase1 = 1.5 + t * col_change_speed;
		double phase2 = 3.0 + t * col_change_speed;

		// Compute current colours, pass into shaders as uniforms.
		vec3 c0, c1, c2;
		{
			float t_x = 0.5f + 0.5f*(float)sin(phase0);
			float t_y = 0.5f + 0.5f*(float)cos(phase0 * 1.05545);
			float t_z = 0.5f + 0.5f*(float)cos(phase0 * 1.4243);

			c0 = vec3(t_x, t_y, t_z);
		}
		{
			float t_x = 0.5f + 0.5f*(float)sin(phase1);
			float t_y = 0.5f + 0.5f*(float)cos(phase1 * 1.4545);
			float t_z = 0.5f + 0.5f*(float)cos(phase1 * 1.2343);

			c1 = vec3(t_x, t_y, t_z);
		}
		{
			float t_x = 0.5f + 0.5f*(float)sin(phase2);
			float t_y = 0.5f + 0.5f*(float)cos(phase2 * 1.1545);
			float t_z = 0.5f + 0.5f*(float)cos(phase2 * 1.7343);

			c2 = vec3(t_x, t_y, t_z);
		}
		
		glUniform3f(col0Loc, c0.x, c0.y, c0.z);
		glUniform3f(col1Loc, c1.x, c1.y, c1.z);
		glUniform3f(col2Loc, c2.x, c2.y, c2.z);

		// Progression of the complex number 'c'
		const double rot_speed = 0.03;
		const double rot_r = 0.7 + sin(t * 0.01343) * 0.15;

		glUniform2f(cLoc, (float)(sin(t * rot_speed) * rot_r), (float)(cos(t * rot_speed) * rot_r));


		// Compute and upload trap parameters:
		const double trap_r2    = pow(1.2 + 0.6*sin(t * 0.0334534), 2.0);
		const double trap_blend = 0.5 + 0.5*sin(t * 0.23453);

		glUniform2f(trapLoc, (float)trap_r2, (float)trap_blend);

		// Render a triangle consisting of 3 vertices:
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		checkForError();
		glDrawArrays(GL_TRIANGLES, 0, 3);
		checkForError();

		// Display
		SDL_GL_SwapWindow(win);
		frame++;

		// Handle any events
		SDL_Event e;
		while(SDL_PollEvent(&e))
		{
			if(e.type == SDL_QUIT) // If user closes the window:
				quit = true;

			if(e.type == SDL_KEYDOWN) 
				if(e.key.keysym.sym == SDLK_ESCAPE) // If user presses ESC key:
					quit = true;
		}
	}

	const double total_elapsed = getCurTimeRealSec() - start_time;
	const double fps = frame / total_elapsed;
	std::cout << "FPS: " << fps << std::endl;

	SDL_Quit();
	return 0;
}
