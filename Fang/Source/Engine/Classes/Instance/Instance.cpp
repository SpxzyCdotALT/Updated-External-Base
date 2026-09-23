#include "../../Engine.h"

namespace SDK {

    std::string Instance::Name() const {
        if (!Address) return "?";

        uintptr_t NameContainer = Driver->Read<uintptr_t>(Address + Offsets::Instance::NameContainer);
        if (!NameContainer) return "?";

        return Driver->Read_String(NameContainer + Offsets::Instance::Name);
    }
    std::string Instance::Text() const {

        return Driver->Read_String(this->Address + Offsets::GuiObject::Text);
    }

    std::string Instance::Class() const {

        if (!Address) return "?";
        uintptr_t Descriptor = Driver->Read<uintptr_t>(Address + Offsets::Instance::ClassDescriptor);
        if (!Descriptor) return "?";
        uintptr_t StringPointer = Driver->Read<uintptr_t>(Descriptor + 0x8);
        if (!StringPointer) return "?";
        return Driver->Read_String(StringPointer);
    }

    Instance Instance::Parent() const {

        if (!Address) return Instance();
        return Driver->Read<Instance>(Address + Offsets::Instance::Parent);
    }

    std::vector<Instance> Instance::Children() const {

        std::vector<Instance> Container;
        if (!Address) return Container;

        auto Start = Driver->Read<uintptr_t>(Address + Offsets::Instance::ChildrenStart);
        if (!Start) return Container;

        auto End = Driver->Read<uintptr_t>(Start + Offsets::Instance::ChildrenEnd);
        for (auto instances = Driver->Read<uintptr_t>(Start); instances != End; instances += 16)
            Container.emplace_back(Driver->Read<Instance>(instances));

        return Container;
    }

    Instance Instance::Find_First_Child(const std::string& Name) const {

        if (!Address || Name.empty()) return Instance();

        for (Instance Child : Children())
        {
            if (!Child.Address) continue;
            if (Child.Name() == Name) return Child;
        }

        return Instance();
    }

    Instance Instance::Find_First_Child_Of_Class(const std::string& Class_Name) const {

        if (!Address || Class_Name.empty()) return Instance();

        for (Instance Child : Children())
        {
            if (!Child.Address) continue;
            if (Child.Class() == Class_Name) return Child;
        }

        return Instance();
    }
}
