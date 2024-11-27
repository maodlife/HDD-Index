#ifndef REPOTREENODEMANAGER_H
#define REPOTREENODEMANAGER_H

#include "repotreemodel.h"
#include "repotreenode.h"
#include <memory>

class RepoTreeNodeManager {
public:
    std::shared_ptr<RepoTreeNode> rootPtr;
    std::shared_ptr<RepoTreeModel> model;
};

#endif
