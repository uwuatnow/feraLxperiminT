#include "Events/Timeline.h"

namespace nyaa {

Timeline::Timeline()
    :Event(0, 0)
{

}

Timeline::~Timeline()
{
    
}

int64_t Timeline::getLengthTime() const
{
    int64_t ret = -1;
    for(auto child : getChildren())
    {
        ret = std::max(ret, child->getEndTime());
    }
    return ret;
}

}