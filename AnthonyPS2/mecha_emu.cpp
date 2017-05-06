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

#include "mecha_emu.h"
#include "util.h"
#include "cipher.h"

#include <stdio.h>

#define _CrtDbgBreak() __asm__ ("\tint $0x3\n")

#pragma pack(1)

struct CryptoContext {
	uint8_t UniqueKey[16];
	uint8_t FileKey[16];
	uint8_t CardIV[8];
	uint8_t CardMaterial[8];
	uint8_t CardNonce[8];
	uint8_t MechaNonce[8];
	uint8_t SessionKey[8];
	uint8_t DecryptedKbit[16];
	uint8_t DecryptedKc[16];
	uint8_t EncryptedKbit[16];
	uint8_t EncryptedKc[16];
}/* __attribute__((packed))*/;

struct Kelf_Header {
	uint32_t unknown1;             
	uint32_t unknown2;
	uint16_t unknown3_half;
	uint16_t version;
	uint32_t unknown4;
	uint32_t ELF_size;		/* Size of data blocks = Decrypted elf size */
	uint16_t Kelf_header_size;	/* Kelf header size                         */
	uint16_t unknown5;
	uint16_t flags;			/* ? for header purpose                     */
	uint16_t BIT_count;		/* used to set/get kbit and kc offset       */
	uint32_t mg_zones;
}/* __attribute__((packed))*/;
#pragma pack()

static struct CryptoContext context;

static const uint8_t MC_CARDKEY_MATERIAL_1[] = { 0x6C, 0x26, 0xD3, 0x7F, 0x46, 0xEE, 0x9D, 0xA9 };
static const uint8_t MC_CARDKEY_HASHKEY_1[]  = { 0xCE, 0x62, 0xF6, 0x84, 0x20, 0xB6, 0x5A, 0x81, 
						 0xE4, 0x59, 0xFA, 0x9A, 0x2B, 0xB3, 0x59, 0x8A };
static const uint8_t MC_CARDKEY_MATERIAL_2[] = { 0x2C, 0xD1, 0x60, 0xFA, 0x8C, 0x2E, 0xD3, 0x62 };
static const uint8_t MC_CARDKEY_HASHKEY_2[]  = { 0x70, 0x14, 0xA3, 0x2F, 0xCC, 0x5B, 0x12, 0x37, 
						 0xAC, 0x1F, 0xBF, 0x4E, 0xD2, 0x6D, 0x1C, 0xC1 };
static const uint8_t MC_CHALLENGE_MATERIAL[] = { 0x2C, 0x5B, 0xF4, 0x8D, 0x32, 0x74, 0x91, 0x27 };

static const uint8_t MG_KBIT_MATERIAL[8]     = { 0x35, 0xC8, 0x60, 0x01, 0x92, 0x22, 0xBB, 0x60 };
static const uint8_t MG_KC_MATERIAL[8]       = { 0x8C, 0x2B, 0xD0, 0x3E, 0xC2, 0x45, 0xC5, 0x6D };
static const uint8_t MG_KBIT_MASTER_KEY[16]  = { 0x50, 0x01, 0xC8, 0x71, 0x21, 0xF9, 0x39, 0xC1,
						 0x44, 0xD8, 0x6B, 0x06, 0x92, 0x24, 0xB2, 0x47 };
static const uint8_t MG_KC_MASTER_KEY[16]    = { 0x77, 0xF3, 0x83, 0x14, 0xB0, 0x47, 0xD8, 0x7C,
						 0x9B, 0x37, 0xD2, 0x66, 0x04, 0x92, 0x28, 0xC4 };

static const uint8_t MG_IV_NULL[8]           = { 0 };


/*
 * meResetCryptoContext: reset the crypto context
 */
void meResetCryptoContext(void)
{
	memset((void *)&context, 0, sizeof(struct CryptoContext));
}

/*
 * meCardCalcUniqueKey: calculate unique key for memory card authentication.
 */
void meCardCalcUniqueKey(uint8_t *CardIV, uint8_t *CardMaterial)
{
	uint8_t Input[8];

	memxor(CardIV, CardMaterial, Input, 8);

	cipherCbcEncrypt(&context.UniqueKey[0], Input, 8, MC_CARDKEY_HASHKEY_1, 2, MC_CARDKEY_MATERIAL_1);
	cipherCbcEncrypt(&context.UniqueKey[8], Input, 8, MC_CARDKEY_HASHKEY_2, 2, MC_CARDKEY_MATERIAL_2);

	memcpy(context.CardIV, CardIV, 8);
	memcpy(context.CardMaterial, CardMaterial, 8);
}

/*
 * meCardGenerateChallenge: generate challenge for memory card.
 */
void meCardGenerateChallenge(uint8_t *CardIV, uint8_t *CardNonce, uint8_t *MechaNonce,
                             uint8_t *MechaChallenge1, uint8_t *MechaChallenge2, uint8_t *MechaChallenge3)
{
	cipherCbcEncrypt(MechaChallenge1, MechaNonce, 8, context.UniqueKey, 2, MC_CHALLENGE_MATERIAL);
	cipherCbcEncrypt(MechaChallenge2, CardNonce,  8, context.UniqueKey, 2, MechaChallenge1);
	cipherCbcEncrypt(MechaChallenge3, CardIV,     8, context.UniqueKey, 2, MechaChallenge2);

	memcpy(context.CardNonce, CardNonce, 8);
	memcpy(context.MechaNonce, MechaNonce, 8);
}

/*
 * meCardVerifyResponse: verify response from memory card.
 */
int meCardVerifyResponse(uint8_t *CardResponse1, uint8_t *CardResponse2, uint8_t *CardResponse3)
{
	uint8_t DecryptedResponse1[8], DecryptedResponse2[8], DecryptedResponse3[8];
	int r = -1;

	cipherCbcDecrypt(DecryptedResponse1, CardResponse1, 8, context.UniqueKey, 2, MC_CHALLENGE_MATERIAL);
	cipherCbcDecrypt(DecryptedResponse2, CardResponse2, 8, context.UniqueKey, 2, CardResponse1);
	cipherCbcDecrypt(DecryptedResponse3, CardResponse3, 8, context.UniqueKey, 2, CardResponse2);

	if (!memcmp(DecryptedResponse1, context.CardNonce, 8) && !memcmp(DecryptedResponse2, context.MechaNonce, 8)) {
		memcpy(context.SessionKey, DecryptedResponse3, 8);
		r = 0;
	}

	return  r;
}

/*
 * meGetContentKeyOffset: calculate ContentKey offset in a kelf
 */
int meGetContentKeyOffset(uint8_t *KelfHeader)
{
	int i, offset;
	struct Kelf_Header *hdr = (struct Kelf_Header *)KelfHeader;

	offset = 32;

	for (i=0; i<read_le_uint16((uint8_t *)&hdr->BIT_count); i++)
		offset += 16;

	if ((read_le_uint32((uint8_t *)&hdr->flags) & 1) != 0)
		offset += KelfHeader[offset] + 1;

	if ((read_le_uint32((uint8_t *)&hdr->flags) & 0xf000) == 0)
		offset += 8;

	return offset;
}

/*
 * meDecryptDiskContentKey: Decrypt the encrypted ContentKey for disk/rom
 * from a given Kelf header
 */
void meDecryptDiskContentKey(uint8_t *KelfHeader)
{
	int CK_offset;
	uint8_t HeaderData[8];

	CK_offset = meGetContentKeyOffset(KelfHeader);

	/* calculate file keys necessary to decrypt Kbit and Kc from MG header */
	memxor(KelfHeader, &KelfHeader[8], HeaderData, 8);
	cipherCbcEncrypt(&context.FileKey[0], HeaderData, 8, MG_KBIT_MASTER_KEY, 2, MG_KBIT_MATERIAL);
	cipherCbcEncrypt(&context.FileKey[8], HeaderData, 8, MG_KC_MASTER_KEY,   2, MG_KC_MATERIAL);	
	memcpy(context.EncryptedKbit, &KelfHeader[CK_offset], 16);
	memcpy(context.EncryptedKc, &KelfHeader[CK_offset+16], 16);

	/* finally Decrypt the ContentKey */
	cipherCbcDecrypt(&context.DecryptedKbit[0], &context.EncryptedKbit[0], 8, context.FileKey, 2, MG_IV_NULL);
	cipherCbcDecrypt(&context.DecryptedKbit[8], &context.EncryptedKbit[8], 8, context.FileKey, 2, MG_IV_NULL);
	cipherCbcDecrypt(&context.DecryptedKc[0],   &context.EncryptedKc[0],   8, context.FileKey, 2, MG_IV_NULL);
	cipherCbcDecrypt(&context.DecryptedKc[8],   &context.EncryptedKc[8],   8, context.FileKey, 2, MG_IV_NULL);
}

/*
 * meEncryptCardContentKey: encrypt the decrypted ContentKey for Memory Card
 */
void meEncryptCardContentKey(uint8_t *ContentKey)
{
	cipherCbcEncrypt(&context.EncryptedKbit[0], &context.DecryptedKbit[0], 8, context.SessionKey, 1, MG_IV_NULL);
	cipherCbcEncrypt(&context.EncryptedKbit[8], &context.DecryptedKbit[8], 8, context.SessionKey, 1, MG_IV_NULL);
	cipherCbcEncrypt(&context.EncryptedKc[0],   &context.DecryptedKc[0],   8, context.SessionKey, 1, MG_IV_NULL);
	cipherCbcEncrypt(&context.EncryptedKc[8],   &context.DecryptedKc[8],   8, context.SessionKey, 1, MG_IV_NULL);

	memcpy(&ContentKey[0], context.EncryptedKbit, 16);
	memcpy(&ContentKey[16], context.EncryptedKc, 16);
}

