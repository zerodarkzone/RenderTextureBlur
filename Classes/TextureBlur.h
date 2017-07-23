#pragma once

#include "cocos2d.h"

class TextureBlur
{
	static cocos2d::GLProgramState* _blurHor;
	static cocos2d::GLProgramState* _blurVer;

public:
	static void initShader();
	static cocos2d::Texture2D* create(cocos2d::Texture2D* target, const int radius, const int step = 1);

private:
	static cocos2d::GLProgramState* getBlurShaderHor(cocos2d::Vec2 resolution, const int radius);
	static cocos2d::GLProgramState* getBlurShaderVer(cocos2d::Vec2 resolution, const int radius);
};