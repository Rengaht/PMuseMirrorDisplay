
precision highp float;

uniform sampler2D tex0;

uniform float amount;
uniform float angle;
uniform float windowWidth;
uniform float windowHeight;

varying vec2 texCoordVarying;


void main()
{
	vec2 dir=vec2(cos(angle)/windowWidth,sin(angle)/windowHeight)*amount;
	vec4 color = vec4(0.0, 0.0, 0.0, 0.0);
	vec4 cr=texture2D(tex0,texCoordVarying+dir);
	vec4 cga=texture2D(tex0,texCoordVarying);
	vec4 cb=texture2D(tex0,texCoordVarying-dir);


    	gl_FragColor = vec4(cr.r,cga.g,cb.b,1.0);
}
