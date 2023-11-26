#ifndef __OPENSPACE_MODULE_NBODY__H
#define __OPENSPACE_MODULE_NBODY__H

#include <openspace/util/openspacemodule.h>

#include <openspace/documentation/documentation.h>

namespace openspace {

    class NBodyModule : public OpenSpaceModule {
    public:
        constexpr static const char* Name = "NBody";

        NBodyModule();
        ~NBodyModule() override = default;

        //std::vector<documentation::Documentation> documentations() const override;
        //scripting::LuaLibrary luaLibrary() const override;

    private:
        //void internalInitialize(const ghoul::Dictionary&) override;
    };

} // namespace openspace

#endif // __OPENSPACE_MODULE_NBODY__H
