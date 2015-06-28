#ifdef OPENGL_ES
precision mediump float;
#endif

varying vec2 v_texCoord;
varying vec4 v_color;
varying vec3 v_normal;

uniform vec4 u_color;
uniform sampler2D u_texture;
uniform int u_inverse;

vec4 inverse(vec4 color)
{
    return vec4(1.0 - color.r, 1.0 - color.g, 1.0 - color.b, color.a);
}

void main()
{
//    float x = v_texCoord.x;
//    if (u_color.a == 0.0) {
//        vec4 tex_color = texture2D(u_texture, v_texCoord) * vec4(1,1,1,u_color.r);
//        if (u_color.g == 1.0) {
//            gl_FragColor = inverse(tex_color);
//        } else {
//            gl_FragColor = tex_color;
//        }
//    } else {
//        gl_FragColor = u_color;
//    }
    gl_FragColor = v_color;
}