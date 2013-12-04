//https://github.com/daw42/glslcookbook/blob/master/ingredients/glslprogram.h
#ifndef GLSLPROGRAM_H
#define GLSLPROGRAM_H


#include <string>
using std::string;
#include <map>

#include <glm/glm.hpp>
using glm::vec2;
using glm::vec3;
using glm::vec4;
using glm::mat4;
using glm::mat3;

namespace GLSLShader {
    enum GLSLShaderType {
        VERTEX, FRAGMENT, GEOMETRY,
        TESS_CONTROL, TESS_EVALUATION,
		COMPUTE
    };
};

class GLSLProgram
{
private:
    int  handle;
    bool linked;
    string logString;

	std::map<string, int> uniformLocations;

    int  getUniformLocation(const char * name );
    bool fileExists( const string & fileName );

public:
    GLSLProgram();

    bool   compileShaderFromFile( const char * fileName, GLSLShader::GLSLShaderType type );
    bool   compileShaderFromString( const string & source, GLSLShader::GLSLShaderType type );
    bool   link();
    bool   validate();
    void   use();
	void   unuse();
	void   remove();

    string log();

    int    getHandle();
    bool   isLinked();

    void   bindAttribLocation( unsigned int location, const char * name);
    void   bindFragDataLocation( unsigned int location, const char * name );

    void   setUniform( const char *name, float x, float y, float z);
    void   setUniform( const char *name, const vec2 & v);
    void   setUniform( const char *name, const vec3 & v);
    void   setUniform( const char *name, const vec4 & v);
    void   setUniform( const char *name, const mat4 & m);
    void   setUniform( const char *name, const mat3 & m);
    void   setUniform( const char *name, float val );
    void   setUniform( const char *name, int val );
    void   setUniform( const char *name, bool val );

    void   printActiveUniforms();
    void   printActiveAttribs();
};

#endif // GLSLPROGRAM_H