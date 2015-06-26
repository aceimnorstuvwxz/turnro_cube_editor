#ifdef OPENGL_ES
precision mediump float;
#endif

varying vec2 v_texCoord;
varying vec4 v_color;
varying vec3 v_normal;

uniform vec4 u_color;

void main()
{
    float x = v_texCoord.x;
    if (x >= 0.95) {
        gl_FragColor = vec4(0.0, 1.0, 1.0, 1.0);
    } else {
        gl_FragColor = u_color;
    }
}