//
//  NewGuideManager.cpp
//  qnol
//
//  Created by lizhou on 15-1-17.
//
//

#include "NewGuideManager.h"
#include "GuideDataManager.h"
#include "GuideView.h"
#include "GuideStep.h"

#include "MessageStruct.h"
#include "ResManager.h"
#include "GameScene.h"
#include "MainMenuPage.h"
#include "BagDataManager.h"
#include "TaskAcceptView.h"
#include "PetDataManager.h"
#include "NewGoodThingPrompt.h"
#include "FirstPayGuideView.h"
#include "DataCollectManager.h"

NewGuideManager::NewGuideManager()
: _allGuideState(0)
, _runningGuideId(GuideId_none)
, _step(GuideStep_none)
, _stepConditionData(NULL)
, oldStep(-1)
{}

NewGuideManager::~NewGuideManager()
{
    CCNotificationCenter::sharedNotificationCenter()->removeObserver(this, NOTIFI_GUIDE_STEP_TRIGGER);
    CCNotificationCenter::sharedNotificationCenter()->removeObserver(this, NOTIFI_GUIDE_CHECK);
    UnCacheFileForName("guide");
    CC_SAFE_RELEASE(_stepConditionData);
}

/*
 * 从服务端获得当前所有引导的状态
 */
void NewGuideManager::setDefaultState(int64_t defState)
{
    _allGuideState = defState;
    CCNotificationCenter::sharedNotificationCenter()->removeObserver(this, NOTIFI_GUIDE_CHECK);
    if (!isAllGuideFinish()) {
        LoadIfNotExist("guide");
        CCNotificationCenter::sharedNotificationCenter()->addObserver(this, callfuncO_selector(NewGuideManager::conditionTrigger), NOTIFI_GUIDE_CHECK, NULL);
    }
    else{
        UnCacheFileForName("guide");
    }
}

/*
 * 接收外部触发条件变化时的回调方法
 */
void NewGuideManager::conditionTrigger(CCObject *obj)
{
    if (!GameScene::openedView || isGuideing()) return;
    
    //51级时把所有引导标记为完成
    if (Hero::Instance() && Hero::Instance()->getInfo() && Hero::Instance()->getInfo()->getLevel() >= 51) {
        _allGuideState = -1;
        NetworkLogic::send_message(GameMessageFactory::construct_TRIGGER_EVENT_OK_REQ(_allGuideState));
        CCNotificationCenter::sharedNotificationCenter()->removeObserver(this, NOTIFI_GUIDE_STEP_TRIGGER);
        CCNotificationCenter::sharedNotificationCenter()->removeObserver(this, NOTIFI_GUIDE_CHECK);
    }
    else{
        CCArray *data = dynamic_cast<CCArray*>(obj);
        if (data && data->count() != 0) {
            checkGuide(data);
        }
    }
}

/*
 * 接收外部触发的回调方法,意指引导的单步条件被触发，解决部分外部触发条件难以捕捉的情况
 */
void NewGuideManager::stepCondition(CCObject *data)
{
    data->retain();
    _stepConditionData = data;
}

/*
 * 检测引导是否已经完成
 */
bool NewGuideManager::isGuideFinish(GuideId guideId)
{
    if (guideId > GuideId_none && guideId < GuideId_count) {
        return (_allGuideState >> guideId) & (uint64_t)1;
    }
    return false;
}

/*
 * 检测所有的引导是否已经完成
 */
bool NewGuideManager::isAllGuideFinish()
{
    for (int guideId = (GuideId_none + 1); guideId < GuideId_count; guideId++) {
        if (!isGuideFinish((GuideId)guideId)) {
            return false;
        }
    }
    return true;
}

/*
 * 是否正有引导在执行
 */
bool NewGuideManager::isGuideing()
{
    return (_runningGuideId > GuideId_none && _runningGuideId < GuideId_count);
}

/*
 * 取得当前正在执行的引导的id, 当返回值是GuideId_none时,表示无意义
 */
GuideId NewGuideManager::getRunningGuideId()
{
    return _runningGuideId;
}

/*
 * 取得当前正在执行的步骤
 */
char NewGuideManager::getCurStep()
{
    return _step;
}

/*
 * 取得当前正在执行的步骤的外部数据(可能为空)
 */
CCObject* NewGuideManager::getCurStepConditionData()
{
    return _stepConditionData;
}

/*
 * 检测是否有适合当前条件下的引导可以发生
 * 外部接收到 用于检测的数据包
 */
void NewGuideManager::checkGuide(CCArray *data)
{
    if (isGuideing()) return;
    if (!data || data->count() == 0) return;
    if (Hero::Instance() && Hero::Instance()->getInfo()->getState() == LivingObject_STATE_DEAD) return;
    
    GuideTriggerType triggerType = (GuideTriggerType)((CCInteger*)data->objectAtIndex(0))->getValue();
    CCLOG("triggerType:%d", triggerType);
    if (triggerType < GuideTriggerType_task && triggerType > GuideTriggerType_level) {
        return;
    }
    
    //任务类时为taskId，物品类时为articleId, UI类时为UI_GuideTriggerType的索引，等级类时为当前等级
    int param_1 = -1;
    //任务类时为taskState，物品类时暂时无意义, UI类时为相应的用户数据，等级类时1.当前等级下经验满，0.未满
    int param_2 = -1;
    
    if (data->count() >= 2) {
        param_1 = ((CCInteger*)data->objectAtIndex(1))->getValue();
    }
    if (data->count() >= 3) {
        param_2 = ((CCInteger*)data->objectAtIndex(2))->getValue();
    }
    CCLOG("param_1:%d", param_1);
    CCLOG("param_2:%d", param_2);
    
    GuideId triggerGuideId = GuideId_none;
    
/*************** 基础条件判定(triggerEvent.xlsx中定义的条件) **************/
    vector<TriggerEventInfo*>* triggerDatas = GuideDataManager::Instance()->getTriggerEvents();
    for (int i = 0; i < triggerDatas->size(); ++i) {
        TriggerEventInfo *info = triggerDatas->at(i);
        if (info && info->getTriggerType() == triggerType) {
            if (triggerType == GuideTriggerType_task)   //任务类
            {
                if (info->getTriggerKey() == param_1) {
                    //达到要求的任务状态
                    if (!isGuideFinish((GuideId)info->getId()) && info->getTriggerValue() == param_2) {
                        triggerGuideId = (GuideId)info->getId();
                    }
                    break;
                }
            }
            else if(triggerType == GuideTriggerType_goods)  //物品类
            {
                if (info->getTriggerKey() == param_1) {
                    if (!isGuideFinish((GuideId)info->getId())) {
                        BagDataManager::Instance()->getItemCountByArticleId(info->getTriggerKey());
                        //物品数量达到要求
                        if (BagDataManager::Instance()->getItemCountByArticleId(info->getTriggerKey()) >= info->getTriggerValue()) {
                            triggerGuideId = (GuideId)info->getId();
                        }
                    }
                    break;
                }
            }
            else if(triggerType == GuideTriggerType_ui)     //UI类
            {
                break;
            }
            else if(triggerType == GuideTriggerType_level)  //等级类
            {
                if (info->getTriggerKey() == param_1) {
                    if (!isGuideFinish((GuideId)info->getId())) {
                        triggerGuideId = (GuideId)info->getId();
                    }
                    break;
                }
            }
        }
    }
    
    //UI类特殊处理
    if(triggerType == GuideTriggerType_ui)     //UI类
    {
        switch (param_1) {
            //新功能开放
            case UI_Trigger_MainMenu_openFunc:
            {
                if (MainMenuPage::openedView) {
                    switch (param_2) {
                        case MainMenuPage::kSkill:
                        {
                            if (!isGuideFinish(GuideId_skill_1)) {
                                triggerGuideId = GuideId_skill_1;
                            }
                            break;
                        }
                        case MainMenuPage::KEquipForging:
                        {
                            if (!isGuideFinish(GuideId_forging_1)) {
                                triggerGuideId = GuideId_forging_1;
                            }
                            break;
                        }
                        case MainMenuPage::kBaguaStove:
                        {
                            if (!isGuideFinish(GuideId_bagua_1)) {
                                triggerGuideId = GuideId_bagua_1;
                            }
                            break;
                        }
                        case MainMenuPage::kGang:
                        {
                            if (!isGuideFinish(GuideId_gang_open)) {
                                triggerGuideId = GuideId_gang_open;
                            }
                            break;
                        }
                        case MainMenuPage::kPet:
                        {
                            if (!isGuideFinish(GuideId_pet_fight)) {
                                triggerGuideId = GuideId_pet_fight;
                            }
                            break;
                        }
                    }
                }
                break;
            }
        }
    }
    
/*************** 多重条件触发的引导，需要在此单独处理配表以外的条件 **************/
    if (triggerGuideId != GuideId_none)
    {
        bool StartFlag = true;
        
        switch (triggerGuideId) {
            case GuideId_skill_1:
            {
                StartFlag = SkillCenter::Instance()->getSkillLevelAtIndex(Skill_index_1) == 0;
                break;
            }
            case GuideId_skill_2:
            {
                StartFlag = SkillCenter::Instance()->getSkillLevelAtIndex(Skill_index_2) == 0;
                break;
            }
            case GuideId_forging_1:
            {
                BagDataManager *bagDataMgr = BagDataManager::Instance();
                //身上或背包有武器，且背包里1级皓月石数量大于0
                StartFlag = ( (bagDataMgr->getEquippedItemOnPart(EQUIP_PART_WEAPON) ||
                                        bagDataMgr->getEquipItemsCountInContainerByPart(kItemsInBag, EQUIP_PART_WEAPON).size() > 0)
                                  && bagDataMgr->getItemCountByArticleId(Article_haoYueShi_1) > 0 );
                break;
            }
            case GuideId_bagua_1:
            {
                //背包里1级皓月石数量大于等于2
                StartFlag = ( BagDataManager::Instance()->getItemCountByArticleId(Article_haoYueShi_1) >= 2 );
                break;
            }
            case GuideId_forging_2:
            {
                //背包里2级皓月石数量大于等于1
                StartFlag = ( BagDataManager::Instance()->getItemCountByArticleId(Article_haoYueShi_2) >= 1 );
                break;
            }
            case GuideId_pet_fight:
            {
                //有灵兽
                StartFlag = (PetDataManager::Instance()->getPetIds().size() > 0);
                break;
            }
            case GuideId_pet_wash:
            {
                //有灵兽
                StartFlag = (PetDataManager::Instance()->getPetIds().size() > 0);
                //背包里凡品紫苏露数量大于0
                StartFlag &= ( BagDataManager::Instance()->getItemCountByArticleId(Article_zhiSuLu_f) > 0 );
                break;
            }
            case GuideId_pet_skill_1:
            {
                //有灵兽
                StartFlag = (PetDataManager::Instance()->getPetIds().size() > 0);
                //背包里有指定的灵兽技能书
                StartFlag &= ( BagDataManager::Instance()->getItemCountByArticleId(Article_pet_skillBook_1) > 0 );
                break;
            }
            case GuideId_pet_possession:
            {
                //身上有可以用来附体用的灵兽,且还没有附体任何灵兽
                //有一只且未出战，或有两只
                PetDataManager *petDataMgr = PetDataManager::Instance();
                StartFlag = (petDataMgr->getPossessionPetID() <= 0) &&
                            ( (petDataMgr->getPetIds().size() == 1 && petDataMgr->getWarPetID() <= 0)
                                || petDataMgr->getPetIds().size() >= 2);
                break;
            }
            case GuideId_pet_skill_2:
            {
                //有灵兽
                StartFlag = (PetDataManager::Instance()->getPetIds().size() > 0);
                break;
            }
            case GuideId_hand_levelUp:
            {
                //当前经验值达到当前等级经验的升级要求
                StartFlag = param_2;
                break;
            }
            default:
                CCLOG("the triggerGuideId: %d case undefine!", triggerGuideId);
        }
        
        if (!StartFlag) {
            triggerGuideId = GuideId_none;
        }
    }
    
    //如果选定了一个引导，则执行它
    if (triggerGuideId != GuideId_none) {
        startGuide(triggerGuideId);
    }
}

/*
 * 引导开始前的准备
 */
void NewGuideManager::guideReady()
{
    //停止战斗
    SkillCenter::shared()->stopAutoAttack();
    //停止行走
    if ( Hero::Instance() && Hero::Instance()->getMoveManager()->getIsMoving()) {
        Hero::Instance()->getMoveManager()->stopMove();
    }
//    //关闭npc对话框
//    if (TaskAcceptView::shared()) {
//        TaskAcceptView::shared()->removeFromParent();
//    }
    //关闭所有窗口
    UIHandle::Instance()->closeViews();
    
    if (NewGoodThingPrompt::shared()) {
        NewGoodThingPrompt::shared()->onExitView();
    }
    
    if (FirstPayGuideView::shared()) {
        FirstPayGuideView::shared()->onExitView();
    }
}

/*
 * 执行一个引导
 */
void NewGuideManager::startGuide(GuideId guideId)
{
    if (isGuideing()) {
        return;
    }
    
    guideReady();
    
    _runningGuideId = guideId;
    _step = GuideStep_1;
    CCNotificationCenter::sharedNotificationCenter()->addObserver(this, callfuncO_selector(NewGuideManager::stepCondition), NOTIFI_GUIDE_STEP_TRIGGER, NULL);
    CCDirector::sharedDirector()->getScheduler()->scheduleSelector(schedule_selector(NewGuideManager::guideingLoop), this, 0, 0);
    
    CCLOG("start guide: %d", _runningGuideId);
}

/*
 * 执行一个进行中的引导
 */
void NewGuideManager::guideingLoop(float t)
{
    if (!isGuideing()) {
        return;
    }
    //如果角色死亡 直接完成引导
    if (Hero::Instance() && Hero::Instance()->getInfo()->getState() == LivingObject_STATE_DEAD) {
        finishGuide();
        return;
    }
    
    bool finishFlag = false;
    switch (_runningGuideId) {
        case GuideId_skill_1:
        {
            finishFlag = GuideStep::GuideId_skill_1_step();
            break;
        }
        case GuideId_forging_1:
        {
            finishFlag = GuideStep::GuideId_forging_1_step();
            break;
        }
        case GuideId_instance_1:
        {
            finishFlag = GuideStep::GuideId_instance_1_step();
            break;
        }
        case GuideId_skill_2:
        {
            finishFlag = GuideStep::GuideId_skill_2_step();
            break;
        }
        case GuideId_bagua_1:
        {
            finishFlag = GuideStep::GuideId_bagua_1_step();
            break;
        }
        case GuideId_forging_2:
        {
            finishFlag = GuideStep::GuideId_forging_2_step();
            break;
        }
        case GuideId_instance_2:
        {
            finishFlag = GuideStep::GuideId_instance_2_step();
            break;
        }
        case GuideId_mall:
        {
            finishFlag = GuideStep::GuideId_mall_step();
            break;
        }
        case GuideId_gang_open:
        {
            finishFlag = GuideStep::GuideId_gang_open_step();
            break;
        }
        case GuideId_pet_fight:
        {
            finishFlag = GuideStep::GuideId_pet_fight_step();
            break;
        }
        case GuideId_pet_wash:
        {
            finishFlag = GuideStep::GuideId_pet_wash_step();
            break;
        }
        case GuideId_pet_skill_1:
        {
            finishFlag = GuideStep::GuideId_pet_skill_1_step();
            break;
        }
        case GuideId_pet_compose:
        {
            finishFlag = GuideStep::GuideId_pet_compose_step();
            break;
        }
        case GuideId_pet_possession:
        {
            finishFlag = GuideStep::GuideId_pet_possession_step();
            break;
        }
        case GuideId_pet_skill_2:
        {
            finishFlag = GuideStep::GuideId_pet_skill_2_step();
            break;
        }
        case GuideId_dailyActive:
        {
            finishFlag = GuideStep::GuideId_dailyActive_step();
            break;
        }
        case GuideId_hand_levelUp:
        {
            finishFlag = GuideStep::GuideId_hand_levelUp_step();
            break;
        }
            
        default:
            CCLOG("runGuide error! un kown guide id:%d", _runningGuideId);
            break;
    }
    
    if (oldStep < _step) {
        oldStep = _step;
        DataCollectManager::Instance()->sendDataCollect(dc_opt_bothKey_cover, dc_type_guide, _step, _runningGuideId);
    }

    if (_stepConditionData) {
        CC_SAFE_RELEASE(_stepConditionData);
        _stepConditionData = NULL;
    }
    
    if (finishFlag) {
        finishGuide();
    }
}

/*
 * 引导步骤递增一步
 */
void NewGuideManager::addStep()
{
    _step++;
}

/*
 * 结束并完成当前正在执行的引导
 */
void NewGuideManager::finishGuide()
{
    if (isGuideing() && !isGuideFinish(_runningGuideId)) {
        _allGuideState = _allGuideState | ((uint64_t)1 << _runningGuideId);
        //send new state to game server
        NetworkLogic::send_message(GameMessageFactory::construct_TRIGGER_EVENT_OK_REQ(_allGuideState));
    }
    stopGuide();
}

/*
 * 终止当前正在执行的引导
 */
void NewGuideManager::stopGuide()
{
    if (isGuideing()) {
        _runningGuideId = GuideId_none;
        CCNotificationCenter::sharedNotificationCenter()->removeObserver(this, NOTIFI_GUIDE_STEP_TRIGGER);
        CCDirector::sharedDirector()->getScheduler()->unscheduleSelector(schedule_selector(NewGuideManager::guideingLoop), this);
        //remove guide view
        if (GuideView::shared()) {
            GuideView::shared()->closeView();
        }
        CCNotificationCenter::sharedNotificationCenter()->postNotification(NOTIFI_GUIDE_FINISH, this);
        oldStep = -1;
    }
}

/*
 * 显示当前步骤下的引导效果
 */
bool NewGuideManager::showStep(const CCRect viewArea, GuideArrowDir arrowDir, CCPoint arrowPosRatio)
{
    if (isGuideing()) {
        bool flag = false;
        TriggerEventInfo *guideInfo = GuideDataManager::Instance()->getTriggerEventInfoById(_runningGuideId);
        if (guideInfo) {
            GuideType guideType = (GuideType)guideInfo->getGuideType();
            vector<string> talks = guideInfo->getTalkByStep(_step);
            //半强制引导的最后一步是非强制的
            if (guideType == GuideType_half) {
                guideType = (_step >= guideInfo->getStepCount()) ? GuideType_free : GuideType_compel;
            }
            if (GuideView::shared()) {
                flag = GuideView::shared()->initWithData(talks, viewArea, guideType, arrowDir, arrowPosRatio);
            }
            else{
                flag = GuideView::createWithData(talks, viewArea, guideType, arrowDir, arrowPosRatio);
            }
            return flag;
        }
    }
    return false;
}

bool NewGuideManager::showStep(CCNode *targetNode, GuideArrowDir arrowDir, CCPoint arrowPosRatio)
{
    if (isGuideing()) {
        bool flag = false;
        TriggerEventInfo *guideInfo = GuideDataManager::Instance()->getTriggerEventInfoById(_runningGuideId);
        if (guideInfo) {
            GuideType guideType = (GuideType)guideInfo->getGuideType();
            vector<string> talks = guideInfo->getTalkByStep(_step);
            //半强制引导的最后一步是非强制的
            if (guideType == GuideType_half) {
                guideType = (_step >= guideInfo->getStepCount()) ? GuideType_free : GuideType_compel;
            }
            if (GuideView::shared()) {
                flag = GuideView::shared()->initWithData(talks, targetNode, guideType, arrowDir, arrowPosRatio);
            }
            else{
                flag = GuideView::createWithData(talks, targetNode, guideType, arrowDir, arrowPosRatio);
            }
            return flag;
        }
    }
    return false;
}
