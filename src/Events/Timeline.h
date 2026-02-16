#pragma once
#include "Events/Event.h"

namespace nyaa {

class Timeline : public Event
{
public:
    Timeline();
    ~Timeline();

    int64_t getLengthTime() const override;
};

} // namespace nyaa
