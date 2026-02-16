#pragma once

#include <vector>
#include <cstdint>

namespace nyaa {

/**
 * Abstract base class for events timed in units with children
 */
class Event {
public:
    /**
     * Constructor
     * @param startTime The start time of the event in units
     * @param lengthTime The duration of the event in units
     */
    Event(int64_t startTime, int64_t lengthTime)
        : startTime(startTime), lengthTime(lengthTime) {}

    /**
     * Virtual destructor for proper cleanup of derived classes
     */
    virtual ~Event() = default;

    /**
     * Get the start time of the event
     * @return The start time in units
     */
    int64_t getStartTime() const { return startTime; }

    /**
     * Get the length/duration of the event
     * @return The length in units
     */
    virtual int64_t getLengthTime() const { return lengthTime; }

    /**
     * Get the end time of the event (start time + length)
     * @return The end time in units
     */
    int64_t getEndTime() const { return startTime + lengthTime; }

    /**
     * Add a child event to this event
     * @param child The child event to add
     */
    void addChild(Event* child) {
        children.push_back(child);
    }

    /**
     * Get all child events
     * @return Reference to the vector of child events
     */
    const std::vector<Event*>& getChildren() const {
        return children;
    }

    /**
     * Get mutable access to child events
     * @return Reference to the vector of child events
     */
    std::vector<Event*>& getChildren() {
        return children;
    }

protected:
    int64_t startTime;  // Start time in units
    int64_t lengthTime; // Length/duration in units
    std::vector<Event*> children; // Child events
};

} // namespace nyaa