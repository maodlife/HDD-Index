#ifndef REPODATA_H
#define REPODATA_H

#include "repotreemodel.h"
#include "repotreenode.h"

struct RepoData {
    std::shared_ptr<RepoTreeNode> rootPtr;
    std::shared_ptr<RepoTreeModel> model;
    bool hasLoaded = false;
    bool isDirty = false;

    void LoadJson(QString path);
    void TryLoadJson(QString path);
};

#endif // REPODATA_H