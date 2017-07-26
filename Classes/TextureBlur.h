#pragma once

#include "cocos2d.h"

class TextureBlur
{
	static cocos2d::GLProgramState* _blurHor;
	static cocos2d::GLProgramState* _blurVer;
	static cocos2d::RenderTexture* rtX;
	static cocos2d::RenderTexture* rtY;

	static cocos2d::Size _textureSize;

public:
	static void initShader(cocos2d::Size tSize);
	static cocos2d::Texture2D* create(cocos2d::Texture2D* target, const int radius, const int step = 1);

private:
	static void calculateGaussianWeights(const int points, float* weights);
	static cocos2d::GLProgramState* getBlurShaderHor(cocos2d::Vec2 pixelSize, int radius, float* weights);
	static cocos2d::GLProgramState* getBlurShaderVer(cocos2d::Vec2 pixelSize, int radius, float* weights);
};