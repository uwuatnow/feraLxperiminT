#include "Game/GuiWidget.h"

namespace nyaa {

GuiWidget* guip = nullptr;
GuiWidget* guip_eof = nullptr;
GuiWidget* guip_eof_LMB_FF = nullptr;
GuiWidget* guip_eof_RMB_FF = nullptr;
GuiWidget* guip_eof_MMB_FF = nullptr;
GuiWidget* guip_selected = nullptr;

GuiWidget::~GuiWidget()
{
	
}

}