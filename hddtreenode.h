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
    HddTreeNode();

    DeclareSaveData saveData;

    QJsonObject toJsonObject() const override;

    // 读文件夹路径, 并访问磁盘目录, 构造一棵树
    static std::shared_ptr<HddTreeNode> CreateTreeNodeByDirPath(QString path);

    void fromJsonObjectExtend(const QJsonObject &json) override;
};

#endif // HDDTREENODE_H
