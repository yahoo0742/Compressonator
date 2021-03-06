//=====================================================================
// Copyright 2016 (c), Advanced Micro Devices, Inc. All rights reserved.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files(the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and / or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions :
// 
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//
//=====================================================================

#ifndef _PROJECTVIEW_H
#define _PROJECTVIEW_H

#include <QtWidgets>
#include <QScrollArea>
#include <QGridLayout>
#include <QLabel>
#include <QIcon>
#include <QMap>
#include <QFile>
#include <QtXml/QDomDocument>
#include <QMutex>
#include <QElapsedTimer>

#include "qteditorfactory.h"
#include "qttreepropertybrowser.h"
#include "qtbuttonpropertybrowser.h"
#include "qtgroupboxpropertybrowser.h"
#include "qtpropertymanager.h"
#include "qtvariantproperty.h"
#include "objectcontroller.h"

// Progress Bar
#include <AMDTApplicationComponents/Include/acProgressDlg.h>

#include "cpTreeWidget.h"
#include "cpImageView.h"
#include "cpProjectData.h"
#include "cpSetCompressOptions.h"
#include "cpCompressStatus.h"
#include "cpImageLoader.h"
#include "cpNewProject.h"
#include "TextureIO.h"



#define    MAX_PROJECTVIEW_COLUMNS               2     // Project view has 2 columns to view


#define     DIFFERENCE_IMAGE_TXT "Difference:"

//
// This Class is defined as Static 
// to enable message handling via emit signals calls from static & global functions
// 
class signalProcessMsgHandler : public QObject
{
    Q_OBJECT

public:
    signalProcessMsgHandler() { };

Q_SIGNALS:
    void  signalProcessMessage();
};


class MyThread : public QThread
{
public:
    // constructor
    // set name and Stop is set as false by default
    MyThread();
    acProgressDlg*                          m_pProgressDlg;

    // overriding the QThread's run() method
    void run();

    void done();
    void show();

    // variable that mutex protects
    bool Stop;
private:
    QString name;
};



class cpItemDelegate : public QItemDelegate
{
public:

    QSize sizeHint(const QStyleOptionViewItem & option, const QModelIndex & index) const
    {
        QSize m_size;
        m_size.setHeight(32);
        m_size.setWidth(32);
        //const QTreeItem* ti(static_cast<TreeItem*>(index.internalPointer()));
        //if (ti->pixmap())
        //    return ti->pixmap()->size();
        QItemDelegate::sizeHint(option, index);
        return m_size;
    }
};


class ProjectView : public QDockWidget
{
    Q_OBJECT
public:
    ProjectView(const QString title, CompressStatusDialog *StatusDialog, QWidget *parent);
    ~ProjectView();
    bool    m_saveProjectChanges;            // Flag if any changes were made to a project file

    QString m_curProjectFilePathName;        // Current Project Full Path and Name
    QString m_curProjectName;            
    void setCurrentProjectName(QString filePathName);    // Set the Member var above using filePathName

    QList<QByteArray> m_supportedFormats;   // List of Qt and Plugin file formats
    QString m_ImageFileFilter;              // String version of m_supportedFormats formated for File Dialogs
    
    CompressStatusDialog *                  m_CompressStatusDialog;
    acProgressDlg*                          m_pProgressDlg;

    void SetupHeader();
    void SetupTreeView();
    bool loadProjectFile(QString fileToLoad);
    bool OpenImageFile();
    int  PromptSaveChanges();
    bool userSaveProjectAndContinue();

    QString GetSourceFileNamePath(QTreeWidgetItem *item);
    QString GetDestinationFileNamePath(QTreeWidgetItem *item);

    void Tree_AddRootNode();
    QTreeWidgetItem *Tree_AddImageFile(QString filePathName, int index, C_Source_Image **m_dataout);
    void Tree_AddCompressFile(QTreeWidgetItem *parent, QString description, bool checkable, bool checked, int levelType, C_Destination_Options *m_data);

    // Changes the Icon for the item if file exists or not return true if file exists
    // Red Icon or Null is used on Files that dont exist
    bool Tree_updateCompressIcon(QTreeWidgetItem *item, QString FilePathName, bool RedIcon);
    QTreeWidgetItem * Tree_SetCurrentItem(QString FilePathName);
    void Tree_setAllItemsSetected();
    void Tree_clearAllItemsSetected();
    void SelectImageItem(QString filePathName);
    bool AnySelectedItems();
    void Tree_selectAllChildItems(QTreeWidgetItem *item);
    int  Tree_numSelectedtems(int &ItemsCount);
    int  Tree_numCompresstemsSelected(int &ItemsCount, int &NumCompressedItems);




    QTreeWidgetItem *GetCurrentItem(int inLevelType);
    QTreeWidgetItem *Tree_FindImageItem(QString filePathName, bool inCludeDestination);
    QTreeWidgetItem *ContextMenu_ImageItem;

    // Delete Items
    void DeleteAllSeletedItems(bool RemoveFromDisk);
    void clearProjectTreeView();
    void UserDeleteItems();
    void DeleteItemData(QTreeWidgetItem *item);
    QTreeWidgetItem * DeleteSelectedItemData(QTreeWidgetItem *item, bool RemoveFromDisk);
    void SignalUpdateData(QTreeWidgetItem * item, int levelType);

    QAction *actOpenContainingFolder;
    QAction *actCopyFullPath;
    QAction *actsaveProjectFile;
    QAction *actopenProjectFile;
    QAction *actCompressProjectFiles;
    QAction *actSeperator;
    QAction *actViewImageDiff;
    QAction *actRemoveImage;

    // Tracks Compressed image items user has clicked on in project view, Null if not on item
    QTreeWidgetItem *m_CurrentCompressedImageItem;
    
    QMenu           *m_contextMenu;
    CImageLoader    *m_imageloader;             // Util that is used to load image files to MIP and Image view files
    QTreeWidgetItem *m_CurrentItem;

    cpNewProject *m_newProjectwindow;


    QElapsedTimer m_elapsedTimer;

Q_SIGNALS:

    void AddedImageFile(QString &fileName);
    void ViewImageFile(QString &fileName, QTreeWidgetItem * item);
    void DeleteImageView(QString &fileName);

    void ViewImageFileDiff(C_Destination_Options *m_data);

    void DeleteImageViewDiff(QString &fileName);

    void AddCompressSettings(QTreeWidgetItem *item);
    void EditCompressSettings(QTreeWidgetItem *item);

    void UpdateData(QObject *data);

    // Signals when processing image for compression
    void OnCompressionDone();
    void OnProcessing(QString &FilePathName);
    void OnCompressionStart();

    // Signals when current selection is on the specified image type
    void OnSourceImage(int ChildCount);
    void OnDecompressImage();

    // Signal when a project file is loaded
    void OnProjectLoaded(int ChildCount);

    // Signal when a compressed item node in tree has been added
    void OnAddedCompressSettingNode();

public slots:

void onTree_ItemDoubleClicked(QTreeWidgetItem * item, int column);
void onTree_ItemClicked(QTreeWidgetItem * item, int column);
void onDroppedImageItem(QString &filePathName, int index);
void onSetCurrentItem(QString &FilePathName);
void onEntered(const QModelIndex & index);
void onShowCompressStatus();
void OnGlobalMessage(const char *msg);
void onSetNewProject(QString &FilePathName);

void OnStartCompression();
void OnCloseCompression();
void onSignalProcessMessage();

void saveProjectFile();
void saveAsProjectFile();
void saveToBatchFile();
void openProjectFile();
void openNewProjectFile();
void compressProjectFiles(QFile *file);
void viewImageDiff();
void removeSelectedImage();
void openContainingFolder();
void copyFullPath();

void onCustomContextMenu(const QPoint &point);

void onTreeMousePress(QMouseEvent  *event);
void onTreeKeyPress(QKeyEvent  *event);

void onImageLoadStart();
void onImageLoadDone();

public:
    // Common for all
    const QString            m_title;
    QWidget                 *m_newWidget;
    QGridLayout             *m_layout;
    QWidget                 *m_parent;
    cpTreeWidget            *m_projectTreeView;
    QTreeWidgetItem         *m_treeRootItem;            // Root of Tree view Items
    QString                     m_RecentImageDirOpen;      // Keep track of recent image files opened dir
    void keyPressEvent(QKeyEvent * event);
    bool                      m_bCompressing;           // Set true when we are compressing project items
    bool                      m_EnableCheckedItemsView; // This flags by default if set to false and enables compression of items
                                                        //  based on checked flags along with highlited items.
    int                       m_NumItems;
    bool                      m_AllItemsSelected;

};
#endif