#ifndef HDDTREEMODEL_H
#define HDDTREEMODEL_H

#include "treemodel.h"
#include "hddtreenode.h"

class HddTreeModel : public TreeModel {
private:
    std::shared_ptr<HddTreeNode> _hddRootPtr;

public:
    HddTreeModel(std::shared_ptr<HddTreeNode> ptr);
};

#endif // HDDTREEMODEL_H
