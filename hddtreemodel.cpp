#include "hddtreemodel.h"
#include "hddtreenode.h"
#include <QBrush>
#include <memory>

HddTreeModel::HddTreeModel(std::shared_ptr<HddTreeNode> ptr) : TreeModel(ptr) {}

QVariant HddTreeModel::data(const QModelIndex &index, int role) const {
    if (!index.isValid()) {
        return QVariant();
    }
    HddTreeNode *ptr = static_cast<HddTreeNode *>(index.internalPointer());

    if (role == Qt::DisplayRole) {
        if (ptr->saveData.path.isEmpty())
            return ptr->name;
        else
            return "*" + ptr->name;
    }

    if (role == Qt::ForegroundRole) {
        if (ptr->saveData.path.isEmpty())
            return QBrush(Qt::black);
        else
            return QBrush(Qt::red);
    }

    return QVariant();
}

void HddTreeModel::Declare(const QModelIndex &index, QString path)
{
    auto ptr = GetSharedPtr(index);
    auto hddTreeNodePtr = std::dynamic_pointer_cast<HddTreeNode>(ptr);
    this->dataChanged(index, index, {Qt::DisplayRole});
    hddTreeNodePtr->saveData.path = path;
}

bool HddTreeModel::NoDeclare(const QModelIndex &index) {
    auto ptr = GetSharedPtr(index);
    auto hddTreeNodePtr = std::dynamic_pointer_cast<HddTreeNode>(ptr);
    if (hddTreeNodePtr->saveData.path.isEmpty())
        return false;
    this->beginResetModel();
    hddTreeNodePtr->saveData.path = "";
    hddTreeNodePtr->saveData.lazyNodePtr.reset();
    this->endResetModel();
    return true;
}

void HddTreeModel::ChangeDeclareRepoPath(std::shared_ptr<HddTreeNode> ptr,
                                         QString newRepoPath) {
    this->beginResetModel();
    ptr->saveData.path = newRepoPath;
    ptr->saveData.lazyNodePtr.reset();
    this->endResetModel();
}

std::shared_ptr<HddTreeNode>
HddTreeModel::CreatePtrByPath(std::shared_ptr<HddTreeNode> rootPtr,
                              QString path) {
    this->beginResetModel();
    auto newNode = TreeNode::CreatePtrByPath<HddTreeNode>(rootPtr, path);
    this->endResetModel();
    auto ret = std::dynamic_pointer_cast<HddTreeNode>(newNode);
    return ret;
}
