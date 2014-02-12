#ifndef __ZLQTTREEMODEL_H__
#define __ZLQTTREEMODEL_H__

#include <QtCore/QMap>

#include <QtCore/QAbstractListModel>
#include <QNetworkAccessManager>
#include <QDialog>

#include <ZLTreeListener.h>

#include "../view/ImageUtils.h"
#include "../view/ImageProvider.h"

class ZLQtTreeModel : public QAbstractListModel {
	Q_OBJECT

public:
        enum TreeRoles {
                SubTitleRole = Qt::UserRole,
                ActivatableRole,
                PageRole
        };

public:
        explicit ZLQtTreeModel(ZLTreeListener::RootNode& rootNode, QDialog* treeDialog,
                               shared_ptr<ZLExecutionData::Listener> listener, QObject *parent = 0);

public:
	int rowCount(const QModelIndex &parent = QModelIndex()) const;
	QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
        const ZLTreeNode* getTreeNode(const QModelIndex& index) const;

public:
	void onNodeBeginInsert(ZLTreeNode *parent, size_t index);
	void onNodeEndInsert();
	void onNodeBeginRemove(ZLTreeNode *parent, size_t index);
	void onNodeEndRemove();
	void onNodeUpdated(ZLTreeNode *node);

signals:

public slots:
	bool back();
	bool enter(QModelIndex index);
        void update();

private:
	ZLTreeListener::RootNode& myRootNode;
        ImageProvider myImageProvider;
	ZLTreeNode* myCurrentNode;
        //TODO remove two-sided pointing (model should not know about ui element)
        QDialog* myTreeDialog;

        shared_ptr<ZLExecutionData::Listener> myListener;
};

#endif /* __ZLQTTREEMODEL_H__ */
