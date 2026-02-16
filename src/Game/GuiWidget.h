#pragma once
#include "Game/Globals.h"

namespace nyaa {

class GuiWidget
{

	//Do not put anything here, EVER.

public:
	virtual ~GuiWidget();
};

extern GuiWidget* guip; /* pointer to the topmost interactable gui element */
extern GuiWidget* guip_eof; /* what guip was at the end of frame */
extern GuiWidget* guip_eof_LMB_FF; /*what guip_eof was on first frame of mouse left btn down */
extern GuiWidget* guip_eof_RMB_FF; /*what guip_eof was on first frame of mouse right btn down */
extern GuiWidget* guip_eof_MMB_FF; /*what guip_eof was on first frame of mouse middle btn down */
extern GuiWidget* guip_selected; //set explicitly

}
