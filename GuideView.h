//
//  GuideView.h
//  qnol
//
//  Created by lizhou on 15-1-19.
//
//

#ifndef __qnol__GuideView__
#define __qnol__GuideView__

#include <iostream>
#include "cocos2d.h"
#include "once_singleton.h"
#include "GuideConfig.h"

USING_NS_CC;

class GuideShader;
class GuideTalk;

class GuideView : public once_singleton<GuideView>
                , public CCNode
                , public CCTouchDelegate
{
public:
    GuideView();
    
    CREATE_FUNC(GuideView);
    bool init();
    /*
     * talkInfo: 谈话内容
     * viewArea: 可点击区域
     * guideType: 引导类型
     * arrowDir: 指引手指的方向
     * arrowPosRatio: 指引手指在viewArea中显示的position百分比
     */
    static GuideView* createWithData(const std::vector<std::string> talkInfo, const CCRect viewArea, GuideType guideType = GuideType_compel, GuideArrowDir arrowDir = guideArrow_UpLeft, CCPoint arrowPosRatio = ccp(0.5, 0.5));
    
    bool initWithData(const std::vector<std::string> talkInfo, const CCRect viewArea, GuideType guideType = GuideType_compel, GuideArrowDir arrowDir = guideArrow_UpLeft, CCPoint arrowPosRatio = ccp(0.5, 0.5));
    
    /*
     * targetNode: 触发下一步引导的node
     */
    static GuideView* createWithData(const std::vector<std::string> talkInfo, CCNode *targetNode, GuideType guideType = GuideType_compel, GuideArrowDir arrowDir = guideArrow_UpLeft, CCPoint arrowPosRatio = ccp(0.5, 0.5));
    
    bool initWithData(const std::vector<std::string> talkInfo, CCNode *targetNode, GuideType guideType = GuideType_compel, GuideArrowDir arrowDir = guideArrow_UpLeft, CCPoint arrowPosRatio = ccp(0.5, 0.5));
    
    void addTalker(std::vector<std::string> talkInfo, CCPoint pos = ccp(250, 150));
    
    void closeView();
    
private:
    void onEnter();
    void onExit();
    
    bool ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent);
    
private:
    GuideShader     *_shaderLayer;
    GuideTalk       *_talkLayer;
};

#endif /* defined(__qnol__GuideView__) */
