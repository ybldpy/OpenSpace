#include <modules/imgui/imguimodule.h>
#include <modules/imgui/include/imgui_include.h>
#include <modules/imgui/include/guiassetfilebrowser.h>
#include <modules/imgui/ext/imgui/imfilebrowser.h>
#include <openspace/engine/globals.h>
#include <openspace/scripting/scriptengine.h>
#include <iostream>



namespace {
    
    ImGui::FileBrowser fileBrowser(ImGuiFileBrowserFlags_CloseOnEsc);
    //ImGui::ImVec2 windowSize(350,350);

    bool flag = false;
    std::string pathCharacterReplace(std::string path,const std::string& replacement="/",const int& replacementSize = 1) {
        auto i = path.find("\\");
        while (i!= std::string::npos) {
            path.replace(i, replacementSize, replacement);
            i = path.find("\\");
        }
        return path;
        

    }
    void addAsset(const std::filesystem::path& filePath) {
        
        std::cout << "openspace.asset.add(\"" + pathCharacterReplace(filePath.parent_path().string() + "\\" + filePath.filename().string()) + "\")" << std::endl;
        openspace::global::scriptEngine->queueScript("openspace.asset.add(\"" + pathCharacterReplace(filePath.parent_path().string() + "\\" + filePath.filename().string()) + "\")", openspace::scripting::ScriptEngine::RemoteScripting::Yes);
    }

}

namespace openspace::gui {

    GuiAssetFileBrowser::GuiAssetFileBrowser():GuiComponent("AssetFileBrowser", "Asset File Browser"){ fileBrowser.SetTitle("Select"); }
    
    void GuiAssetFileBrowser::updateFileBrowser() {
        //if (!fileBrowser.IsOpened()) { return; }
        fileBrowser.Display();
        if (fileBrowser.HasSelected()) {
            addAsset(fileBrowser.GetSelected());
            fileBrowser.ClearSelected();
            fileBrowser.Close();
        }
    }
    
    void GuiAssetFileBrowser::render() {

        ImGui::SetNextWindowCollapsed(_isCollapsed);
        bool v = _isEnabled;
        ImGui::SetNextWindowBgAlpha(0.5f);
        ImGui::Begin(guiName().c_str(),&v,ImGuiWindowFlags_AlwaysAutoResize);
        _isEnabled = v;
        _isCollapsed = ImGui::IsWindowCollapsed();
        
        if (ImGui::Button("Add")&&!fileBrowser.IsOpened())
        {
            fileBrowser.Open();
        }

        updateFileBrowser();
       
        

        ImGui::End();
       

        

    }


}
