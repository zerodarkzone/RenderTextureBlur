#ifndef __HELLOWORLD_SCENE_H__
#define __HELLOWORLD_SCENE_H__

#include "cocos2d.h"
#include "TextureBlur.h"

namespace cocos2d
{
	class RenderTexture;
}


class HelloWorld : public cocos2d::Layer
{
public:
    static cocos2d::Scene* createScene();

    virtual bool init();
    
    // a selector callback
    void menuCloseCallback(cocos2d::Ref* pSender);
    
    // implement the "static create()" method manually
    CREATE_FUNC(HelloWorld);

    RenderTexture *render;
    TextureBlur textureBlur;
    
private:
	void completionCallback(const std::string& name);
};

#endif // __HELLOWORLD_SCENE_H__
