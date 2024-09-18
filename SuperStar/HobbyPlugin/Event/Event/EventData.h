#pragma once

#include "Engine/Common/CustomString.h"
#include "Engine/MiniEngine.h"

namespace Event
{
    using EventTypeStr = Core::Common::FixString128;

    class EventDataInterface
    {
    public:
        virtual ~EventDataInterface() = default;

        virtual const EventTypeStr& VEventTypeStr() const = 0;
        virtual Float32 VTimeStamp() const                = 0;
        // virtual void VSerialize(std::ostrstream& out) const = 0;
    };

    using EventDataInterfacePtr = Core::Memory::SharedPtr<EventDataInterface>;

    class BaseEventData : public EventDataInterface
    {
    public:
        enum EType
        {
            EType_SpecalEvent = 0,
        };

        explicit BaseEventData(const Float32 in_fTimeStamp = 0.0f) : _fTimeStamp(in_fTimeStamp) {}
        virtual ~BaseEventData()                          = default;
        virtual const EventTypeStr& VEventTypeStr() const = 0;
        Float32 VTimeStamp() const override final { return this->_fTimeStamp; }
        // virtual void VSerialize(std::ostrstream& out) const {}

    protected:
        const Float32 _fTimeStamp;
    };

}  // namespace Event
