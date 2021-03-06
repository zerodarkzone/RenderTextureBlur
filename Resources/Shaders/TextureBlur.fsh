#ifdef GL_ES
precision lowp float;
#endif

//"in" attributes from our vertex shader
varying vec2 v_texCoord;

//declare uniforms
uniform sampler2D u_texture;                                        
uniform vec2 uResolution;

uniform float radius;
uniform float dirx;
uniform float diry;

void main()
{
    //this will be our RGBA sum
    vec4 sum = vec4(0.0);

    //our original texcoord for this fragment
    vec2 tc = v_texCoord;

    //the amount to blur, i.e. how far off center to sample from
    //1.0 -> blur by one pixel
    //2.0 -> blur by two pixels, etc.
    float blur = radius / uResolution.x;

    //the direction of our blur
    //(1.0, 0.0) -> x-axis blur
    //(0.0, 1.0) -> y-axis blur
    float hstep = dirx;
    float vstep = diry;

    //apply blurring, using a 9-tap filter with predefined gaussian weights

    sum += texture2D(CC_Texture0, vec2(tc.x - 4.0*blur*hstep, tc.y - 4.0*blur*vstep)) * 0.05747;    
    sum += texture2D(CC_Texture0, vec2(tc.x - 3.0*blur*hstep, tc.y - 3.0*blur*vstep)) * 0.08203;    
	sum += texture2D(CC_Texture0, vec2(tc.x - 2.0*blur*hstep, tc.y - 2.0*blur*vstep)) * 0.11279;	
	sum += texture2D(CC_Texture0, vec2(tc.x - 1.0*blur*hstep, tc.y - 1.0*blur*vstep)) * 0.14938;

    sum += texture2D(CC_Texture0, vec2(tc.x, tc.y)) * 0.16951;

    sum += texture2D(CC_Texture0, vec2(tc.x + 1.0*blur*hstep, tc.y + 1.0*blur*vstep)) * 0.14938;    
    sum += texture2D(CC_Texture0, vec2(tc.x + 2.0*blur*hstep, tc.y + 2.0*blur*vstep)) * 0.11279;    
	sum += texture2D(CC_Texture0, vec2(tc.x + 3.0*blur*hstep, tc.y + 3.0*blur*vstep)) * 0.08203;	
	sum += texture2D(CC_Texture0, vec2(tc.x + 4.0*blur*hstep, tc.y + 4.0*blur*vstep)) * 0.05747;

    //discard alpha for our simple demo, multiply by vertex color and return
    gl_FragColor = vec4(sum.rgb, 1.0);
}
/*
w: 0.016951
w: 0.014938
w: 0.013041
w: 0.011279
w: 0.009664
w: 0.008203
w: 0.006898
w: 0.005747*/                                                                
