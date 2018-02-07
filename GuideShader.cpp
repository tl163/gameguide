//
//  GuideShader.cpp
//  jxcq
//
//  Created by liu geng on 13-9-17.
//
//

#include "GuideShader.h"
#include "ValueDefine.h"
#include "UDButton.h"
#include "OutsideLight.h"
#include "RoleManager.h"
#include "RoleConfigs.h"
#include "UDFormatText.h"
#include "AnimationNode.h"
#include "CBConst.h"
#include "Global.h"

#pragma mark -GuideShader

GuideShader::GuideShader()
: _shaderView(NULL)
{}

GuideShader *GuideShader::create(const CCRect viewArea, GuideType guideType, GuideArrowDir arrowDir, CCPoint arrowPosRatio) {

	GuideShader *ret = new GuideShader;
	if (ret && ret->init(viewArea, guideType, arrowDir, arrowPosRatio)) {
		ret->autorelease();
		return ret;
	}
	CC_SAFE_DELETE(ret);
	return NULL;
}

bool GuideShader::init(const CCRect viewArea, GuideType guideType, GuideArrowDir arrowDir, CCPoint arrowPosRatio) {

	if (CCLayer::init()) {

		_area = viewArea;
        _area.origin.x = MAX(_area.origin.x, 0);
        _area.origin.y = MAX(_area.origin.y, 0);
        CCLOG("viewArea: x=%f, y=%f, w=%f, h=%f", viewArea.origin.x, viewArea.origin.y, viewArea.size.width, viewArea.size.height);
        CCLOG("guideType:%d", guideType);
		(guideType == GuideType_compel)? addShader() : hideShader();

		CCPoint p = ccp(_area.origin.x + _area.size.width*arrowPosRatio.x,
						_area.origin.y + _area.size.height*arrowPosRatio.y);
		GuideArrow *arrow = GuideArrow::create(arrowDir);
		arrow->setPosition(p);
		addChild(arrow);

		return true;
	}
	return false;
}

void GuideShader::onEnter() {

	CCLayer::onEnter();

	CCSize s(600, 600);

	CCPoint center = _area.origin+ccpMult(_area.size, 0.5);

	LineBox *box = LineBox::create();
	box->setContentSize(s);
	box->setAnchorPoint(ccp(0.5, 0.5));
	box->setPosition(center);
	addChild(box);

	box->runAction(CCSequence::createWithTwoActions(CCScaleTo::create(0.5, _area.size.width/s.width, _area.size.height/s.height), CCCallFunc::create(box, callfunc_selector(LineBox::removeFromParent))));
}

void GuideShader::hideShader()
{
    if (_shaderView) {
        _shaderView->setVisible(false);
    }
}

void GuideShader::addShader() {

//	CCSprite *shader = CCSprite::create("png/guide_shader.png");
//	shader->setScaleX(_area.size.width/shader->getContentSize().width);
//	shader->setScaleY(_area.size.height/shader->getContentSize().height);
//	shader->setAnchorPoint(CCPointZero);
//	shader->setPosition(_area.origin);
//	addChild(shader);
//	shader->setOpacity(150);
    
    if (!_shaderView) {
        _shaderView = CCNode::create();
        _shaderView->setContentSize(getContentSize());
        addChild(_shaderView);
    }
    _shaderView->setVisible(true);

	ccColor4B c = ccc4(0, 0, 0, 150);
	CCSize s = getContentSize();

	CCLayerColor *lay0 = CCLayerColor::create(c);
	lay0->setContentSize(CCSizeMake(s.width, _area.origin.y));
	_shaderView->addChild(lay0, 0, 0);

	CCLayerColor *lay1 = CCLayerColor::create(c);
	lay1->setContentSize(CCSizeMake(s.width, s.height-_area.origin.y-_area.size.height));
	lay1->setPosition(ccp(0, _area.origin.y+_area.size.height));
	_shaderView->addChild(lay1, 0, 1);

	CCLayerColor *lay2 = CCLayerColor::create(c);
	lay2->setContentSize(CCSizeMake(_area.origin.x, _area.size.height));
	lay2->setPosition(ccp(0, _area.origin.y));
	_shaderView->addChild(lay2, 0, 2);

	CCLayerColor *lay3 = CCLayerColor::create(c);
	lay3->setContentSize(CCSizeMake(s.width-_area.origin.x-_area.size.width, _area.size.height));
	lay3->setPosition(ccp(_area.origin.x+_area.size.width, _area.origin.y));
	_shaderView->addChild(lay3, 0, 3);
}

bool GuideShader::isShader()
{
    if (_shaderView) {
        return _shaderView->isVisible();
    }
    return false;
}

#pragma mark -GuideTalk

GuideTalk::GuideTalk()
: _guider(NULL)
, _talkView(NULL)
, _textView(NULL)
, _talkIdx(0)
{}

bool GuideTalk::init()
{
    if (CCNode::init()) {
        setContentSize(CCDirector::sharedDirector()->getWinSize());
        
        return true;
    }
    return false;
}

GuideTalk *GuideTalk::createWithData(CCPoint pos, vector<string>& talks)
{
    GuideTalk *ret = GuideTalk::create();
    if (ret && ret->initWithDate(pos, talks)) {
        return ret;
    }
    CC_SAFE_RELEASE(ret);
    return NULL;
}

bool GuideTalk::initWithDate(CCPoint pos, vector<string>& talks)
{
    if (talks.size() != 0) {
        _talks = talks;
        _talkIdx = 0;
        
        if (!_guider) {
//            _guider = NormalAniNode::create(MonsterType, LivingObject_STATE_STAND, MoveDir_DOWN);
//            _guider = CCSprite::create("icon/head/half_npc_5.pvr.ccz");
            _guider = CCSprite::create("png/guider.png");
            _guider->setAnchorPoint(ccp(0.5f, 0));
            addChild(_guider);
            
            CCSprite *titleBg = CCSprite::createWithSpriteFrameName("npc_namebox.png");
            titleBg->setAnchorPoint(ccp(0.5f, 1.0f));
            titleBg->setPosition(ccp(_guider->getContentSize().width/2, 0));
            _guider->addChild(titleBg);
            
            CCLabelTTF *lab = CCLabelTTF::create(String(guiderName), "Arial", 24);
            lab->setColor(ccYELLOW);
            AddStroke(lab);
            lab->setAnchorPoint(ccp(0.5f, 0.5f));
            lab->setPosition(ccpFromSize(titleBg->getContentSize())/2);
            titleBg->addChild(lab);
        }
        
        if (!_talkView) {
            _talkView = CCSprite::createWithSpriteFrameName("npc_window_speak.png");
            addChild(_talkView);
        }
        
        if (!_textView) {
            _textView = UDFormatText::createWithContentSize(ccpFromSize(_talkView->getContentSize())*0.9);
            _textView->setPosition(ccpFromSize(_talkView->getContentSize())*0.05 - ccp(5, 2));
            _talkView->addChild(_textView);
        }
        else{
            showTalk();
        }
        
        setGuiderPosition(pos);
        _talkView->setVisible(false);
        
        return true;
    }
    
    return false;
}

void GuideTalk::setGuiderPosition(CCPoint pos)
{
    _guider->setPosition(pos);
    if (pos.x < getContentSize().width/2) {
        _guider->setFlipX(false);
        _talkView->setFlipX(false);
        _talkView->setAnchorPoint(CCPointZero);
        _talkView->setPosition(ccp(pos.x + 20, pos.y + 230));
        
        _textView->setPosition(ccpFromSize(_talkView->getContentSize())*0.05 - ccp(-11, 2));
    }
    else{
        _guider->setFlipX(true);
        _talkView->setFlipX(true);
        _talkView->setAnchorPoint(ccp(1, 0));
        _talkView->setPosition(ccp(pos.x - 20, pos.y + 230));
        
        _textView->setPosition(ccpFromSize(_talkView->getContentSize())*0.05 - ccp(5, 2));
    }
}

void GuideTalk::onEnter()
{
    CCNode::onEnter();
//    CCDirector::sharedDirector()->getTouchDispatcher()->addTargetedDelegate(this, kTouchPriority_Top, true);
    showTalk();
}

void GuideTalk::onExit()
{
    unschedule(schedule_selector(GuideTalk::talkViewAction));
//    CCDirector::sharedDirector()->getTouchDispatcher()->removeDelegate(this);
    CCNode::onExit();
}

bool GuideTalk::touchBegan(CCTouch *pTouch, CCEvent *pEvent)
{
    if (showTalk()) {
        return true;
    }
    
    return false;
}

bool GuideTalk::showTalk()
{
    if (_talkIdx < _talks.size()) {
        TagParser parser;
        string str("");
        parser.addText(str, ccBLACK, 20, _talks[_talkIdx++].data());
        
        _talkView->setVisible(false);
        
        _textView->setText(str, false, 20);
        schedule(schedule_selector(GuideTalk::talkViewAction), 0, 1, 0.1f);
        return true;
    }
    return false;
}

void GuideTalk::talkViewAction(float t)
{
    unschedule(schedule_selector(GuideTalk::talkViewAction));
    
    _talkView->setVisible(true);
    _talkView->setScale(0.01);
    _talkView->runAction(CCScaleTo::create(0.1f, 1.0f, 1.0f));
}

#pragma mark -LineBox

void LineBox::draw() {

	CCNode::draw();
	ccDrawColor4B(255, 255, 0, 255);
	glLineWidth(1);
	ccDrawRect(CCPointZero, getContentSize());
}

#pragma mark -GuideArrow

GuideArrow *GuideArrow::create(char arrowDir) {

	GuideArrow *ret = new GuideArrow;
	if (ret && ret->init(arrowDir)) {
		ret->autorelease();
		return ret;
	}
	CC_SAFE_DELETE(ret);
	return NULL;
}

bool GuideArrow::init(char arrowDir) {

	if (!CCSprite::initWithSpriteFrameName("shouzhi-1.png")) return false;

	switch (arrowDir) {
		case 0:	// upleft
			setAnchorPoint(ccp(0.08, 0.82));
			break;
		case 1:	// upright
			setAnchorPoint(ccp(0.92, 0.82));
			setFlipX(true);
			break;
		case 2:	// downleft
			setAnchorPoint(ccp(0.08, 0.82));
			setRotation(-90);
			break;
		case 3:	// downright
			setAnchorPoint(ccp(0.92, 0.82));
			setRotation(90);
			setFlipX(true);
			break;
		default:
			break;
	}

	Ripple *rip = Ripple::create();
	rip->setPosition(getContentSize().width*getAnchorPoint().x,
					 getContentSize().height*getAnchorPoint().y);
	addChild(rip, -1);

	return true;
}

void GuideArrow::onEnter() {

	CCSprite::onEnter();

	CCSpriteFrame *frame = CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName("shouzhi-1.png");
	CCSpriteFrame *frame1 = CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName("shouzhi-2.png");
	CCAnimation *animation = CCAnimation::createWithSpriteFrames(CCArray::create(frame, frame1, NULL), 0.4);
	CCAnimate *arrowAnimate = CCAnimate::create(animation);

	runAction(CCRepeatForever::create(arrowAnimate));
}


#pragma mark -Ripple

void Ripple::onEnter() {

	CCNode::onEnter();
	schedule(schedule_selector(Ripple::addRipple), 0.4);
}

void Ripple::onExit() {

	CCNode::onExit();
	unschedule(schedule_selector(Ripple::addRipple));
	removeAllChildren();
}

void Ripple::addRipple(float dt) {

	CCSprite *img = CCSprite::createWithSpriteFrameName("guangquan.png");
	img->setScale(0.1);
	addChild(img);

	float dur = 1.2f;
	img->runAction(CCSequence::createWithTwoActions(CCSpawn::createWithTwoActions(CCScaleTo::create(dur, 1.2), CCSequence::create(CCFadeTo::create(dur/2, 255), CCFadeTo::create(dur/2, 0), NULL)), CCCallFunc::create(img, callfunc_selector(CCSprite::removeFromParent))));
}

