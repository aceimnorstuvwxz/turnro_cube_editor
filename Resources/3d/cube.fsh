#ifdef OPENGL_ES
precision mediump float;
#endif

varying vec2 v_texCoord;
varying vec4 v_color;
varying vec3 v_normal;

uniform vec4 u_color;
uniform sampler2D u_texture;

void main()
{
    float x = v_texCoord.x;
    if (u_color.a == 0.0) {
        gl_FragColor = texture2D(u_texture, v_texCoord) * vec4(1,1,1,u_color.r);
    } else {
        gl_FragColor = u_color;
    }
}