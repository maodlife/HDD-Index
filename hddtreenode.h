#ifndef HDDTREENODE_H
#define HDDTREENODE_H

#include "treenode.h"

struct DeclareSaveData {
    QString path;
    std::weak_ptr<TreeNode> lazyNodePtr;
};

class HddTreeNode;

using HddTreeNodeCompareRet =
    std::tuple<std::vector<std::shared_ptr<HddTreeNode>>,
               std::vector<std::shared_ptr<HddTreeNode>>>;

class HddTreeNode : public TreeNode {
public:
    DeclareSaveData saveData;

    HddTreeNode();

    QJsonObject toJsonObject() const override;

    void fromJsonObjectExtend(const QJsonObject &json) override;

    // 读文件夹路径, 并访问磁盘目录, 构造一棵树
    static std::shared_ptr<HddTreeNode> CreateTreeNodeByDirPath(QString path);

    // return:
    // source中的待删除
    // target中的待添加
    static HddTreeNodeCompareRet
    CompareTree(std::shared_ptr<HddTreeNode> source,
                std::shared_ptr<HddTreeNode> target);
};

#endif // HDDTREENODE_H
