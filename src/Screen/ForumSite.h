#pragma once
#include "Screen/PCSite.h"
#include "Game/Fonts.h"
#include <SFML/Graphics/Color.hpp>

namespace nyaa {

class ForumSite : public PCSite {
public:
    ForumSite();
    virtual ~ForumSite();

    virtual void onLinkClicked(std::string link) override;
    virtual std::string getTitle() const override;

private:
    void showForumHome();
    void showThread(const std::string& threadId);
    void showNewPostForm();
    void showPostSuccess();
    
    struct ForumReply {
        std::string author;
        std::string content;
        std::string timestamp;
    };

    struct ForumThread {
        std::string id;
        std::string title;
        std::string author;
        std::string content;
        std::vector<ForumReply> replies;
        std::string timestamp;
    };
    
    static std::vector<ForumThread> threads;
    static void generateSampleThreads();
};

}