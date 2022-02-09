#ifndef __UI_HELPER_H__
#define __UI_HELPER_H__

#include "../model/modeldata.h"
#include <rapidjson/document.h>

class UiHelper
{
public:
    static NODE_DESCS parseDescs(const rapidjson::Value &descs);
    static NODE_DESCS loadDescsFromTempFile();
    static QPainterPath getRoundPath(QRectF r, int lt, int rt, int lb, int rb, bool bFixRadius);
    static QString generateUuid(const QString &name = "x");
    static QVariant _parseDefaultValue(const QString& defaultValue);
    static QVariant parseVariantValue(const rapidjson::Value& val);
    static QSizeF viewItemTextLayout(QTextLayout& textLayout, int lineWidth, int maxHeight = -1, int* lastVisibleLine = nullptr);

private:
    static PARAM_CONTROL _getControlType(const QString &type);
    static std::pair<qreal, qreal> getRxx2(QRectF r, qreal xRadius, qreal yRadius, bool AbsoluteSize);
};

#endif