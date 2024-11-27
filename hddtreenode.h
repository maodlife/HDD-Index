#ifndef HDDTREENODE_H
#define HDDTREENODE_H

#include "treenode.h"

struct DeclareSaveData {
    QString path;
    std::weak_ptr<TreeNode> lazyNodePtr;
};

class HddTreeNode : public TreeNode
{
public:
    QString dirPath; // 绝对路径，用于关联到实际磁盘
    DeclareSaveData saveData;

    HddTreeNode();

    QJsonObject toJsonObject() const override;

    void fromJsonObjectExtend(const QJsonObject &json) override;

    // 读文件夹路径, 并访问磁盘目录, 构造一棵树
    static std::shared_ptr<HddTreeNode> CreateTreeNodeByDirPath(QString path);
};

#endif // HDDTREENODE_H
