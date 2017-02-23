/* Atelier KDE Printer Host for 3D Printing
    Copyright (C) <2016>
    Author: Lays Rodrigues - laysrodrigues@gmail.com

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
#include "gcodeeditorwidget.h"
#include <QVBoxLayout>
#include <QLabel>
#include <KLocalizedString>

GCodeEditorWidget::GCodeEditorWidget(QWidget *parent) :
    QWidget(parent)
{
    editor = KTextEditor::Editor::instance();
    doc = editor->createDocument(this);
    doc->setMode("G-Code");
    view = doc->createView(this);
    QVBoxLayout *layout = new QVBoxLayout();
    QLabel *lb = new QLabel(i18n("G-Code Editor"));
    layout->addWidget(lb);
    layout->addWidget(view);
    this->setLayout(layout);
    setupInterface();
}

void GCodeEditorWidget::loadFile(const QUrl &fileName)
{
    doc->openUrl(fileName);
    doc->setHighlightingMode(QString("G-Code"));
}

KTextEditor::View* GCodeEditorWidget::gcodeView() const
{
    return view;
}

void GCodeEditorWidget::setupInterface()
{
    interface = qobject_cast<KTextEditor::ConfigInterface*>(view);
    interface->setConfigValue("line-numbers", true);
}
