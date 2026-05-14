#ifndef STORYFRAGMENT_H
#define STORYFRAGMENT_H

#include <QObject>
#include <QString>

class StoryFragment : public QObject
{
    Q_OBJECT

public:
    explicit StoryFragment(QObject *parent = nullptr);

    // 设置和获取故事内容
    void setContent(const QString& content);
    QString getContent() const;

    void setImagePath(const QString& path);
    QString getImagePath() const;

private:
    QString content;
    QString imagePath;
};

#endif // STORYFRAGMENT_H
