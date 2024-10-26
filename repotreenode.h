#ifndef REPOTREENODE_H
#define REPOTREENODE_H

#include "treenode.h"

class NodeSaveData {
    QString hddLabel;
    QString treePath; // 包含根节点name的path
    std::weak_ptr<TreeNode> lazyNodePtr;
};

// Repository树节点
class RepoTreeNode : public TreeNode {
public:
    RepoTreeNode();
    std::vector<NodeSaveData> nodeSaveDatas;

    QJsonObject toJsonObject() const override;
    void fromJsonObjectExtend(const QJsonObject &json) override;
};

#endif // REPOTREENODE_H
