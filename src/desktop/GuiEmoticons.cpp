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
#include "Settings.h"


GuiEmoticons::GuiEmoticons( QWidget* parent )
  : QTabWidget( parent )
{
  setObjectName( "GuiEmoticons" );
  setTabPosition( QTabWidget::West );
  setUsesScrollButtons( true );
  setSizePolicy( QSizePolicy::Preferred, QSizePolicy::Expanding );
  connect( this, SIGNAL( currentChanged( int ) ), this, SLOT( loadEmoticons( int ) ) );
}

QSize GuiEmoticons::sizeHint() const
{
  return QSize( BEE_DOCK_WIDGET_SIZE_HINT_WIDTH + 6, BEE_DOCK_WIDGET_SIZE_HINT_HEIGHT );
}

void GuiEmoticons::initEmoticons( int current_index )
{
  mp_recent = new GuiEmoticonWidget( this );
  m_recentTabIndex = addEmoticonTab( mp_recent, Emoticon::Unknown, tr( "Recent" ) );
  GuiEmoticonWidget* emoticon_widget = new GuiEmoticonWidget( this );
  int smiley_index = addEmoticonTab( emoticon_widget, Emoticon::People, tr( "Smiley" ) );
  emoticon_widget = new GuiEmoticonWidget( this );
  addEmoticonTab( emoticon_widget, Emoticon::Objects, tr( "Objects" ) );
  emoticon_widget = new GuiEmoticonWidget( this );
  addEmoticonTab( emoticon_widget, Emoticon::Nature, tr( "Nature" ) );
  emoticon_widget = new GuiEmoticonWidget( this );
  addEmoticonTab( emoticon_widget, Emoticon::Places, tr( "Places" ) );
  emoticon_widget = new GuiEmoticonWidget( this );
  addEmoticonTab( emoticon_widget, Emoticon::Symbols, tr( "Symbols" ) );

  if( current_index >= 0 && current_index < count() )
    setCurrentIndex( current_index );
  else
    setCurrentIndex( Settings::instance().recentEmoticons().size() == Settings::instance().emoticonInRecentMenu() ? m_recentTabIndex : smiley_index );
}

int GuiEmoticons::addEmoticonTab( GuiEmoticonWidget* emoticon_widget, Emoticon::Group emoticon_group, const QString& group_name )
{
  QIcon group_icon = emoticon_group != Emoticon::Unknown ? Emoticon::groupIcon( emoticon_group ) : QIcon( ":/images/recent.png");
  emoticon_widget->setObjectName( emoticon_widget->objectName() + group_name );
  emoticon_widget->setEmoticonGroup( emoticon_group );

  QScrollArea* scroll_area = new QScrollArea( this );
  scroll_area->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding );
  scroll_area->setBackgroundRole( QPalette::Light );
  scroll_area->setWidgetResizable( true );
  scroll_area->setWidget( emoticon_widget );
  emoticon_widget->setParent( scroll_area );

  int tab_id = addTab( scroll_area, group_icon, "" );
  setTabToolTip( tab_id, group_name );
  return tab_id;
}

void GuiEmoticons::loadEmoticons( int current_index )
{
  if( current_index < 0 )
    return;

  GuiEmoticonWidget* gwe = 0;
  QList<Emoticon> emoticon_list;

  if( current_index != m_recentTabIndex )
  {
    QWidget *w = widget( current_index );
    if( !w )
      return;

    QScrollArea* scroll_area = qobject_cast<QScrollArea*>( w );
    if( !scroll_area )
      return;

    w = scroll_area->widget();
    if( !w )
      return;

    gwe = (GuiEmoticonWidget*)w;
    if( !gwe )
      return;
  }
  else
    gwe = mp_recent;

  if( !gwe->emoticonButtons().isEmpty() )
    return;

  QApplication::setOverrideCursor( Qt::WaitCursor );
  if( gwe == mp_recent )
  {
    emoticon_list = EmoticonManager::instance().recentEmoticons();
    while( emoticon_list.size() < Settings::instance().emoticonInRecentMenu() )
      emoticon_list << Emoticon();
  }
  else
    emoticon_list =  EmoticonManager::instance().emoticonsByGroup( gwe->emoticonGroup() );

  QList<QPushButton*> button_list;
  QPushButton* emoticon_button = 0;

  int emoticon_size = Settings::instance().emoticonSizeInMenu();
  gwe->setEmoticonSize( emoticon_size );
  QFont f = Settings::instance().chatFont();

  if( Settings::instance().useNativeEmoticons() )
  {
    Emoticon e_to_check = EmoticonManager::instance().emoticon( QString::fromUtf8( "✅" ) );
    if( e_to_check.isValid() )
    {
      QFontMetrics fm( f );
      int i_count = 0;
      while( i_count < emoticon_size )
      {
        if( (fm.height() + 4) >= emoticon_size || (fm.width( e_to_check.textToMatch() ) + 4) >= emoticon_size )
          break;
        else
          f.setPointSize( f.pointSize() + 1 );
        fm = QFontMetrics( f );
        i_count++;
      }
    }
  }

  foreach( Emoticon e, emoticon_list )
  {
    emoticon_button = new QPushButton( gwe );
    emoticon_button->setIconSize( gwe->emoticonSize() );
    emoticon_button->setSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed );
    emoticon_button->setFixedSize( gwe->emoticonButtonSize() );
    emoticon_button->setStyleSheet( "QPushButton:hover{ background-color: #ffcf04; }");
    if( Settings::instance().useNativeEmoticons() )
      emoticon_button->setFont( f );
    setEmoticonToButton( e, emoticon_button );
    connect( emoticon_button, SIGNAL( clicked() ), this, SLOT( emoticonClicked() ) );
    button_list.append( emoticon_button );
  }

  gwe->setEmoticonButtons( button_list );
  QApplication::restoreOverrideCursor();
}

void GuiEmoticons::updateEmoticons()
{
  int current_index = -1;

  if( count() > 0 )
  {
    QWidget* w;
    current_index = currentIndex();
    for( int i = 0; i < count(); i++ )
    {
      w = widget( i );
      w->deleteLater();
    }
    clear();
  }

  initEmoticons( current_index );
}

void GuiEmoticons::setEmoticonToButton( const Emoticon& e, QPushButton* pb )
{
  if( Settings::instance().useNativeEmoticons() )
    pb->setText( e.textToMatch() );
  else
    pb->setIcon( e.icon() );
  QString object_name = QString( "GuiEmoticonCode" ) + e.textToMatch();
  pb->setObjectName( object_name );
}

Emoticon GuiEmoticons::emoticonFromObject( QObject* emoticon_object )
{
  QString emoticon_code = emoticon_object->objectName();
  emoticon_code.remove( "GuiEmoticonCode" );
  return emoticon_code.isEmpty() ? Emoticon() : EmoticonManager::instance().emoticon( emoticon_code );
}

void GuiEmoticons::emoticonClicked()
{
  QObject* emoticon_object = sender();
  if( !emoticon_object )
    return;

  Emoticon e = emoticonFromObject( emoticon_object );
  if( e.isValid() )
  {
#ifdef BEEBEEP_DEBUG
    qDebug() << "Emoticon clicked is" << qPrintable( e.textToMatch() ) << e.name();
#endif
    if( currentIndex() != m_recentTabIndex )
    {
      if( EmoticonManager::instance().addToRecentEmoticons( e ) )
        setRecentEmoticons( EmoticonManager::instance().recentEmoticons() );
    }

    emit( emoticonSelected( e ) );
  }
}

void GuiEmoticons::setRecentEmoticons( const QList<Emoticon>& emoticon_list )
{
  QList<QPushButton*> recent_button_list = mp_recent->emoticonButtons();
  QPushButton* emoticon_button;
  Emoticon e;
  for( int i = 0; i < recent_button_list.size(); i++ )
  {
    emoticon_button = recent_button_list.at( i );
    Emoticon e = emoticon_list.value( i );
    setEmoticonToButton( e, emoticon_button );
  }
  mp_recent->setEmoticonButtons( recent_button_list );
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
  mp_layout->setHorizontalSpacing( 1 );
  mp_layout->setVerticalSpacing( 1 );
  mp_layout->setContentsMargins( 1, 1, 1, 1 );

  m_hasPainted = false;
  m_emoticonSize = Settings::instance().emoticonSizeInMenu();
  m_emoticonGroup = Emoticon::Unknown;
}

void GuiEmoticonWidget::setEmoticonButtons( const QList<QPushButton*>& new_value )
{
  m_buttons = new_value;
  paintEmoticonButtons( width() );
}

void GuiEmoticonWidget::resizeEvent( QResizeEvent* e )
{
  QWidget::resizeEvent( e );
  clearLayout();
  if( !m_hasPainted )
    paintEmoticonButtons( e->size().width() );
}

void GuiEmoticonWidget::clearLayout()
{
  foreach( QWidget* w, m_buttons )
    mp_layout->removeWidget( w );
}

void GuiEmoticonWidget::paintEmoticonButtons( int box_width )
{
  if( m_buttons.isEmpty() )
    return;

  int emoticon_button_size = emoticonButtonSize().width();
  int x = 0;
  int y = 0;
  int max_icons_per_row = qMax( 2, static_cast<int>((box_width-2) / emoticon_button_size) ) - 1;

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

  // tells to scroll area the size of the widget
  setMinimumSize( QSize( (max_icons_per_row+1) * emoticonButtonSize().width() + 1, (y+1) * emoticonButtonSize().height() + 1 ) );
}
