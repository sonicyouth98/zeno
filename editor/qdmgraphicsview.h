#ifndef QDMGRAPHICSVIEW_H
#define QDMGRAPHICSVIEW_H

#include <zeno/common.h>
#include <QGraphicsView>
#include <QWidget>
#include <QPointF>

ZENO_NAMESPACE_BEGIN

class QDMGraphicsView : public QGraphicsView
{
    Q_OBJECT

    QPointF m_lastMousePos;
    bool m_mouseDragging{false};

public:
    explicit QDMGraphicsView(QWidget *parent = nullptr);

    virtual void mousePressEvent(QMouseEvent *event) override;
    virtual void mouseMoveEvent(QMouseEvent *event) override;
    virtual void mouseReleaseEvent(QMouseEvent *event) override;
    virtual void wheelEvent(QWheelEvent *event) override;
    virtual QSize sizeHint() const override;

    static constexpr float ZOOMFACTOR = 1.25f;

public slots:
    void addNodeByName(QString name);
};

ZENO_NAMESPACE_END

#endif // QDMGRAPHICSVIEW_H
