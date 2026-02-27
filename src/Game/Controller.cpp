#include "Game/Controller.h"

namespace nyaa
{

float Controller::dpx = 0;
float Controller::dpy = 0;
unsigned int Controller::dpxFrames = 0;
unsigned int Controller::dpyFrames = 0;
float Controller::rsX = 0;
float Controller::rsY = 0;
std::map<Btn, float> Controller::HeldMillis;
std::map<Btn, unsigned int> Controller::BtnFrames;
std::map<Btn, bool> Controller::BtnRel;

} // namespace nyaa
