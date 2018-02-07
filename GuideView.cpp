//
//  GuideView.cpp
//  qnol
//
//  Created by lizhou on 15-1-19.
//
//

#include "GuideView.h"
#include "GuideShader.h"
#include "GameScene.h"

GuideView::GuideView()
: _shaderLayer(NULL)
, _talkLayer(NULL)
{}

bool GuideView::init()
{
    if (CCNode::init()) {
        setContentSize(CCDirector::sharedDirector()->getWinSize());
        GameScene::openedView->addChild(this);
        return true;
    }
    return false;
}

GuideView* GuideView::createWithData(std::vector<std::string> talkInfo, const CCRect viewArea, GuideType guideType, GuideArrowDir arrowDir, CCPoint arrowPosRatio)
{
    GuideView *ret = GuideView::create();
    if (ret && ret->initWithData(talkInfo, viewArea, guideType, arrowDir, arrowPosRatio)) {
        return ret;
    }
    return NULL;
}

bool GuideView::initWithData(std::vector<std::string> talkInfo, const CCRect viewArea, GuideType guideType, GuideArrowDir arrowDir, CCPoint arrowPosRatio)
{
    if (_shaderLayer) {
        _shaderLayer->removeFromParent();
    }
    _shaderLayer = GuideShader::create(viewArea, guideType, arrowDir, arrowPosRatio);
	addChild(_shaderLayer);
    
    CCPoint pos = (viewArea.getMidX() < getContentSize().width/2) ? GuideTalk::getRightDefaultPos() : GuideTalk::getLeftDefaultPos();
    if (!_talkLayer) {
        _talkLayer = GuideTalk::createWithData(pos, talkInfo);
        if (_talkLayer) {
            addChild(_talkLayer, 1);
        }
    }
    else{
        _talkLayer->setVisible( _talkLayer->initWithDate(pos, talkInfo) );
    }
    
    return true;
}

GuideView* GuideView::createWithData(const std::vector<std::string> talkInfo, CCNode *targetNode, GuideType guideType, GuideArrowDir arrowDir, CCPoint arrowPosRatio)
{
    GuideView *ret = GuideView::create();
    if (ret && ret->initWithData(talkInfo, targetNode, guideType, arrowDir, arrowPosRatio)) {
        return ret;
    }
    return NULL;
}

bool GuideView::initWithData(const std::vector<std::string> talkInfo, CCNode *targetNode, GuideType guideType, GuideArrowDir arrowDir, CCPoint arrowPosRatio)
{
    CCRect viewArea = CCRectZero;
    if (targetNode) {
        CCRect viewArea = targetNode->boundingBox();
        viewArea.origin = targetNode->convertToWorldSpace(CCSizeZero);
        
        //消除nodo缩放后的影响
        float scaleX = targetNode->getScaleX();
        float scaleY = targetNode->getScaleY();
        CCNode *parent = targetNode->getParent();
        while (parent) {
            
            if (parent->getParent() == UIHandle::shared()) {
                float saveScale = parent->getScale();
                parent->setScale(1.0);
                viewArea.origin = targetNode->convertToWorldSpace(CCPointZero);
                parent->setScale(saveScale);
                break;
            }
            
            scaleX *= parent->getScaleX();
            scaleY *= parent->getScaleY();
            parent = parent->getParent();
        }
        
        viewArea.size.width *= scaleX;
        viewArea.size.height *= scaleY;
        
        return initWithData(talkInfo, viewArea, guideType, arrowDir, arrowPosRatio);
    }
    
    return false;
}

void GuideView::onEnter()
{
    CCNode::onEnter();
    CCDirector::sharedDirector()->getTouchDispatcher()->addTargetedDelegate(this, kTouchPriority_Guide, true);
}

void GuideView::onExit()
{
    CCDirector::sharedDirector()->getTouchDispatcher()->removeDelegate(this);
    CCNode::onExit();
}

bool GuideView::ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent)
{
    CCLOG("--GuideView::ccTouchBegan--");
    //是否朦灰
    if (_shaderLayer && _shaderLayer->isShader()) {
        //点击高亮区域时 直接穿过guide层到达下面ui层
        if(_shaderLayer->getArea().containsPoint(pTouch->getLocation())){
            //把pTouch的坐标改为_shaderLayer->getArea()的中心点，以保证因为_shaderLayer->getArea()区域过大时错点到旁边的UI
            CCPoint p = ccp(_shaderLayer->getArea().getMidX(), _shaderLayer->getArea().getMidY());
            p = CCDirector::sharedDirector()->convertToGL(p);
			pTouch->setTouchInfo(pTouch->getID(), p.x, p.y);
            return false;
        }
    }
    
    if (_talkLayer && _talkLayer->isVisible()) {
        _talkLayer->touchBegan(pTouch, pEvent);
    }
    
    return true;
}

void GuideView::addTalker(std::vector<std::string> talkInfo, CCPoint pos)
{
    GuideTalk *talkLayer = GuideTalk::createWithData(pos, talkInfo);
    if (talkLayer) {
        addChild(talkLayer, 1);
    }
}

void GuideView::closeView()
{
    removeFromParent();
}