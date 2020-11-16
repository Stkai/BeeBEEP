
BEEBEEP_LOCALE_FOLDER= $$PWD
INCLUDEPATH += $$BEEBEEP_LOCALE_FOLDER
DEPENDPATH += $$BEEBEEP_LOCALE_FOLDER

BEEBEEP_TRANSLATIONS = $$BEEBEEP_LOCALE_FOLDER/beebeep_xx.ts \
                $$BEEBEEP_LOCALE_FOLDER/beebeep_it.ts \
                $$BEEBEEP_LOCALE_FOLDER/beebeep_de.ts \
                $$BEEBEEP_LOCALE_FOLDER/beebeep_en.ts \
                $$BEEBEEP_LOCALE_FOLDER/beebeep_rs.ts \
                $$BEEBEEP_LOCALE_FOLDER/beebeep_ru.ts \
                $$BEEBEEP_LOCALE_FOLDER/beebeep_nb.ts \
                $$BEEBEEP_LOCALE_FOLDER/beebeep_nl.ts \
                $$BEEBEEP_LOCALE_FOLDER/beebeep_fr.ts \
                $$BEEBEEP_LOCALE_FOLDER/beebeep_cz.ts \
                $$BEEBEEP_LOCALE_FOLDER/beebeep_pt.ts \
                $$BEEBEEP_LOCALE_FOLDER/beebeep_sv.ts \
                $$BEEBEEP_LOCALE_FOLDER/beebeep_es.ts \
                $$BEEBEEP_LOCALE_FOLDER/beebeep_hu.ts \
                $$BEEBEEP_LOCALE_FOLDER/beebeep_pl.ts \
                $$BEEBEEP_LOCALE_FOLDER/beebeep_vi.ts \
                $$BEEBEEP_LOCALE_FOLDER/beebeep_ua.ts \
                $$BEEBEEP_LOCALE_FOLDER/beebeep_tr.ts \
                $$BEEBEEP_LOCALE_FOLDER/beebeep_tw.ts \
                $$BEEBEEP_LOCALE_FOLDER/beebeep_cn.ts \
                $$BEEBEEP_LOCALE_FOLDER/beebeep_ar.ts \
                $$BEEBEEP_LOCALE_FOLDER/beebeep_ja.ts \
                $$BEEBEEP_LOCALE_FOLDER/beebeep_sk.ts \
                $$BEEBEEP_LOCALE_FOLDER/beebeep_bg.ts


isEmpty(QMAKE_LRELEASE) {
  win32|os2:QMAKE_LRELEASE = $$[QT_INSTALL_BINS]\\lrelease.exe
  else:QMAKE_LRELEASE = $$[QT_INSTALL_BINS]/lrelease
  unix {
    !exists($$QMAKE_LRELEASE) { QMAKE_LRELEASE = lrelease-qt4 }
  } else {
    !exists($$QMAKE_LRELEASE) { QMAKE_LRELEASE = lrelease }
  }
}

updateqm.input = BEEBEEP_TRANSLATIONS
updateqm.output = $$BEEBEEP_LOCALE_FOLDER/${QMAKE_FILE_BASE}.qm
updateqm.commands = $$QMAKE_LRELEASE -silent ${QMAKE_FILE_IN} -qm $$BEEBEEP_LOCALE_FOLDER/${QMAKE_FILE_BASE}.qm
updateqm.CONFIG += no_link target_predeps


#CONFIG(release,debug|release) {
#  QMAKE_EXTRA_COMPILERS += updateqm
#}

