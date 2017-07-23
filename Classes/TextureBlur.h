#pragma once

#include "cocos2d.h"

USING_NS_CC;

class TextureBlur
{
	static GLProgramState* _blurHor;
	static GLProgramState* _blurVer;

public:
	void initShader(cocos2d::Size tSize);
	Texture2D* create(cocos2d::Texture2D* target, const int radius, const int step = 1);
    
private:
    RenderTexture* rtX;
    RenderTexture* rtY;

	Size _textureSize;
    
    static GLProgramState* getBlurShaderHor(Vec2 resolution, const int radius);
	static GLProgramState* getBlurShaderVer(Vec2 resolution, const int radius);
};
