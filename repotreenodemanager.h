#ifndef REPOTREENODEMANAGER_H
#define REPOTREENODEMANAGER_H

#include "repotreemodel.h"
#include "repotreenode.h"
#include <memory>

// 对repo tree node进行操作的上层接口，
// 即在tree view中显示的时候用model操作，
// 没有显示的时候用TreeNode操作

class RepoTreeNodeManager {
public:
    std::shared_ptr<RepoTreeNode> rootPtr;
    std::shared_ptr<RepoTreeModel> model;
};

#endif
