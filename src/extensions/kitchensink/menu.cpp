#include <extensions/kitchensink/menu.h>

#include <extensions/kitchensink/quest.h>
#include <extensions/kitchensink/dialog.h>

#include <cstring>

namespace tram::Ext::Kitchensink {
    
const size_t SEARCH_STRING_LENGTH = 100;

ListDialog::ListDialog() : ListDialog(name_t()) {}

ListDialog::ListDialog(name_t dialog) {
    this->dialog_topic = dialog;
}

ListDialog::~ListDialog() {
    if (search_string) delete[] search_string;
}

void ListDialog::Display() {
    if (dialog_topic) goto dialog; else goto selection;
selection:
    
    if (!search_string) search_string = new char[SEARCH_STRING_LENGTH + 1], *search_string = '\0';
    
    GUI::PushFrameRelative(GUI::FRAME_CENTER_HORIZONTAL, 640);
    GUI::PushFrameRelative(GUI::FRAME_CENTER_VERTICAL, 480);
    GUI::FillFrame(Ext::Menu::FONT_WIDGETS, GUI::WIDGET_BUTTON);
    GUI::PushFrameRelative(GUI::FRAME_INSET, 5);
    
        GUI::TextBox(search_string, SEARCH_STRING_LENGTH, true, 400);
        GUI::NewLine();
    
        for (auto& topic : PoolProxy<DialogTopic>::GetPool()) {
            if (!strstr(topic.name, search_string)) continue;
            
            if (GUI::Button(topic.name)) dialog_topic = topic.name;
            
            GUI::NewLine();
        }
    
    GUI::PopFrame();
    GUI::PopFrame();
    GUI::PopFrame();

    return;

dialog:
    GUI::PushFrameRelative(GUI::FRAME_CENTER_HORIZONTAL, 640);
    
    GUI::PushFrameRelative(GUI::FRAME_BOTTOM, 250);
    GUI::PushFrameRelative(GUI::FRAME_TOP, 200);


    GUI::FillFrame(Ext::Menu::FONT_WIDGETS, GUI::WIDGET_BUTTON);
    GUI::PushFrameRelative(GUI::FRAME_INSET, 5);

        auto topic = DialogTopic::Find(dialog_topic);
    
        if (!topic) {
            GUI::Text(Ext::Menu::FONT_TEXT, "Not found: ");
            GUI::Text(Ext::Menu::FONT_TEXT, dialog_topic);
        } else {
            auto next_topics = topic->GetValidNextTopics();
    
            GUI::Text(Ext::Menu::FONT_TEXT, topic->answer);
            
            GUI::NewLine();
            
            for (auto next : next_topics) {
                auto next_topic = DialogTopic::Find(next);
                
                if (!next_topic) {
                    GUI::Button(next, false);
                } else {
                    if (GUI::Button(next_topic->prompt)) {
                        next_topic->action.Perform();
                        dialog_topic = next;
                    }
                }
            }
        }
    
    GUI::PopFrame();
    GUI::PopFrame();
    GUI::PopFrame();
    GUI::PopFrame();
}

    
    
    
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
            
            if (objective.objective.value == "unknown") GUI::SetColor(Render::COLOR_GRAY);
            if (objective.objective.value == "obsolete") GUI::SetColor(Render::COLOR_BLUE);
            if (objective.objective.value == "finished") GUI::SetColor(Render::COLOR_GREEN);
            if (objective.objective.value == "failed") GUI::SetColor(Render::COLOR_RED);
            if (objective.objective.value == "known") GUI::SetColor(Render::COLOR_BLACK);
            
            GUI::Text(Ext::Menu::FONT_TEXT_BOLD, objective.objective.title); GUI::NewLine(GUI::LINE_LOW);
            GUI::Text(Ext::Menu::FONT_TEXT, objective.objective.subtitle); GUI::NewLine();
        }
        
        GUI::SetColor(Render::COLOR_BLACK);
        
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
    
    if (GUI::Button("Show dialogs", true, 90)) {
        Menu::Push(new ListDialog);
    }
    
    GUI::NewLine();
    
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