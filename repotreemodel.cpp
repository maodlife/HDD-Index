#include "repotreemodel.h"
#include <algorithm>

RepoTreeModel::RepoTreeModel(std::shared_ptr<RepoTreeNode> ptr)
    : TreeModel(ptr)
{
    _repoRootPtr = ptr;
}

QVariant RepoTreeModel::data(const QModelIndex &index, int role) const {
    if (!index.isValid() || role != Qt::DisplayRole) {
        return QVariant();
    }
    RepoTreeNode *ptr = static_cast<RepoTreeNode *>(index.internalPointer());
    if (ptr->nodeSaveDatas.empty())
        return ptr->name;
    else
        return "*" + ptr->name;
}

void RepoTreeModel::CreateAndDeclare(const QModelIndex &index, QString hddLabel,
                                     std::shared_ptr<TreeNode> hddNode) {
    // index节点的treenode节点指针
    std::shared_ptr<RepoTreeNode> treeNodePtr = nullptr;
    if (index.isValid() == false) {
        // 取根节点
        treeNodePtr = _repoRootPtr;
    } else {
        auto ptr = TreeNode::get_shared_ptr(
            static_cast<TreeNode *>(index.internalPointer()));
        treeNodePtr = ptr == nullptr ? _repoRootPtr : std::dynamic_pointer_cast<RepoTreeNode>(ptr);
    }
    // 检查当前节点是否是目录
    if (treeNodePtr->isDir == false)
        return;
    // 检查是否存在同名子目录
    if (find_if(treeNodePtr->childs.begin(), treeNodePtr->childs.end(),
                [hddLabel](auto value) { return value->name == hddLabel; }) !=
        treeNodePtr->childs.end()) {
        return;
    }
    // 更优雅的做法应该是递归调用beginInsertRows，但摆烂直接beginResetModel算了
    this->beginResetModel();
    auto copied = RepoTreeNode::CopyHierarchy(treeNodePtr, hddNode);
    // declare
    NodeSaveData saveData;
    saveData.hddLabel = hddLabel;
    saveData.treePath = hddNode->getPath();
    copied->nodeSaveDatas.push_back(saveData);
    this->endResetModel();
}
