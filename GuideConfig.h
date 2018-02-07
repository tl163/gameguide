//
//  GuideConfig.h
//  qnol
//
//  Created by lizhou on 15-1-17.
//
//

#ifndef qnol_GuideConfig_h
#define qnol_GuideConfig_h

/*
 * 引导触发类型
 * 0.任务触发 1.物品触发 2.ui触发 3.角色等级触发
 */
enum GuideTriggerType
{
    GuideTriggerType_task   = 0,
    GuideTriggerType_goods  = 1,
    GuideTriggerType_ui     = 2,
    GuideTriggerType_level  = 3,
};

/*
 * ui触发类型的引导下的UI索引
 * 0.MainMenuPage 开放新功能
 *
 *
 *
 */
enum UI_GuideTriggerType
{
    UI_Trigger_MainMenu_openFunc = 0,
    UI_Trigger_hand_levelUp      = 1,
};

/*
 * 引导类型
 * 0.强制引导
 * 1.自由选择引导
 * 2.半强制  目前只有最后一步是非强制，其余步骤全是强制
 */
enum GuideType
{
    GuideType_compel = 0,
    GuideType_free   = 1,
    GuideType_half   = 2
};

/*
 * 引导步骤
 *
 */
enum GuideStepIdx
{
    GuideStep_none  = 0,
    GuideStep_1     = 1,
    GuideStep_2     = 2,
    GuideStep_3     = 3,
    GuideStep_4     = 4,
    GuideStep_5     = 5,
    GuideStep_6     = 6,
    GuideStep_7     = 7,
    GuideStep_8     = 8,
    GuideStep_9     = 9,
    GuideStep_10    = 10,
    GuideStep_11    = 11,
    GuideStep_12    = 12,
    GuideStep_13    = 13,
    GuideStep_14    = 14,
    GuideStep_15    = 15,
};

/*
 * 手指的方向
 */
enum GuideArrowDir {
	guideArrow_UpLeft,
	guideArrow_UpRight,
	guideArrow_DownLeft,
	guideArrow_DownRight,
};

/*
 * 引导索引
 * 对应策划文件 Resource/xlsx/triggerEvent.xlsx
 * 0. 第一次学习人物技能
 * 1. 第一次强化装备
 * 2. 第一次进入副本
 * 3. 第二次学习人物技能
 * 4. 第一次使用八卦炉合成
 * 5. 第二次强化装备
 * 6. 第二次进入副本
 * 7. 商城购买道具
 * 8. 帮会功能开启
 * 9. 灵兽出战
 * 10. 灵兽洗练
 * 11. 灵兽学习主动技能
 * 12. 灵兽合成
 * 13. 灵兽附体
 * 14. 灵兽学习附体技能
 * 15. 每日活动
 * 16. 手动升级
 */
enum GuideId
{
    GuideId_none          = -1,
    GuideId_skill_1       = 0,
    GuideId_forging_1     = 1,
    GuideId_instance_1    = 2,
    GuideId_skill_2       = 3,
    GuideId_bagua_1       = 4,
    GuideId_forging_2     = 5,
    GuideId_instance_2    = 6,
    GuideId_mall          = 7,
    GuideId_gang_open     = 8,
    GuideId_pet_fight     = 9,
    GuideId_pet_wash      = 10,
    GuideId_pet_skill_1   = 11,
    GuideId_pet_compose   = 12,
    GuideId_pet_possession= 13,
    GuideId_pet_skill_2   = 14,
    GuideId_dailyActive   = 15,
    GuideId_hand_levelUp  = 16,
    
    GuideId_count,
    GuideId_MAX           = 64
};

/*
 * 引导相关写死在客户端的数据
 */
enum
{
    Skill_index_1           = 1,            //第一个人物技能的索引
    Skill_index_2           = 2,            //第二个人物技能的索引
    Article_haoYueShi_1     = 11000100,     //1级锆月石
    Article_haoYueShi_2     = 11000101,     //2级锆月石
    Article_zhiSuLu_f       = 11001700,     //凡品紫苏露
    Article_pet_skillBook_1 = 11002120,     //灵兽主动技能书
    Article_pet_xingJunZhu  = 11007828,     //行军猪锦盒
    Level_51                = 51,           //等级51级
};

#endif
