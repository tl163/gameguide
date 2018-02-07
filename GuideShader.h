//
//  GuideShader.h
//  jxcq
//
//  Created by liu geng on 13-9-17.
//
//

#ifndef __jxcq__GuideShader__
#define __jxcq__GuideShader__

#include <iostream>
#include "cocos2d.h"
#include "Tools.h"
#include "GuideConfig.h"

USING_NS_CC;

class UDFormatText;
class NormalAniNode;

#pragma mark -GuideShader

class GuideShader : public CCLayer {
public:
    GuideShader();
    
	static GuideShader *create(const CCRect viewArea, GuideType guideType = GuideType_compel, GuideArrowDir arrowDir = guideArrow_UpLeft, CCPoint arrowPosRatio = ccp(0.5, 0.5));
	
	void onEnter();
	const CCRect& getArea() {
		return _area;
	}
    
    /*
     * 是否显示了遮挡区域
     */
    bool isShader();
    
    void hideShader();
    
private:
    bool init(const CCRect viewArea, GuideType guideType = GuideType_compel, GuideArrowDir arrowDir = guideArrow_UpLeft, CCPoint arrowPosRatio = ccp(0.5, 0.5));
    
	void addShader();
    CCNode  *_shaderView;
	CCRect _area;
	CCSprite *_arrow;
	CCAnimate *_arrowAnimate;
};

#pragma mark -GuideTalk

class GuideTalk : public Utility::Singleton<GuideTalk>
                , public CCNode
//                , public CCTouchDelegate
{
public:
    GuideTalk();
    
    CREATE_FUNC(GuideTalk);
    bool init();
    
    static GuideTalk *createWithData(CCPoint pos, vector<string>& talks);
    bool initWithDate(CCPoint pos, vector<string>& talks);
    
    static GuideTalk *createWithDefLeft(vector<string>& talks){return createWithData(ccp(250, 150), talks);};
    static GuideTalk *createWithDefRight(vector<string>& talks){return createWithData(ccp(750, 150), talks);};
    
    static CCPoint getLeftDefaultPos(){return ccp(250, 150);};
    static CCPoint getRightDefaultPos(){return ccp(750, 150);};
    
    bool touchBegan(CCTouch *pTouch, CCEvent *pEvent);
    
private:
    void onEnter();
    void onExit();
    
    void talkViewAction(float t);
    
    bool showTalk();
    
    void setGuiderPosition(CCPoint pos);
    
private:
    CCSprite        *_guider;
    CCSprite        *_talkView;
    UDFormatText    *_textView;
    
    enum{MonsterType = 39};
    vector<string>  _talks;
    
    int             _talkIdx;
};

#pragma mark -LineBox

class LineBox : public CCNode {
public:
	CREATE_FUNC(LineBox);
	void draw();
};

#pragma mark -GuideArrow

class GuideArrow : public CCSprite {
public:
	static GuideArrow *create(char arrowDir);
	bool init(char arrowDir);
	void onEnter();
};

#pragma mark -Ripple

class Ripple : public CCNode {
public:
	CREATE_FUNC(Ripple);
	void onEnter();
	void onExit();
	void addRipple(float dt);
};

#endif /* defined(__jxcq__GuideShader__) */
