/*
 *  LoadingView.cpp
 *  netpanzer-osx
 *
 *  Created by krom on 4/27/08.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */

#include "LoadingView.hpp"

#include "Classes/ScreenSurface.hpp"
#include "Classes/Network/NetworkClient.hpp"

#include "Interfaces/GameConfig.hpp"
#include "Interfaces/GameManager.hpp"
#include "Interfaces/PlayerInterface.hpp"

#include "Views/Components/Desktop.hpp"
#include "Views/Components/Label.hpp"
#include "Views/Game/VehicleSelectionView.hpp"

#include "Particles/ParticleInterface.hpp"

std::list<std::string> LoadingView::lines;
bool LoadingView::dirty = true;

static void bAbort()
{
    if(gameconfig->quickConnect)
    {
        GameManager::exitNetPanzer();
        return;
    }
    GameManager::quitNetPanzerGame();
    Desktop::setVisibilityAllWindows(false);
    Desktop::setVisibility("MainView", true);
}

void
LoadingView::init()
{
    setSearchName("LoadingView");
    setTitle("Loading Progress");
    setSubTitle("");

    setAllowResize(false);
    setAllowMove(false);
    setDisplayStatusBar(false);
    setVisible(false);
    setBordered(false);
    need_password = false;
    password_str.init("", 31,31);

    resize(800, 600);

    addButtonCenterText(iXY(348, 260), 101, "Abort", "Cancel the joining of this game.", bAbort);


    okButton = Button::createTextButton("OK", "Enter", iXY(348,105), 100);
    okButton->setTextColors(Color::black,Color::red,Color::darkGray);
    passwordLabel = new Label(348, 60, "Game Password", Color::lightGray);

}


void
LoadingView::doDraw(Surface &viewArea, Surface &clientArea)
{
    if (dirty)
        render();

    screen->fill(Color::black);
    backgroundSurface.blt(clientArea, 0, 0);
    clientArea.FillRoundRect(iRect(166, 40, 628, 225), 10, Color::black);
    clientArea.RoundRect(iRect(166, 40, 628, 225), 10, Color::lightGray);
    surface.blt(clientArea, 172, 45);

    View::doDraw(viewArea, clientArea);
}

void
LoadingView::render()
{
    dirty=false;

    surface.fill(Color::black);

    int ypos = surface.getHeight() - Surface::getFontHeight();
    int fontHeight = Surface::getFontHeight();

    std::list<std::string>::reverse_iterator i = lines.rbegin();
    while ( i != lines.rend() && ypos > -fontHeight )
    {
        surface.bltString( 0, ypos, (*i).c_str(), Color::white);
        i++;
        ypos -= fontHeight;
    }
}

void
LoadingView::doActivate()
{
    backgroundSurface.loadBMP("pics/backgrounds/menus/menu/defaultMB.bmp");
    surface.create(455, 175, 1);
    dirty=true;
}

void
LoadingView::doDeactivate()
{
    backgroundSurface.free();
    surface.free();
}

void
LoadingView::setNeedPassword(bool need_password)
{
    if ( this->need_password != need_password )
    {
        this->need_password = need_password;
        if ( need_password )
        {
            password_field = addInputField(iXY(175+88, 80), &password_str, "", true, 31);
            add(okButton);
            add(passwordLabel);
        }
        else
        {
            removeInputField(password_field);
            removeComponent(okButton);
            removeComponent(passwordLabel);
            password_field = 0;
        }
    }
}

void
LoadingView::onComponentClicked(Component* c)
{
    std::string cname = c->getName();
    if ( !cname.compare("Button.OK") )
    {
        CLIENT->joinServer(gameconfig->serverConnect, NPString(password_str.getString()));
        ClientConnectDaemon::startConnectionProcess();
        setNeedPassword(false);
    }
    else
    {
        View::onComponentClicked(c);
    }
}

void
LoadingView::update(const NPString& text)
{
    LOGGER.info("Loading: %s", text.c_str());
    lines.pop_back();
    lines.push_back(text);
    dirty=true;
}

void
LoadingView::append(const NPString& text)
{
    LOGGER.info("Loading: %s", text.c_str());
    if ( lines.size() >= LINELIMIT )
    {
        lines.pop_front();
    }
    lines.push_back(text);
    dirty=true;
}

void
LoadingView::loadFinish()
{
    Desktop::setVisibilityAllWindows(false);

    // XXX rebuild the particle stuff here for units...
    ParticleInterface::rebuildUnitParticleData();

    // XXX this needed because has to create the special buttons for the
    // defined units, has to be here because it has to be the "netp" palette
    // and after loading the unit profiles
    View * v = Desktop::getView("VehicleSelectionView");
    if ( v )
    {
        Desktop::remove(v);
        delete v;
    }

    Desktop::add(new VehicleSelectionView());

    GameManager::setNetPanzerGameOptions();
    Desktop::setVisibility("MiniMapView", true);
    Desktop::setVisibility("MiniMapViewAlt", false);
    Desktop::setVisibility("GameView", true);
    if ( PlayerInterface::getLocalPlayer()->isSelectingFlag() )
    {
        Desktop::setVisibility("UStyleSelectionView", true);
        Desktop::setActiveView("UStyleSelectionView");

        //Desktop::setVisibility("GFlagSelectionView", true);
        //Desktop::setActiveView("GFlagSelectionView");
    }
}

void
LoadingView::loadFinish_d()
{
    //Desktop::setVisibilityAllWindows(false);

    // XXX rebuild the particle stuff here for units...
    ParticleInterface::rebuildUnitParticleData();

    // XXX this needed because has to create the special buttons for the
    // defined units, has to be here because it has to be the "netp" palette
    // and after loading the unit profiles
    /*
    View * v = Desktop::getView("VehicleSelectionView");
    if ( v )
    {
        Desktop::remove(v);
        delete v;
    }

    Desktop::add(new VehicleSelectionView());
    */
    GameManager::setNetPanzerGameOptions();
    /*
    Desktop::setVisibility("MiniMapView", true);
    Desktop::setVisibility("GameView", true);
    if ( PlayerInterface::getLocalPlayer()->isSelectingFlag() )
    {
        Desktop::setVisibility("GFlagSelectionView", true);
        Desktop::setActiveView("GFlagSelectionView");
    }
    */
}





void
LoadingView::loadError()
{
    if ( gameconfig->quickConnect )
    {
        GameManager::exitNetPanzer();
    }
    else if ( Desktop::getVisible("LoadingView") )
    {
        Desktop::setVisibilityAllWindows(false);
        Desktop::setVisibility("MainView", true);
        GameManager::quitNetPanzerGame();
    }
}

void
LoadingView::show()
{
    Desktop::setVisibilityAllWindows(false);
    Desktop::setVisibility("LoadingView", true);
    View *v = Desktop::getView("LoadingView");
    Desktop::setFocusView(v);
}

void
LoadingView::hide()
{
    Desktop::setVisibility("LoadingView", false);
}
