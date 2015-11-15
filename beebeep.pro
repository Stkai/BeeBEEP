include(beebeep.pri)

TEMPLATE = subdirs

SUBDIRS += src plugins

CONFIG += ordered

TRANSLATIONS += locale/beebeep_xx.ts \
                locale/beebeep_it.ts \
                locale/beebeep_de.ts \
                locale/beebeep_ru.ts \
                locale/beebeep_nb.ts \
                locale/beebeep_nl.ts \
                locale/beebeep_fr.ts \
                locale/beebeep_cz.ts \
                locale/beebeep_sv.ts
