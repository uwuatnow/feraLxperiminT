#include "Screen/PCScreen.h"
#include "Game/Game.h"
#include "Screen/InGameScreen.h"
#include "Game/Controller.h"
#include "Game/Fonts.h"
#include "Entity/Computer.h"
#include "Game/Mouse.h"
#include "Game/GameClock.h"
#include "Screen/PCSite.h"
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/Text.hpp>
#include "Game/Sfx.h"
#include <random>

namespace nyaa {

PCScreen* PCScreen::Instance = nullptr;

PCScreen::PCScreen()
{
	Instance = this;
	accessedFrom = nullptr;

	PCSiteRegistry::Init();
}

PCScreen::~PCScreen()
{
}

void PCScreen::doTick(RendTarget* renderTarget)
{
	Screen::doTick(renderTarget);

	// Update window animations
	float dt = nyaa::G->frameDeltaMillis / 1000.0f;
	float animSpeed = 5.0f; // Full open in 0.2s

	if (accessedFrom)
	{
		for (auto it = accessedFrom->windows.begin(); it != accessedFrom->windows.end();)
		{
			if (it->isClosing)
			{
				it->scale -= animSpeed * dt;
				if (it->scale <= 0.0f)
				{
					it->scale = 0.0f;
					it->isOpen = false;
					it->isClosing = false; // Reset state
				}
			}
			else if (it->isOpen)
			{
				it->scale += animSpeed * dt;
				if (it->scale > 1.0f) it->scale = 1.0f;
			}

			// Only increment if we didn't erase
			++it;
		}

	}

	IGS->otherScreenUpdate();

	// Update the boot timer if active
	if (accessedFrom && accessedFrom->bootTimer)
	{
		accessedFrom->bootTimer->update();
	}

	renderTarget->clear(sf::Color::Black);

	if (accessedFrom)
	{
		if (accessedFrom->state == Computer::State_Booting)
		{
			if (accessedFrom->bootTimer && accessedFrom->bootTimer->secs() >= 2.0f)
			{
				accessedFrom->state = Computer::State_Desktop;
			}
			else
			{
				// Render "booting up"
				sf::Text text("Booting up...", *Fonts::OSDFont, 32);
				text.setPosition(Game::ScreenWidth / 2.0f - text.getLocalBounds().width / 2.0f, Game::ScreenHeight / 2.0f - 50);
				text.setFillColor(sf::Color::White);
				renderTarget->draw(text);

				// Progress bar at bottom center
				float progress = accessedFrom->bootTimer ? accessedFrom->bootTimer->secs() / 2.0f : 0.0f;
				sf::RectangleShape bar(sf::Vector2f(300 * progress, 20));
				bar.setPosition(Game::ScreenWidth / 2.0f - 150, Game::ScreenHeight - 50);
				bar.setFillColor(sf::Color::Green);
				renderTarget->draw(bar);

				// Outline
				sf::RectangleShape outline(sf::Vector2f(300, 20));
				outline.setPosition(Game::ScreenWidth / 2.0f - 150, Game::ScreenHeight - 50);
				outline.setFillColor(sf::Color::Transparent);
				outline.setOutlineColor(sf::Color::White);
				outline.setOutlineThickness(2);
				renderTarget->draw(outline);
			}
		}
		else if (accessedFrom->state == Computer::State_Desktop)
		{
			// Need to run animation update logic BEFORE input/render loop for correct frame handling
			// But doTick calls this function logic at end. 
			// We moved the update logic to be at the end of doTick, which is after this block.
			// Ideally we want to update animations, calculate positions, THEN handle input/render.
			// Let's move the animation/physics block to the top of doTick or before this if/else.
			
			handleDesktopInput();
			renderDesktop(renderTarget);
		}
	}

	// (Animation block moved to top)

	// Switch back on escape
	if (Kb::IsKeyReleased(KB::Escape) || Controller::BtnFrames[Btn_Circle] == 1)
	{
	}

	// Calculate Repulsion
	// Reset render coords
	if (accessedFrom)
	{
		for (auto& win : accessedFrom->windows)
		{
			win.renderX = win.x;
			win.renderY = win.y;
		}

		for (const auto& opener : accessedFrom->windows)
		{
			// Identify opening windows (scale < 1.0, not closing, isOpen)
			if (opener.isOpen && !opener.isClosing && opener.scale > 0.0f && opener.scale < 1.0f)
			{
				sf::Vector2f centerOpener(opener.x + opener.w / 2.0f, opener.y + opener.h / 2.0f);

				// Repulse others
				for (auto& other : accessedFrom->windows)
				{
					if (&other == &opener) continue;
					if (!other.isOpen) continue;
					if (other.isClosing) continue; 
					if (other.dragging) continue; // Don't push windows while they are being dragged!

					sf::Vector2f centerOther(other.x + other.w / 2.0f, other.y + other.h / 2.0f);
					sf::Vector2f diff = centerOther - centerOpener;
					float distSq = diff.x * diff.x + diff.y * diff.y;
					float dist = std::sqrt(distSq);

					if (dist < 0.1f) dist = 0.1f; // Avoid div by zero

					// Force direction
					sf::Vector2f dir = diff / dist;

					// Repulsion logic
					// We want a bump that peaks in the middle of the animation?
					// Or simply proportional to the expansion rate?
					// User said: "pushed away... when done, go back"
					// A simple sin wave based on opener.scale works well for 0->1->0 effect

					float pushStrength = 150.0f; // Max pixels to push
					float effect = std::sin(opener.scale * 3.14159f);

					// Falloff based on distance (so near windows move more)
					float distFactor = 1.0f - (dist / 800.0f); // 800px radius
					if (distFactor < 0) distFactor = 0;

					float totalPush = pushStrength * effect * distFactor;

					other.renderX += dir.x * totalPush;
					other.renderY += dir.y * totalPush;
				}
			}
		}
	}
}

bool PCScreen::onEnter()
{
	if (accessedFrom) accessedFrom->desktopClock.restart();
	return true;
}

void PCScreen::handleDesktopInput()
{
	if (!accessedFrom) return;

	float currentTime = accessedFrom->desktopClock.getElapsedTime().asSeconds();
	sf::Vector2f mousePos((float)Mouse::Pos_X, (float)Mouse::Pos_Y);

	// Handle dragging first
	bool somethingDragged = false;

	// Windows (top to bottom for interaction)
	for (int i = (int)accessedFrom->windows.size() - 1; i >= 0; --i)
	{
		auto& win = accessedFrom->windows[i];
		if (!win.isOpen) continue;

		// Ignore input if animating
		if (win.scale < 0.95f) continue;

		// Close button
		if (mousePos.x >= win.renderX + win.w - 24 && mousePos.x <= win.renderX + win.w - 4 &&
			mousePos.y >= win.renderY + 2 && mousePos.y <= win.renderY + 22)
		{
			if (Mouse::LeftFrames == 1)
			{
				win.isClosing = true;
				Sfx::Close->play();
				return;
			}
		}

		// Title bar dragging
		if (mousePos.x >= win.renderX && mousePos.x <= win.renderX + win.w &&
			mousePos.y >= win.renderY && mousePos.y <= win.renderY + 24)
		{
			if (Mouse::LeftFrames == 1)
			{
				win.dragging = true;
				win.dragOffsetX = win.renderX - mousePos.x; // Use renderX for visual offset
				win.dragOffsetY = win.renderY - mousePos.y;

				// Move to front
				DesktopWindow moved = win;
				if (i < (int)accessedFrom->windows.size() - 1)
				{
					if (moved.scale > 0.95f)
					{
						moved.scale = 0.0f;
						Sfx::Open->play();
					}
				}

				accessedFrom->windows.erase(accessedFrom->windows.begin() + i);
				accessedFrom->windows.push_back(moved);
				return;
			}
		}

		// Content area interactions (scrolling)
		if (mousePos.x >= win.renderX && mousePos.x <= win.renderX + win.w &&
			mousePos.y >= win.renderY + 24 && mousePos.y <= win.renderY + win.h)
		{
			if (Mouse::WheelDelta != 0)
			{
				win.scrollY -= Mouse::WheelDelta * 20;
				if (win.scrollY < 0) win.scrollY = 0;

				if (win.site)
				{
					float contentH = win.site->getContentHeight(win.w - 4);
					float visibleH = win.h - 24 - 30; // h - titlebar - header
					float maxScroll = contentH - visibleH;
					if (maxScroll < 0) maxScroll = 0;
					if (win.scrollY > maxScroll) win.scrollY = maxScroll;
				}
			}

			if (win.site)
			{
				float headerH = 30.0f;
				float relX = mousePos.x - (win.renderX + 2);
				float relY = mousePos.y - (win.renderY + 24);

				if (relY < headerH)
				{
					if (win.site->handleHeaderInput(relX, relY, Mouse::LeftFrames == 1))
					{
						// Back button clicked
						if (!win.history.empty())
						{
							std::string lastState = win.history.back();
							win.history.pop_back();

							size_t separator = lastState.find('|');
							if (separator != std::string::npos)
							{
								std::string siteTitle = lastState.substr(0, separator);
								std::string siteLink = lastState.substr(separator + 1);

								// Find site in registry
								if (siteTitle != win.site->getTitle())
								{
									PCSite* newSite = PCSiteRegistry::CreateSite(siteTitle);
									if (newSite)
									{
										delete win.site;
										win.site = newSite;
									}
								}
								win.currentLink = siteLink;
								win.site->onLinkClicked(siteLink);
								win.scrollY = 0;
							}
						}
					}
				}
				else
				{
					win.site->handleContentInput(mousePos.x - win.renderX, relY - headerH + win.scrollY, win.w - 4, Mouse::LeftFrames == 1);
				}

				// Handle navigation requests
				if (!win.site->navRequest.empty())
				{
					std::string req = win.site->navRequest;
					win.site->navRequest = "";

					std::string currentSiteTitle = win.site ? win.site->getTitle() : "";
					
					if (req.find("site:") == 0)
					{
						std::string targetTitle = req.substr(5);
						if (targetTitle != win.site->getTitle())
						{
							PCSite* newSite = PCSiteRegistry::CreateSite(targetTitle);
							if (newSite)
							{
								// Record history before switching
								win.history.push_back(currentSiteTitle + "|" + win.currentLink);

								delete win.site;
								win.site = newSite;
								win.currentLink = "root";
								win.site->onLinkClicked("root");
								win.scrollY = 0;
							}
						}
					}
					else 
					{
						// Internal link navigation
						win.history.push_back(currentSiteTitle + "|" + win.currentLink);
						win.currentLink = req;
						win.site->onLinkClicked(req);
						win.scrollY = 0;
					}
				}

				if (win.site->scrollResetRequested) {
					win.scrollY = 0;
					win.site->scrollResetRequested = false;
				}
			}
		}

		if (win.dragging)
		{
			if (Mouse::LeftRel || Mouse::LeftFrames == 0)
			{
				win.dragging = false;
			}
			else
			{
				// Here we need to update the base position (win.x, win.y)
				// We know that win.renderX = win.x + PushOffset
				// And we want win.renderX to be at Mouse + Offset
				// So win.x + PushOffset = Mouse + Offset
				// win.x = Mouse + Offset - pushX

				// Let's deduce the push offset from the current state:
				float pushX = win.renderX - win.x;
				float pushY = win.renderY - win.y;

				win.x = mousePos.x + win.dragOffsetX - pushX;
				win.y = mousePos.y + win.dragOffsetY - pushY;

				// Prevent dragging under the taskbar
				if (win.y + win.h > Game::ScreenHeight - 40) {
					win.y = Game::ScreenHeight - 40 - win.h;
				}

				// Prevent dragging above the top edge
				if (win.y < 0) {
					win.y = 0;
				}
				somethingDragged = true;
			}
		}

		if (win.dragging || (mousePos.x >= win.renderX && mousePos.x <= win.renderX + win.w && mousePos.y >= win.renderY && mousePos.y <= win.renderY + win.h))
		{
			// If we clicked the window but it wasn't the top one (top is size()-1)
			if (Mouse::LeftFrames == 1 && i < (int)accessedFrom->windows.size() - 1)
			{
				DesktopWindow moved = win;
				if (moved.scale > 0.95f)
				{
					moved.scale = 0.0f;
					Sfx::Open->play();
				}
				accessedFrom->windows.erase(accessedFrom->windows.begin() + i);
				accessedFrom->windows.push_back(moved);
			}
			return;
		}
	}

	// Taskbar interaction
	if (mousePos.y >= Game::ScreenHeight - 40)
	{
		if (Mouse::LeftFrames == 1)
		{
			// Check window buttons
			float startX = 10;
			for (size_t i = 0; i < accessedFrom->windows.size(); ++i)
			{
				auto& win = accessedFrom->windows[i];
				if (!win.isOpen) continue;

				sf::FloatRect btnRect(startX, Game::ScreenHeight - 35, 120, 30);
				if (btnRect.contains(mousePos))
				{
					// Pull to front if not already front
					if (i < accessedFrom->windows.size() - 1)
					{
						DesktopWindow moved = win;
						if (moved.scale > 0.95f)
						{
							moved.scale = 0.0f;
							Sfx::Open->play();
						}
						accessedFrom->windows.erase(accessedFrom->windows.begin() + i);
						accessedFrom->windows.push_back(moved);
					}
					return;
				}
				startX += 130;
			}
		}
		return; // Don't allow clicks to pass through taskbar
	}

	// Icons - First pass: handle existing drag
	for (auto& icon : accessedFrom->icons)
	{
		if (icon.dragging)
		{
			if (Mouse::LeftRel || Mouse::LeftFrames == 0)
			{
				icon.dragging = false;
			}
			else
			{
				icon.posX = mousePos.x + icon.dragOffsetX;
				icon.posY = mousePos.y + icon.dragOffsetY;
			}
			return;
		}
	}

	// Icons - Second pass: start new drag or handle click
	for (auto& icon : accessedFrom->icons)
	{
		if (mousePos.x >= icon.posX - 30 && mousePos.x <= icon.posX + 30 &&
			mousePos.y >= icon.posY - 30 && mousePos.y <= icon.posY + 50)
		{
			if (Mouse::LeftFrames == 1)
			{
				if (icon.lastClickTime > 0 && currentTime - icon.lastClickTime < 0.3f)
				{
					// Double click
					if (icon.label == "Shutdown")
					{
						IGS->switchTo();
						return;
					}

					bool found = false;
					for (size_t i = 0; i < accessedFrom->windows.size(); ++i)
					{
						if (accessedFrom->windows[i].title == icon.label)
						{
							if (!accessedFrom->windows[i].isOpen)
							{
								accessedFrom->windows[i].scale = 0.0f;
								Sfx::Open->play();
							}
							accessedFrom->windows[i].isOpen = true;
							accessedFrom->windows[i].isClosing = false;

							// Move to front if not already
							if (i < accessedFrom->windows.size() - 1) {
								DesktopWindow temp = accessedFrom->windows[i];
								accessedFrom->windows.erase(accessedFrom->windows.begin() + i);
								accessedFrom->windows.push_back(temp);
							}

							found = true;
							break;
						}
					}
					if (!found)
					{
						PCSite* targetSite = PCSiteRegistry::CreateSite(icon.label);
						
						// Add random variation to window opening position
						float baseX = 200.0f;
						float baseY = 100.0f;
						float randomX = accessedFrom->windowPosRandomDist(accessedFrom->windowPosRandomGen);
						float randomY = accessedFrom->windowPosRandomDist(accessedFrom->windowPosRandomGen);

						// Ensure window stays within screen bounds
						float finalX = baseX + randomX;
						float finalY = baseY + randomY;
						if (finalX < 0) finalX = 0;
						if (finalY < 0) finalY = 0;
						if (finalX + 400 > Game::ScreenWidth) finalX = Game::ScreenWidth - 400;
						if (finalY + 300 > Game::ScreenHeight - 40) finalY = Game::ScreenHeight - 40 - 300;

						accessedFrom->windows.push_back({ icon.label, finalX, finalY, 400, 300, true, false, 0, 0, targetSite, 0, 0.0f, false, finalX, finalY, {}, "root" });
						Sfx::Open->play();
					}
					icon.lastClickTime = -1.0f;
				}
				else
				{
					icon.lastClickTime = currentTime;
					icon.dragging = true;
					icon.dragOffsetX = icon.posX - mousePos.x;
					icon.dragOffsetY = icon.posY - mousePos.y;
				}
				return;
			}
		}
	}
}

bool PCScreen::onLeave()
{
	if (accessedFrom) accessedFrom->shutdown();
	accessedFrom = nullptr;
	return true;
}

void PCScreen::renderDesktop(RendTarget* renderTarget)
{
	if (!accessedFrom) return;

	// Icons
	for (auto& icon : accessedFrom->icons)
	{
		// Icon box
		sf::RectangleShape box(sf::Vector2f(32, 32));
		box.setOrigin(16, 16);
		box.setPosition(icon.posX, icon.posY);
		box.setFillColor(sf::Color(100, 150, 255));
		box.setOutlineColor(sf::Color::White);
		box.setOutlineThickness(1);
		renderTarget->draw(box);

		// Icon label
		sf::Text text(icon.label, *Fonts::OSDFont, 14);
		text.setOrigin(text.getLocalBounds().width / 2, 0);
		text.setPosition(icon.posX, icon.posY + 20);
		text.setFillColor(sf::Color::White);
		text.setOutlineColor(sf::Color::Black);
		text.setOutlineThickness(1);
		renderTarget->draw(text);
	}

	// Windows
	for (auto& win : accessedFrom->windows)
	{
		if (!win.isOpen && win.scale <= 0.0f) continue;

		// Setup scaled view for the frame
		float s = win.dragging ? 1.0f : win.scale;
		if (s <= 0.001f) continue;
		float screenW = (float)Game::ScreenWidth;
		float screenH = (float)Game::ScreenHeight;
		sf::Vector2f winCenter(win.renderX + win.w / 2.0f, win.renderY + win.h / 2.0f);
		sf::Vector2f screenCenter(screenW / 2.0f, screenH / 2.0f);

		sf::View oldView = renderTarget->getView();
		sf::View scaledView = oldView;

		scaledView.setSize(screenW / s, screenH / s);
		scaledView.setCenter((screenCenter - winCenter) / s + winCenter);

		renderTarget->setView(scaledView);

		// Frame
		sf::RectangleShape frame(sf::Vector2f(win.w, win.h));
		frame.setPosition(win.renderX, win.renderY);
		frame.setFillColor(sf::Color(40, 40, 45));
		frame.setOutlineColor(sf::Color(180, 180, 180));
		frame.setOutlineThickness(2);
		renderTarget->draw(frame);

		// Title bar
		sf::RectangleShape titleBar(sf::Vector2f(win.w, 24));
		titleBar.setPosition(win.renderX, win.renderY);
		titleBar.setFillColor(sf::Color(70, 70, 80));
		renderTarget->draw(titleBar);

		// Title
		sf::Text title(win.title, *Fonts::OSDFont, 14);
		title.setPosition(win.renderX + 8, win.renderY + 4);
		title.setFillColor(sf::Color::White);
		renderTarget->draw(title);

		// Close button
		sf::RectangleShape closeBtn(sf::Vector2f(18, 18));
		closeBtn.setPosition(win.renderX + win.w - 21, win.renderY + 3);
		closeBtn.setFillColor(sf::Color(180, 50, 50));
		renderTarget->draw(closeBtn);

		sf::Text xText("x", *Fonts::OSDFont, 14);
		xText.setPosition(win.renderX + win.w - 16, win.renderY + 3);
		xText.setFillColor(sf::Color::White);
		renderTarget->draw(xText);

		// Sticky Header Area (Back button etc)
		if (win.site)
		{
			// Render sticky header if it exists
			win.site->renderHeader(renderTarget, win.renderX + 2, win.renderY + 26, win.w - 4, 30, !win.history.empty());
		}

		// Content area clipping
		float headerHeight = 30.0f;

		// Calculate the "visual" content rect on screen for the viewport
		// VisualRect = Center + (OriginalRect - Center) * s
		// We need TopLeft and Size.
		// VisualTL = Center + (OriginalTL - Center) * s
		// VisualSize = OriginalSize * s

		sf::Vector2f origTL(win.renderX + 2, win.renderY + 26 + headerHeight);
		sf::Vector2f visualTL = winCenter + (origTL - winCenter) * s;
		sf::Vector2f origSize(win.w - 4, win.h - 28 - headerHeight);
		sf::Vector2f visualSize = origSize * s;

		sf::FloatRect scaledContentRect(visualTL.x, visualTL.y, visualSize.x, visualSize.y);

		// Separator line (drawn in scaled view)
		sf::RectangleShape line(sf::Vector2f(win.w - 4, 1));
		line.setPosition(win.renderX + 2, win.renderY + 26 + headerHeight - 1);
		line.setFillColor(sf::Color(100, 100, 100));
		renderTarget->draw(line);

		// Map screen coords to view coords
		sf::FloatRect viewport(scaledContentRect.left / Game::ScreenWidth, scaledContentRect.top / Game::ScreenHeight,
							   scaledContentRect.width / Game::ScreenWidth, scaledContentRect.height / Game::ScreenHeight);

		sf::View contentView(sf::FloatRect(0, win.scrollY, origSize.x, origSize.y));
		contentView.setViewport(viewport);
		renderTarget->setView(contentView);

		if (win.site)
		{
			// Render content (elements)
			// We pass a flag or a special call to render only elements
			// For now, let's just use the same render but PCSite will handle it
			win.site->renderContent(renderTarget, 0, 0, origSize.x, origSize.y);
		}
		else
		{
			// Content placeholder
			sf::Text content("Content for " + win.title, *Fonts::OSDFont, 20);
			content.setPosition(20, 40);
			content.setFillColor(sf::Color(150, 150, 150));
			renderTarget->draw(content);
		}

		renderTarget->setView(oldView);
	}

	// Taskbar
	sf::RectangleShape taskbar(sf::Vector2f((float)Game::ScreenWidth, 40));
	taskbar.setPosition(0, (float)Game::ScreenHeight - 40);
	taskbar.setFillColor(sf::Color(60, 60, 70));
	taskbar.setOutlineColor(sf::Color(100, 100, 110));
	taskbar.setOutlineThickness(-1);
	renderTarget->draw(taskbar);

	// Taskbar clock
	if (IGS && IGS->gameClock)
	{
		sf::Text clockText(IGS->gameClock->timeStr, *Fonts::OSDFont, 10);
		sf::FloatRect bounds = clockText.getLocalBounds();
		clockText.setOrigin(bounds.width, bounds.height / 2.0f);
		clockText.setPosition((float)Game::ScreenWidth - 15, (float)Game::ScreenHeight - 20);
		clockText.setFillColor(sf::Color::White);
		renderTarget->draw(clockText);
	}

	// Taskbar window buttons
	float startX = 10;
	for (auto& win : accessedFrom->windows)
	{
		if (!win.isOpen) continue;

		sf::RectangleShape btn(sf::Vector2f(120, 30));
		btn.setPosition(startX, (float)Game::ScreenHeight - 35);
		btn.setFillColor(sf::Color(80, 80, 95));
		btn.setOutlineColor(sf::Color::White);
		btn.setOutlineThickness(1);
		renderTarget->draw(btn);

		sf::Text btnText(win.title, *Fonts::OSDFont, 12);
		float textWidth = btnText.getLocalBounds().width;
		btnText.setPosition(startX + 60 - textWidth / 2, (float)Game::ScreenHeight - 28);
		btnText.setFillColor(sf::Color::White);
		renderTarget->draw(btnText);

		startX += 130;
	}
}

}