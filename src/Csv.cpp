#include "Csv.h"

#include <QDebug>

Csv::Csv(QString delimiter, bool includeHeader, DataFormatter dataFormatter) :
    m_delimiter(delimiter),
    m_includeHeader(includeHeader),
    m_dataFormatter(dataFormatter)
{

}

void Csv::write(QTextStream *stream, QAbstractItemModel *model)
{
    //header
    if (m_includeHeader)
    {
        QStringList header;
        for (int col = 0; col < model->columnCount(); ++col)
        {
            header.append(m_dataFormatter.format(model->headerData(col, Qt::Horizontal)));
        }
        (*stream) << header.join(m_delimiter) << endl;
    }

    //data
    for (int row = 0; row < model->rowCount(); ++row)
    {
        QStringList rowData;
        for (int col = 0; col < model->columnCount(); ++col)
        {
            rowData.append(m_dataFormatter.format(model->index(row, col).data()));
        }
        (*stream) << rowData.join(m_delimiter) << endl;
    }
}

void Csv::write(QTextStream *stream, QSqlQuery *query, bool* stopFlag)
{
    //header
    if (m_includeHeader)
    {
        QSqlRecord record = query->record();
        QStringList header;
        for (int col=0; col < record.count(); ++col)
        {
            header.append(record.field(col).name().toUpper());
        }
        (*stream) << header.join(m_delimiter) << endl;
    }

    //data
    while (!*stopFlag && query->next())
    {
        QSqlRecord record = query->record();
        QStringList rowData;
        for (int col=0; col < record.count(); ++col)
        {
            rowData.append(m_dataFormatter.format(record.field(col).value()));
        }
        (*stream) << rowData.join(m_delimiter) << endl;
    }
}

QString Csv::writeSelectionToString(QAbstractItemModel *model, const QItemSelection &selection, bool includeHeader)
{
    QString text;

    QItemSelectionRange range = selection.isEmpty() ? QItemSelectionRange() : selection.first(); //TODO support multiple selections?

    if (includeHeader)
    {
        int left = range.left();
        int right = range.right();

        if (selection.isEmpty())
        {
            left = 0;
            right = model->columnCount() - 1;
        }

        QStringList rowContents;
        for (auto col = left; col <= right; ++col)
            rowContents << m_dataFormatter.format(model->headerData(col, Qt::Horizontal).toString());
        text += rowContents.join(m_delimiter);
        text += "\n";
    }

    if (!selection.isEmpty())
    {
        for (auto row = range.top(); row <= range.bottom(); ++row)
        {
            QStringList rowContents;
            for (auto col = range.left(); col <= range.right(); ++col)
            {
                rowContents << m_dataFormatter.format(model->index(row, col).data());
            }
            text += rowContents.join(m_delimiter);
            text += "\n";
        }
    }

    return text;
}

QString Csv::writeSelectionToString(QAbstractItemModel *model, int sampleSize)
{
    QString text;

    if (m_includeHeader)
    {
        QStringList header;
        for (int col=0; col < model->columnCount(); ++col)
        {
            header.append(m_dataFormatter.format(model->headerData(col, Qt::Horizontal).toString()));
        }
        text += header.join(m_delimiter) + "\n";
    }

    for (int row=0; row < model->rowCount() && row < sampleSize; ++row)
    {
        QStringList rowContents;
        for (int col=0; col < model->columnCount(); ++col)
        {
            rowContents.append(m_dataFormatter.format(model->index(row, col).data()));
        }
        text += rowContents.join(m_delimiter) + "\n";
    }

    return text;
}
