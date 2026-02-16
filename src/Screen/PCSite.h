#pragma once
#include <string>
#include <vector>
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Text.hpp>

namespace sf { class RenderTarget; }

#include "Game/Fonts.h"

namespace nyaa {

class PCSite;

class PCSiteElement {
public:
	virtual ~PCSiteElement() {}
	virtual void render(sf::RenderTarget* target, float x, float y, float w) = 0;
	virtual bool handleInput(float relX, float relY, float w, bool leftClick, PCSite* parent) = 0;
	virtual float getHeight(float w) const = 0;
};

// Element implementations
class PCTextElement : public PCSiteElement {
	std::string text;
	unsigned int size;
	sf::Color color;
	mutable float lastW;
	mutable std::vector<std::string> lines;

	void wrap(float w) const {
		if (w == lastW) return;
		lastW = w;
		lines.clear();

		std::istringstream iss(text);
		std::string word;
		std::string currentLine;
		sf::Text helper("", *Fonts::OSDFont, size);

		while (iss >> word) {
			std::string test = currentLine.empty() ? word : currentLine + " " + word;
			helper.setString(test);
			if (helper.getLocalBounds().width > w) {
				if (!currentLine.empty()) lines.push_back(currentLine);
				currentLine = word;
			} else {
				currentLine = test;
			}
		}
		if (!currentLine.empty()) lines.push_back(currentLine);
	}

public:
	PCTextElement(std::string t, unsigned int s = 16, sf::Color c = sf::Color::White)
		: text(t), size(s), color(c), lastW(-1.0f) {}
	
	virtual void render(sf::RenderTarget* target, float x, float y, float w) override;
	virtual bool handleInput(float relX, float relY, float w, bool leftClick, PCSite* parent) override { return false; }
	virtual float getHeight(float w) const override;
};

struct RichPiece {
	std::string text;
	sf::Color color;
	std::string link;
	sf::FloatRect bounds;
};

struct RichLine {
	std::vector<RichPiece> pieces;
};

class PCRichTextElement : public PCSiteElement {
	struct Span {
		std::string text;
		sf::Color color;
		std::string link;
	};
	std::vector<Span> spans;
	unsigned int size;
	mutable float lastW;
	mutable std::vector<RichLine> lines;

	void wrap(float w) const;

public:
	PCRichTextElement(unsigned int s = 16) : size(s), lastW(-1.0f) {}

	void addSpan(std::string t, sf::Color c = sf::Color::White, std::string l = "");

	virtual void render(sf::RenderTarget* target, float x, float y, float w) override;
	virtual bool handleInput(float relX, float relY, float w, bool leftClick, PCSite* parent) override;
	virtual float getHeight(float w) const override;
};

class PCLinkElement : public PCSiteElement {
	std::string text;
	std::string link;
public:
	PCLinkElement(std::string t, std::string l) : text(t), link(l) {}

	virtual void render(sf::RenderTarget* target, float x, float y, float w) override;
	virtual bool handleInput(float relX, float relY, float w, bool leftClick, PCSite* parent) override;
	virtual float getHeight(float w) const override;
};

class PCInputElement : public PCSiteElement {
	std::string text;
	std::string placeholder;
	bool focused;
public:
	PCInputElement(std::string p = "Search...") : placeholder(p), focused(false) {}

	virtual void render(sf::RenderTarget* target, float x, float y, float w) override;
	virtual bool handleInput(float relX, float relY, float w, bool leftClick, PCSite* parent) override;
	virtual float getHeight(float w) const override;
	
	std::string getText() const { return text; }
	void setText(std::string t) { text = t; }
};

class PCSite {
public:
	PCSite();
	virtual ~PCSite();

	void addContent(PCSiteElement* element);
	void clearContent();

	virtual void renderHeader(sf::RenderTarget* target, float x, float y, float w, float h, bool canGoBack);
	virtual void renderContent(sf::RenderTarget* target, float x, float y, float w, float h);
	
	virtual bool handleHeaderInput(float relX, float relY, bool leftClick);
	virtual void handleContentInput(float relX, float relY, float w, bool leftClick);
	
	float getContentHeight(float w) const;
	virtual std::string getTitle() const = 0;

	virtual void onLinkClicked(std::string link) = 0;
	void navigateTo(std::string link);

	std::string navRequest;
	bool scrollResetRequested;

protected:
	std::vector<PCSiteElement*> elements;
};

class PCSiteRegistry {
public:
	static PCSite* CreateSite(const std::string& title);
	static std::vector<std::string> GetAvailableSiteTitles();
	static void Init();
};

}
