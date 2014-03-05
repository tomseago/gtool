#version 150 core

//uniform sampler2D qt_Texture0;
//varying vec4 qt_TexCoord0;

//in vec4 gl_Position;
out vec4 fColor;

void main(void)
{
//    gl_FragColor = texture2D(qt_Texture0, qt_TexCoord0.st);

    fColor = vec4(0.0, 0.2, 1.0, 1.0);

}
