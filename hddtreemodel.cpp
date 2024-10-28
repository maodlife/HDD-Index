#include "hddtreemodel.h"

HddTreeModel::HddTreeModel(std::shared_ptr<HddTreeNode> ptr) : TreeModel(ptr) {}

QVariant HddTreeModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || role != Qt::DisplayRole) {
        return QVariant();
    }
    HddTreeNode *ptr = static_cast<HddTreeNode *>(index.internalPointer());
    if (ptr->saveData.path.isEmpty())
        return ptr->name;
    else
        return "*" + ptr->name;
}

void HddTreeModel::Declare(const QModelIndex &index, QString path)
{
    auto ptr = GetSharedPtr(index);
    auto hddTreeNodePtr = std::dynamic_pointer_cast<HddTreeNode>(ptr);
    this->dataChanged(index, index, {Qt::DisplayRole});
    hddTreeNodePtr->saveData.path = path;
}
