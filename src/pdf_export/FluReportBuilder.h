#pragma once

#include <QString>
#include <QVariantList>
#include <QVariantMap>
#include <QObject>
#include <memory>

class QPdfWriter;
class QPainter;

/**
 * @brief PDF报告生成器 - 支持生成包含表格、图表、文本的多页PDF报告
 */
class FluReportBuilder : public QObject {
    Q_OBJECT
    QML_NAMED_ELEMENT(FluReportBuilder)

public:
    explicit FluReportBuilder(QObject *parent = nullptr);
    ~FluReportBuilder();
    
    // ============ 内容添加接口 ============
    
    /**
     * @brief 添加封面页
     */
    Q_INVOKABLE void addCoverPage(const QString &title, 
                                  const QString &subtitle,
                                  const QString &date,
                                  const QString &author);
    
    /**
     * @brief 添加目录
     */
    Q_INVOKABLE void addTableOfContents();
    
    /**
     * @brief 添加章节标题
     */
    Q_INVOKABLE void addSection(const QString &title);
    
    /**
     * @brief 添加表格
     * @param headers 表头列表
     * @param rows 数据行列表，每行是QVariantList
     */
    Q_INVOKABLE void addTable(const QVariantList &headers,
                             const QVariantList &rows);
    
    /**
     * @brief 添加图表 (快速快照)
     * @param chartItem 图表QObject指针
     * @param caption 图表标题
     */
    Q_INVOKABLE void addChart(QObject *chartItem, 
                             const QString &caption);
    
    /**
     * @brief 添加纯文本内容（支持基本HTML标签）
     */
    Q_INVOKABLE void addText(const QString &htmlContent);
    
    /**
     * @brief 添加分页符
     */
    Q_INVOKABLE void addPageBreak();
    
    // ============ PDF生成接口 ============
    
    /**
     * @brief 生成PDF文件
     * @param filePath 输出文件路径
     * @return 生成成功返回true
     */
    Q_INVOKABLE bool generate(const QString &filePath);
    
    // ============ 属性接口 ============
    Q_PROPERTY_AUTO(QString, title)          // 报告标题
    Q_PROPERTY_AUTO(QString, author)         // 报告作者
    Q_PROPERTY_AUTO(QString, company)        // 公司名称
    Q_PROPERTY_AUTO(int, pageWidth)          // A4宽: 210mm
    Q_PROPERTY_AUTO(int, pageHeight)         // A4高: 297mm
    Q_PROPERTY_AUTO(int, marginLeft)         // 左边距: 20mm
    Q_PROPERTY_AUTO(int, marginRight)        // 右边距: 20mm
    Q_PROPERTY_AUTO(int, marginTop)          // 上边距: 20mm
    Q_PROPERTY_AUTO(int, marginBottom)       // 下边距: 20mm
    Q_PROPERTY_AUTO(QString, fontFamily)     // 字体家族
    Q_PROPERTY_AUTO(int, fontSize)           // 基础字号: 11pt
    Q_PROPERTY_AUTO(QString, colorHeaderBg)  // 表头背景色
    Q_PROPERTY_AUTO(int, tableMaxRows)       // 单页最大行数

signals:
    /**
     * @brief PDF生成进度 0-100
     */
    void generationProgress(int percentage);
    
    /**
     * @brief 生成错误
     */
    void generationError(const QString &error);

private:
    // 内部结构定义
    struct ContentItem {
        enum Type { 
            Cover,          // 封面
            TableOfContents,// 目录
            Section,        // 章节
            Paragraph,      // 段落文本
            Table,          // 表格
            Chart,          // 图表
            PageBreak       // 分页
        } type;
        
        QString title;
        QString text;
        QVariantList headers;
        QVariantList rows;
        QObject* chartItem;
        QString caption;
    };
    
    // 内部渲染函数
    bool renderPdf(QPdfWriter &writer, const QString &filePath);
    void renderCoverPage(QPainter &painter, const ContentItem &item);
    void renderSection(QPainter &painter, const ContentItem &item);
    void renderParagraph(QPainter &painter, const ContentItem &item, int &yPos);
    void renderTable(QPainter &painter, const ContentItem &item, int &yPos);
    void renderChart(QPainter &painter, const ContentItem &item);
    
    // 辅助函数
    int calculateTableHeight(const ContentItem &item);
    void drawTableRow(QPainter &painter, const QStringList &cells, 
                     int x, int &y, const QFont &font, bool isHeader);
    void wrapText(const QString &text, int maxWidth, QStringList &lines);
    void drawCenteredText(QPainter &painter, const QString &text, 
                         int x, int &y, int width, const QFont &font);
    
    QList<ContentItem> m_contents;
    int m_pageCount;
    
    // 默认值初始化
    void initializeDefaults();
};
