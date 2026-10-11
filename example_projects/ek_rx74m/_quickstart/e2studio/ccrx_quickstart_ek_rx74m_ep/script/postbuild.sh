#!/bin/sh

CONFIGURATION_XML_FILE="../configuration.xml"
CPROJECT_XML_FILE="../.cproject"

if grep -Fq -- 'Renesas_RXC' "$CONFIGURATION_XML_FILE" && grep -Fq -- '_RX_PZ_SECURE' "$CPROJECT_XML_FILE"; then
  touch "secure.o"
fi
