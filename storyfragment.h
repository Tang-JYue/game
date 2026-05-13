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

    // 设置和获取是否已阅读
    void setRead(bool read);
    bool isRead() const;

    void setImagePath(const QString& path);
    QString getImagePath() const;

    // 重置
    void reset();

signals:
    void fragmentRead(bool read);

private:
    QString content;
    bool read = false;
    QString imagePath;
};

#endif // STORYFRAGMENT_H
