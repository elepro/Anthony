/*
 * ps3mca-tool - PlayStation 3 Memory Card Adaptor Software
 * Copyright (C) 2011 - jimmikaelkael <jimmikaelkael@wanadoo.fr>
 * Copyright (C) 2011 - "someone who wants to stay anonymous"
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef __MECHAEMU_H__
#define __MECHAEMU_H__

#include <memory.h>
#include <stdlib.h>
#include <inttypes.h>

void meResetCryptoContext(void);
void meCardCalcUniqueKey(uint8_t *CardIV, uint8_t *CardMaterial);
void meCardGenerateChallenge(uint8_t *CardIV, uint8_t *CardNonce,
                             uint8_t *MechaNonce,
                             uint8_t *MechaChallenge1, uint8_t *MechaChallenge2, uint8_t *MechaChallenge3);
int meCardVerifyResponse(uint8_t *CardResponse1, uint8_t *CardResponse2, uint8_t *CardResponse3);
int meGetContentKeyOffset(uint8_t *KelfHeader);
void meDecryptDiskContentKey(uint8_t *KelfHeader);
void meEncryptCardContentKey(uint8_t *ContentKey);

#endif

