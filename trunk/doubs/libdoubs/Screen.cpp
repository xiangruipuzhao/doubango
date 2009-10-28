#include "Screen.h"
#include "globals.h"

Screen::Screen(SCREEN_TYPE_T _type, QWidget *parent, Qt::WFlags flags):
QWidget(parent, flags), type(_type)
{
	/* add the screen to the map */
	//globals::getScreenMgr()->addScreen(this);
}

Screen::~Screen()
{
	/* remove the screen from the map */
	//globals::getScreenMgr()->removeScreen(this);
}