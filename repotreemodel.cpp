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

std::shared_ptr<TreeNode> RepoTreeModel::CreateAndDeclare(const QModelIndex &index, QString hddLabel,
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
        return nullptr;
    // 检查是否存在同名子目录
    if (find_if(treeNodePtr->childs.begin(), treeNodePtr->childs.end(),
                [hddLabel](auto value) { return value->name == hddLabel; }) !=
        treeNodePtr->childs.end()) {
        return nullptr;
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
    return copied;
}

void RepoTreeModel::AddDeclare(const QModelIndex &index, QString hddLabel,
                               std::shared_ptr<TreeNode> hddNode) {
    auto ptr = std::dynamic_pointer_cast<RepoTreeNode>(this->GetSharedPtr(index));
    if (find_if(ptr->nodeSaveDatas.begin(), ptr->nodeSaveDatas.end(),
                [=](const auto &value) { return value.hddLabel == hddLabel; }) !=
        ptr->nodeSaveDatas.end()) {
        return;
    }
    this->beginResetModel();
    NodeSaveData saveData;
    saveData.hddLabel = hddLabel;
    saveData.treePath = hddNode->getPath();
    ptr->nodeSaveDatas.push_back(saveData);
    this->endResetModel();
    return;
}

void RepoTreeModel::CutRepoNode(
    std::shared_ptr<RepoTreeNode> repoNode,
    std::shared_ptr<RepoTreeNode> targetParentNode) {
    // 如果已有同名文件夹，则跳过
    if (find_if(targetParentNode->childs.begin(), targetParentNode->childs.end(),
                [=](const auto &value) {
                    return value->name == repoNode->name;
        }) != targetParentNode->childs.end()) {
        return;
    }
    this->beginResetModel();
    // 原本的父节点里删掉
    auto parent = repoNode->parent.lock();
    parent->childs.erase(find_if(
        parent->childs.begin(), parent->childs.end(),
        [=](const auto &value) { return value->name == repoNode->name; }));
    // 修改parent指针
    repoNode->parent = targetParentNode;
    // 新的父节点里增加
    targetParentNode->childs.push_back(repoNode);
    targetParentNode->sortChildByName();
    this->endResetModel();
}
