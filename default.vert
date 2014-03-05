#version 150 core

in vec4 Vertex;
uniform mat4 ModelViewProject;

//in vec4 qt_MultiTexCoord0;
//uniform mat4 qt_ModelViewProjectionMatrix;
//varying vec4 qt_TexCoord0;

void main(void)
{
    //gl_Position = qt_ModelViewProjectionMatrix * qt_Vertex;
    //qt_TexCoord0 = qt_MultiTexCoord0;

    gl_Position = ModelViewProject * Vertex;
}
