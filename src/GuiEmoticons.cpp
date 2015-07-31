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

#include "EmoticonManager.h"
#include "GuiConfig.h"
#include "GuiEmoticons.h"


GuiEmoticons::GuiEmoticons( QWidget* parent )
  : QTabWidget( parent )
{
  setObjectName( "GuiEmoticons" );
}

QSize GuiEmoticons::sizeHint() const
{
  return QSize( BEE_DOCK_WIDGET_SIZE_HINT_WIDTH, 40 );
}

void GuiEmoticons::initEmoticons()
{
  QList<Emoticon> emoticon_list = EmoticonManager::instance().emoticons( true );
  addEmoticonTab( emoticon_list, QIcon( ":images/emoticon.png"), tr( "Smiley" ) );
}

void GuiEmoticons::addEmoticonTab( const QList<Emoticon>& emoticon_list, const QIcon& group_icon, const QString& group_name )
{
  GuiEmoticonWidget* emoticon_widget = new GuiEmoticonWidget( this );

  QList<QPushButton*> button_list;
  QPushButton* emoticon_button = 0;

  foreach( Emoticon e, emoticon_list )
  {
    emoticon_button = new QPushButton( emoticon_widget );
    emoticon_button->setIconSize( QSize( 24, 24 ) );
    emoticon_button->setSizePolicy( QSizePolicy::Minimum, QSizePolicy::Minimum );
    emoticon_button->setMinimumSize( 26, 26 );
    emoticon_button->setIcon( e.pixmap() );
    emoticon_button->setStyleSheet( "QPushButton:hover{ background-color: #ffcf04; }");
    emoticon_button->setObjectName( QString( "GuiEmoticonCode%1" ).arg( e.textToMatch() ) );
    connect( emoticon_button, SIGNAL( clicked() ), this, SLOT( emoticonClicked() ) );
    button_list.append( emoticon_button );
  }

  emoticon_widget->setEmoticonButtons( button_list );
  emoticon_widget->setObjectName( emoticon_widget->objectName() + group_name );
  addTab( emoticon_widget, group_icon, group_name );
}

void GuiEmoticons::emoticonClicked()
{
  QObject* emoticon_object = sender();
  if( !emoticon_object )
    return;

  QString emoticon_code = emoticon_object->objectName();
  emoticon_code.remove( "GuiEmoticonCode" );
#ifdef BEEBEEP_DEBUG
  qDebug() << "Emoticon clicked is" << emoticon_code;
#endif
  Emoticon e = EmoticonManager::instance().emoticon( emoticon_code );
  if( e.isValid() )
    emit( emoticonSelected( e ) );
}


/******************************
 * GuiEmoticonWidget
 *
 */

GuiEmoticonWidget::GuiEmoticonWidget( QWidget* parent )
  : QWidget( parent )
{
  setObjectName( "GuiEmoticonWidget" );

  mp_layout = new QGridLayout( this );
  mp_layout->setHorizontalSpacing( 0 );
  mp_layout->setVerticalSpacing( 0 );
  mp_layout->setContentsMargins( 0, 0, 0, 0 );

  setMinimumSize( BEE_DOCK_WIDGET_SIZE_HINT_WIDTH, 30 );
}

void GuiEmoticonWidget::resizeEvent( QResizeEvent* e )
{
  QWidget::resizeEvent( e );
  clearLayout();
  paintEmoticonButtons( e->size().width() );
}

void GuiEmoticonWidget::clearLayout()
{
  foreach( QWidget* w, m_buttons )
    mp_layout->removeWidget( w );
}

void GuiEmoticonWidget::paintEmoticonButtons( int box_width )
{
  int x = 0;
  int y = 0;
  int max_icons_per_row = qMax( 1, static_cast<int>((box_width-2) / 26) );

  foreach( QWidget* w, m_buttons )
  {
    mp_layout->addWidget( w, y, x );
    x++;
    if( x > max_icons_per_row )
    {
      y++;
      x = 0;
    }
  }
}

