//////////////////////////////////////////////////////////////////////
//
// This file is part of BeeBEEP.
//
// BeeBEEP is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published
// by the Free Software Foundation, either version 3 of the License,
// or (at your option) any later version.
//
// BeeBEEP is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with BeeBEEP.  If not, see <http://www.gnu.org/licenses/>.
//
// Author: Marco Mastroddi <marco.mastroddi(AT)gmail.com>
//
// $Id$
//
//////////////////////////////////////////////////////////////////////

#ifndef BEEBEEP_GUIEMOTICONS_H
#define BEEBEEP_GUIEMOTICONS_H

#include "Config.h"
#include "Emoticon.h"

class GuiEmoticons : public QTabWidget
{
  Q_OBJECT

public:
  explicit GuiEmoticons( QWidget *parent = 0 );
  void initEmoticons();

signals:
  void emoticonSelected( const Emoticon& );

private slots:
  void emoticonClicked();

protected:
  QSize sizeHint() const;
  void addEmoticonTab( const QList<Emoticon>&, const QIcon&, const QString& );

};

class GuiEmoticonWidget : public QWidget
{
public:
  explicit GuiEmoticonWidget( QWidget *parent = 0 );

  inline void setEmoticonButtons( const QList<QPushButton*>& );

protected:
  void paintEmoticonButtons( int );
  void clearLayout();
  void resizeEvent( QResizeEvent* );

private:
  QGridLayout* mp_layout;
  QList<QPushButton*> m_buttons;
  bool m_hasPainted;

};


// Inline Functions
inline void GuiEmoticonWidget::setEmoticonButtons( const QList<QPushButton*>& new_value ) { m_buttons = new_value; }

#endif // BEEBEEP_GUIEMOTICONS_H
