#ifdef OPENGL_ES
precision mediump float;
#endif

varying vec2 v_texCoord;

void main()
{
    float x = v_texCoord.x;
    if (x >= 0.98) {
        gl_FragColor = vec4(0.0, 1.0, 0.0, 1.0);
    } else {
        gl_FragColor = vec4(x, 1.0, 1.0, 1.0);
    }
}