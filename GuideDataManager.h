//
//  GuideDataManager.h
//  qnol
//
//  Created by lizhou on 15-1-16.
//
//

#ifndef __qnol__GuideDataManager__
#define __qnol__GuideDataManager__

#include <iostream>
#include "Tools.h"

using namespace std;

class TriggerEventInfo;

class GuideDataManager : public Utility::Singleton<GuideDataManager>
{
public:
    GuideDataManager();
    ~GuideDataManager();
    
    /*
     * 从服务端获得当前所有引导的状态
     */
    void setDefaultState(int64_t defState);
    
    void setTriggerEvents(vector<TriggerEventInfo*>& triggerEvents);
    vector<TriggerEventInfo*>* getTriggerEvents();
    
    TriggerEventInfo* getTriggerEventInfoById(int guideId);
    
    map<int, vector<string> >  getTalkMapById(int guideId);
    
private:
    vector<TriggerEventInfo*>   _guideDatas;
};

#endif /* defined(__qnol__GuideDataManager__) */
