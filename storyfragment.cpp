#include "storyfragment.h"

StoryFragment::StoryFragment(QObject *parent) : QObject(parent)
{
    // 默认故事内容
    content = "这是一个故事碎片，记录着游戏中的某个情节...";
}

void StoryFragment::setContent(const QString& newContent)
{
    if (content != newContent) {
        content = newContent;
    }
}

QString StoryFragment::getContent() const
{
    return content;
}

void StoryFragment::setImagePath(const QString& path) {
    imagePath = path;
}
QString StoryFragment::getImagePath() const {
    return imagePath;
}
