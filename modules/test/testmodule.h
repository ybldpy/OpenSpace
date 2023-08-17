#pragma once

#include <openspace/util/openspacemodule.h>

#include <openspace/documentation/documentation.h>
#include <openspace/properties/scalar/boolproperty.h>
#include <openspace/properties/scalar/floatproperty.h>
#include <openspace/properties/stringproperty.h>

namespace openspace{
    class TestModule : public OpenSpaceModule {
    public:
        constexpr static const char* Name = "TestModule";
        TestModule();

    protected:
       
        properties::BoolProperty _enabled;
        properties::FloatProperty _x;
        properties::FloatProperty _y;
        properties::FloatProperty _z;

    private:
        void stopOritNavigator();

     
    };
    
}
