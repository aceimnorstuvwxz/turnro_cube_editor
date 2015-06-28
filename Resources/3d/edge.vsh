#ifdef GL_ES
precision mediump float;
#endif

// Inputs
attribute vec4 a_color;
attribute vec4 a_position;
attribute vec2 a_texCoord;
attribute vec3 a_normal;

// Varyings
//varying vec2 v_texCoord;
#ifdef GL_ES
varying mediump vec2 v_texCoord;
varying mediump vec4 v_color;
varying mediump vec3 v_normal;
#else
varying vec2 v_texCoord;
varying vec4 v_color;
varying vec3 v_normal;
#endif

//uniform	float radius ;
//uniform	float angle ;

//vec2 vortex( vec2 uv )
//{
//	uv -= vec2(0.5, 0.5);
//	float dist = length(uv);
//	float percent = (radius - dist) / radius;
//	if ( percent < 1.0 && percent >= 0.0) 
//	{
//		float theta = percent * percent * angle * 8.0;
//		float s = sin(theta);
//		float c = cos(theta);
//		uv = vec2(dot(uv, vec2(c, -s)), dot(uv, vec2(s, c)));
//	}
//	uv += vec2(0.5, 0.5);
//
//	return uv;
//}

uniform vec4 u_color;

void main()
{
    gl_Position = CC_MVPMatrix * a_position;
    v_texCoord = a_texCoord;
    v_color = a_color;
    v_normal = a_normal;
}