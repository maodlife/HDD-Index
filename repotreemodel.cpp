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
                                     std::shared_ptr<TreeNode> hddNode, bool declare) {
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
                [=](auto value) { return value->name == hddNode->name; }) !=
        treeNodePtr->childs.end()) {
        return nullptr;
    }
    // 更优雅的做法应该是递归调用beginInsertRows，但摆烂直接beginResetModel算了
    this->beginResetModel();
    auto copied = RepoTreeNode::CopyHierarchy(treeNodePtr, hddNode);
    // declare
    if (declare) {
        NodeSaveData saveData;
        saveData.hddLabel = hddLabel;
        saveData.treePath = hddNode->getPath();
        copied->nodeSaveDatas.push_back(saveData);
    }
    this->endResetModel();
    return copied;
}

bool RepoTreeModel::AddDeclare(const QModelIndex &index, QString hddLabel,
                               std::shared_ptr<TreeNode> hddNode) {
    auto ptr = std::dynamic_pointer_cast<RepoTreeNode>(this->GetSharedPtr(index));
    if (find_if(ptr->nodeSaveDatas.begin(), ptr->nodeSaveDatas.end(),
                [=](const auto &value) { return value.hddLabel == hddLabel; }) !=
        ptr->nodeSaveDatas.end()) {
        return false;
    }
    this->beginResetModel();
    NodeSaveData saveData;
    saveData.hddLabel = hddLabel;
    saveData.treePath = hddNode->getPath();
    ptr->nodeSaveDatas.push_back(saveData);
    this->endResetModel();
    return true;
}

bool RepoTreeModel::RemoveDeclare(const QModelIndex &index, QString hddLabel)
{
    auto ptr = std::dynamic_pointer_cast<RepoTreeNode>(this->GetSharedPtr(index));
    auto foundIt = find_if(ptr->nodeSaveDatas.begin(),
                           ptr->nodeSaveDatas.end(),
                           [=](const auto &value) { return value.hddLabel == hddLabel; });
    if (foundIt == ptr->nodeSaveDatas.end())
        return false;
    this->beginResetModel();
    ptr->nodeSaveDatas.erase(foundIt);
    this->endResetModel();
    return true;
}

bool RepoTreeModel::ChangeDeclare(const QModelIndex &index, QString hddLabel,
                                  std::shared_ptr<TreeNode> hddNode) {
    auto ptr =
        std::dynamic_pointer_cast<RepoTreeNode>(this->GetSharedPtr(index));
    auto foundIt =
        find_if(ptr->nodeSaveDatas.begin(), ptr->nodeSaveDatas.end(),
                [=](const auto &value) { return value.hddLabel == hddLabel; });

    if (foundIt == ptr->nodeSaveDatas.end()) {
        return false;
    }

    this->beginResetModel();
    auto &saveData = *foundIt;
    saveData.treePath = hddNode->getPath();
    this->endResetModel();
    return true;
}
