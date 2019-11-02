//////////////////////////////////////////////////////////////////////
//
// BeeBEEP Copyright (C) 2010-2019 Marco Mastroddi
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

#include "AudioManager.h"
#include "BeeUtils.h"
#include "GuiRecordVoiceMessageSettings.h"
#include "IconManager.h"
#include "Settings.h"


GuiRecordVoiceMessageSettings::GuiRecordVoiceMessageSettings( QWidget *parent )
  : QDialog( parent ), mp_bgEncodingMode( Q_NULLPTR ), m_currentInputDevice(), m_currentFileContainer(), m_currentAudioEncoderSettings()
{
  setupUi( this );
  setObjectName( "GuiRecordVoiceMessageSettings" );
  setWindowTitle( tr( "Voice encoder settings" ) + QString( " - %1" ).arg( Settings::instance().programName() ) );
  setWindowIcon( IconManager::instance().icon( "audio-settings.png" ) );
  Bee::removeContextHelpButton( this );

  mp_cbUseCustomSettings->setText( QString( "%1 (%2)" ).arg( tr( "Use custom encoder settings" ) ).arg( tr( "expert users only" ) ) );
  mp_bgEncodingMode = new QButtonGroup( this );
  mp_bgEncodingMode->setExclusive( true );
  mp_bgEncodingMode->addButton( mp_rbConstantQuality, static_cast<int>( QMultimedia::ConstantQualityEncoding ) );
  mp_bgEncodingMode->addButton( mp_rbConstantBitRate, static_cast<int>( QMultimedia::ConstantBitRateEncoding ) );
  mp_bgEncodingMode->addButton( mp_rbAverageBitRate, static_cast<int>( QMultimedia::AverageBitRateEncoding ) );
  mp_bgEncodingMode->addButton( mp_rbTwoPass, static_cast<int>( QMultimedia::TwoPassEncoding ) );

  mp_audioRecorder = new QAudioRecorder( this );

  connect( mp_cbUseCustomSettings, SIGNAL( toggled( bool ) ), this, SLOT( toggleCustomSettings( bool ) ) );
  connect( mp_bgEncodingMode, SIGNAL( buttonClicked( int ) ), this, SLOT( encodingModeChanged( int ) ) );
  connect( mp_pbOk, SIGNAL( clicked() ), this, SLOT( saveSettings() ) );
  connect( mp_pbCancel, SIGNAL( clicked() ), this, SLOT( reject() ) );
  connect( mp_pbReset, SIGNAL( clicked() ), this, SLOT( resetSettings() ) );
}

void GuiRecordVoiceMessageSettings::init()
{
  mp_comboInputDevice->addItem( tr( "Default" ), QVariant( QString() ) );
  foreach( const QString& input_device, mp_audioRecorder->audioInputs() )
    mp_comboInputDevice->addItem( input_device, QVariant( input_device ) );

  mp_comboAudioCodec->addItem( tr( "Default" ), QVariant( QString() ) );
  foreach( const QString& codec_name, mp_audioRecorder->supportedAudioCodecs() )
    mp_comboAudioCodec->addItem( codec_name, QVariant( codec_name ) );

  mp_comboContainer->addItem( tr( "Default" ), QVariant( QString() ) );
  foreach( const QString& container_name, mp_audioRecorder->supportedContainers() )
    mp_comboContainer->addItem( container_name, QVariant( container_name ) );

  mp_comboChannels->addItem( tr( "Default" ), QVariant( -1 ) );
  mp_comboChannels->addItem( QString( "1" ), QVariant( 1 ) );
  mp_comboChannels->addItem( QString( "2" ), QVariant( 2 ) );
  mp_comboChannels->addItem( QString( "4" ), QVariant( 4 ) );

  mp_comboBitrate->addItem( tr( "Default" ), QVariant( 0 ) );
  mp_comboBitrate->addItem( QString( "32000" ), QVariant( 32000 ) );
  mp_comboBitrate->addItem( QString( "64000" ), QVariant( 64000 ) );
  mp_comboBitrate->addItem( QString( "96000" ), QVariant( 96000 ) );
  mp_comboBitrate->addItem( QString( "128000" ), QVariant( 128000 ) );

  mp_sliderQuality->setRange( 0, static_cast<int>( QMultimedia::VeryHighQuality ) );
  mp_sliderQuality->setValue( static_cast<int>( QMultimedia::NormalQuality ) );
}

void GuiRecordVoiceMessageSettings::encodingModeChanged( int button_clicked )
{
  if( button_clicked == static_cast<int>( QMultimedia::ConstantQualityEncoding ) )
  {
    mp_comboBitrate->setEnabled( false );
    mp_sliderQuality->setEnabled( true );
  }
  else if( button_clicked == static_cast<int>( QMultimedia::ConstantBitRateEncoding ) )
  {
    mp_comboBitrate->setEnabled( true );
    mp_sliderQuality->setEnabled( false );
  }
  else if( button_clicked == static_cast<int>( QMultimedia::AverageBitRateEncoding ) )
  {
    mp_comboBitrate->setEnabled( false );
    mp_sliderQuality->setEnabled( false );
  }
  else if( button_clicked == static_cast<int>( QMultimedia::TwoPassEncoding ) )
  {
    mp_comboBitrate->setEnabled( false );
    mp_sliderQuality->setEnabled( false );
  }
  else
  {
#ifdef BEEBEEP_DEBUG
    qWarning() << "Invalid button id" << button_clicked << "clicked in GuiRecordVoiceMessageSettings::encodingModeChanged(...)";
#endif
  }
}

void GuiRecordVoiceMessageSettings::toggleCustomSettings( bool )
{
  mp_gbVoiceRecorder->setEnabled( mp_cbUseCustomSettings->isChecked() );
  mp_gbEncodingMode->setEnabled( mp_cbUseCustomSettings->isChecked() );
}

void GuiRecordVoiceMessageSettings::loadSettings()
{
  mp_cbUseSystemEncoder->setChecked( Settings::instance().useSystemVoiceEncoderSettings() );
  mp_cbUseCustomSettings->setChecked( Settings::instance().useCustomVoiceEncoderSettings() );
  m_currentInputDevice = AudioManager::instance().voiceInputDeviceName();
  m_currentFileContainer = AudioManager::instance().voiceMessageFileContainer();
  m_currentAudioEncoderSettings = AudioManager::instance().voiceMessageEncoderSettings();
  updateGui();
}

void GuiRecordVoiceMessageSettings::updateGui()
{
  mp_comboSampleRate->clear();
  mp_comboSampleRate->addItem( tr( "Default" ), QVariant( 0 ) );
  foreach( int sample_rate, mp_audioRecorder->supportedAudioSampleRates( m_currentAudioEncoderSettings ) )
    mp_comboSampleRate->addItem( QString::number( sample_rate ), QVariant( sample_rate ) );
  if( !Bee::selectComboBoxData( mp_comboInputDevice, m_currentInputDevice ) )
    mp_comboInputDevice->setCurrentIndex( 0 );
  if( !Bee::selectComboBoxData( mp_comboContainer, m_currentFileContainer ) )
    mp_comboContainer->setCurrentIndex( 0 );
  if( !Bee::selectComboBoxData( mp_comboAudioCodec, m_currentAudioEncoderSettings.codec() ) )
    mp_comboAudioCodec->setCurrentIndex( 0 );
  if( !Bee::selectComboBoxData( mp_comboSampleRate, m_currentAudioEncoderSettings.sampleRate() ) )
    mp_comboSampleRate->setCurrentIndex( 0 );
  if( !Bee::selectComboBoxData( mp_comboChannels, m_currentAudioEncoderSettings.channelCount() ) )
    mp_comboChannels->setCurrentIndex( 0 );
  if( !Bee::selectComboBoxData( mp_comboBitrate, m_currentAudioEncoderSettings.bitRate() ) )
    mp_comboBitrate->setCurrentIndex( 0 );
  mp_sliderQuality->setValue( static_cast<int>( m_currentAudioEncoderSettings.quality() ) );
  int button_id = static_cast<int>( m_currentAudioEncoderSettings.encodingMode() );
  QAbstractButton* p_button = mp_bgEncodingMode->button( button_id );
  if( p_button )
  {
    p_button->setChecked( true );
    encodingModeChanged( button_id );
  }
  toggleCustomSettings( mp_cbUseCustomSettings->isChecked() );
}

void GuiRecordVoiceMessageSettings::saveSettings()
{
  m_currentInputDevice = Bee::comboBoxData( mp_comboInputDevice ).toString();
  Settings::instance().setVoiceInputDeviceName( !m_currentInputDevice.isEmpty() && m_currentInputDevice == AudioManager::instance().defaultInputDeviceName() ? QString() : m_currentInputDevice );
  m_currentFileContainer = Bee::comboBoxData( mp_comboContainer ).toString();
  Settings::instance().setVoiceFileMessageContainer( !m_currentFileContainer.isEmpty() && m_currentFileContainer == AudioManager::instance().voiceMessageFileContainer() ? QString() : m_currentFileContainer );
  QAudioEncoderSettings aes;
  aes.setCodec( Bee::comboBoxData( mp_comboAudioCodec ).toString() );
  aes.setSampleRate( Bee::comboBoxData( mp_comboSampleRate ).toInt());
  aes.setBitRate( Bee::comboBoxData( mp_comboBitrate ).toInt() );
  aes.setChannelCount( Bee::comboBoxData( mp_comboChannels ).toInt() );
  aes.setEncodingMode( static_cast<QMultimedia::EncodingMode>( mp_bgEncodingMode->checkedId() ) );
  aes.setQuality( static_cast<QMultimedia::EncodingQuality>( mp_sliderQuality->value() ) );
  m_currentAudioEncoderSettings = aes;
  Settings::instance().setVoiceCodec( aes.codec() == AudioManager::instance().defaultVoiceMessageEncoderSettings().codec() ? QString() : aes.codec() );
  Settings::instance().setVoiceSampleRate( aes.sampleRate() == AudioManager::instance().defaultVoiceMessageEncoderSettings().sampleRate() ? 0 : aes.sampleRate() );
  Settings::instance().setVoiceBitRate( aes.bitRate() == AudioManager::instance().defaultVoiceMessageEncoderSettings().bitRate() ? 0 : aes.bitRate() );
  Settings::instance().setVoiceChannels( aes.channelCount() == AudioManager::instance().defaultVoiceMessageEncoderSettings().channelCount() ? -1 : aes.channelCount() );
  Settings::instance().setVoiceEncodingMode( aes.encodingMode() == AudioManager::instance().defaultVoiceMessageEncoderSettings().encodingMode() ? -1 : aes.encodingMode() );
  Settings::instance().setVoiceEncodingQuality( aes.quality() == AudioManager::instance().defaultVoiceMessageEncoderSettings().quality() ? -1 : aes.quality() );
  Settings::instance().setUseSystemVoiceEncoderSettings( mp_cbUseSystemEncoder->isChecked() );
  Settings::instance().setUseCustomVoiceEncoderSettings( mp_cbUseCustomSettings->isChecked() );
  accept();
}

void GuiRecordVoiceMessageSettings::resetSettings()
{
  mp_cbUseCustomSettings->setChecked( false );
  m_currentInputDevice = AudioManager::instance().defaultInputDeviceName();
  m_currentFileContainer = AudioManager::instance().voiceMessageFileContainer();
  m_currentAudioEncoderSettings = AudioManager::instance().defaultVoiceMessageEncoderSettings();
  updateGui();
}

