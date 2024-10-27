#include "repotreemodel.h"
#include <algorithm>

RepoTreeModel::RepoTreeModel(std::shared_ptr<RepoTreeNode> ptr)
    : TreeModel(ptr)
{
    _repoRootPtr = ptr;
}

void RepoTreeModel::CreateAndDeclare(const QModelIndex &index, QString hddLabel,
                                     std::shared_ptr<TreeNode> hddNode) {
    // index节点的treenode节点指针
    std::shared_ptr<RepoTreeNode> treeNodePtr = nullptr;
    if (index.isValid() == false) {
        // 取根节点
        treeNodePtr = _repoRootPtr;
    } else {
        treeNodePtr =
            std::dynamic_pointer_cast<RepoTreeNode>(TreeNode::get_shared_ptr(
            static_cast<TreeNode *>(index.internalPointer())));
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
    RepoTreeNode::CopyHierarchy(treeNodePtr, hddNode);
    this->endResetModel();
}
