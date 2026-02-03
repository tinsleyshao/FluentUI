#include "FluReportBuilder.h"
#include <QPdfWriter>
#include <QPainter>
#include <QFont>
#include <QDebug>
#include <QDateTime>
#include <QFontMetrics>
#include <QRect>

FluReportBuilder::FluReportBuilder(QObject *parent)
    : QObject(parent), m_pageCount(0)
{
    initializeDefaults();
}

FluReportBuilder::~FluReportBuilder()
{
}

void FluReportBuilder::initializeDefaults()
{
    _title = QStringLiteral("报告");
    _author = QStringLiteral("FluentUI");
    _company = QStringLiteral("公司名称");
    _pageWidth = 210;      // A4宽 mm
    _pageHeight = 297;     // A4高 mm
    _marginLeft = 20;
    _marginRight = 20;
    _marginTop = 20;
    _marginBottom = 20;
    _fontFamily = QStringLiteral("SimSun");
    _fontSize = 11;
    _colorHeaderBg = QStringLiteral("#E0E0E0");
    _tableMaxRows = 20;
}

void FluReportBuilder::addCoverPage(const QString &title,
                                    const QString &subtitle,
                                    const QString &date,
                                    const QString &author)
{
    ContentItem item;
    item.type = ContentItem::Cover;
    item.title = title;
    item.text = subtitle;
    m_contents.append(item);
}

void FluReportBuilder::addTableOfContents()
{
    ContentItem item;
    item.type = ContentItem::TableOfContents;
    m_contents.append(item);
}

void FluReportBuilder::addSection(const QString &title)
{
    ContentItem item;
    item.type = ContentItem::Section;
    item.title = title;
    m_contents.append(item);
}

void FluReportBuilder::addTable(const QVariantList &headers,
                               const QVariantList &rows)
{
    ContentItem item;
    item.type = ContentItem::Table;
    item.headers = headers;
    item.rows = rows;
    m_contents.append(item);
}

void FluReportBuilder::addChart(QObject *chartItem,
                               const QString &caption)
{
    ContentItem item;
    item.type = ContentItem::Chart;
    item.chartItem = chartItem;
    item.caption = caption;
    m_contents.append(item);
}

void FluReportBuilder::addText(const QString &htmlContent)
{
    ContentItem item;
    item.type = ContentItem::Paragraph;
    item.text = htmlContent;
    m_contents.append(item);
}

void FluReportBuilder::addPageBreak()
{
    ContentItem item;
    item.type = ContentItem::PageBreak;
    m_contents.append(item);
}

bool FluReportBuilder::generate(const QString &filePath)
{
    try {
        QPdfWriter pdfWriter(filePath);
        
        // 设置PDF纸张大小和边距 (单位: mm)
        pdfWriter.setPageSize(QPageSize(QSizeF(_pageWidth, _pageHeight), QPageSize::Millimeter));
        pdfWriter.setMargins({_marginLeft, _marginTop, _marginRight, _marginBottom},
                            QPagedPaintDevice::Millimeter);
        pdfWriter.setTitle(_title);
        pdfWriter.setCreator(QStringLiteral("FluentUI PDF Export"));
        
        return renderPdf(pdfWriter, filePath);
    } catch (const std::exception &e) {
        emit generationError(QString::fromStdString(e.what()));
        return false;
    }
}

bool FluReportBuilder::renderPdf(QPdfWriter &writer, const QString &filePath)
{
    QPainter painter(&writer);
    int totalItems = m_contents.size();
    
    for (int i = 0; i < totalItems; ++i) {
        const ContentItem &item = m_contents.at(i);
        
        // 更新进度
        int progress = (i * 100) / totalItems;
        emit generationProgress(progress);
        
        switch (item.type) {
            case ContentItem::Cover:
                renderCoverPage(painter, item);
                painter.drawText(QRect(), "");  // 触发新页
                break;
                
            case ContentItem::Section: {
                QFont titleFont(_fontFamily);
                titleFont.setPointSize(_fontSize + 4);
                titleFont.setBold(true);
                painter.setFont(titleFont);
                
                int y = _marginTop;
                drawCenteredText(painter, item.title, _marginLeft, y,
                               writer.width() - _marginLeft - _marginRight,
                               titleFont);
                y += 20;
                
                // 分页
                if (!writer.newPage()) {
                    emit generationError(QStringLiteral("无法创建新页"));
                    return false;
                }
                break;
            }
                
            case ContentItem::Paragraph: {
                QFont bodyFont(_fontFamily);
                bodyFont.setPointSize(_fontSize);
                painter.setFont(bodyFont);
                
                int y = _marginTop;
                renderParagraph(painter, item, y);
                
                if (!writer.newPage()) {
                    emit generationError(QStringLiteral("无法创建新页"));
                    return false;
                }
                break;
            }
                
            case ContentItem::Table: {
                int y = _marginTop;
                renderTable(painter, item, y);
                
                if (!writer.newPage()) {
                    emit generationError(QStringLiteral("无法创建新页"));
                    return false;
                }
                break;
            }
                
            case ContentItem::PageBreak:
                if (!writer.newPage()) {
                    emit generationError(QStringLiteral("无法创建新页"));
                    return false;
                }
                break;
                
            default:
                break;
        }
    }
    
    painter.end();
    emit generationProgress(100);
    return true;
}

void FluReportBuilder::renderCoverPage(QPainter &painter, const ContentItem &item)
{
    QPdfWriter *pdfWriter = dynamic_cast<QPdfWriter*>(painter.device());
    if (!pdfWriter) return;
    
    int pageHeight = pdfWriter->height();
    int pageWidth = pdfWriter->width();
    
    // 绘制背景
    painter.fillRect(0, 0, pageWidth, pageHeight, QColor(240, 240, 240));
    
    // 标题
    QFont titleFont(_fontFamily);
    titleFont.setPointSize(_fontSize + 12);
    titleFont.setBold(true);
    painter.setFont(titleFont);
    
    int y = pageHeight / 3;
    drawCenteredText(painter, item.title, _marginLeft, y,
                    pageWidth - _marginLeft - _marginRight, titleFont);
    
    // 副标题
    QFont subtitleFont(_fontFamily);
    subtitleFont.setPointSize(_fontSize + 4);
    painter.setFont(subtitleFont);
    y += 60;
    drawCenteredText(painter, item.text, _marginLeft, y,
                    pageWidth - _marginLeft - _marginRight, subtitleFont);
    
    // 日期和作者
    QFont infoFont(_fontFamily);
    infoFont.setPointSize(_fontSize);
    painter.setFont(infoFont);
    
    y = pageHeight - _marginBottom - 100;
    QString dateStr = QDateTime::currentDateTime().toString("yyyy-MM-dd");
    drawCenteredText(painter, dateStr, _marginLeft, y,
                    pageWidth - _marginLeft - _marginRight, infoFont);
    
    y += 30;
    drawCenteredText(painter, _author, _marginLeft, y,
                    pageWidth - _marginLeft - _marginRight, infoFont);
}

void FluReportBuilder::renderParagraph(QPainter &painter, const ContentItem &item, int &yPos)
{
    // 简单的HTML去标签处理
    QString text = item.text;
    text.replace("<h2>", "\n");
    text.replace("</h2>", "\n");
    text.replace("<h3>", "\n");
    text.replace("</h3>", "\n");
    text.replace("<p>", "");
    text.replace("</p>", "\n");
    text.replace("<br>", "\n");
    text.replace("<br/>", "\n");
    text.replace("&nbsp;", " ");
    
    QStringList lines = text.split('\n');
    QFont font = painter.font();
    QFontMetrics fm(font);
    
    for (const QString &line : lines) {
        if (line.trimmed().isEmpty()) {
            yPos += fm.height();
            continue;
        }
        
        if (yPos + fm.height() > painter.device()->height() - _marginBottom) {
            if (!dynamic_cast<QPdfWriter*>(painter.device())->newPage()) {
                return;
            }
            yPos = _marginTop;
        }
        
        painter.drawText(QRect(_marginLeft, yPos,
                             painter.device()->width() - _marginLeft - _marginRight,
                             fm.height()),
                        Qt::TextWordWrap, line);
        yPos += fm.height() + 5;
    }
}

void FluReportBuilder::renderTable(QPainter &painter, const ContentItem &item, int &yPos)
{
    if (item.headers.isEmpty()) return;
    
    QFont headerFont(_fontFamily);
    headerFont.setPointSize(_fontSize);
    headerFont.setBold(true);
    
    QFont dataFont(_fontFamily);
    dataFont.setPointSize(_fontSize - 1);
    
    QPdfWriter *pdfWriter = dynamic_cast<QPdfWriter*>(painter.device());
    if (!pdfWriter) return;
    
    // 计算列宽
    int columnCount = item.headers.size();
    int tableWidth = pdfWriter->width() - _marginLeft - _marginRight;
    int columnWidth = tableWidth / columnCount;
    
    // 绘制表头
    painter.setFont(headerFont);
    QBrush headerBrush(QColor(_colorHeaderBg));
    painter.fillRect(_marginLeft, yPos, tableWidth, 25, headerBrush);
    
    QStringList headerList;
    for (const QVariant &h : item.headers) {
        headerList.append(h.toString());
    }
    drawTableRow(painter, headerList, _marginLeft, yPos, headerFont, true);
    yPos += 25;
    
    // 绘制数据行
    painter.setFont(dataFont);
    int rowCount = 0;
    
    for (const QVariant &rowVar : item.rows) {
        if (rowCount >= _tableMaxRows) {
            if (!pdfWriter->newPage()) return;
            yPos = _marginTop;
            rowCount = 0;
        }
        
        QStringList row;
        if (rowVar.type() == QVariant::List) {
            for (const QVariant &cell : rowVar.toList()) {
                row.append(cell.toString());
            }
        }
        
        if (row.size() != columnCount) continue;
        
        drawTableRow(painter, row, _marginLeft, yPos, dataFont, false);
        yPos += 20;
        rowCount++;
    }
}

void FluReportBuilder::renderChart(QPainter &painter, const ContentItem &item)
{
    if (!item.chartItem) return;
    
    // 这里简化处理 - 实际应该调用图表的render/paint方法
    QFont font(_fontFamily);
    font.setPointSize(_fontSize);
    painter.setFont(font);
    
    painter.drawText(50, 100, QStringLiteral("[图表: ") + item.caption + "]");
}

int FluReportBuilder::calculateTableHeight(const ContentItem &item)
{
    // 表头高度25 + 每行20
    return 25 + item.rows.size() * 20;
}

void FluReportBuilder::drawTableRow(QPainter &painter, const QStringList &cells,
                                   int x, int &y, const QFont &font, bool isHeader)
{
    QPdfWriter *pdfWriter = dynamic_cast<QPdfWriter*>(painter.device());
    if (!pdfWriter) return;
    
    int tableWidth = pdfWriter->width() - _marginLeft - _marginRight;
    int columnWidth = tableWidth / cells.size();
    int rowHeight = 20;
    if (isHeader) rowHeight = 25;
    
    painter.setFont(font);
    painter.setPen(QPen(Qt::black, 1));
    
    for (int i = 0; i < cells.size(); ++i) {
        int xPos = x + i * columnWidth;
        painter.drawRect(xPos, y, columnWidth, rowHeight);
        painter.drawText(QRect(xPos + 5, y + 2, columnWidth - 10, rowHeight - 4),
                        Qt::AlignLeft | Qt::AlignVCenter, cells[i]);
    }
    
    y += rowHeight;
}

void FluReportBuilder::wrapText(const QString &text, int maxWidth, QStringList &lines)
{
    QFont font(_fontFamily);
    font.setPointSize(_fontSize);
    QFontMetrics fm(font);
    
    QString line;
    for (const QString &word : text.split(' ')) {
        if (fm.horizontalAdvance(line + " " + word) > maxWidth) {
            if (!line.isEmpty()) {
                lines.append(line);
            }
            line = word;
        } else {
            if (!line.isEmpty()) line += " ";
            line += word;
        }
    }
    
    if (!line.isEmpty()) {
        lines.append(line);
    }
}

void FluReportBuilder::drawCenteredText(QPainter &painter, const QString &text,
                                       int x, int &y, int width, const QFont &font)
{
    painter.setFont(font);
    QFontMetrics fm(font);
    
    QStringList lines;
    wrapText(text, width, lines);
    
    for (const QString &line : lines) {
        painter.drawText(QRect(x, y, width, fm.height()),
                        Qt::AlignHCenter | Qt::AlignTop, line);
        y += fm.height() + 5;
    }
}
