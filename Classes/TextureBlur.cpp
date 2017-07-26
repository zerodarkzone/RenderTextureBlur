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

void TextureBlur::calculateGaussianWeights(const int points, float* weights)
{
	float dx = 1.0f / float(points - 1);
	float sigma = 1.0f / 3.0f;
	float norm = 1.0f / (sqrtf(2.0f*M_PI)*sigma*points);
	float divsigma2 = 0.5f / (sigma*sigma);
	weights[0] = 1.0f;
	for (int i = 1; i < points; i++)
	{
		float x = float(i)*dx;
		weights[i] = norm*expf(-x*x*divsigma2);
		weights[0] -= 2.0f*weights[i];
	}
}

cocos2d::GLProgramState * TextureBlur::getBlurShaderHor(cocos2d::Vec2 pixelSize, int radius, float * weights)
{
	_blurHor->setUniformVec2("pixelSize", pixelSize);
	_blurHor->setUniformVec2("direction", Vec2(1.0f, 0.0f));
	_blurHor->setUniformInt("radius", radius);
	_blurHor->setUniformFloatv("weights", radius, weights);

	return _blurHor;
}

cocos2d::GLProgramState * TextureBlur::getBlurShaderVer(cocos2d::Vec2 pixelSize, int radius, float * weights)
{
	_blurVer->setUniformVec2("pixelSize", pixelSize);
	_blurVer->setUniformVec2("direction", Vec2(0.0f, 1.0f));
	_blurVer->setUniformInt("radius", radius);
	_blurVer->setUniformFloatv("weights", radius, weights);

	return _blurVer;
}

void TextureBlur::initShader(cocos2d::Size tSize)
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

	float* weights = new float[maxRadius];
	calculateGaussianWeights(radiusWithStep, weights);

	Sprite* stepX = CCSprite::createWithTexture(target);
	stepX->retain();
	stepX->setPosition(Point(0.5f*textureSize.width, 0.5f*textureSize.height));
	stepX->setFlippedY(true);

	GLProgramState* blurX = getBlurShaderHor(pixelSize, radiusWithStep, weights);
	stepX->setGLProgramState(blurX);

	rtX->begin();
	stepX->visit();
	rtX->end();

	Sprite* stepY = CCSprite::createWithTexture(rtX->getSprite()->getTexture());
	stepY->retain();
	stepY->setAnchorPoint(Point(0.0f, 0.0f));
	stepY->setPosition(Point(0.0f, 0.0f));
	stepY->setFlippedY(true);

	Size screenSize = Director::getInstance()->getWinSize();

	GLProgramState* blurY = getBlurShaderVer(pixelSize, radiusWithStep, weights);
	stepY->setGLProgramState(blurY);

	rtY->begin();
	stepY->visit();
	rtY->end();

	auto end = std::chrono::high_resolution_clock::now();
	auto dur = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
	CCLOG("Measured time: %d ms", dur.count());
	std::cout << "Measured time: " << dur.count() << "ms" << std::endl;

	return rtY->getSprite()->getTexture();
}

