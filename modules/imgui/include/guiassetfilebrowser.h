#pragma once
#include <modules/imgui/include/guicomponent.h>



namespace openspace::gui {

    class GuiAssetFileBrowser : public GuiComponent {
    public:
        GuiAssetFileBrowser();

        void render() override;

    private:
        bool fileBrowserOpen = false;
        void updateFileBrowser();

    };

}
