#include "esUtil.h"
#include <stdlib.h>

GLuint ESUTIL_API esLoadShader( GLenum type, const char* shaderSrc )
{
    GLuint shader;
    GLint compiled;

    // Create the shader object
    shader = glCreateShader( type );
    if( 0 == shader )
    {
        return shader;
    }

    // Load the shader source
    glShaderSource( shader, 1, &shaderSrc, NULL );

    // Compile the shader
    glCompileShader( shader );

    // Check the compile status
    glGetShaderiv( shader, GL_COMPILE_STATUS, &compiled );
    if( !compiled )
    {
        GLint infoLen = 0;
        glGetShaderiv( shader, GL_INFO_LOG_LENGTH, &infoLen);

        if( infoLen > 1 )
        {
            char* infoLog = (char*)malloc( sizeof( char ) * infoLen );
            glGetShaderInfoLog( shader, infoLen, NULL, infoLog );

            esLogMessage( "Error compiling shader:\n%s\n", infoLog );
            free( infoLog );
        }

        glDeleteShader( shader );
        return 0;
    }

    return shader;
}

GLuint ESUTIL_API esLoadProgram( const char* vertShaderSrc, const char* fragShaderSrc )
{
    GLuint vertexShader;
    GLuint fragmentShader;
    GLuint programObject;
    GLint linked;

    // Load the vertex/fragment shader
    vertexShader = esLoadShader( GL_VERTEX_SHADER, vertShaderSrc );
    if( 0 == vertexShader )
    {
        return 0;
    }

    fragmentShader = esLoadShader( GL_FRAGMENT_SHADER, fragShaderSrc );
    if( 0 == fragmentShader )
    {
        glDeleteShader( vertexShader );
        return 0;
    }

    // Create the program object
    programObject = glCreateProgram();
    if( 0 == programObject )
    {
        return 0;
    }

    glAttachShader( programObject, vertexShader );
    glAttachShader( programObject, fragmentShader );

    // Link the program
    glLinkProgram( programObject );

    // Check the link status
    glGetProgramiv( programObject, GL_LINK_STATUS, &linked );
    if( !linked )
    {
        GLint infoLen = 0;
        glGetProgramiv( programObject, GL_INFO_LOG_LENGTH, &infoLen );
        if( infoLen > 1 )
        {
            char* infoLog = (char*)malloc( sizeof( char ) * infoLen );
            glGetProgramInfoLog( programObject, infoLen, NULL, infoLog );

            esLogMessage( "Error linking program:\n%s\n", infoLog );
            free( infoLog );
        }

        glDeleteProgram( programObject );
        return 0;
    }

    return programObject;
}