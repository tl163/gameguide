//
//  GuideStep.cpp
//  qnol
//
//  Created by lizhou on 15-1-19.
//
//

#include "GuideStep.h"
#include "GuideDataManager.h"
#include "NewGuideManager.h"
#include "MainMenuPage.h"
#include "MainUIPage.h"
#include "NewSkillView.h"
#include "NewSkillViewTips.h"
#include "ActiveDetailDialog.h"
#include "MainMutualMenu.h"
#include "ForgingView.h"
#include "BagDataManager.h"
#include "BaGuaStovePanel.h"
#include "BagItemCell.h"
#include "GangList.h"
#include "MyGang.h"
#include "MyGang_info.h"
#include "PetDataManager.h"
#include "PetDevelopView.h"
#include "PetView.h"
#include "PetItem.h"
#include "PetDevelop_skill.h"
#include "BagView.h"
#include "ActiveView.h"
#include "PropertyPanel.h"
#include "UDStackSegment.h"
#include "MeltingItemsView.h"
#include "MallPage.h"
#include "shop_manager.h"
#include "GoodsBuyOptBox.h"

/*
 * 单步引导中可能需要用到上一步中的数据，可以存放在此
 * 使用安全需要自行保障
 */
int64_t stepUserData = 0;

NewGuideManager* getGuide()
{
    return NewGuideManager::Instance();
}

GuideDataManager* getGuideDateMgr()
{
    return GuideDataManager::Instance();
}

MainUIPage* getMainUIView()
{
    return MainUIPage::Instance();
}

BagDataManager* getBagDataMgr()
{
    return BagDataManager::Instance();
}

PetDataManager* getPetDataMgr()
{
    return PetDataManager::Instance();
}

bool GuideStep::GuideId_skill_1_step()
{
    if (NewGuideManager::Instance()->getRunningGuideId() != GuideId_skill_1) {
        return false;
    }
    
    switch (NewGuideManager::Instance()->getCurStep()) {
        case GuideStep_1:   //如果菜单未显示，点击菜单切换按钮
        {
            if (!MainMenuPage::isMenuItemsVisible()) {
                getGuide()->showStep(getMainUIView()->getMenuBtn());
            }
            getGuide()->addStep();
            break;
        }
        case GuideStep_2:   //选择技能菜单
        {
            if (MainMenuPage::isMenuItemsVisible()) {
                getGuide()->showStep( MainMenuPage::openedView->getItemByType(MainMenuPage::kSkill) );
                getGuide()->addStep();
            }
            break;
        }
        case GuideStep_3:   //选择第一个技能
        {
            if (NewSkillView::openedView && NewSkillView::openedView->isRunning()) {
                //第一个技能
                NewSkillViewCell *cell = (NewSkillViewCell*)NewSkillView::openedView->getSkillCells()->objectAtIndex(0);
                if (cell) {
                    getGuide()->showStep(cell->getSkillIconSprite());
                }
                getGuide()->addStep();
            }
            break;
        }
        case GuideStep_4:   //点击升级
        {
            if (NewSkillViewTips::openedView && NewSkillView::openedView->isRunning()) {
                getGuide()->showStep(NewSkillViewTips::openedView->getBtnFunc());
                getGuide()->addStep();
            }
            break;
        }
        case GuideStep_5:   //关闭技能界面
        {
            if (NewSkillView::openedView && NewSkillView::openedView->isRunning() && dynamic_cast<NewSkillViewTips*>(getGuide()->getCurStepConditionData())) {
                getGuide()->showStep(NewSkillView::openedView->getBtnClose());
                getGuide()->addStep();
            }
            break;
        }
        case GuideStep_6:   //切换功能菜单
        {
            if (!NewSkillView::openedView) {
                //手动关闭
//                getGuide()->showStep(getMainUIView()->getMenuBtn());
                if (MainMenuPage::isMenuItemsVisible()) {
                    getMainUIView()->button_menuItems();
                }
                getGuide()->addStep();
            }
            break;
        }
        case GuideStep_7:   //结束
        {
            if (!MainMenuPage::isMenuItemsVisible()) {
                return true;
            }
            break;
        }
    }
    
    return false;
}

bool GuideStep::GuideId_forging_1_step()
{
    if (NewGuideManager::Instance()->getRunningGuideId() != GuideId_forging_1) {
        return false;
    }
    
    ForgingView *forgingView = ForgingView::shared();
    
    switch (NewGuideManager::Instance()->getCurStep()) {
        case GuideStep_1:   //如果菜单未显示，点击菜单切换按钮
        {
            if (!MainMenuPage::isMenuItemsVisible()) {
                getGuide()->showStep(getMainUIView()->getMenuBtn());
            }
            getGuide()->addStep();
            break;
        }
        case GuideStep_2:   //选择锻造菜单
        {
            if (MainMenuPage::isMenuItemsVisible()) {
                getGuide()->showStep( MainMenuPage::openedView->getItemByType(MainMenuPage::KEquipForging) );
                getGuide()->addStep();
            }
            break;
        }
        case GuideStep_3:   //选择未装备(如果需要)
        {
            if (forgingView && forgingView->isRunning()) {
                //如果在身上找不到武器 切到未装备页
                if (!getBagDataMgr()->getEquippedItemOnPart(EQUIP_PART_WEAPON)) {
                    CCSprite *btn = forgingView->getEquipListCCB()->getUsingBtn();

                    CCRect area = btn->boundingBox();
                    area.origin = btn->convertToWorldSpace(ccp(area.size.width + 30, -30));
                    getGuide()->showStep(area);
                    getGuide()->addStep();
                }
                else{
                    getGuide()->addStep();
                    getGuide()->addStep();
                }
            }
            break;
        }
        case GuideStep_4:   //切到未装备页(如果需要)
        {
            if (forgingView && forgingView->isRunning() && forgingView->getEquipListCCB()->getCurSelectMenu() == 1) {
                getGuide()->addStep();
            }
            break;
        }
        case GuideStep_5:   //选择一把武器
        {
            //把武器排在第一位
            CCNode *firstCell = NULL;
            if (forgingView->getEquipListCCB()->resortByWeapon()) {
                firstCell = forgingView->getEquipListCCB()->getCellAtIdx(0);
            }
            
            if (firstCell) {
                getGuide()->showStep(firstCell);
                getGuide()->addStep();
            }
            else{
                return true;
            }
            break;
        }
        case GuideStep_6:   //点击强化按钮
        {
            if (forgingView && forgingView->isRunning() && forgingView->getSelectedEquipIid() > 0) {
                getGuide()->showStep(forgingView->getConfirmBtn());
                getGuide()->addStep();
            }
            break;
        }
        case GuideStep_7:   //点击关闭按钮
        {
            if (forgingView && forgingView->isRunning() && getGuide()->getCurStepConditionData() == forgingView) {
                getGuide()->showStep(forgingView->getCloseBtn());
                getGuide()->addStep();
            }
            break;
        }
        case GuideStep_8:   //切换功能菜单
        {
            if (!forgingView) {
                //手动关闭
//                getGuide()->showStep(getMainUIView()->getMenuBtn());
                if (MainMenuPage::isMenuItemsVisible()) {
                    getMainUIView()->button_menuItems();
                }
                getGuide()->addStep();
            }
            break;
        }
        case GuideStep_9:   //结束
        {
            if (!MainMenuPage::isMenuItemsVisible()) {
                return true;
            }
            break;
        }
    }
    
    return false;
}

//第一次进入副本
bool GuideStep::GuideId_instance_1_step()
{
    if (NewGuideManager::Instance()->getRunningGuideId() != GuideId_instance_1) {
        return false;
    }
    
    MainMutualMenu *teamTaskBar = getMainUIView()->getTeamTaskBar();
    
    switch (NewGuideManager::Instance()->getCurStep()) {
        case GuideStep_1:   //点击任务队伍面板功能按钮
        {
            if (!getMainUIView()->getIsTeamTaskOpen()) {
                getGuide()->showStep(getMainUIView()->getTeamTaskBtn());
            }
            getGuide()->addStep();
            break;
        }
        case GuideStep_2:   //点击任务菜单按钮
        {
            if (getMainUIView()->getIsTeamTaskOpen()) {
                if (teamTaskBar->getCurWinIdx() != MainMutualMenu::kTask) {
                    getGuide()->showStep(teamTaskBar->getTaskBtn());
                }
                getGuide()->addStep();
            }
            break;
        }
        case GuideStep_3:   //选择副本任务
        {
            if (getMainUIView()->getIsTeamTaskOpen() && teamTaskBar->getCurWinIdx() == MainMutualMenu::kTask) {
                
                CCNode *taskCell = teamTaskBar->getTeamTaskWin()->getCellByTaskId( getGuideDateMgr()->getTriggerEventInfoById(getGuide()->getRunningGuideId())->getTriggerKey() );
                if (taskCell) {
                    getGuide()->showStep(taskCell);
                    getGuide()->addStep();
                }
                else{
                    return true;
                }
            }
            break;
        }
        case GuideStep_4: //点击进入
        {
            ActiveDetailDialog *instanceView = ActiveDetailDialog::shared();
            if (instanceView && instanceView->isRunning() && instanceView->getBtnSignUp()->isVisible()) {
                getGuide()->showStep(instanceView->getBtnSignUp());
                getGuide()->addStep();
            }
            break;
        }
        case GuideStep_5:
        {
            if (!ActiveDetailDialog::shared()) {
                return true;
            }
            break;
        }
    }
    return false;
}

//第二次学习人物技能
bool GuideStep::GuideId_skill_2_step()
{
    if (NewGuideManager::Instance()->getRunningGuideId() != GuideId_skill_2) {
        return false;
    }
    
    switch (NewGuideManager::Instance()->getCurStep()) {
        case GuideStep_1:   //如果菜单未显示，点击菜单切换按钮
        {
            if (!MainMenuPage::isMenuItemsVisible()) {
                getGuide()->showStep(getMainUIView()->getMenuBtn());
            }
            getGuide()->addStep();
            break;
        }
        case GuideStep_2:   //选择技能菜单
        {
            if (MainMenuPage::isMenuItemsVisible()) {
                getGuide()->showStep( MainMenuPage::openedView->getItemByType(MainMenuPage::kSkill) );
                getGuide()->addStep();
            }
            break;
        }
        case GuideStep_3:   //选择第二个技能
        {
            if (NewSkillView::openedView && NewSkillView::openedView->isRunning()) {
                //第一个技能
                NewSkillViewCell *cell = (NewSkillViewCell*)NewSkillView::openedView->getSkillCells()->objectAtIndex(1);
                if (cell) {
                    getGuide()->showStep(cell->getSkillIconSprite());
                }
                getGuide()->addStep();
            }
            break;
        }
        case GuideStep_4:   //点击升级
        {
            if (NewSkillViewTips::openedView && NewSkillView::openedView->isRunning()) {
                getGuide()->showStep(NewSkillViewTips::openedView->getBtnFunc());
                getGuide()->addStep();
            }
            break;
        }
        case GuideStep_5:   //关闭技能界面
        {
            if (NewSkillView::openedView && NewSkillView::openedView->isRunning() && dynamic_cast<NewSkillViewTips*>(getGuide()->getCurStepConditionData())) {
                getGuide()->showStep(NewSkillView::openedView->getBtnClose());
                getGuide()->addStep();
            }
            break;
        }
        case GuideStep_6:   //切换功能菜单
        {
            if (!NewSkillView::openedView) {
                //手动关闭
                //                getGuide()->showStep(getMainUIView()->getMenuBtn());
                if (MainMenuPage::isMenuItemsVisible()) {
                    getMainUIView()->button_menuItems();
                }
                getGuide()->addStep();
            }
            break;
        }
        case GuideStep_7:   //结束
        {
            if (!MainMenuPage::isMenuItemsVisible()) {
                return true;
            }
            break;
        }
    }
    
    return false;
}

//第一次使用八卦炉合成
bool GuideStep::GuideId_bagua_1_step()
{
    if (NewGuideManager::Instance()->getRunningGuideId() != GuideId_bagua_1) {
        return false;
    }
    
    switch (NewGuideManager::Instance()->getCurStep()) {
        case GuideStep_1:   //如果菜单未显示，点击菜单切换按钮
        {
            if (!MainMenuPage::isMenuItemsVisible()) {
                getGuide()->showStep(getMainUIView()->getMenuBtn());
            }
            getGuide()->addStep();
            break;
        }
        case GuideStep_2:   //选择技能菜单
        {
            if (MainMenuPage::isMenuItemsVisible()) {
                getGuide()->showStep( MainMenuPage::openedView->getItemByType(MainMenuPage::kBaguaStove) );
                getGuide()->addStep();
            }
            break;
        }
        case GuideStep_3:   //清除
        {
            if (BaGuaStovePanel::shared() && BaGuaStovePanel::shared()->isRunning()) {
                //清除八卦炉中已经存在的物品
                BaGuaStovePanel::shared()->getAllBaGuaStoveItems();
                getGuide()->addStep();
            }
            break;
        }
        case GuideStep_4:   //选择一级皓月石
        {
            if (BaGuaStovePanel::shared() && BaGuaStovePanel::shared()->isRunning() && !getBagDataMgr()->isHaveItemIncontainer(kItemsBaGuaStove)) {
                CCNode *itemCell = BaGuaStovePanel::shared()->getBagBox()->getCellAndShowInPage(Article_haoYueShi_1, 2);
                if (itemCell) {
                    getGuide()->showStep(itemCell);
                    getGuide()->addStep();
                }
                else{
                    return true;
                }
            }
            break;
        }
        case GuideStep_5:   //点击合成
        {
            //八卦炉内有物品
            if (BaGuaStovePanel::shared() && BaGuaStovePanel::shared()->isRunning() && getBagDataMgr()->isHaveItemIncontainer(kItemsBaGuaStove)) {
                getGuide()->showStep(BaGuaStovePanel::shared()->getOnceCompoundBtn());
                getGuide()->addStep();
            }
            break;
        }
        case GuideStep_6:   //取回合成物
        {
            //八卦炉产出区内有物品
            if (BaGuaStovePanel::shared() && BaGuaStovePanel::shared()->isRunning() && getBagDataMgr()->isHaveItemIncontainer(kItemsBaGuaStoveOutPut)) {
                getGuide()->showStep(BaGuaStovePanel::shared()->getGetAllBtn());
                getGuide()->addStep();
            }
            break;
        }
//        case GuideStep_7:   //查看八卦秘方
//        {
//            //八卦炉产出区内有物品
//            if (BaGuaStovePanel::shared() && BaGuaStovePanel::shared()->isRunning() && !getBagDataMgr()->isHaveItemIncontainer(kItemsBaGuaStoveOutPut)) {
//                getGuide()->showStep(BaGuaStovePanel::shared()->getRecipeBtn());
//                getGuide()->addStep();
//            }
//            break;
//        }
//        case GuideStep_8:   //结束
//        {
//            if (MeltingItemsView::shared() && MeltingItemsView::shared()->isRunning()) {
//                return true;
//            }
//            break;
//        }
        case GuideStep_7:   //关闭八卦炉
        {
            if (BaGuaStovePanel::shared() && BaGuaStovePanel::shared()->isRunning() && !getBagDataMgr()->isHaveItemIncontainer(kItemsBaGuaStoveOutPut)) {
                getGuide()->showStep(BaGuaStovePanel::shared()->getCloseBtn());
                getGuide()->addStep();
            }
            break;
        }
        case GuideStep_8:   //切换功能菜单
        {
            if (!BaGuaStovePanel::shared()) {
                //手动关闭
                //                getGuide()->showStep(getMainUIView()->getMenuBtn());
                if (MainMenuPage::isMenuItemsVisible()) {
                    getMainUIView()->button_menuItems();
                }
                getGuide()->addStep();
            }
            break;
        }
        case GuideStep_9:   //结束
        {
            if (!MainMenuPage::isMenuItemsVisible()) {
                return true;
            }
            break;
        }
    }
    return false;
}

//第二次强化装备
bool GuideStep::GuideId_forging_2_step()
{
    if (NewGuideManager::Instance()->getRunningGuideId() != GuideId_forging_2) {
        return false;
    }
    
    ForgingView *forgingView = ForgingView::shared();
    
    switch (NewGuideManager::Instance()->getCurStep()) {
        case GuideStep_1:   //如果菜单未显示，点击菜单切换按钮
        {
            if (!MainMenuPage::isMenuItemsVisible()) {
                getGuide()->showStep(getMainUIView()->getMenuBtn());
            }
            getGuide()->addStep();
            break;
        }
        case GuideStep_2:   //选择锻造菜单
        {
            if (MainMenuPage::isMenuItemsVisible()) {
                getGuide()->showStep( MainMenuPage::openedView->getItemByType(MainMenuPage::KEquipForging) );
                getGuide()->addStep();
            }
            break;
        }
        case GuideStep_3:   //选择未装备(如果需要)
        {
            if (forgingView && forgingView->isRunning()) {
                //如果在身上找不到武器 切到未装备页
                if (!getBagDataMgr()->getEquippedItemOnPart(EQUIP_PART_WEAPON)) {
                    CCSprite *btn = forgingView->getEquipListCCB()->getUsingBtn();
                    
                    CCRect area = btn->boundingBox();
                    area.origin = btn->convertToWorldSpace(ccp(area.size.width + 30, -30));
                    getGuide()->showStep(area);
                    getGuide()->addStep();
                }
                else{
                    getGuide()->addStep();
                    getGuide()->addStep();
                }
            }
            break;
        }
        case GuideStep_4:   //切到未装备页(如果需要)
        {
            if (forgingView && forgingView->isRunning() && forgingView->getEquipListCCB()->getCurSelectMenu() == 1) {
                getGuide()->addStep();
            }
            break;
        }
        case GuideStep_5:   //选择一把武器
        {
            //把武器排在第一位
            CCNode *firstCell = NULL;
            if (forgingView->getEquipListCCB()->resortByWeapon()) {
                firstCell = forgingView->getEquipListCCB()->getCellAtIdx(0);
            }
            
            if (firstCell) {
                getGuide()->showStep(firstCell);
                getGuide()->addStep();
            }
            else{
                return true;
            }
            break;
        }
        case GuideStep_6:   //点击强化按钮
        {
            if (forgingView && forgingView->isRunning() && forgingView->getSelectedEquipIid() > 0) {
                getGuide()->showStep(forgingView->getConfirmBtn());
                getGuide()->addStep();
            }
            break;
        }
        case GuideStep_7:   //点击关闭按钮
        {
            if (forgingView && forgingView->isRunning() && getGuide()->getCurStepConditionData() == forgingView) {
                getGuide()->showStep(forgingView->getCloseBtn());
                getGuide()->addStep();
            }
            break;
        }
        case GuideStep_8:   //切换功能菜单
        {
            if (!forgingView) {
                //手动关闭
                //                getGuide()->showStep(getMainUIView()->getMenuBtn());
                if (MainMenuPage::isMenuItemsVisible()) {
                    getMainUIView()->button_menuItems();
                }
                getGuide()->addStep();
            }
            break;
        }
        case GuideStep_9:   //结束
        {
            if (!MainMenuPage::isMenuItemsVisible()) {
                return true;
            }
            break;
        }
    }
    
    return false;
}

//第二次进入副本
bool GuideStep::GuideId_instance_2_step()
{
    if (NewGuideManager::Instance()->getRunningGuideId() != GuideId_instance_2) {
        return false;
    }
    
    switch (NewGuideManager::Instance()->getCurStep()) {
        case GuideStep_1:   //点击功能按钮
        {
            if (!getMainUIView()->getIsFuncListOpen()) {
                getGuide()->showStep(getMainUIView()->getFuncListBtn(), guideArrow_UpRight);
            }
            getGuide()->addStep();
            break;
        }
        case GuideStep_2:   //点击日常活动
        {
            if (getMainUIView()->getIsFuncListOpen()) {
                CCNode *node = getMainUIView()->getHUDMenuItem(kHUDMenuNormalDay);
                getGuide()->showStep(node);
                getGuide()->addStep();
            }
            break;
        }
        case GuideStep_3:   //点击剧情副本页签
        {
            if (ActiveView::shared() && ActiveView::shared()->isRunning()) {
                CCNode *node = ActiveView::shared()->getSegment()->getChildByTag(ActiveView::seg_type_instance);
                getGuide()->showStep(node);
                getGuide()->addStep();
            }
            break;
        }
        case GuideStep_4:   //点击聚魂阵
        {
            if (ActiveView::shared() && ActiveView::shared()->isRunning() && ActiveView::shared()->getChoosedSegmentIdx() == ActiveView::seg_type_instance && ActiveView::shared()->getGrid()) {
                Task *task = TaskManager::Instance()->getTaskWithId(getGuideDateMgr()->getTriggerEventInfoById(getGuide()->getRunningGuideId())->getTriggerKey());
                CCNode *target = NULL;
                if (task && task->getInstanceId().size() > 0) {
                    target = ActiveView::shared()->getActiveCellByInstanceId(task->getInstanceId());
                }
                
                if (target) {
                    getGuide()->showStep(target);
                    getGuide()->addStep();
                }
                else{
                    return true;
                }
            }
            break;
        }
        case GuideStep_5: //点击进入
        {
            ActiveDetailDialog *instanceView = ActiveDetailDialog::shared();
            if (instanceView && instanceView->isRunning() && instanceView->getBtnSignUp()->isVisible()) {
                getGuide()->showStep(instanceView->getBtnSignUp());
                getGuide()->addStep();
            }
            break;
        }
        case GuideStep_6:   //结束
        {
            if (!ActiveDetailDialog::openedView) {
                return true;
            }
            break;
        }
    }
    
    return false;
}

//商城购买道具
bool GuideStep::GuideId_mall_step()
{
    if (NewGuideManager::Instance()->getRunningGuideId() != GuideId_mall) {
        return false;
    }
    
    switch (NewGuideManager::Instance()->getCurStep()) {
        case GuideStep_1:   //点击功能按钮
        {
            if (!getMainUIView()->getIsFuncListOpen()) {
                getGuide()->showStep(getMainUIView()->getFuncListBtn(), guideArrow_UpRight);
            }
            getGuide()->addStep();
            break;
        }
        case GuideStep_2:   //点击商城
        {
            if (getMainUIView()->getIsFuncListOpen()) {
                CCNode *node = getMainUIView()->getHUDMenuItem(kHUDMenuMall);
                getGuide()->showStep(node);
                getGuide()->addStep();
            }
            break;
        }
        case GuideStep_3:   //点击绑定商城
        {
            if (MallPage::shared() && MallPage::shared()->isRunning()) {
                UDTabBar *bar = dynamic_cast<UDTabBar*>(MallPage::shared()->getParent());
                if (bar) {
                    getGuide()->showStep(bar->getTabAtIndex(1));
                    getGuide()->addStep();
                }
                else{
                    return true;
                }
            }
            break;
        }
        case GuideStep_4:   //点击备武令
        {
            CCLOG("payType:%d", MallPage::shared()->getCurShopPayType());
            if (MallPage::shared() && MallPage::shared()->isRunning() &&
                MallPage::shared()->getCurShopPayType() == kShopPaidByBingingingot &&
                MallPage::shared()->getShopBox() &&
                MallPage::shared()->getShopBox()->numberOfCellsInTableView(NULL) > 0) {
                CCNode *target = MallPage::shared()->getShopCellByIdx(0, 0);
                
                if (target) {
                    getGuide()->showStep(target);
                    getGuide()->addStep();
                }
                else{
                    return true;
                }
            }
            break;
        }
        case GuideStep_5: //点击购买
        {
            if (GoodsBuyOptBox::shared() && GoodsBuyOptBox::shared()->isRunning()) {
                getGuide()->showStep(GoodsBuyOptBox::shared()->getOptBtn());
                getGuide()->addStep();
            }
            break;
        }
        case GuideStep_6:   //关闭商城
        {
            if (!GoodsBuyOptBox::shared() && MallPage::shared()) {
                getGuide()->showStep(MallPage::shared()->getColseBtn());
                getGuide()->addStep();
            }
            break;
        }
        case GuideStep_7:   //结束
        {
            if (!MallPage::shared()) {
                return true;
            }
            break;
        }
    }
    
    return false;
}

//帮会功能开启
bool GuideStep::GuideId_gang_open_step()
{
    if (NewGuideManager::Instance()->getRunningGuideId() != GuideId_gang_open) {
        return false;
    }
    
    switch (NewGuideManager::Instance()->getCurStep()) {
        case GuideStep_1:   //如果菜单未显示，点击菜单切换按钮
        {
            if (!MainMenuPage::isMenuItemsVisible()) {
                getGuide()->showStep(getMainUIView()->getMenuBtn());
            }
            getGuide()->addStep();
            break;
        }
        case GuideStep_2:   //选择帮会菜单
        {
            if (MainMenuPage::isMenuItemsVisible()) {
                getGuide()->showStep( MainMenuPage::openedView->getItemByType(MainMenuPage::kGang) );
                getGuide()->addStep();
            }
            break;
        }
        case GuideStep_3:   //关闭
        {
            /*
            if ((GangList::shared() && GangList::shared()->isRunning()) || (MyGang_info::shared() && MyGang_info::shared()->isRunning())) {
                return true;
//                getGuide()->showStep(GangList::shared()->getCloseBtn());
//                getGuide()->addStep();
            }
             */
            
            if ((MyGang::shared() && MyGang::shared()->isRunning()) || (MyGang_info::shared() && MyGang_info::shared()->isRunning())) {
                return true;
            }
            
            break;
        }
//        case GuideStep_4:   //切换功能菜单
//        {
//            if (!GangList::shared()) {
//                getGuide()->showStep(getMainUIView()->getMenuBtn());
//                getGuide()->addStep();
//            }
//            break;
//        }
//        case GuideStep_5:   //结束
//        {
//            if (!MainMenuPage::isMenuItemsVisible()) {
//                return true;
//            }
//            break;
//        }
    }
    
    return false;
}

//灵兽出战
bool GuideStep::GuideId_pet_fight_step()
{
    if (NewGuideManager::Instance()->getRunningGuideId() != GuideId_pet_fight) {
        return false;
    }
    
    switch (NewGuideManager::Instance()->getCurStep()) {
        case GuideStep_1:   //如果菜单未显示，点击菜单切换按钮
        {
            if (!MainMenuPage::isMenuItemsVisible()) {
                getGuide()->showStep(getMainUIView()->getMenuBtn());
            }
            getGuide()->addStep();
            break;
        }
        case GuideStep_2:   //选择灵兽菜单
        {
            if (MainMenuPage::isMenuItemsVisible()) {
                getGuide()->showStep( MainMenuPage::openedView->getItemByType(MainMenuPage::kPet) );
                getGuide()->addStep();
            }
            break;
        }
        case GuideStep_3:   //选择灵兽(选择默认)
        {
            getGuide()->addStep();
            break;
        }
        case GuideStep_4:   //点击出战
        {
            if (PetView::shared() && PetView::shared()->isRunning()) {
                getGuide()->showStep(PetView::shared()->getFightBtn());
                getGuide()->addStep();
            }
            break;
        }
        case GuideStep_5:   //关闭
        {
            //出战成功
            if (PetView::shared() && PetView::shared()->isRunning() && PetDataManager::Instance()->getWarPetID() > 0) {
                getGuide()->showStep(PetView::shared()->getCloseBtn());
                getGuide()->addStep();
            }
            break;
        }
        case GuideStep_6:   //切换功能菜单
        {
            if (!PetView::shared()) {
                //手动关闭
                //                getGuide()->showStep(getMainUIView()->getMenuBtn());
                if (MainMenuPage::isMenuItemsVisible()) {
                    getMainUIView()->button_menuItems();
                }
                getGuide()->addStep();
            }
            break;
        }
        case GuideStep_7:   //结束
        {
            if (!MainMenuPage::isMenuItemsVisible()) {
                return true;
            }
            break;
        }
    }
    
    return false;
}

//灵兽洗练
bool GuideStep::GuideId_pet_wash_step()
{
    if (NewGuideManager::Instance()->getRunningGuideId() != GuideId_pet_wash) {
        return false;
    }
    
    switch (NewGuideManager::Instance()->getCurStep()) {
        case GuideStep_1:   //如果菜单未显示，点击菜单切换按钮
        {
            if (!MainMenuPage::isMenuItemsVisible()) {
                getGuide()->showStep(getMainUIView()->getMenuBtn());
            }
            getGuide()->addStep();
            break;
        }
        case GuideStep_2:   //选择灵兽菜单
        {
            if (MainMenuPage::isMenuItemsVisible()) {
                getGuide()->showStep( MainMenuPage::openedView->getItemByType(MainMenuPage::kPet) );
                getGuide()->addStep();
            }
            break;
        }
        case GuideStep_3:   //点击灵兽养成
        {
            if (PetView::shared() && PetView::shared()->isRunning()) {
                UDTabBar *bar = dynamic_cast<UDTabBar*>(PetView::shared()->getParent());
                if (bar) {
                    getGuide()->showStep(bar->getTabAtIndex(1));
                    getGuide()->addStep();
                }
                else{
                    return true;
                }
            }
            break;
        }
        case GuideStep_4:   //选择灵兽(默认)
        {
            getGuide()->addStep();
            break;
        }
        case GuideStep_5:   //点击洗练
        {
            if (PetDevelopView::shared() && PetDevelopView::shared()->isRunning()) {
                getGuide()->showStep(PetDevelopView::shared()->getConfirmBtn());
                getGuide()->addStep();
            }
            break;
        }
        case GuideStep_6:   //点击关闭
        {
            if (PetDevelopView::shared() && PetDevelopView::shared()->isRunning() && getGuide()->getCurStepConditionData() == PetDevelopView::shared()) {
                getGuide()->showStep(PetDevelopView::shared()->getCloseBtn());
                getGuide()->addStep();
            }
            break;
        }
        case GuideStep_7:   //切换功能菜单
        {
            if (!PetDevelopView::shared()) {
                //手动关闭
                //                getGuide()->showStep(getMainUIView()->getMenuBtn());
                if (MainMenuPage::isMenuItemsVisible()) {
                    getMainUIView()->button_menuItems();
                }
                getGuide()->addStep();
            }
            break;
        }
        case GuideStep_8:   //结束
        {
            if (!MainMenuPage::isMenuItemsVisible()) {
                return true;
            }
            break;
        }
    }
    
    return false;
}

//灵兽学习主动技能
bool GuideStep::GuideId_pet_skill_1_step()
{
    if (NewGuideManager::Instance()->getRunningGuideId() != GuideId_pet_skill_1) {
        return false;
    }
    
    switch (NewGuideManager::Instance()->getCurStep()) {
        case GuideStep_1:   //如果菜单未显示，点击菜单切换按钮
        {
            if (!MainMenuPage::isMenuItemsVisible()) {
                getGuide()->showStep(getMainUIView()->getMenuBtn());
            }
            getGuide()->addStep();
            break;
        }
        case GuideStep_2:   //选择灵兽菜单
        {
            if (MainMenuPage::isMenuItemsVisible()) {
                getGuide()->showStep( MainMenuPage::openedView->getItemByType(MainMenuPage::kPet) );
                getGuide()->addStep();
            }
            break;
        }
        case GuideStep_3:   //点击灵兽养成
        {
            if (PetView::shared() && PetView::shared()->isRunning()) {
                UDTabBar *bar = dynamic_cast<UDTabBar*>(PetView::shared()->getParent());
                if (bar) {
                    getGuide()->showStep(bar->getTabAtIndex(1));
                    getGuide()->addStep();
                }
                else{
                    return true;
                }
            }
            break;
        }
        case GuideStep_4:   //选择灵兽
        {
            PetDevelopView *devView = PetDevelopView::shared();
            if (devView && devView->isRunning()) {
                CCNode *petCell = NULL;
                //如果出战了灵兽，则选择出战的灵兽来学习主动技能，否则默认选第一个
                if (getPetDataMgr()->getWarPetID() > 0 && devView->getSelectPetId() != getPetDataMgr()->getWarPetID()) {
                    petCell = (CCNode*)devView->getCellByPetId(getPetDataMgr()->getWarPetID());
                }
                
                if (petCell) {
                    getGuide()->showStep(petCell);
                    getGuide()->addStep();
                }
                else{
                    getGuide()->addStep();
                    getGuide()->addStep();
                }
            }
            break;
        }
        case GuideStep_5:   //点击了指定的灵兽
        {
            PetDevelopView *devView = PetDevelopView::shared();
            if (devView && devView->isRunning() && devView->getSelectPetId() == getPetDataMgr()->getWarPetID()) {
                getGuide()->addStep();
            }
            break;
        }
        case GuideStep_6:   //点击技能菜单
        {
            if (PetDevelopView::shared() && PetDevelopView::shared()->isRunning()) {
                getGuide()->showStep( PetDevelopView::shared()->getSegMenu()->getChildByTag(1) );
                getGuide()->addStep();
            }
            break;
        }
        case GuideStep_7:   //点击添加技能书
        {
            if (PetDevelopView::shared() && PetDevelop_skill::shared() && PetDevelop_skill::shared()->isRunning()) {
                getGuide()->showStep(PetDevelopView::shared()->getGoodsBtn());
                getGuide()->addStep();
            }
            break;
        }
        case GuideStep_8:   //选择技能书
        {
            //选择指定技能书 如果没有结束引导
            if (Forging_goodsList::shared() && Forging_goodsList::shared()->isRunning()) {
                Forging_goodsItemCell *cell = Forging_goodsList::shared()->getCellByArticleId(Article_pet_skillBook_1);
                
                if (cell && cell->getItem()) {
                    getGuide()->showStep(cell->getItem()->getPutBtn());
                    getGuide()->addStep();
                }
                else{
                    return true;
                }
            }
            break;
        }
        case GuideStep_9:   //点击学习技能
        {
            PetDevelopView *developView = PetDevelopView::shared();
            if (developView && developView->isRunning() && developView->getSelectedGoodsIid() > 0) {
                getGuide()->showStep(developView->getConfirmBtn());
                getGuide()->addStep();
            }
            break;
        }
        case GuideStep_10:   //点击关闭
        {
            if (PetDevelopView::shared() && PetDevelopView::shared()->isRunning() && getGuide()->getCurStepConditionData() == PetDevelopView::shared()) {
                getGuide()->showStep(PetDevelopView::shared()->getCloseBtn());
                getGuide()->addStep();
            }
            break;
        }
        case GuideStep_11:   //切换功能菜单
        {
            if (!PetDevelopView::shared()) {
                //手动关闭
                //                getGuide()->showStep(getMainUIView()->getMenuBtn());
                if (MainMenuPage::isMenuItemsVisible()) {
                    getMainUIView()->button_menuItems();
                }
                getGuide()->addStep();
            }
            break;
        }
        case GuideStep_12:   //结束
        {
            if (!MainMenuPage::isMenuItemsVisible()) {
                return true;
            }
            break;
        }
    }
    
    return false;
}

//灵兽合成
bool GuideStep::GuideId_pet_compose_step()
{
    if (NewGuideManager::Instance()->getRunningGuideId() != GuideId_pet_compose) {
        return false;
    }
    
    switch (NewGuideManager::Instance()->getCurStep()) {
        case GuideStep_1:   //如果菜单未显示，点击菜单切换按钮
        {
            if (!MainMenuPage::isMenuItemsVisible()) {
                getGuide()->showStep(getMainUIView()->getMenuBtn());
            }
            getGuide()->addStep();
            break;
        }
        case GuideStep_2:   //选择技能菜单
        {
            if (MainMenuPage::isMenuItemsVisible()) {
                getGuide()->showStep( MainMenuPage::openedView->getItemByType(MainMenuPage::kBaguaStove) );
                getGuide()->addStep();
            }
            break;
        }
        case GuideStep_3:   //清除
        {
            if (BaGuaStovePanel::shared() && BaGuaStovePanel::shared()->isRunning()) {
                //清除八卦炉中已经存在的物品
                BaGuaStovePanel::shared()->getAllBaGuaStoveItems();
                getGuide()->addStep();
            }
            break;
        }
        case GuideStep_4:   //放入物品(灵兽魂魄)
        {
            if (BaGuaStovePanel::shared() && BaGuaStovePanel::shared()->isRunning() && !getBagDataMgr()->isHaveItemIncontainer(kItemsBaGuaStove)) {
                CCNode *itemCell = NULL;
                TriggerEventInfo *trigger = getGuideDateMgr()->getTriggerEventInfoById(getGuide()->getRunningGuideId());
                if (trigger) {
                    int articleId = trigger->getTriggerKey();
                    int num = trigger->getTriggerValue();
                    itemCell = BaGuaStovePanel::shared()->getBagBox()->getCellAndShowInPage(articleId, num);
                }
                
                if (itemCell) {
                    getGuide()->showStep(itemCell);
                    getGuide()->addStep();
                }
                else{
                    return true;
                }
            }
            break;
        }
        case GuideStep_5:   //点击合成
        {
            //八卦炉内有物品
            if (BaGuaStovePanel::shared() && BaGuaStovePanel::shared()->isRunning() && getBagDataMgr()->isHaveItemIncontainer(kItemsBaGuaStove)) {
                getGuide()->showStep(BaGuaStovePanel::shared()->getOnceCompoundBtn());
                getGuide()->addStep();
            }
            break;
        }
        case GuideStep_6:   //取回合成物
        {
            //八卦炉产出区内有物品
            if (BaGuaStovePanel::shared() && BaGuaStovePanel::shared()->isRunning() && getBagDataMgr()->isHaveItemIncontainer(kItemsBaGuaStoveOutPut)) {
                getGuide()->showStep(BaGuaStovePanel::shared()->getGetAllBtn());
                getGuide()->addStep();
            }
            break;
        }
        case GuideStep_7:   //关闭八卦炉
        {
            //八卦炉产出区内有物品
            if (BaGuaStovePanel::shared() && BaGuaStovePanel::shared()->isRunning() && !getBagDataMgr()->isHaveItemIncontainer(kItemsBaGuaStoveOutPut)) {
                getGuide()->showStep(BaGuaStovePanel::shared()->getCloseBtn());
                getGuide()->addStep();
            }
            break;
        }
        case GuideStep_8:   //点击背包菜单
        {
            if (!BaGuaStovePanel::shared()) {
                getGuide()->showStep( MainMenuPage::openedView->getItemByType(MainMenuPage::kBag) );
                getGuide()->addStep();
            }
            break;
        }
        case GuideStep_9:   //使用行军猪锦盒
        {
            if (BagView::shared() && BagView::shared()->isRunning() && BagView::shared()->getBagBox()) {
                BagItemCell *itemCell = BagView::shared()->getBagBox()->getCellAndShowInPage(Article_pet_xingJunZhu);
                if (itemCell) {
                    stepUserData = itemCell->getIdentifier();
                    getGuide()->showStep(itemCell);
                    getGuide()->addStep();
                }
                else{
                    return true;
                }
            }
            break;
        }
        case GuideStep_10:  //关闭背包
        {
            //行军猪锦盒已使用
            if (BagView::shared() && !getBagDataMgr()->isItemInContainer(kItemsInBag, stepUserData)) {
                getGuide()->showStep(BagView::shared()->getCloseBtn());
                getGuide()->addStep();
            }
            break;
        }
        case GuideStep_11:   //切换功能菜单
        {
            if (!BagView::openedView) {
                //手动关闭
                //                getGuide()->showStep(getMainUIView()->getMenuBtn());
                if (MainMenuPage::isMenuItemsVisible()) {
                    getMainUIView()->button_menuItems();
                }
                getGuide()->addStep();
            }
            break;
        }
        case GuideStep_12:   //结束
        {
            if (!MainMenuPage::isMenuItemsVisible()) {
                return true;
            }
            break;
        }
    }
    
    return false;
}

//灵兽附体
bool GuideStep::GuideId_pet_possession_step()
{
    if (NewGuideManager::Instance()->getRunningGuideId() != GuideId_pet_possession) {
        return false;
    }
    
    switch (NewGuideManager::Instance()->getCurStep()) {
        case GuideStep_1:   //如果菜单未显示，点击菜单切换按钮
        {
            if (!MainMenuPage::isMenuItemsVisible()) {
                getGuide()->showStep(getMainUIView()->getMenuBtn());
            }
            getGuide()->addStep();
            break;
        }
        case GuideStep_2:   //选择灵兽菜单
        {
            if (MainMenuPage::isMenuItemsVisible()) {
                getGuide()->showStep( MainMenuPage::openedView->getItemByType(MainMenuPage::kPet) );
                getGuide()->addStep();
            }
            break;
        }
        case GuideStep_3:   //选择灵兽
        {
            PetView *petView = PetView::shared();
            if (petView && petView->isRunning()) {
                CCNode *petCell = NULL;
                //选择一个未出战的灵兽
                if (petView->getSelectPetId() == getPetDataMgr()->getWarPetID()) {
                    vector<int> petIds = getPetDataMgr()->getPetIds();
                    for (auto it : petIds) {
                        if (it != petView->getSelectPetId()) {
                            petCell = petView->getPetCellById(it);
                            if (petCell) {
                                stepUserData = it;
                               break;
                            }
                        }
                    }
                }
                
                if (petCell) {
                    getGuide()->showStep(petCell);
                    getGuide()->addStep();
                }
                else{
                    getGuide()->addStep();
                    getGuide()->addStep();
                }
            }
            break;
        }
        case GuideStep_4:   //点击了指定的灵兽
        {
            PetView *petView = PetView::shared();
            if (petView && petView->isRunning() && petView->getSelectPetId() == stepUserData) {
                getGuide()->addStep();
            }
            break;
        }
        case GuideStep_5:   //点击附体
        {
            if (PetView::shared() && PetView::shared()->isRunning()) {
                getGuide()->showStep(PetView::shared()->getPossessionBtn());
                getGuide()->addStep();
            }
            break;
        }
        case GuideStep_6:   //关闭
        {
            //附体成功
            if (PetView::shared() && PetView::shared()->isRunning() && PetDataManager::Instance()->getPossessionPetID() > 0) {
                getGuide()->showStep(PetView::shared()->getCloseBtn());
                getGuide()->addStep();
            }
            break;
        }
        case GuideStep_7:   //切换功能菜单
        {
            if (!PetView::shared()) {
                //手动关闭
                //                getGuide()->showStep(getMainUIView()->getMenuBtn());
                if (MainMenuPage::isMenuItemsVisible()) {
                    getMainUIView()->button_menuItems();
                }
                getGuide()->addStep();
            }
            break;
        }
        case GuideStep_8:   //结束
        {
            if (!MainMenuPage::isMenuItemsVisible()) {
                return true;
            }
            break;
        }
    }
    
    return false;
}

//灵兽学习附体技能
bool GuideStep::GuideId_pet_skill_2_step()
{
    if (NewGuideManager::Instance()->getRunningGuideId() != GuideId_pet_skill_2) {
        return false;
    }
    
    switch (NewGuideManager::Instance()->getCurStep()) {
        case GuideStep_1:   //如果菜单未显示，点击菜单切换按钮
        {
            if (!MainMenuPage::isMenuItemsVisible()) {
                getGuide()->showStep(getMainUIView()->getMenuBtn());
            }
            getGuide()->addStep();
            break;
        }
        case GuideStep_2:   //选择灵兽菜单
        {
            if (MainMenuPage::isMenuItemsVisible()) {
                getGuide()->showStep( MainMenuPage::openedView->getItemByType(MainMenuPage::kPet) );
                getGuide()->addStep();
            }
            break;
        }
        case GuideStep_3:   //点击灵兽养成
        {
            if (PetView::shared() && PetView::shared()->isRunning()) {
                UDTabBar *bar = dynamic_cast<UDTabBar*>(PetView::shared()->getParent());
                if (bar) {
                    getGuide()->showStep(bar->getTabAtIndex(1));
                    getGuide()->addStep();
                }
                else{
                    return true;
                }
            }
            break;
        }
        case GuideStep_4:   //选择灵兽
        {
            PetDevelopView *devView = PetDevelopView::shared();
            if (devView && devView->isRunning()) {
                CCNode *petCell = NULL;
                //如果附体了灵兽，则选择附体的灵兽来学习主动技能，否则默认选第一个
                if (getPetDataMgr()->getPossessionPetID() > 0 && devView->getSelectPetId() != getPetDataMgr()->getPossessionPetID()) {
                    petCell = (CCNode*)devView->getCellByPetId(getPetDataMgr()->getPossessionPetID());
                }
                
                if (petCell) {
                    getGuide()->showStep(petCell);
                    getGuide()->addStep();
                }
                else{
                    getGuide()->addStep();
                    getGuide()->addStep();
                }
            }
            break;
        }
        case GuideStep_5:   //点击了指定的灵兽
        {
            PetDevelopView *devView = PetDevelopView::shared();
            if (devView && devView->isRunning() && devView->getSelectPetId() == getPetDataMgr()->getPossessionPetID()) {
                getGuide()->addStep();
            }
            break;
        }
        case GuideStep_6:   //点击技能菜单
        {
            if (PetDevelopView::shared() && PetDevelopView::shared()->isRunning()) {
                getGuide()->showStep( PetDevelopView::shared()->getSegMenu()->getChildByTag(1) );
                getGuide()->addStep();
            }
            break;
        }
        case GuideStep_7:   //点击添加技能书
        {
            if (PetDevelopView::shared() && PetDevelop_skill::shared() && PetDevelop_skill::shared()->isRunning()) {
                getGuide()->showStep(PetDevelopView::shared()->getGoodsBtn());
                getGuide()->addStep();
            }
            break;
        }
        case GuideStep_8:   //选择技能书
        {
            //选择指定技能书 如果没有结束引导
            if (Forging_goodsList::shared() && Forging_goodsList::shared()->isRunning()) {
                TriggerEventInfo *triggerInfo = getGuideDateMgr()->getTriggerEventInfoById(getGuide()->getRunningGuideId());
                Forging_goodsItemCell *cell = NULL;
                if (triggerInfo) {
                    cell = Forging_goodsList::shared()->getCellByArticleId(triggerInfo->getTriggerKey());
                }
                
                if (cell && cell->getItem()) {
                    getGuide()->showStep(cell->getItem()->getPutBtn());
                    getGuide()->addStep();
                }
                else{
                    return true;
                }
            }
            break;
        }
        case GuideStep_9:   //点击学习技能
        {
            PetDevelopView *developView = PetDevelopView::shared();
            if (developView && developView->isRunning() && developView->getSelectedGoodsIid() > 0) {
                getGuide()->showStep(developView->getConfirmBtn());
                getGuide()->addStep();
            }
            break;
        }
        case GuideStep_10:   //点击关闭
        {
            if (PetDevelopView::shared() && PetDevelopView::shared()->isRunning() && getGuide()->getCurStepConditionData() == PetDevelopView::shared()) {
                getGuide()->showStep(PetDevelopView::shared()->getCloseBtn());
                getGuide()->addStep();
            }
            break;
        }
        case GuideStep_11:   //切换功能菜单
        {
            if (!PetDevelopView::shared()) {
                //手动关闭
                //                getGuide()->showStep(getMainUIView()->getMenuBtn());
                if (MainMenuPage::isMenuItemsVisible()) {
                    getMainUIView()->button_menuItems();
                }
                getGuide()->addStep();
            }
            break;
        }
        case GuideStep_12:   //结束
        {
            if (!MainMenuPage::isMenuItemsVisible()) {
                return true;
            }
            break;
        }
    }
    
    return false;
}

//每日活动
bool GuideStep::GuideId_dailyActive_step()
{
    if (NewGuideManager::Instance()->getRunningGuideId() != GuideId_dailyActive) {
        return false;
    }
    
    switch (NewGuideManager::Instance()->getCurStep()) {
        case GuideStep_1:   //点击功能按钮
        {
            if (!getMainUIView()->getIsFuncListOpen()) {
                getGuide()->showStep(getMainUIView()->getFuncListBtn(), guideArrow_UpRight);
            }
            getGuide()->addStep();
            break;
        }
        case GuideStep_2:   //点击日常活动
        {
            if (getMainUIView()->getIsFuncListOpen()) {
                getGuide()->showStep(getMainUIView()->getHUDMenuItem(kHUDMenuNormalDay));
                getGuide()->addStep();
            }
            break;
        }
        case GuideStep_3:   //结束
        {
            if (ActiveView::shared() && ActiveView::shared()->isRunning()) {
                return true;
            }
            break;
        }
    }
    
    return false;
}

//手动升级
bool GuideStep::GuideId_hand_levelUp_step()
{
    if (NewGuideManager::Instance()->getRunningGuideId() != GuideId_hand_levelUp) {
        return false;
    }
    
    switch (NewGuideManager::Instance()->getCurStep()) {
        case GuideStep_1:   //点击头像
        {
            getGuide()->showStep(getMainUIView()->getHeroInfo()->getHeadProfile());
            getGuide()->addStep();
            break;
        }
        case GuideStep_2:   //点击升级
        {
            if (PropertyPanel::shared() && PropertyPanel::shared()->isRunning()) {
                getGuide()->showStep(PropertyPanel::shared()->getUplevelSpr());
                getGuide()->addStep();
            }
            break;
        }
        case GuideStep_3:   //点击关闭
        {
            if (PropertyPanel::shared() && Hero::Instance()->getInfo()->getLevel() == Level_51) {
                getGuide()->showStep(PropertyPanel::shared()->getCloseBtn());
                getGuide()->addStep();
            }
            break;
        }
        case GuideStep_4: //结束
        {
            if (!PropertyPanel::shared()) {
                return true;
            }
            break;
        }
    }
    
    return false;
}