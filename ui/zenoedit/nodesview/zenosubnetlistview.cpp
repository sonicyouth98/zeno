#include "zenosubnetlistview.h"
#include "zenoapplication.h"
#include "graphsmanagment.h"
#include "style/zenostyle.h"
#include "../model/graphstreemodel.h"
#include "../model/graphsplainmodel.h"
#include "model/graphsmodel.h"
#include "zsubnetlistitemdelegate.h"
#include <comctrl/zlabel.h>
#include <zenoui/model/modelrole.h>
#include <zenoui/include/igraphsmodel.h>
#include <zeno/utils/logger.h>
#include "util/log.h"


ZSubnetListModel::ZSubnetListModel(IGraphsModel* pModel, QObject* parent)
    : QStandardItemModel(parent)
    , m_model(nullptr)
{
    m_model = qobject_cast<GraphsModel*>(pModel);
    Q_ASSERT(m_model);
}

int ZSubnetListModel::rowCount(const QModelIndex& parent) const
{
    return m_model->rowCount(parent) + 1;
}

QVariant ZSubnetListModel::data(const QModelIndex& index, int role) const
{
    if (index.row() == 0)
    {
        if (role == Qt::DisplayRole)
        {
            const QString& filePath = m_model->filePath();
            QFileInfo fi(filePath);
            const QString& fn = fi.fileName();
            return fn;
        }
        else
        {
            zeno::log_error("not display role");
            return QVariant();
        }
    }
    else
    {
        return m_model->data(createIndex(index.row() - 1, index.column(), index.internalId()));
    }
}

QModelIndex ZSubnetListModel::index(int row, int column, const QModelIndex& parent) const
{
    return QStandardItemModel::index(row, column, parent);
}


ZenoSubnetListView::ZenoSubnetListView(QWidget* parent)
    : QListView(parent)
{
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setFrameShape(QFrame::NoFrame);
    setFrameShadow(QFrame::Plain);
}

ZenoSubnetListView::~ZenoSubnetListView()
{
}

void ZenoSubnetListView::initModel(IGraphsModel* pModel)
{
    setModel(pModel);
    setItemDelegate(new ZSubnetListItemDelegate(pModel, this));
    viewport()->setAutoFillBackground(false);
    update();
}

void ZenoSubnetListView::edittingNew()
{
    GraphsModel* pModel = qobject_cast<GraphsModel*>(model());
    ZASSERT_EXIT(pModel);

    SubGraphModel* pSubModel = new SubGraphModel(pModel);
    pModel->appendSubGraph(pSubModel);

    const QModelIndex& idx = pModel->indexBySubModel(pSubModel);
    setCurrentIndex(idx);
    edit(idx);
}

void ZenoSubnetListView::closeEditor(QWidget* editor, QAbstractItemDelegate::EndEditHint hint)
{
    QModelIndex idx = currentIndex();
    QListView::closeEditor(editor, hint);
    
    GraphsModel* pModel = qobject_cast<GraphsModel*>(model());
    ZASSERT_EXIT(pModel);
    switch (hint)
    {
        case QAbstractItemDelegate::RevertModelCache:
        {
            pModel->revert(idx);
            break;
        }
        case QAbstractItemDelegate::SubmitModelCache:
        {
            //activate the tab widget.
            QString subgName = idx.data().toString();
            emit graphToBeActivated(subgName);
            break;
        }
    }
}

QSize ZenoSubnetListView::sizeHint() const
{
    if (model() == nullptr)
        return QListView::sizeHint();

    if (model()->rowCount() == 0)
        return QListView::sizeHint();

    int nToShow = model()->rowCount();
    return QSize(sizeHintForColumn(0), nToShow * sizeHintForRow(0));
}

void ZenoSubnetListView::paintEvent(QPaintEvent* e)
{
    QListView::paintEvent(e);
}
