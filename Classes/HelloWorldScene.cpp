#include "HelloWorldScene.h"
#include "TextureBlur.h"
#include "2d/CCRenderTexture.h"

USING_NS_CC;

Scene* HelloWorld::createScene()
{
	// 'scene' is an autorelease object
	auto scene = Scene::create();

	// 'layer' is an autorelease object
	auto layer = HelloWorld::create();

	// add layer as a child to scene
	scene->addChild(layer);

	// return the scene
	return scene;
}

// on "init" you need to initialize your instance
bool HelloWorld::init()
{
	//////////////////////////////
	// 1. super init first
	if (!Layer::init())
	{
		return false;
	}

	Size screenSize = Director::getInstance()->getWinSize();

	float scale = 2.0f;

	/*auto sky = Sprite::create("sky.png");
	sky->retain();
	sky->setScaleX(screenSize.width / (scale * sky->getBoundingBox().size.width));
	sky->setScaleY(screenSize.height / (scale * sky->getBoundingBox().size.height));
	sky->setAnchorPoint(Point(0.0f, 0.0f));
	sky->setPosition(0.0f, 0.0f);

	auto clouds = Sprite::create("clouds.png");
	clouds->retain();
	clouds->setScale(screenSize.width / (scale * clouds->getBoundingBox().size.width));
	clouds->setAnchorPoint(Point(0.0f, 1.0f));
	clouds->setPosition(Point(0, screenSize.height / scale));

	auto ground = Sprite::create("ground.png");
	ground->retain();
	ground->setScale(screenSize.width / (scale * ground->getBoundingBox().size.width));
	ground->setAnchorPoint(Point(0.0f, 0.0f));
	ground->setPosition(0.0f, 0.0f);*/

	auto image = Sprite::create("badland.png");
	image->retain();
	image->setScale(screenSize.width / (scale * image->getBoundingBox().size.width));
	image->setAnchorPoint(Point(0.0f, 0.0f));
	image->setPosition(0.0f, 0.0f);

	auto rt = RenderTexture::create(screenSize.width / scale, screenSize.height / scale, Texture2D::PixelFormat::RGB565);
	rt->retain();
	rt->begin();
	image->visit();
	rt->end();

	TextureBlur::initShader(screenSize / scale);
	auto texture = TextureBlur::create(rt->getSprite()->getTexture(), 32);
	auto sprite = Sprite::createWithTexture(texture);
	sprite->getTexture()->setAntiAliasTexParameters();
	sprite->setAnchorPoint(Point(0.0f, 0.0f));
	sprite->setScale(scale);
	sprite->setFlippedY(true);
	addChild(sprite);

	return true;
}
