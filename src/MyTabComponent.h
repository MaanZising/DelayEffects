#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

class MyTabComponent : public juce::TabbedComponent
{
public:
    MyTabComponent() : juce::TabbedComponent(juce::TabbedButtonBar::TabsAtLeft)
	{
	}

    // Callback used by the editor to listen for changes
    std::function<void(int)> onTabChanged;

	void currentTabChanged (int newCurrentTabIndex, const juce::String& newCurrentTabName) override
	{
        juce::ignoreUnused (newCurrentTabName);
        if (onTabChanged != nullptr)
            onTabChanged (newCurrentTabIndex);
	}
};