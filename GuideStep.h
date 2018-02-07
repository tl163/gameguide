//  包含了所有的引导的步骤实现, 与GuideConfig下的GuideId一一对应
//  GuideStep.h
//  qnol
//
//  Created by lizhou on 15-1-19.
//
//

#ifndef __qnol__GuideStep__
#define __qnol__GuideStep__

class GuideStep
{
public:
    /*
     * return true: 引导完成，flase: 引导未完成(进行中)
     */
    
    //学习第一个技能
    static bool GuideId_skill_1_step();
    
    //第一次强化装备
    static bool GuideId_forging_1_step();
    
    //第一次进入副本
    static bool GuideId_instance_1_step();
    
    //第二次学习人物技能
    static bool GuideId_skill_2_step();
    
    //第一次使用八卦炉合成
    static bool GuideId_bagua_1_step();
    
    //第二次强化装备
    static bool GuideId_forging_2_step();
    
    //第二次进入副本
    static bool GuideId_instance_2_step();
    
    //商城购买道具
    static bool GuideId_mall_step();
    
    //帮会功能开启
    static bool GuideId_gang_open_step();
    
    //灵兽出战
    static bool GuideId_pet_fight_step();
    
    //灵兽洗练
    static bool GuideId_pet_wash_step();
    
    //灵兽学习主动技能
    static bool GuideId_pet_skill_1_step();
    
    //灵兽合成
    static bool GuideId_pet_compose_step();
    
    //灵兽附体
    static bool GuideId_pet_possession_step();
    
    //灵兽学习附体技能
    static bool GuideId_pet_skill_2_step();
    
    //每日活动
    static bool GuideId_dailyActive_step();
    
    //手动升级
    static bool GuideId_hand_levelUp_step();
};

#endif /* defined(__qnol__GuideStep__) */
