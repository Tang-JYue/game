#include "storyfragment.h"
#include <QDebug>

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

void StoryFragment::setRead(bool isRead)
{
    if (read != isRead) {
        read = isRead;
        emit fragmentRead(read);
    }
}

bool StoryFragment::isRead() const
{
    return read;
}

void StoryFragment::reset()
{
    read = false;
}
