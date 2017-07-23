#include <iostream>
#include <chrono>
#include "TextureBlur.h"

USING_NS_CC;

static const int maxRadius = 64;

GLProgramState* TextureBlur::_blurHor = nullptr;
GLProgramState* TextureBlur::_blurVer = nullptr;

cocos2d::GLProgramState * TextureBlur::getBlurShaderHor(cocos2d::Vec2 resolution, const int radius)
{
	_blurHor->setUniformVec2("uResolution", resolution);
	_blurHor->setUniformFloat("dirx", 1.0);
	_blurHor->setUniformFloat("diry", 0.0);
	_blurHor->setUniformFloat("radius", radius);
	return _blurHor;
}

cocos2d::GLProgramState * TextureBlur::getBlurShaderVer(cocos2d::Vec2 resolution, const int radius)
{
	_blurVer->setUniformVec2("uResolution", resolution);
	_blurVer->setUniformFloat("dirx", 0.0);
	_blurVer->setUniformFloat("diry", 1.0);
	_blurVer->setUniformFloat("radius", radius);
	return _blurVer;
}

void TextureBlur::initShader()
{
	std::string blurShaderPath = FileUtils::getInstance()->fullPathForFilename("Shaders/TextureBlur.fsh");
	const GLchar* blur_frag = String::createWithContentsOfFile(blurShaderPath.c_str())->getCString();

	auto _blurH = GLProgram::createWithByteArrays(ccPositionTextureColor_vert, blur_frag);
	auto _blurV = GLProgram::createWithByteArrays(ccPositionTextureColor_vert, blur_frag);

	_blurHor = GLProgramState::getOrCreateWithGLProgram(_blurH);
	_blurVer = GLProgramState::getOrCreateWithGLProgram(_blurV);
}

Texture2D* TextureBlur::create(Texture2D* target, const int radius, const int step)
{
	CCASSERT(target != nullptr, "Null pointer passed as a texture to blur");
	CCASSERT(radius <= maxRadius, "Blur radius is too big");
	CCASSERT(radius > 0, "Blur radius is too small");
	CCASSERT(step <= radius / 2 + 1, "Step is too big");
	CCASSERT(step > 0, "Step is too small");

	auto start = std::chrono::high_resolution_clock::now();

	Size textureSize = target->getContentSize();
	Vec2 pixelSize = Vec2(float(step) / textureSize.width, float(step) / textureSize.height);
	int radiusWithStep = radius / step;


	Sprite* stepX = CCSprite::createWithTexture(target);
	stepX->retain();
	stepX->setPosition(Point(0.5f*textureSize.width, 0.5f*textureSize.height));
	stepX->setFlippedY(true);

	GLProgramState* blurX = getBlurShaderHor({ textureSize.width , textureSize.height }, radiusWithStep);
	stepX->setGLProgramState(blurX);

	RenderTexture* rtX = RenderTexture::create(textureSize.width, textureSize.height);
	rtX->retain();
	rtX->begin();
	stepX->visit();
	rtX->end();

	Sprite* stepY = CCSprite::createWithTexture(rtX->getSprite()->getTexture());
	stepY->retain();
	stepY->setAnchorPoint(Point(0.0f, 0.0f));
	stepY->setPosition(Point(0.0f, 0.0f));
	stepY->setFlippedY(true);

	Size screenSize = Director::getInstance()->getWinSize();

	GLProgramState* blurY = getBlurShaderVer({ textureSize.width , textureSize.height }, radiusWithStep);
	stepY->setGLProgramState(blurY);

	RenderTexture* rtY = RenderTexture::create(textureSize.width, textureSize.height);
	rtY->retain();
	rtY->begin();
	stepY->visit();
	rtY->end();

	auto end = std::chrono::high_resolution_clock::now();
	auto dur = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
	CCLOG("Measured time: %d ms", (int)dur.count());

	return rtY->getSprite()->getTexture();
}

