#ifndef TILESETEDITORWINDOW_H
#define TILESETEDITORWINDOW_H

#include <QMainWindow>
#include <QImage>
#include <QStringListModel>
#include <QStandardItemModel>
#include <QComboBox>

#include "tileset.h"
#include "tileseteditorwidgets.h"


namespace Ui {
class TilesetEditorWindow;
}

class TilesetEditorWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit TilesetEditorWindow(QWidget *parent, Tileset *tileset);
    ~TilesetEditorWindow();

signals:
    void selectedObjectChanged(int objNbr);

public slots:
    void updateSelectedTile(int tile);
    void setSelTileData(QString text);
    void updateObjectEditor();

private slots:
    void on_hexLineEdit_textEdited(const QString &text);

    void on_actionExportImage_triggered();

    void on_sBehaviorListView_clicked(const QModelIndex &index);

    void on_parameterListView_clicked(const QModelIndex &index);

    void on_actionSetBackgroundColor_triggered();

    void on_hitBoxComboBox_currentIndexChanged(int index);

    void on_terrainTypeComboBox_currentIndexChanged(int index);

    void on_depthComboBox_currentIndexChanged(int index);

    void on_pipeColorComboBox_currentIndexChanged(int index);

    void on_actionSave_triggered();

    void on_objectsListView_clicked(const QModelIndex &index);

    void on_removeObjectButton_clicked();

    void on_moveObjectUpButton_clicked();

    void on_moveObjectDownButton_clicked();

    void on_addObjectPushButton_clicked();

    void on_actionDeleteAllObjects_triggered();

    void on_oWidthSpinBox_valueChanged(int width);

    void on_oHeightSpinBox_valueChanged(int height);

private:
    Ui::TilesetEditorWindow *ui;
    TilesetPicker* tilesetPicker;
    ObjectEditor* objectEditor;

    struct parameter
    {
        parameter(int byte, QString description) : byte(byte), description(description) {}
        int byte;
        QString description;
    };


    struct specialBehavior
    {
        int value;                      // Value of Byte 0
        QString description;            // Description of the SpecialBehavior
        QList<parameter> parameters;    // List of all used Parameters
    };

    Tileset* tileset;

    int selectedTile;

    int selectedSpecialBehavior;
    int selectedParameter;

    QList<specialBehavior> specialBehaviors;
    QList<parameter> hitboxes;
    QList<parameter> terrainTypes;
    QList<parameter> depthBehaviors;
    QList<parameter> pipeColors;

    void loadBehaviors();
    void updateHex();
    void updateBehavior();
    void updateParameters();
    void updateComboBoxes();
    void setStaticModels();
    void setParametersModel();
    void updateComboBox(int byteNbr, QList<parameter> &list, QComboBox *comboBox);


    void setupObjectsModel(bool keepIndex);
    void updateObjectInfo();
};

#endif // TILESETEDITORWINDOW_H