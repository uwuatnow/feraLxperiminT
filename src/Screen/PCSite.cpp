#include "Screen/PCSite.h"
#include "Screen/ForumSite.h"
#include "Game/Fonts.h"
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <sstream>
#include "Game/Game.h"

namespace nyaa {

// --- Element Implementations ---

void PCTextElement::render(sf::RenderTarget* target, float x, float y, float w) {
	wrap(w);
	float curY = y;
	for (const auto& line : lines) {
		sf::Text t(line, *Fonts::OSDFont, size);
		t.setPosition(x, curY);
		t.setFillColor(color);
		target->draw(t);
		curY += (float)size + 4.0f;
	}
}

float PCTextElement::getHeight(float w) const {
	wrap(w);
	return lines.size() * ((float)size + 4.0f) + 6.0f;
}

void PCRichTextElement::wrap(float w) const {
	if (w == lastW) return;
	lastW = w;
	lines.clear();

	sf::Text helper("", *Fonts::OSDFont, size);
	float curX = 0;
	RichLine curLine;

	auto flushLine = [&]() {
		if (!curLine.pieces.empty()) {
			lines.push_back(curLine);
			curLine = RichLine();
		}
		curX = 0;
	};

	for (const auto& span : spans) {
		std::stringstream ss(span.text);
		std::string word;
		while (ss >> word) {
			std::string spaceWord = (curX == 0) ? word : " " + word;
			helper.setString(spaceWord);
			float wordW = helper.getLocalBounds().width;

			if (curX + wordW > w && curX > 0) {
				flushLine();
				spaceWord = word; // No leading space on new line
				helper.setString(spaceWord);
				wordW = helper.getLocalBounds().width;
			}

			// Try to merge with last piece if same style
			if (!curLine.pieces.empty() && curLine.pieces.back().link == span.link && curLine.pieces.back().color == span.color) {
				curLine.pieces.back().text += spaceWord;
				helper.setString(curLine.pieces.back().text);
				curLine.pieces.back().bounds.width = helper.getLocalBounds().width;
			} else {
				RichPiece p;
				p.text = spaceWord;
				p.color = span.color;
				p.link = span.link;
				p.bounds = sf::FloatRect(curX, 0, wordW, (float)size + 4.0f);
				curLine.pieces.push_back(p);
			}
			curX += wordW;
		}
	}
	flushLine();
}

void PCRichTextElement::addSpan(std::string t, sf::Color c, std::string l) {
	spans.push_back({ t, c, l });
	lastW = -1.0f;
}

void PCRichTextElement::render(sf::RenderTarget* target, float x, float y, float w) {
	wrap(w);
	float curY = y;
	for (const auto& line : lines) {
		for (const auto& p : line.pieces) {
			sf::Text t(p.text, *Fonts::OSDFont, size);
			t.setPosition(x + p.bounds.left, curY);
			t.setFillColor(p.color);
			// Underline links
			if (!p.link.empty()) {
				sf::RectangleShape under(sf::Vector2f(p.bounds.width, 1));
				under.setPosition(x + p.bounds.left, curY + size + 2);
				under.setFillColor(p.color);
				target->draw(under);
			}
			target->draw(t);
		}
		curY += (float)size + 6.0f;
	}
}

bool PCRichTextElement::handleInput(float relX, float relY, float w, bool leftClick, PCSite* parent) {
	if (!leftClick) return false;
	wrap(w);
	float curY = 0;
	for (const auto& line : lines) {
		if (relY >= curY && relY <= curY + size + 6.0f) {
			for (const auto& p : line.pieces) {
				if (!p.link.empty() && relX >= p.bounds.left && relX <= p.bounds.left + p.bounds.width) {
					parent->onLinkClicked(p.link);
					return true;
				}
			}
		}
		curY += (float)size + 6.0f;
	}
	return false;
}

float PCRichTextElement::getHeight(float w) const {
	wrap(w);
	return lines.size() * ((float)size + 6.0f) + 4.0f;
}

void PCLinkElement::render(sf::RenderTarget* target, float x, float y, float w) {
	sf::RectangleShape box(sf::Vector2f(w, 30));
	box.setPosition(x, y);
	box.setFillColor(sf::Color(60, 60, 80));
	target->draw(box);

	sf::Text t(text, *Fonts::OSDFont, 16);
	t.setPosition(x + 10, y + 5);
	t.setFillColor(sf::Color::Cyan);
	target->draw(t);
}

bool PCLinkElement::handleInput(float relX, float relY, float w, bool leftClick, PCSite* parent) {
	if (relY >= 0 && relY <= 30 && leftClick) {
		parent->onLinkClicked(link);
		return true;
	}
	return false;
}

float PCLinkElement::getHeight(float w) const { return 40.0f; }

void PCInputElement::render(sf::RenderTarget* target, float x, float y, float w) {
	sf::RectangleShape box(sf::Vector2f(w, 30));
	box.setPosition(x, y);
	box.setFillColor(sf::Color(30, 30, 40));
	box.setOutlineColor(focused ? sf::Color::Cyan : sf::Color(100, 100, 100));
	box.setOutlineThickness(1);
	target->draw(box);

	std::string display = text;
	if (display.empty() && !focused) {
		sf::Text t(placeholder, *Fonts::OSDFont, 16);
		t.setPosition(x + 10, y + 5);
		t.setFillColor(sf::Color(100, 100, 100));
		target->draw(t);
	} else {
		if (focused && (G->framesPassed / 30) % 2 == 0) display += "|"; // Basic cursor blinking
		sf::Text t(display, *Fonts::OSDFont, 16);
		t.setPosition(x + 10, y + 5);
		t.setFillColor(sf::Color::White);
		target->draw(t);
	}
}

bool PCInputElement::handleInput(float relX, float relY, float w, bool leftClick, PCSite* parent) {
	if (leftClick) {
		focused = (relY >= 0 && relY <= 30);
		return focused;
	}

	if (focused) {
		if (Kb::IsKeyFirstFrame(KB::BackSpace)) {
			if (!text.empty()) text.pop_back();
			return true;
		}
		if (Kb::IsKeyFirstFrame(KB::Return) || Kb::IsKeyFirstFrame(KB::Enter)) {
			parent->onLinkClicked("search:" + text);
			focused = false;
			return true;
		}
		if (!Kb::LastEnteredText.empty()) {
			for (char c : Kb::LastEnteredText) {
				if (c >= 32 && c <= 126) text += c;
			}
			return true;
		}
	}

	return false;
}

float PCInputElement::getHeight(float w) const { return 40.0f; }

// --- PCSite ---

PCSite::PCSite() : scrollResetRequested(false) {}

PCSite::~PCSite() {
	clearContent();
}

void PCSite::addContent(PCSiteElement* element) {
	elements.push_back(element);
}

void PCSite::clearContent() {
	for (auto e : elements) delete e;
	elements.clear();
	scrollResetRequested = true;
}

void PCSite::renderHeader(sf::RenderTarget* target, float x, float y, float w, float h, bool canGoBack) {
	// Site Title
	sf::Text titleTxt(getTitle(), *Fonts::OSDFont, 14);
	titleTxt.setPosition(x + (w / 2.0f) - (titleTxt.getLocalBounds().width / 2.0f), y + 7);
	titleTxt.setFillColor(sf::Color(180, 180, 180));
	target->draw(titleTxt);

	// Back button if we can go back
	if (canGoBack) {
		sf::RectangleShape backBtn(sf::Vector2f(60, 24));
		backBtn.setPosition(x + 5, y + 3);
		backBtn.setFillColor(sf::Color(100, 100, 120));
		target->draw(backBtn);

		sf::Text backTxt("< Back", *Fonts::OSDFont, 14);
		backTxt.setPosition(x + 10, y + 7);
		backTxt.setFillColor(sf::Color::White);
		target->draw(backTxt);
	}
}

void PCSite::renderContent(sf::RenderTarget* target, float x, float y, float w, float h) {
	float curY = y;
	for (auto e : elements) {
		e->render(target, x + 10, curY, w - 20);
		curY += e->getHeight(w - 20);
	}
}

bool PCSite::handleHeaderInput(float relX, float relY, bool leftClick) {
	if (leftClick) {
		// relX/relY are relative to header area
		if (relX >= 5 && relX <= 65 && relY >= 3 && relY <= 27) {
			return true; // signal back click
		}
	}
	return false;
}

void PCSite::handleContentInput(float relX, float relY, float w, bool leftClick) {
	float curY = 0;
	bool textTyped = !Kb::LastEnteredText.empty() || Kb::IsKeyFirstFrame(KB::BackSpace) || Kb::IsKeyFirstFrame(KB::Return) || Kb::IsKeyFirstFrame(KB::Enter);

	for (auto e : elements) {
		float h = e->getHeight(w - 20);
		// Call handleInput if mouse is over OR if we have keyboard input (let the element decide if it's focused)
		if ((relY >= curY && relY <= curY + h) || textTyped) {
			if (e->handleInput(relX - 10, relY - curY, w - 20, leftClick, this)) {
				// We don't necessarily return here for text input because we want to allow other elements? 
				// Actually, usually only one thing is focused.
				if (leftClick || textTyped) return;
			}
		}
		curY += h;
	}
}

float PCSite::getContentHeight(float w) const {
	float h = 0;
	for (auto e : elements) h += e->getHeight(w - 20);
	return h;
}

void PCSite::navigateTo(std::string link) {
	navRequest = link;
}

// --- Specific Sites ---

class SiteIndex : public PCSite {
public:
	SiteIndex() {
		refresh();
	}

	void refresh() {
		// Only refresh if empty to avoid infinite loops or clearing nested state
		if (elements.size() > 2) return;

		clearContent();
		addContent(new PCTextElement("Site Index", 24));
		addContent(new PCTextElement("Explore the local network:", 16, sf::Color(180, 180, 180)));
		
		auto titles = PCSiteRegistry::GetAvailableSiteTitles();
		for (const auto& title : titles) {
			addContent(new PCLinkElement(title, "site:" + title));
		}
	}

	virtual void onLinkClicked(std::string link) override {
		if (link.find("site:") == 0) {
			navigateTo(link);
		} else if (link == "index" || link == "root") {
			refresh();
		}
	}

	virtual std::string getTitle() const override { return "Index"; }
};

class NewsSite : public PCSite {
public:
	NewsSite() {
		addContent(new PCTextElement("Global News", 24, sf::Color::Yellow));
		addContent(new PCTextElement("Breaking: AI builds OS", 18));
		addContent(new PCTextElement("Local developer finally gets off his dumb lazy ass and adds scrolling.", 14, sf::Color(200, 200, 200)));
		addContent(new PCLinkElement("Read More", "article_1"));
		addContent(new PCLinkElement("Weather Report", "weather"));
	}

	virtual void onLinkClicked(std::string link) override {
		if (link == "root") {
			clearContent();
			addContent(new PCTextElement("Global News", 24, sf::Color::Yellow));
			addContent(new PCTextElement("Breaking: AI builds OS", 18));
			addContent(new PCTextElement("Local developer finally adds scrolling.", 14, sf::Color(200, 200, 200)));
			addContent(new PCLinkElement("Read More", "article_1"));
			addContent(new PCLinkElement("Weather Report", "weather"));
			return;
		}

		clearContent();
		if (link == "article_1") {
			addContent(new PCTextElement("AI Build OS Story", 20));
			auto rich = new PCRichTextElement(16);
			rich->addSpan("In a surprising turn of events, the AI has successfully ");
			rich->addSpan("implemented a taskbar ", sf::Color::Cyan, "taskbar_help");
			rich->addSpan("and a ");
			rich->addSpan("site system ", sf::Color::Cyan, "site_system_info");
			rich->addSpan("in Python-less C++.");
			addContent(rich);

			auto sub = new PCRichTextElement(14);
			sub->addSpan("Special thanks to the ");
			sub->addSpan("Developer", sf::Color::Magenta, "dev_profile");
			sub->addSpan(" for getting off his ass.");
			addContent(sub);
		} else if (link == "taskbar_help") {
			addContent(new PCTextElement("Taskbar Info", 20));
			addContent(new PCTextElement("The taskbar helps you switch between open windows.", 14));
		} else if (link == "site_system_info") {
			addContent(new PCTextElement("Site System Info", 20));
			addContent(new PCTextElement("This is the modular OOP GUI system you are currently using!", 14));
		} else if (link == "dev_profile") {
			addContent(new PCTextElement("Developer Profile", 20));
			addContent(new PCTextElement("Status: No longer lazy.", 14, sf::Color::Green));
		} else if (link == "weather") {
			addContent(new PCTextElement("Weather: Static-y", 20));
			addContent(new PCTextElement("Expect heavy TV static transitions in some areas.", 14));
		}
	}

	virtual std::string getTitle() const override { return "News"; }
};

class MyComputerSite : public PCSite {
public:
	MyComputerSite() {
		addContent(new PCTextElement("My Computer", 24));
		addContent(new PCTextElement("Local Disk (C:)", 18, sf::Color::Green));
		addContent(new PCLinkElement("System", "dir:system"));
		addContent(new PCLinkElement("Users", "dir:users"));
		addContent(new PCLinkElement("Games", "dir:games"));
	}

	virtual void onLinkClicked(std::string link) override {
		if (link == "root") {
			clearContent();
			addContent(new PCTextElement("My Computer", 24));
			addContent(new PCTextElement("Local Disk (C:)", 18, sf::Color::Green));
			addContent(new PCLinkElement("System", "dir:system"));
			addContent(new PCLinkElement("Users", "dir:users"));
			addContent(new PCLinkElement("Games", "dir:games"));
			return;
		}
		clearContent();
		addContent(new PCTextElement("Directory: " + link, 20));
		addContent(new PCTextElement("This folder is empty.", 14, sf::Color(150, 150, 150)));
	}

	virtual std::string getTitle() const override { return "My Computer"; }
};

class SearchSite : public PCSite {
	PCInputElement* searchBox;
public:
	SearchSite() {
		searchBox = new PCInputElement("Search for sites...");
		addContent(new PCTextElement("In-Game Search Engine", 24, sf::Color::Cyan));
		addContent(new PCTextElement("Find local websites and services:", 14, sf::Color(180, 180, 180)));
		addContent(searchBox);
	}

	virtual void onLinkClicked(std::string link) override {
		if (link == "root") {
			clearContent();
			addContent(new PCTextElement("In-Game Search Engine", 24, sf::Color::Cyan));
			addContent(new PCTextElement("Find local websites and services:", 14, sf::Color(180, 180, 180)));
			searchBox = new PCInputElement("Search for sites...");
			addContent(searchBox);
			return;
		}

		if (link.find("search:") == 0) {
			std::string query = link.substr(7);
			// Transform to lowercase for case-insensitive search
			std::string lowerQuery = query;
			for (auto& c : lowerQuery) c = tolower(c);

			clearContent();
			addContent(new PCTextElement("Search results for: " + query, 20));

			bool found = false;
			auto titles = PCSiteRegistry::GetAvailableSiteTitles();
			for (const auto& title : titles) {
				std::string lowerTitle = title;
				for (auto& c : lowerTitle) c = tolower(c);

				if (lowerTitle.find(lowerQuery) != std::string::npos) {
					addContent(new PCLinkElement(title, "site:" + title));
					found = true;
				}
			}

			if (!found) {
				addContent(new PCTextElement("No results found.", 14, sf::Color::Red));
			}
			
			// Add a way to go back to search
			addContent(new PCLinkElement("Back to Search", "root"));
		} else if (link.find("site:") == 0) {
			navigateTo(link);
		}
	}

	virtual std::string getTitle() const override { return "Search Engine"; }
};

static std::vector<std::string> g_SiteTitles;

PCSite* PCSiteRegistry::CreateSite(const std::string& title) {
	if (title == "Index" || title == "Site Index") return new SiteIndex();
	if (title == "News") return new NewsSite();
	if (title == "My Computer") return new MyComputerSite();
	if (title == "Forum") return new ForumSite();
	if (title == "Search Engine") return new SearchSite();
	return nullptr;
}

std::vector<std::string> PCSiteRegistry::GetAvailableSiteTitles() {
	return g_SiteTitles;
}

void PCSiteRegistry::Init() {
	if (g_SiteTitles.empty()) {
		g_SiteTitles.push_back("News");
		g_SiteTitles.push_back("My Computer");
		g_SiteTitles.push_back("Forum");
		g_SiteTitles.push_back("Search Engine");
	}
}

}
