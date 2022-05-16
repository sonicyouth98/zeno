#ifndef __ZENO_EDITOR_DOCKWIDGET_H__
#define __ZENO_EDITOR_DOCKWIDGET_H__

#include "zenodockwidget.h"

class ZenoDockTitleWidget : public QWidget
{
	Q_OBJECT
public:
	ZenoDockTitleWidget(QWidget* parent = nullptr);
	~ZenoDockTitleWidget();
	QSize sizeHint() const override;
	void updateByType(DOCK_TYPE type);
	virtual void setupUi();

signals:
	void dockOptionsClicked();
	void dockSwitchClicked(DOCK_TYPE);
    void doubleClicked();

protected:
	void paintEvent(QPaintEvent* event) override;
	void mouseDoubleClickEvent(QMouseEvent* event) override;
	virtual void initTitleContent(QHBoxLayout* pHLayout);

private slots:
	void onDockSwitchClicked();
};

class IGraphsModel;

class ZenoEditorDockTitleWidget : public ZenoDockTitleWidget
{
	Q_OBJECT
public:
	ZenoEditorDockTitleWidget(QWidget* parent = nullptr);
	~ZenoEditorDockTitleWidget();
	void initModel();

signals:
	void actionTriggered(QAction* action);

public slots:
	void setTitle(const QString& title);
	void onModelInited(IGraphsModel* pModel);
	void onModelClear();
	void onPathChanged(const QString& newPath);
	void onDirtyChanged();

protected:
	void initTitleContent(QHBoxLayout* pHLayout) override;
	void paintEvent(QPaintEvent* event) override;

private:
	QMenuBar* initMenu();
	QAction* createAction(const QString& text);

	QLabel* m_lblTitle;
};

class ZenoViewDockTitle : public ZenoDockTitleWidget
{
	Q_OBJECT
public:
	ZenoViewDockTitle(QWidget* parent = nullptr);
	~ZenoViewDockTitle();

signals:
	void actionTriggered(QAction* action);

protected:
	void initTitleContent(QHBoxLayout* pHLayout) override;

private:
	QMenuBar* initMenu();
	QAction* createAction(const QString& text);
};

class ZenoPropDockTitleWidget : public ZenoDockTitleWidget
{
	Q_OBJECT
public:
	ZenoPropDockTitleWidget(QWidget* parent = nullptr);
	~ZenoPropDockTitleWidget();

public slots:
	void setTitle(const QString& title);

protected:
	void paintEvent(QPaintEvent* event) override;

private:
	QString m_title;
};

#endif