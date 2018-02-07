//
//  TriggerEventManager.cpp
//  qnol
//
//  Created by lizhou on 15-1-16.
//
//

#include "GuideDataManager.h"
#include "NewGuideManager.h"
#include "MessageStruct.h"
#include "CB_Macros.h"

GuideDataManager::GuideDataManager()
{
    //初始化GuideDataManager
    NewGuideManager::Instance();
}

void GuideDataManager::setDefaultState(int64_t defState)
{
    NewGuideManager::Instance()->setDefaultState(defState);
}

void GuideDataManager::setTriggerEvents(vector<TriggerEventInfo*>& triggerEvents)
{
    SAFE_DELETE_STD_VECTOR_2(_guideDatas);
    _guideDatas = triggerEvents;
    triggerEvents.clear();
}

vector<TriggerEventInfo*>* GuideDataManager::getTriggerEvents()
{
    return &_guideDatas;
}

TriggerEventInfo* GuideDataManager::getTriggerEventInfoById(int guideId)
{
    if (guideId < _guideDatas.size()) {
        if (_guideDatas[guideId]->getId() == guideId) {
            return _guideDatas[guideId];
        }
        else{
            for (auto it : _guideDatas) {
                if (it->getId() == guideId) {
                    return it;
                }
            }
        }
    }
    return NULL;
}

map<int, vector<string> > GuideDataManager::getTalkMapById(int guideId)
{
    map<int, vector<string> > talkMap;
    TriggerEventInfo * info = getTriggerEventInfoById(guideId);
    if (info) {
        talkMap = info->getTalkMap();
    }
    
    return talkMap;
}

GuideDataManager::~GuideDataManager()
{
    SAFE_DELETE_STD_VECTOR_2(_guideDatas);
}