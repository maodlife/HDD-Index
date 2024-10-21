#include "treemodel.h"

TreeModel::TreeModel(std::shared_ptr<TreeNode> ptr) { this->_rootPtr = ptr; }

TreeModel::~TreeModel() { this->_rootPtr = nullptr; }

int TreeModel::rowCount(const QModelIndex &parent) const {
    if (!parent.isValid()) {
        return _rootPtr->childs.size();
    }
    TreeNode *ptr = static_cast<TreeNode *>(parent.internalPointer());
    return ptr->childs.size();
}

int TreeModel::columnCount(const QModelIndex &parent) const { return 1; }

QModelIndex TreeModel::parent(const QModelIndex &index) const {
    TreeNode *child = static_cast<TreeNode *>(index.internalPointer());
    auto parent = child->parent;
    auto sp = parent.lock();
    if (sp == this->_rootPtr) {
        return QModelIndex();
    } else {
        auto par_par_ptr = sp->parent.lock();
        auto found = std::find(par_par_ptr->childs.begin(),
                               par_par_ptr->childs.end(), par_par_ptr);
        return createIndex(std::distance(par_par_ptr->childs.begin(), found), 0,
                           sp.get());
    }
}

QModelIndex TreeModel::index(int row, int column,
                             const QModelIndex &parent) const {
    TreeNode *ptr;
    if (!parent.isValid())
        ptr = this->_rootPtr.get();
    else {
        ptr = static_cast<TreeNode *>(parent.internalPointer());
    }
    auto child = ptr->childs[row];
    return createIndex(row, column, child.get());
}

QVariant TreeModel::data(const QModelIndex &index, int role) const {
    if (!index.isValid() || role != Qt::DisplayRole) {
        return QVariant();
    }
    TreeNode *ptr = static_cast<TreeNode *>(index.internalPointer());
    return ptr->name;
}
