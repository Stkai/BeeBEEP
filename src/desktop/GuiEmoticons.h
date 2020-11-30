//////////////////////////////////////////////////////////////////////
//
// BeeBEEP Copyright (C) 2010-2020 Marco Mastroddi
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
// along with BeeBEEP. If not, see <http://www.gnu.org/licenses/>.
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
class GuiEmoticonWidget;


class GuiEmoticons : public QTabWidget
{
  Q_OBJECT

public:
  explicit GuiEmoticons( QWidget *parent = Q_NULLPTR );

  void deleteAllTabs();

signals:
  void emoticonSelected( const Emoticon& );
  void emoticonsUpdated();

public slots:
  void updateEmoticons();

private slots:
  void emoticonClicked();
  void loadEmoticons( int );

protected:
  QSize sizeHint() const;
  int addEmoticonTab( GuiEmoticonWidget*, Emoticon::Group, const QString& );
  void setFavoriteEmoticons( const QList<Emoticon>& );
  void setRecentEmoticons( const QList<Emoticon>& );
  void setEmoticonToButton( const Emoticon&, QPushButton* );
  void initEmoticons( int );

private:
  GuiEmoticonWidget* mp_favorite;
  GuiEmoticonWidget* mp_recent;
  int m_favoriteTabIndex;
  int m_recentTabIndex;

};

class GuiEmoticonWidget : public QWidget
{
public:
  explicit GuiEmoticonWidget( QWidget *parent = Q_NULLPTR );

  inline void setEmoticonSize( int );
  inline QSize emoticonSize() const;
  inline QSize emoticonButtonSize() const;
  void setEmoticonButtons( const QList<QPushButton*>& );
  inline const QList<QPushButton*>& emoticonButtons() const;
  inline void setEmoticonGroup( Emoticon::Group );
  inline Emoticon::Group emoticonGroup() const;

protected:
  void paintEmoticonButtons( int );
  void clearLayout();
  void resizeEvent( QResizeEvent* );

private:
  QGridLayout* mp_layout;
  QList<QPushButton*> m_buttons;
  bool m_hasPainted;
  int m_emoticonSize;
  Emoticon::Group m_emoticonGroup;

};


// Inline Functions
inline void GuiEmoticonWidget::setEmoticonSize( int new_value ) { m_emoticonSize = new_value; }
inline QSize GuiEmoticonWidget::emoticonSize() const { return QSize( m_emoticonSize, m_emoticonSize ); }
inline QSize GuiEmoticonWidget::emoticonButtonSize() const { return QSize( m_emoticonSize + 4, m_emoticonSize + 4 ); }
inline const QList<QPushButton*>& GuiEmoticonWidget::emoticonButtons() const { return m_buttons; }
inline void GuiEmoticonWidget::setEmoticonGroup( Emoticon::Group new_value ) { m_emoticonGroup = new_value; }
inline Emoticon::Group GuiEmoticonWidget::emoticonGroup() const { return m_emoticonGroup; }

#endif // BEEBEEP_GUIEMOTICONS_H
