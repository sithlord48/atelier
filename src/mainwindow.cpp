/* Atelier KDE Printer Host for 3D Printing
    Copyright (C) <2016>
    Author: Lays Rodrigues - laysrodrigues@gmail.com
            Chris Rizzitello - rizzitello@kde.org

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <KLocalizedString>
#include <KStandardAction>
#include <KActionCollection>
#include <KXMLGUIFactory>
#include <dialogs/connectsettingsdialog.h>
#include <dialogs/profilesdialog.h>
#include <widgets/gcodeeditorwidget.h>
#include <set>
#include <QToolButton>
#include <memory>
#include <QStackedWidget>
#include <widgets/3dview/viewer3d.h>
#include <widgets/videomonitorwidget.h>

MainWindow::MainWindow(QWidget *parent) :
    KXmlGuiWindow(parent),
    ui(new Ui::MainWindow),
    m_curr_editor_view(nullptr)
{
    ui->setupUi(this);
    initWidgets();
    setupActions();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::initWidgets()
{
    /*
    auto instance = qobject_cast<AtCoreInstanceWidget*>(ui->tabWidget->currentWidget());
    connectSettingsDialog->setFirmwareList(instance->firmwares());
    profilesDialog->setBaudRates(instance->baudRates());
    */

    setupLateralArea();
}

// Move to LateralArea.
void MainWindow::setupLateralArea()
{
    m_lateral.m_toolBar = new QWidget();
    m_lateral.m_stack = new QStackedWidget();
    auto *buttonLayout = new QVBoxLayout();

    auto setupButton = [this, buttonLayout](const QString& s, QWidget *w) {
        QToolButton *btn = nullptr;
        btn = new QToolButton(m_lateral.m_toolBar);
        btn->setText(s);
        btn->setAutoExclusive(true);
        btn->setCheckable(true);
        m_lateral.m_stack->addWidget(w);
        m_lateral.m_map[s] = {btn, w};

        buttonLayout->addWidget(btn);
        connect(btn, &QToolButton::toggled, [this, w](bool checked) {
            if (checked)
                m_lateral.m_stack->setCurrentWidget(w);
        });
    };

    auto *gcodeEditor = new GCodeEditorWidget(this);
     connect(gcodeEditor, &GCodeEditorWidget::updateClientFactory, this, [&](KTextEditor::View* view){
         guiFactory()->removeClient(m_curr_editor_view);
         guiFactory()->addClient(view);
         m_curr_editor_view = view;
     });

    setupButton(i18n("&3D"), new Viewer3D(this));
    setupButton(i18n("&GCode"), gcodeEditor);
    setupButton(i18n("&Video"), new VideoMonitorWidget(this));

    m_lateral.m_toolBar->setLayout(buttonLayout);
    m_lateral.m_toolBar->show();
    m_lateral.m_stack->show();


}

void MainWindow::setupActions()
{
    // Actions for the Toolbar
    QAction *action;
    action = actionCollection()->addAction(QStringLiteral("open_gcode"));
    action->setText(i18n("&Open GCode"));
    connect(action, &QAction::triggered, this, &MainWindow::openFile);

    action = actionCollection()->addAction(QStringLiteral("connect"));
    action->setText(i18n("&Connect"));
    connect(action, &QAction::triggered, [ & ]{
            std::unique_ptr<ConnectSettingsDialog> csd(new ConnectSettingsDialog);
            connect(csd.get(), &ConnectSettingsDialog::startConnection, [ & ](const QString& port, const QMap<QString, QVariant>& data) {
                newConnection(port, data);
            });
            csd->exec();
    });

    action = actionCollection()->addAction(QStringLiteral("profiles"));
    action->setText(i18n("&Profiles"));
    connect(action, &QAction::triggered, [ & ] {
        std::unique_ptr<ProfilesDialog> pd(new ProfilesDialog);
        pd->exec();
    });

    #ifdef Q_OS_LINUX
    //only set icons from theme on linux
        actionCollection()->action(QStringLiteral("profiles"))->setIcon(QIcon::fromTheme("emblem-favorite"));
    #endif
    //use style's standardIcon for the icons we can.
    actionCollection()->action(QStringLiteral("open_gcode"))->setIcon(style()->standardIcon(QStyle::SP_DirOpenIcon));

    action = KStandardAction::quit(qApp, SLOT(quit()), actionCollection());

    setupGUI(Default, ":/atelierui");
}

void MainWindow::openFile()
{
    QUrl fileNameFromDialog = QFileDialog::getOpenFileUrl(this, i18n("Open GCode"),
                              QDir::homePath(), i18n("GCode (*.gco *.gcode)"));
/*
    if (!fileNameFromDialog.isEmpty()) {
        ui->gcodeEditorWidget->loadFile(fileNameFromDialog);
        ui->view3DWidget->drawModel(fileNameFromDialog.toString());
        m_openFiles.append(fileNameFromDialog);
    }
    */
}
void MainWindow::newConnection(const QString& port, const QMap<QString, QVariant>& profile)
{
    /*
    const int tabs = ui->tabWidget->count();
    if(tabs == 1){
        auto instance = qobject_cast<AtCoreInstanceWidget*>(ui->tabWidget->currentWidget());
        if(!instance->connected()){
            instance->startConnection(port, profile);
            ui->tabWidget->setTabText(ui->tabWidget->currentIndex(), profile["name"].toString());
            return;
        }
    }
    auto newInstanceWidget = new AtCoreInstanceWidget();
    ui->tabWidget->addTab(newInstanceWidget, profile["name"].toString());
    newInstanceWidget->startConnection(port, profile);
    */
}
