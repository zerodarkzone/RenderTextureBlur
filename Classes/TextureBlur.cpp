#include <iostream>
#include <chrono>
#include "TextureBlur.h"

USING_NS_CC;

static const int maxRadius = 64;

GLProgramState* TextureBlur::_blurHor = nullptr;
GLProgramState* TextureBlur::_blurVer = nullptr;
RenderTexture* TextureBlur::rtX = nullptr;
RenderTexture* TextureBlur::rtY = nullptr;
Size TextureBlur::_textureSize;

GLProgramState * TextureBlur::getBlurShaderHor(Vec2 resolution, const float radius)
{
	_blurHor->setUniformVec2("uResolution", resolution);
	_blurHor->setUniformFloat("dirx", 1.0);
	_blurHor->setUniformFloat("diry", 0.0);
	_blurHor->setUniformFloat("radius", radius);
	return _blurHor;
}

GLProgramState * TextureBlur::getBlurShaderVer(Vec2 resolution, const float radius)
{
	_blurVer->setUniformVec2("uResolution", resolution);
	_blurVer->setUniformFloat("dirx", 0.0);
	_blurVer->setUniformFloat("diry", 1.0);
	_blurVer->setUniformFloat("radius", radius);
	return _blurVer;
}

void TextureBlur::initShader(Size tSize)
{
	std::string blurShaderPath = FileUtils::getInstance()->fullPathForFilename("Shaders/TextureBlur.fsh");
	const GLchar* blur_frag = __String::createWithContentsOfFile(blurShaderPath.c_str())->getCString();

	auto _blurH = GLProgram::createWithByteArrays(ccPositionTextureColor_vert, blur_frag);
	auto _blurV = GLProgram::createWithByteArrays(ccPositionTextureColor_vert, blur_frag);

	_blurHor = GLProgramState::getOrCreateWithGLProgram(_blurH);
	_blurVer = GLProgramState::getOrCreateWithGLProgram(_blurV);

	_textureSize = tSize;

	rtX = RenderTexture::create(_textureSize.width, _textureSize.height, Texture2D::PixelFormat::RGB565);
	rtX->retain();

	rtY = RenderTexture::create(_textureSize.width, _textureSize.height, Texture2D::PixelFormat::RGB565);
	rtY->retain();

}

Texture2D* TextureBlur::create(Texture2D* target, const float radius, const int step)
{
	CCASSERT(target != nullptr, "Null pointer passed as a texture to blur");
	CCASSERT(radius <= maxRadius, "Blur radius is too big");
	CCASSERT(radius > 0, "Blur radius is too small");
	CCASSERT(step <= radius / 2 + 1, "Step is too big");
	CCASSERT(step > 0, "Step is too small");

	auto start = std::chrono::high_resolution_clock::now();

	Size textureSize = target->getContentSize();

	CCLOG("%f %f", textureSize.width, textureSize.height);
	CCLOG("%f %f", _textureSize.width, _textureSize.height);

	Vec2 pixelSize = Vec2(float(step) / textureSize.width, float(step) / textureSize.height);
	float radiusWithStep = radius / step;


	Sprite* stepX = Sprite::createWithTexture(target);
	stepX->retain();
	stepX->setPosition(Point(0.5f*textureSize.width, 0.5f*textureSize.height));
	stepX->setFlippedY(true);

	GLProgramState* blurX = getBlurShaderHor({ textureSize.width , textureSize.height }, radiusWithStep);
	stepX->setGLProgramState(blurX);


	rtX->begin();
	stepX->visit();
	rtX->end();

	Sprite* stepY = Sprite::createWithTexture(rtX->getSprite()->getTexture());
	stepY->retain();
	stepY->setAnchorPoint(Point(0.0f, 0.0f));
	stepY->setPosition(Point(0.0f, 0.0f));
	stepY->setFlippedY(true);

	GLProgramState* blurY = getBlurShaderVer({ textureSize.width , textureSize.height }, radiusWithStep);
	stepY->setGLProgramState(blurY);

	rtY->begin();
	stepY->visit();
	rtY->end();

	auto end = std::chrono::high_resolution_clock::now();
	auto dur = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
	CCLOG("Measured time: %lld ms", dur.count());

	return rtY->getSprite()->getTexture();
}

