#include <extensions/kitchensink/menu.h>

#include <extensions/kitchensink/quest.h>

namespace tram::Ext::Kitchensink {

void QuestList::Display() {
    GUI::PushFrameRelative(GUI::FRAME_CENTER_HORIZONTAL, 640);
    GUI::PushFrameRelative(GUI::FRAME_CENTER_VERTICAL, 480);
    GUI::FillFrame(Ext::Menu::FONT_WIDGETS, GUI::WIDGET_BUTTON);
    GUI::PushFrameRelative(GUI::FRAME_INSET, 5);
    
    for (const auto& quest : PoolProxy<Quest>::GetPool()) {
        GUI::Text(Ext::Menu::FONT_TEXT_BOLD, quest.name);
        GUI::NewLine();
        
        GUI::PushFrameRelativeKeepCursor(GUI::FRAME_LEFT_INV, 10);
        
        for (const auto& objective : quest.variables) {
            if (objective.type != QUEST_VAR_OBJECTIVE) continue;
            
            //if (objective.state == 1);
            
            GUI::Text(Ext::Menu::FONT_TEXT_BOLD, objective.value1); GUI::NewLine(GUI::LINE_LOW);
            GUI::Text(Ext::Menu::FONT_TEXT, objective.value2); GUI::NewLine();
        }
        
        GUI::PopFrameKeepCursor();
    }
    
    GUI::PopFrame();
    GUI::PopFrame();
    GUI::PopFrame();
}

void KitchensinkMenuList::Display() {
    GUI::PushFrameRelative(GUI::FRAME_CENTER_HORIZONTAL, 100);
    GUI::PushFrameRelative(GUI::FRAME_CENTER_VERTICAL, 100);
    GUI::FillFrame(Ext::Menu::FONT_WIDGETS, GUI::WIDGET_BUTTON);
    GUI::PushFrameRelative(GUI::FRAME_INSET, 5);
    
    if (GUI::Button("Show quests", true, 90)) {
        Menu::Push(new QuestList);
    }
    
    GUI::NewLine();
    
    if (GUI::Button("Show inventory", true, 90)) {
        Menu::Push(new QuestList);
    }
    
    
    GUI::PopFrame();
    GUI::PopFrame();
    GUI::PopFrame();
}
    
}