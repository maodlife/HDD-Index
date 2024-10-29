#ifndef REPOTREENODE_H
#define REPOTREENODE_H

#include "hddtreenode.h"
#include "treenode.h"

struct NodeSaveData {
    QString hddLabel; // 哪个HDD
    QString treePath; // 包含根节点name的path
    std::weak_ptr<HddTreeNode> lazyNodePtr;
};

// Repository树节点
class RepoTreeNode : public TreeNode {
public:
    RepoTreeNode();
    std::vector<NodeSaveData> nodeSaveDatas;

    QJsonObject toJsonObject() const override;

    void fromJsonObjectExtend(const QJsonObject &json) override;

    void AddChild(QString name, std::shared_ptr<TreeNode> rootPtr) override;

    static std::shared_ptr<RepoTreeNode>
    CopyHierarchy(std::shared_ptr<RepoTreeNode> repoRootPtr,
                  std::shared_ptr<TreeNode> treeNodePtr);
};

#endif // REPOTREENODE_H
