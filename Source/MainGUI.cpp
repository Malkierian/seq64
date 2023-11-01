/*
 * ============================================================================
 *
 * MainGUI.cpp
 * Outer application and GUI window.
 * 
 * From seq64 - Sequenced music editor for first-party N64 games
 * Copyright (C) 2014-2023 Sauraen
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 * ============================================================================
*/

#include "Common.hpp"
#include "SeqEditor.hpp"
#include "CommandLine.hpp"
#include <fstream>

seq64Application* seq64Application::Instance;

const String seq64Application::getApplicationName() {
    return "SEQ64 V" + String(ProjectInfo::versionString);
}
const String seq64Application::getApplicationVersion() {
    return ProjectInfo::versionString;
}
bool seq64Application::moreThanOneInstanceAllowed() {
    return true;
}

void seq64Application::SaveSettings() {
    std::ofstream o("./settings.json");
    o << std::setw(4) << settings << std::endl;
    o.close();
}

void seq64Application::initialise (const String& commandLine) {
    if(!commandLine.isEmpty()){
        StringArray args = StringArray::fromTokens(commandLine, true);
        args.insert(0, "seq64_gui"); //Arg 0 is the program name and is ignored
        seq64_cli(args);
        quit();
    }else{
        if (!std::filesystem::exists("./settings.json")) {
            settings = nlohmann::json::object();
            SaveSettings();
        } else {
            std::ifstream i("./settings.json");
            i >> settings;
            i.close();
        }
        DefaultDirectory = File::getSpecialLocation(File::userHomeDirectory).getFullPathName();
        Instance = this;
        mainWindow.reset (new MainWindow (getApplicationName()));
    }
}
void seq64Application::shutdown() {
    mainWindow = nullptr;
}
void seq64Application::systemRequestedQuit() {
    quit();
}
void seq64Application::anotherInstanceStarted (const String& commandLine) {
    ignoreUnused(commandLine);
}

int seq64Application::SettingsGetInteger(String key, int defaultValue) {
    if (settings.contains(key.toStdString())) {
        return settings.at(key.toStdString());
    } else {
        return defaultValue;
    }
}

String seq64Application::SettingsGetString(String key, String defaultValue) {
    if (settings.contains(key.toStdString())) {
        return settings.at(key.toStdString());
    }
    else {
        return defaultValue;
    }
}

void seq64Application::SettingsSetInteger(String key, int value) {
    settings[key.toStdString()] = value;
    SaveSettings();
}

void seq64Application::SettingsSetString(String key, String value) {
    settings[key.toStdString()] = value.toStdString();
    SaveSettings();
}

seq64Application::MainWindow::MainWindow(String name) : DocumentWindow (name,
    juce::Desktop::getInstance().getDefaultLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId),
    DocumentWindow::allButtons)
{
    setUsingNativeTitleBar (true);
    setContentOwned (new SeqEditor(), true);
    setResizable (true, true);
    centreWithSize (getWidth(), getHeight());
    setVisible (true);
}

void seq64Application::MainWindow::closeButtonPressed() {
    JUCEApplication::getInstance()->systemRequestedQuit();
}

START_JUCE_APPLICATION (seq64Application)
