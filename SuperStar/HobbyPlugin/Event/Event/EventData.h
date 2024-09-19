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

        // virtual const EventTypeStr& VEventTypeStr() const = 0;
        virtual Uint64 VEventTypeHash() const = 0;
        virtual Uint64 VDataTypeHash() const  = 0;
        virtual Float32 VTimeStamp() const    = 0;
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

        explicit BaseEventData(const EventTypeStr& in_rEventType, const EventTypeStr& in_rDataType,
                               const Float32 in_fTimeStamp = 0.0f)
            : _fTimeStamp(in_fTimeStamp),
              _ulEventTypeHash(in_rEventType.Hash()),
              _ulDataTypeHash(in_rDataType.Hash())
        {
        }

        virtual ~BaseEventData() = default;
        // virtual const EventTypeStr& VEventTypeStr() const = 0;
        Uint64 VEventTypeHash() const override final { return this->_ulEventTypeHash; }
        Uint64 VDataTypeHash() const override final { return this->_ulDataTypeHash; }
        Float32 VTimeStamp() const override final { return this->_fTimeStamp; }
        // virtual void VSerialize(std::ostrstream& out) const {}

    protected:
        const Float32 _fTimeStamp     = 0.0f;
        const Uint64 _ulEventTypeHash = 0;
        const Uint64 _ulDataTypeHash  = 0;
    };
}  // namespace Event
