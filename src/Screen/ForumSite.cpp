#include "Screen/ForumSite.h"
#include "Game/Fonts.h"
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <sstream>
#include <ctime>

namespace nyaa {

ForumSite::ForumSite() {
    generateSampleThreads();
    showForumHome();
}

ForumSite::~ForumSite() {
    clearContent();
}

std::vector<ForumSite::ForumThread> ForumSite::threads;

void ForumSite::generateSampleThreads() {
    if (threads.empty()) {
        threads = {
            {"1", "Welcome to the Forum!", "Admin", "Welcome everyone to our new forum! Feel free to introduce yourselves and share your thoughts.", 
                {
                    {"User1", "Great to be here!", "2024-01-15 10:30"},
                    {"User2", "nice site layout.", "2024-01-15 11:00"}
                }, "2024-01-15"},
            {"2", "Best places to find gas cans?", "Newbie123", "I'm new to the area and wondering where the best spots are to find gas cans for my car. Any tips?", 
                {
                    {"CarEnthusiast", "For gas cans, try checking behind gas stations and in industrial areas. That's where I usually find them.", "2024-01-16 14:20"},
                    {"Scavenger", "Garages sometimes have them too.", "2024-01-16 15:45"}
                }, "2024-01-16"},
            {"3", "Strange noises at night", "NightWatcher", "Has anyone else been hearing strange noises coming from the industrial area after midnight? Sounds like machinery but not quite...", 
                {
                    {"LocalResident", "I can confirm the industrial area does get noisy at night. Weird stuff goes on there.", "2024-01-17 09:15"},
                    {"MysteryHunter", "The noises could be from underground tunnels. There's a whole network beneath the city.", "2024-01-17 10:30"},
                    {"Skeptic", "Probably just the wind.", "2024-01-17 12:00"}
                }, "2024-01-17"},
            {"4", "DIY Home Security Tips", "SafetyFirst", "Here are some tips I've learned about securing your home:\n1. Always lock doors\n2. Keep windows closed\n3. Consider getting a dog\n4. Don't leave valuables visible", 
                {
                    {"SafetyExpert", "Great tips! I'd also recommend reinforcing windows with plywood if you're really concerned.", "2024-01-18 16:20"},
                    {"DogLover", "Dogs are the best alarm system.", "2024-01-18 17:00"}
                }, "2024-01-18"},
            {"5", "Where to find good food?", "Foodie", "Looking for recommendations on where to find decent food in this town. The convenience store selection is pretty limited.", 
                {
                   {"ChefBoy", "The diner on Main St is okay.", "2024-01-19 19:30"} 
                }, "2024-01-19"}
        };
    }
}

void ForumSite::showForumHome() {
    clearContent();
    
    addContent(new PCTextElement("Community Forum", 24, sf::Color::Cyan));
    addContent(new PCTextElement("Share your thoughts and connect with others!", 16, sf::Color(180, 180, 180)));
    addContent(new PCLinkElement("Create New Thread", "new_thread"));
    
    addContent(new PCTextElement("Recent Discussions:", 18, sf::Color::Yellow));
    
    for (const auto& thread : ForumSite::threads) {
        auto threadLink = new PCLinkElement("📄 " + thread.title, "thread:" + thread.id);
        addContent(threadLink);
        
        auto info = new PCTextElement("  by " + thread.author + " • " + std::to_string(thread.replies.size()) + " replies • " + thread.timestamp,
                                   14, sf::Color(150, 150, 150));
        addContent(info);
    }
}

void ForumSite::showThread(const std::string& threadId) {
    clearContent();
    
    // Find the thread
    const ForumThread* currentThread = nullptr;
    for (const auto& thread : ForumSite::threads) {
        if (thread.id == threadId) {
            currentThread = &thread;
            break;
        }
    }
    
    if (!currentThread) {
        addContent(new PCTextElement("Thread not found", 20, sf::Color::Red));
        addContent(new PCTextElement("Available threads: " + std::to_string(ForumSite::threads.size()), 14, sf::Color::Yellow));
        for (const auto& thread : ForumSite::threads) {
            addContent(new PCTextElement("Thread ID: " + thread.id, 14, sf::Color::Yellow));
        }
        return;
    }
    
    // Thread header
    addContent(new PCTextElement(currentThread->title, 20, sf::Color::Cyan));
    addContent(new PCTextElement("by " + currentThread->author + " • " + currentThread->timestamp, 14, sf::Color(150, 150, 150)));
    
    // Original post
    addContent(new PCTextElement("Original Post:", 16, sf::Color::Yellow));
    auto content = new PCRichTextElement(14);
    std::stringstream ss(currentThread->content);
    std::string line;
    while (std::getline(ss, line)) {
        if (!line.empty()) {
            content->addSpan(line + "\n", sf::Color::White);
        } else {
            content->addSpan("\n", sf::Color::White);
        }
    }
    addContent(content);
    
    // Replies
    addContent(new PCTextElement("Replies (" + std::to_string(currentThread->replies.size()) + "):", 16, sf::Color::Yellow));
    
    int replyIndex = 1;
    for (const auto& reply : currentThread->replies) {
        addContent(new PCTextElement("Reply " + std::to_string(replyIndex++) + " by " + reply.author + " (" + reply.timestamp + "):", 14, sf::Color(180, 180, 180)));
        auto replyContent = new PCRichTextElement(13);
        replyContent->addSpan(reply.content + "\n", sf::Color(200, 200, 200));
        addContent(replyContent);
    }
    
    addContent(new PCLinkElement("Reply to Thread", "reply:" + threadId));
    addContent(new PCLinkElement("Back to Forum", "home"));
}

void ForumSite::showNewPostForm() {
    clearContent();
    
    addContent(new PCTextElement("Create New Thread", 20, sf::Color::Cyan));
    addContent(new PCTextElement("Title:", 16));
    addContent(new PCTextElement("Enter your thread title here...", 14, sf::Color(150, 150, 150)));
    addContent(new PCTextElement("Content:", 16));
    addContent(new PCTextElement("Write your message here...", 14, sf::Color(150, 150, 150)));
    addContent(new PCLinkElement("Post Thread", "post_success"));
    addContent(new PCLinkElement("Cancel", "home"));
}

void ForumSite::showPostSuccess() {
    clearContent();
    
    addContent(new PCTextElement("Thread Posted Successfully!", 20, sf::Color::Green));
    addContent(new PCTextElement("Your thread has been created and is now visible to other users.", 14));
    addContent(new PCLinkElement("View Your Thread", "thread:" + threads.back().id));
    addContent(new PCLinkElement("Back to Forum", "home"));
}

void ForumSite::onLinkClicked(std::string link) {
    if (link.find("site:") == 0) {
        navigateTo(link);
    } else if (link == "home" || link == "root") {
        showForumHome();
    } else if (link == "new_thread") {
        showNewPostForm();
    } else if (link == "post_success") {
        // Create a new thread
        std::string newId = std::to_string(threads.size() + 1);
        threads.push_back({newId, "New User Thread " + newId, "Player", "This is a thread created by the player.", {}, "Just Now"});
        showPostSuccess();
    } else if (link.find("thread:") == 0) {
        std::string threadId = link.substr(7);
        showThread(threadId);
    } else if (link.find("reply:") == 0) {
        std::string threadId = link.substr(6);
        
        // Find thread and add reply
        for (auto& thread : threads) {
            if (thread.id == threadId) {
                thread.replies.push_back({"Player", "This is a reply from the player.", "Just Now"});
                break;
            }
        }
        clearContent();
        addContent(new PCTextElement("Reply Posted!", 20, sf::Color::Green));
        addContent(new PCTextElement("Your reply has been added to the thread.", 14));
        addContent(new PCLinkElement("Back to Thread", "thread:" + threadId));
    }
}

std::string ForumSite::getTitle() const {
    return "Forum";
}

}