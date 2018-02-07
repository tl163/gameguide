//
//  NewGuideManager.h
//  qnol
//
//  Created by lizhou on 15-1-17.
//
//

#ifndef __qnol__NewGuideManager__
#define __qnol__NewGuideManager__

#include "cocos2d.h"
#include "Singleton.h"
#include "GuideConfig.h"

using namespace std;
USING_NS_CC;

class Task;
class GuideShader;
class TriggerEventInfo;

class NewGuideManager : public Utility::Singleton<NewGuideManager>
                      , public CCObject
{
public:
    friend class Global;
    friend class GuideStep;
    friend class MainMenuPage;
    
	NewGuideManager();
    ~NewGuideManager();
    
    /*
     * 从服务端获得当前所有引导的状态
     */
    void setDefaultState(int64_t defState);
    
    /*
     * 是否正有引导在执行
     */
    bool isGuideing();
    
    /*
     * 检测所有的引导是否已经完成
     */
    bool isAllGuideFinish();
    
    /*
     * 取得当前正在执行的引导的id, 当返回值是GuideId_none时,表示无意义
     */
    GuideId getRunningGuideId();
    
    /*
     * 取得当前正在执行的步骤
     */
    char getCurStep();
    
    /*
     * 取得当前正在执行的步骤的外部数据(可能为空)
     */
    CCObject* getCurStepConditionData();
    
    /*
     * 显示当前步骤下的引导效果
     * talkInfo: 谈话内容
     * viewArea: 可点击区域
     * guideType: 引导类型
     * arrowDir: 指引手指的方向
     * arrowPosRatio: 指引手指在viewArea中显示的position百分比
     * targetNode: 触发下一步引导的node
     * result: 成功时返回true，失败返回false
     */
    bool showStep(const CCRect viewArea, GuideArrowDir arrowDir = guideArrow_UpLeft, CCPoint arrowPosRatio = ccp(0.5, 0.5));
    bool showStep(CCNode *targetNode, GuideArrowDir arrowDir = guideArrow_UpLeft, CCPoint arrowPosRatio = ccp(0.5, 0.5));
    
private:
    /*
     * 接收外部触发条件变化时的回调方法
     */
    void conditionTrigger(CCObject *obj);
    
    /*
     * 接收外部触发的回调方法,意指引导的单步条件被触发，解决部分外部触发条件难以捕捉的情况
     */
    void stepCondition(CCObject *data);
    
    /*
     * 检测引导是否已经完成
     */
    bool isGuideFinish(GuideId guideId);
    
    /*
     * 检测是否有适合当前条件下的引导可以发生
     * 外部接收到 用于检测的数据包
     */
    void checkGuide(CCArray *data);
    
    /*
     * 引导开始前的准备
     */
    void guideReady();
    
    /*
     * 执行一个引导
     */
    void startGuide(GuideId guideId);
    
    /*
     * 执行一个进行中的引导
     */
    void guideingLoop(float t);
    
    /*
     * 引导步骤递增一步
     */
    void addStep();
    
    /*
     * 结束并完成当前正在执行的引导
     */
    void finishGuide();
    
    /*
     * 终止当前正在执行的引导
     */
    void stopGuide();
    
private:
    
    /*
     * 根据GuideId按位表示对应引导的完成状态, 0.未完成, 1.已完成
     */
    int64_t   _allGuideState;
    
    /*
     * 当前正在执行的引导id, 默认值为: GuideName_none
     */
    GuideId   _runningGuideId;
    
    /*
     * 当前正在执行的引导的执行进度, 默认为: GuideStep_none
     */
    char   _step;
    
    char   oldStep;
    
    /*
     * 引导的单步条件被触发时，由外部传来的数据，只在有引导在执行时使用，会在每一个guideingLoop后重新置空
     */
    CCObject *_stepConditionData;
};

#endif /* defined(__qnol__NewGuideManager__) */
