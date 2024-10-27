#ifndef SOLUTION_H
#define SOLUTION_H

#include "hddtreenode.h"
#include "repotreenode.h"
#include "treemodel.h"
#include "repotreemodel.h"
#include <vector>

struct HddData {
    std::shared_ptr<HddTreeNode> rootPtr;
    std::shared_ptr<TreeModel> model;  // todo: 继承下
    QString labelName;
    bool hasLoaded = false;
    bool isDirty = false;
    void LoadJson(QString path);
    void TryLoadJson(QString path);
};

struct RepoData {
    std::shared_ptr<RepoTreeNode> rootPtr;
    std::shared_ptr<RepoTreeModel> model;
    bool hasLoaded = false;
    bool isDirty = false;
    void LoadJson(QString path);
    void TryLoadJson(QString path);
};

class Solution {
public:
    Solution();
    std::vector<HddData> hddDataList;
    RepoData repoData;

private:
};

#endif // SOLUTION_H
