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
	if (guip == this) guip = nullptr;
    if (guip_eof == this) guip_eof = nullptr;
    if (guip_eof_LMB_FF == this) guip_eof_LMB_FF = nullptr;
    if (guip_eof_RMB_FF == this) guip_eof_RMB_FF = nullptr;
    if (guip_eof_MMB_FF == this) guip_eof_MMB_FF = nullptr;
    if (guip_selected == this) guip_selected = nullptr;
}

}