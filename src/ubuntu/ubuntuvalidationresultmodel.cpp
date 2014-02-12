#include "ubuntuvalidationresultmodel.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QRegularExpression>
#include <QRegularExpressionMatchIterator>
#include <QDebug>
#include <QStringList>
#include <QVariant>
#include <QIcon>


namespace Ubuntu {
namespace Internal {

UbuntuValidationResultModel::UbuntuValidationResultModel(QObject *parent)
    : QAbstractItemModel(parent)
    , m_rootItem(new ClickRunChecksParser::DataItem())
    , m_nextSectionOffset(0)
{
    m_rootItem->type = QLatin1String("RootItem");
    QHash<int,QByteArray> roleNames;
    roleNames[TypeRole] = "TypeRole";

    setRoleNames(roleNames);
}

UbuntuValidationResultModel::~UbuntuValidationResultModel()
{
    delete m_rootItem;
}

void UbuntuValidationResultModel::clear()
{
    beginRemoveRows(QModelIndex(),0,m_rootItem->children.size()-1);
    delete m_rootItem;
    m_rootItem = new ClickRunChecksParser::DataItem();
    m_rootItem->type = QLatin1String("RootItem");
    endRemoveRows();
}

QModelIndex UbuntuValidationResultModel::index(int row, int column, const QModelIndex &parent) const
{
    ClickRunChecksParser::DataItem* item = getItem(parent);
    if(!item) {
        return QModelIndex();
    }

    if(row < 0 || row > item->children.length()-1 || column != 0){
        return QModelIndex();
    }

    ClickRunChecksParser::DataItem* indexItem = item->children[row];
    return createIndex(row,column,indexItem);
}

QModelIndex UbuntuValidationResultModel::parent(const QModelIndex &child) const
{
    if(!child.isValid())
        return QModelIndex();

    ClickRunChecksParser::DataItem* item = getItem(child);
    ClickRunChecksParser::DataItem* parentItem = item->parent;

    if(!parentItem || parentItem == m_rootItem)
        return QModelIndex();

    ClickRunChecksParser::DataItem* ppItem = parentItem->parent;
    if(!ppItem)
        return QModelIndex();

    int row = ppItem->children.indexOf(parentItem);
    Q_ASSERT_X(row >= 0,Q_FUNC_INFO,"Child must be a element of parent list");

    return createIndex(row,0,parentItem);
}

int UbuntuValidationResultModel::rowCount(const QModelIndex &parent) const
{
    int rC = 1;
    ClickRunChecksParser::DataItem* item = getItem(parent);
    if(item)
        rC = item->children.size();

    return rC;
}

int UbuntuValidationResultModel::columnCount(const QModelIndex &parent) const
{
    return 1;
}

QVariant UbuntuValidationResultModel::data(const QModelIndex &index, int role) const
{
    if(!index.isValid())
        return QVariant();

    ClickRunChecksParser::DataItem* item = getItem(index);
    if(item == m_rootItem)
        return QVariant();

    switch(role) {
    case Qt::DisplayRole:
    case Qt::EditRole: {
        QString text = (item->type+QString::fromLatin1(": ")+item->text);
        return text;
        break;
    }
    case TypeRole: {
        return item->type;
        break;
    }
    case Qt::DecorationRole:
        switch(item->icon){
        case ClickRunChecksParser::Warning:
        return QIcon(QLatin1String(":/projectexplorer/images/compile_warning.png"));
        case ClickRunChecksParser::Error:
        return QIcon(QLatin1String(":/projectexplorer/images/compile_error.png"));
        case ClickRunChecksParser::Check:
        return QIcon(QLatin1String(":/projectexplorer/images/run.png"));
        default:
            break;
        }
        break;
    }

    return QVariant();
}

Qt::ItemFlags UbuntuValidationResultModel::flags(const QModelIndex &index) const
{
    return QAbstractItemModel::flags(index);
}

void UbuntuValidationResultModel::appendItem(ClickRunChecksParser::DataItem *item)
{
    beginInsertRows(QModelIndex(),m_rootItem->children.length(),m_rootItem->children.length());
    item->parent = m_rootItem;
    m_rootItem->children.append(item);
    endInsertRows();
}

ClickRunChecksParser::DataItem *UbuntuValidationResultModel::getItem(const QModelIndex &index) const
{
    if(index.isValid()){
        ClickRunChecksParser::DataItem* elem = static_cast<ClickRunChecksParser::DataItem*>(index.internalPointer());
        if(elem) {
            return elem;
        }
    }
    return m_rootItem;
}


ClickRunChecksParser::DataItem::DataItem()
    : parent(0)
    , icon(ClickRunChecksParser::NoIcon)
{

}

ClickRunChecksParser::DataItem::~DataItem()
{
    qDeleteAll(children.begin(),children.end());
    children.clear();
}


ClickRunChecksParser::ClickRunChecksParser(QObject *parent)
    : QObject(parent)
    , m_nextSectionOffset(0)
    , m_errorCount(0)
    , m_warnCount(0)
{

}

void ClickRunChecksParser::beginRecieveData(const QString &data)
{
    m_data.clear();
    m_nextSectionOffset = m_errorCount = m_warnCount = 0;
    m_data.append(data);

    bool canContinue = true;
    while(canContinue) canContinue=tryParseNextSection();
}

void ClickRunChecksParser::addRecievedData(const QString &data)
{
    m_data.append(data);

    bool canContinue = true;
    while(canContinue) canContinue=tryParseNextSection();
}

void ClickRunChecksParser::endRecieveData(const QString &data)
{
    m_data.append(data);

    bool canContinue = true;
    while(canContinue) canContinue=tryParseNextSection(true);
}

/**
 * @brief UbuntuValidationResultModel::tryParseNextSection
 * Tries to parse the next output section from the dataString
 *
 * @return false if there is no next section
 */
bool ClickRunChecksParser::tryParseNextSection(bool dataComplete)
{
    QRegularExpression re(QLatin1String("^([=]+[\\s]+[A-Za-z0-9\\-_]+[\\s]+[=]+)$")); // match section beginnings
    re.setPatternOptions(QRegularExpression::MultilineOption);

    QRegularExpressionMatchIterator matchIter = re.globalMatch(m_data,m_nextSectionOffset);
    if(!matchIter.hasNext())
        return false;

    QRegularExpressionMatch match = matchIter.next();
    int startOffset = match.capturedStart(1) + match.capturedLength(1);

    int endOffset   = -1;
    if(matchIter.hasNext()) {
        QRegularExpressionMatch match = matchIter.next();
        endOffset = match.capturedStart();
    }

    if(endOffset < 0 && dataComplete)
        endOffset = m_data.length()-1;

    if(startOffset == -1 || endOffset == -1)
        return false;

    //set the offset for the next parse
    m_nextSectionOffset = endOffset;

    QString type = match.captured(1);
    type.remove(QLatin1String("="));
    type = type.trimmed();

    qDebug()<<"Found section type: "<<type;

    if(type == QLatin1String("click-check-lint")
            || type == QLatin1String("click-check-desktop")
            || type == QLatin1String("click-check-security")
            || type == QLatin1String("click-check-functional")) {
        parseJsonSection(type,startOffset,endOffset-startOffset);
    } else {
        //ignore unknown sections
        return true;
    }

    return true;
}

void ClickRunChecksParser::parseJsonSection(const QString &sectionName, int offset, int length)
{
    QJsonParseError error;

    QString sectionData = m_data.mid(offset,length);
    sectionData = sectionData.trimmed();

    if(sectionData.startsWith(QLatin1String("E")))

    qDebug()<<"Parsing JSON: "<<sectionData;
    QJsonDocument doc = QJsonDocument::fromJson(sectionData.toUtf8(),&error);

    if(error.error != QJsonParseError::NoError) {
        emitParseErrorItem(QString::fromLocal8Bit("Json Parse Error: %0").arg(error.errorString()));
        return;
    }

    if(!doc.isObject()) {
        emitParseErrorItem(QString::fromLocal8Bit("Json unexpected format"));
        return;
    }

    bool hasErrors = false;
    bool hasWarnings = false;
    DataItem* item = new DataItem();
    item->text = sectionName;
    item->type = QLatin1String("ResultData");
    item->icon = Check; // we are optimisic

    QJsonObject obj = doc.object();

    QJsonValue errValue = obj.value(QLatin1String("error"));
    QJsonValue warnValue = obj.value(QLatin1String("warn"));
    QJsonValue infoValue = obj.value(QLatin1String("info"));

    if(errValue.isObject()) {
        QJsonObject errors = errValue.toObject();
        QStringList keys = errors.keys();
        hasErrors = (errors.keys().size() > 0);
        foreach(const QString &key, keys) {
            QJsonValue messageValue = errors.value(key);
            if(messageValue.isObject()){
                QJsonObject messageObject = messageValue.toObject();

                QString text = messageObject.value(QLatin1String("text")).toString();
                DataItem* subItem = new DataItem();
                subItem->parent = item;
                subItem->text = text;
                subItem->type = key;
                subItem->icon = Error;
                item->children.append(subItem);
            }
        }
    }

    if(warnValue.isObject()) {
        QJsonObject warnings = warnValue.toObject();
        QStringList keys = warnings.keys();
        hasWarnings = (warnings.keys().size() > 0);
        foreach(const QString &key, keys) {
            QJsonValue messageValue = warnings.value(key);
            if(messageValue.isObject()){
                QJsonObject messageObject = messageValue.toObject();

                QString text = messageObject.value(QLatin1String("text")).toString();
                DataItem* subItem = new DataItem();
                subItem->parent = item;
                subItem->text = text;
                subItem->type = key;
                subItem->icon = Warning;
                item->children.append(subItem);
            }
        }
    }

    if(infoValue.isObject()) {
        QJsonObject infos = infoValue.toObject();
        QStringList keys = infos.keys();
        foreach(const QString &key, keys) {
            QJsonValue messageValue = infos.value(key);
            if(messageValue.isObject()){
                QJsonObject messageObject = messageValue.toObject();

                QString text = messageObject.value(QLatin1String("text")).toString();
                DataItem* subItem = new DataItem();
                subItem->parent = item;
                subItem->type = key;
                subItem->text = text;
                subItem->icon = Check;
                item->children.append(subItem);
            }
        }
    }

    if(hasErrors)
        item->icon = Error;
    else if(hasWarnings)
        item->icon = Warning;

    emit parsedNewTopLevelItem(item);
}

void ClickRunChecksParser::emitParseErrorItem(const QString &text)
{
    qDebug()<<"Appending error "<<text;
    DataItem *elem = new DataItem();
    elem->icon = Error;
    elem->type = QLatin1String("Error");
    elem->text = text;

    emit parsedNewTopLevelItem(elem);
}

void ClickRunChecksParser::emitTextItem(const QString &text, const ItemIcon icon)
{
    qDebug()<<"Appending text "<<text;
    DataItem *elem = new DataItem();
    elem->icon = icon;
    elem->type = QLatin1String("Text");
    elem->text = text;

    emit parsedNewTopLevelItem(elem);
}

} // namespace Internal
} // namespace Ubuntu
