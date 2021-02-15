/*****************************************************************************
  AUTO-GENERATED CODE:  Graphics Resource Converter version: 4.02.00

  Company:
      Microchip Technology, Inc.
 
  File Name:
     internal_resource_an1182_reference.c
 
  Summary:
      This file is generated by the Microchip's Graphics Resource Converter.
  Description:
      This file is generated by the Graphics Resource Converter containing
      resources such as images and fonts that can be used by Microchip's
      Graphics Library, which is part of the MLA.
 *****************************************************************************/

// DOM-IGNORE-BEGIN
/*****************************************************************************
  Software License Agreement

  Copyright(c) 2013 Microchip Technology Inc.  All rights reserved.
  Microchip licenses to you the right to use, modify, copy and distribute
  Software only when embedded on a Microchip microcontroller or digital
  signal controller that is integrated into your product or third party
  product (pursuant to the sublicense terms in the accompanying license
  agreement).
 
  You should refer to the license agreement accompanying this Software
  for additional information regarding your rights and obligations.
 
  SOFTWARE AND DOCUMENTATION ARE PROVIDED “AS IS” WITHOUT WARRANTY OF ANY
  KIND, EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION, ANY WARRANTY
  OF MERCHANTABILITY, TITLE, NON-INFRINGEMENT AND FITNESS FOR A PARTICULAR
  PURPOSE. IN NO EVENT SHALL MICROCHIP OR ITS LICENSORS BE LIABLE OR
  OBLIGATED UNDER CONTRACT, NEGLIGENCE, STRICT LIABILITY, CONTRIBUTION,
  BREACH OF WARRANTY, OR OTHER LEGAL EQUITABLE THEORY ANY DIRECT OR INDIRECT
  DAMAGES OR EXPENSES INCLUDING BUT NOT LIMITED TO ANY INCIDENTAL, SPECIAL,
  INDIRECT, PUNITIVE OR CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA,
  COST OF PROCUREMENT OF SUBSTITUTE GOODS, TECHNOLOGY, SERVICES, OR ANY
  CLAIMS BY THIRD PARTIES (INCLUDING BUT NOT LIMITED TO ANY DEFENSE THEREOF),
  OR OTHER SIMILAR COSTS.
 *****************************************************************************/
// DOM-IGNORE-END

/*****************************************************************************
 * SECTION:  Includes
 *****************************************************************************/
#include <gfx/gfx.h>
#include <stdint.h>

/*****************************************************************************
 * SECTION:  Graphics Library Format Check
 *****************************************************************************/
#ifndef GFX_RESOURCE_FORMAT_VERSION
    #if(GRAPHICS_LIBRARY_VERSION != 0x0401)
    #warning "It is suggested to use Graphics Library version 4.01 or later with this version of GRC."
    #endif
#elif(GFX_RESOURCE_FORMAT_VERSION != 0x0101)
#warning "It is suggested to use Graphics Library with the resource format version 1.01 with this version of GRC."
#endif

/*****************************************************************************
 * SECTION:  Graphics Bitmap Padding Check
 *****************************************************************************/
#ifdef GFX_CONFIG_IMAGE_PADDING_DISABLE
#error These bitmap resources have been converted with padding of the horizontal lines, but GFX_CONFIG_IMAGE_PADDING_DISABLE is defined in system_config.h.
#endif


/*****************************************************************************
 * SECTION:  Graphics Configuration: Bitmap Check
 *****************************************************************************/

#ifdef GFX_CONFIG_IMAGE_FLASH_DISABLE
#error The GFX_CONFIG_IMAGE_FLASH_DISABLE is defined in system_config.h header file.
#endif

/*****************************************************************************
 * SECTION:  Graphics Configuration: Font Check
 *****************************************************************************/

#ifdef GFX_CONFIG_FONT_FLASH_DISABLE
#error The GFX_CONFIG_FONT_FLASH_DISABLE is defined in system_config.h header file.
#endif


/*****************************************************************************
 * SECTION:  ChineseFont - Phrases
 *****************************************************************************/

const GFX_XCHAR ChineseStr[] = {   0x0022, 0x0023, 0x0021, 0x0024, 0x0020, 0x0000    };    // In Chinese

/*****************************************************************************
 * SECTION:  JapaneseFont - Phrases
 *****************************************************************************/

const GFX_XCHAR JapaneseStr[] = {   0x0027, 0x0024, 0x0022, 0x0028, 0x0021, 0x0028, 0x0025, 0x0023, 0x0026, 0x0020, 0x0000    };    // In Japanese

/*****************************************************************************
 * SECTION:  HindiFont - Phrases
 *****************************************************************************/

const GFX_XCHAR HindiStr[] = {   0x0024, 0x0023, 0x0028, 0x0025, 0x0020, 0x0027, 0x0020, 0x0022, 0x0026, 0x0029, 0x0026, 0x0021, 0x0000    };    // Hindi Font: Jaipur

/*****************************************************************************
 * SECTION:  ThaiFont - Phrases
 *****************************************************************************/

const GFX_XCHAR ThaiStr[] = {   0x0026, 0x0025, 0x0027, 0x0026, 0x0023, 0x0029, 0x0022, 0x0028, 0x0025, 0x002A, 0x0024, 0x0021, 0x0020, 0x0000    };    // Thai Font: DB Thai Text

/*****************************************************************************
 * SECTION:  ChineseFont - Unicode to GRC Index Table
 *****************************************************************************/

#ifdef GFX_CONFIG_UNICODE_INDEX_TABLE_ENABLE
const uint16_t chinesefontUnicodeIndexTable[5][2] =
{
//    Unicode   GRC Index
//    -------   ---------
    {  0x0021,   0x0020  },
    {  0x4E16,   0x0021  },
    {  0x4F60,   0x0022  },
    {  0x597D,   0x0023  },
    {  0x754C,   0x0024  },
};
#endif
/*****************************************************************************
 * SECTION:  JapaneseFont - Unicode to GRC Index Table
 *****************************************************************************/

#ifdef GFX_CONFIG_UNICODE_INDEX_TABLE_ENABLE
const uint16_t japanesefontUnicodeIndexTable[9][2] =
{
//    Unicode   GRC Index
//    -------   ---------
    {  0x0021,   0x0020  },
    {  0x3053,   0x0021  },
    {  0x3055,   0x0022  },
    {  0x3061,   0x0023  },
    {  0x306A,   0x0024  },
    {  0x306B,   0x0025  },
    {  0x306F,   0x0026  },
    {  0x307F,   0x0027  },
    {  0x3093,   0x0028  },
};
#endif
/*****************************************************************************
 * SECTION:  HindiFont - Unicode to GRC Index Table
 *****************************************************************************/

#ifdef GFX_CONFIG_UNICODE_INDEX_TABLE_ENABLE
const uint16_t hindifontUnicodeIndexTable[10][2] =
{
//    Unicode   GRC Index
//    -------   ---------
    {  0x0020,   0x0020  },
    {  0x0021,   0x0021  },
    {  0x0069,   0x0022  },
    {  0x006D,   0x0023  },
    {  0x006E,   0x0024  },
    {  0x0074,   0x0025  },
    {  0x0076,   0x0026  },
    {  0x0077,   0x0027  },
    {  0x00DF,   0x0028  },
    {  0x2248,   0x0029  },
};
#endif
/*****************************************************************************
 * SECTION:  ThaiFont - Unicode to GRC Index Table
 *****************************************************************************/

#ifdef GFX_CONFIG_UNICODE_INDEX_TABLE_ENABLE
const uint16_t thaifontUnicodeIndexTable[11][2] =
{
//    Unicode   GRC Index
//    -------   ---------
    {  0x0021,   0x0020  },
    {  0x00A1,   0x0021  },
    {  0x00AA,   0x0022  },
    {  0x00B4,   0x0023  },
    {  0x00C5,   0x0024  },
    {  0x00C7,   0x0025  },
    {  0x00CA,   0x0026  },
    {  0x00D1,   0x0027  },
    {  0x00D2,   0x0028  },
    {  0x00D5,   0x0029  },
    {  0x00E2,   0x002A  },
};
#endif
/*****************************************************************************
 * SECTION:  BITMAPS
 *****************************************************************************/

/*********************************
 * Bitmap Structure
 * Label: redLArrow
 * Description:  45x40 pixels, 4-bits per pixel
 ***********************************/
#if defined (GFX_CONFIG_IMAGE_PACKED_ENABLE) && (__XC16_VERSION__ >= 1011)
extern uint8_gfx_image_prog __redLArrow[] __attribute__((space(prog), section("grc_images")));
#else
extern uint8_gfx_image_prog __redLArrow[] __attribute__((space(prog), section("grc_images"), aligned(2)));
#endif

const GFX_RESOURCE_HDR redLArrow =
{
    .type = (GFX_RESOURCE_MEMORY_FLASH | GFX_RESOURCE_TYPE_MCHP_MBITMAP | GFX_RESOURCE_COMP_NONE),
    .ID = 0,
    .resource.image.location.progByteAddress = (uint8_gfx_image_prog *) __redLArrow,
    .resource.image.width = 45,
    .resource.image.height = 40,
    .resource.image.parameter1.compressedSize = 0,
    .resource.image.parameter2.rawSize = 956,
    .resource.image.colorDepth = 4,
    .resource.image.type = 0x00,
    .resource.image.paletteID = 0x0000
};

/*********************************
 * Bitmap Structure
 * Label: redRArrow
 * Description:  45x40 pixels, 4-bits per pixel
 ***********************************/
#if defined (GFX_CONFIG_IMAGE_PACKED_ENABLE) && (__XC16_VERSION__ >= 1011)
extern uint8_gfx_image_prog __redRArrow[] __attribute__((space(prog), section("grc_images")));
#else
extern uint8_gfx_image_prog __redRArrow[] __attribute__((space(prog), section("grc_images"), aligned(2)));
#endif

const GFX_RESOURCE_HDR redRArrow =
{
    .type = (GFX_RESOURCE_MEMORY_FLASH | GFX_RESOURCE_TYPE_MCHP_MBITMAP | GFX_RESOURCE_COMP_NONE),
    .ID = 0,
    .resource.image.location.progByteAddress = (uint8_gfx_image_prog *) __redRArrow,
    .resource.image.width = 45,
    .resource.image.height = 40,
    .resource.image.parameter1.compressedSize = 0,
    .resource.image.parameter2.rawSize = 956,
    .resource.image.colorDepth = 4,
    .resource.image.type = 0x00,
    .resource.image.paletteID = 0x0000
};

/*****************************************************************************
 * SECTION:  FONTS
 *****************************************************************************/

/*********************************
 * TTF Font File Structure
 * Label: ChineseFont
 * Description:  Height: 22 pixels, 2 bits per pixel, Range: '!' to '界'
 * Comment Block:
 * ========================================================================== 
 *  
 * Created from AR PL New Sung font (fireflysung.ttf). 
 *  
 * AR PL New Sung font is 
 *     Copyright (c) 1994-1999, Arphic Technology Co., Ltd. 
 *     Copyright (C) 1999-2004, Firefly and Arphic Technology Co., Ltd. 
 * All Rights Reserved. 
 *  
 * NOTICE: This Truetype font contains embedded bitmap fonts made 
 *     by firefly and is released as a whole 
 *     under the ARPHIC PUBLIC LICENSE. 
 *     There are also seperate bitmap fonts made by Firefly and 
 *     released under the GENERAL PUBLIC LICENSE (GPL): 
 *     http://www.study-area.org/apt/firefly-font/bitmaps/ 
 *  
 * Copyright: 
 *     (Copied from 'license/english/ARPHICPL.TXT'. 
 *     See 'license/big5/ARPHICPL.TXT' or 'license/gb/ARPHICPL.TXT' 
 *     for the Chinese version.) 
 *  
 * ========================================================================== 
 *  
 * ARPHIC PUBLIC LICENSE 
 *  
 * Copyright (C) 1999 Arphic Technology Co., Ltd. 
 * 11Fl. No.168, Yung Chi Rd., Taipei, 110 Taiwan 
 * All rights reserved except as specified below. 
 *  
 * Everyone is permitted to copy and distribute verbatim copies of this 
 * license document, but changing it is forbidden. 
 *  
 * Preamble 
 *  
 *    The licenses for most software are designed to take away your freedom 
 * to share and change it. By contrast, the ARPHIC PUBLIC LICENSE 
 * specifically permits and encourages you to use this software, provided 
 * that you give the recipients all the rights that we gave you and make 
 * sure they can get the modifications of this software. 
 *  
 * Legal Terms 
 *  
 * 0. Definitions: 
 *    Throughout this License, "Font" means the TrueType fonts "AR PL 
 * Mingti2L Big5", "AR PL KaitiM Big5" (BIG-5 character set) and "AR PL 
 * SungtiL GB", "AR PL KaitiM GB" (GB character set) which are originally 
 * distributed by Arphic, and the derivatives of those fonts created 
 * through any modification including modifying glyph, reordering glyph, 
 * converting format, changing font name, or adding/deleting some 
 * characters in/from glyph table. 
 *  
 *    "PL" means "Public License". 
 *  
 *    "Copyright Holder" means whoever is named in the copyright or 
 * copyrights for the Font. 
 *  
 *    "You" means the licensee, or person copying, redistributing or 
 * modifying the Font. 
 *  
 *    "Freely Available" means that you have the freedom to copy or modify 
 * the Font as well as redistribute copies of the Font under the same 
 * conditions you received, not price. If you wish, you can charge for this 
 * service. 
 *  
 * 1. Copying & Distribution 
 *    You may copy and distribute verbatim copies of this Font in any 
 * medium, without restriction, provided that you retain this license file 
 * (ARPHICPL.TXT) unaltered in all copies. 
 *  
 * 2. Modification 
 *    You may otherwise modify your copy of this Font in any way, including 
 * modifying glyph, reordering glyph, converting format, changing font 
 * name, or adding/deleting some characters in/from glyph table, and copy 
 * and distribute such modifications under the terms of Section 1 above, 
 * provided that the following conditions are met: 
 *  
 *    a) You must insert a prominent notice in each modified file stating 
 * how and when you changed that file. 
 *  
 *    b) You must make such modifications Freely Available as a whole to 
 * all third parties under the terms of this License, such as by offering 
 * access to copy the modifications from a designated place, or 
 * distributing the modifications on a medium customarily used for software 
 * interchange. 
 *  
 *    c) If the modified fonts normally reads commands interactively when 
 * run, you must cause it, when started running for such interactive use in 
 * the most ordinary way, to print or display an announcement including an 
 * appropriate copyright notice and a notice that there is no warranty (or 
 * else, saying that you provide a warranty) and that users may 
 * redistribute the Font under these conditions, and telling the user how 
 * to view a copy of this License. 
 *  
 *    These requirements apply to the modified work as a whole. If 
 * identifiable sections of that work are not derived from the Font, and 
 * can be reasonably considered independent and separate works in 
 * themselves, then this License and its terms, do not apply to those 
 * sections when you distribute them as separate works. Therefore, mere 
 * aggregation of another work not based on the Font with the Font on a 
 * volume of a storage or distribution medium does not bring the other work 
 * under the scope of this License. 
 *  
 * 3. Condition Subsequent 
 *    You may not copy, modify, sublicense, or distribute the Font except 
 * as expressly provided under this License. Any attempt otherwise to copy, 
 * modify, sublicense or distribute the Font will automatically 
 * retroactively void your rights under this License. However, parties who 
 * have received copies or rights from you under this License will keep 
 * their licenses valid so long as such parties remain in full compliance. 
 *  
 * 4. Acceptance 
 *    You are not required to accept this License, since you have not 
 * signed it. However, nothing else grants you permission to copy, modify, 
 * sublicense or distribute the Font. These actions are prohibited by law 
 * if you do not accept this License. Therefore, by copying, modifying, 
 * sublicensing or distributing the Font, you indicate your acceptance of 
 * this License and all its terms and conditions. 
 *  
 * 5. Automatic Receipt 
 *    Each time you redistribute the Font, the recipient automatically 
 * receives a license from the original licensor to copy, distribute or 
 * modify the Font subject to these terms and conditions. You may not 
 * impose any further restrictions on the recipients' exercise of the 
 * rights granted herein. You are not responsible for enforcing compliance 
 * by third parties to this License. 
 *  
 * 6. Contradiction 
 *    If, as a consequence of a court judgment or allegation of patent 
 * infringement or for any other reason (not limited to patent issues), 
 * conditions are imposed on you (whether by court order, agreement or 
 * otherwise) that contradict the conditions of this License, they do not 
 * excuse you from the conditions of this License. If you cannot distribute 
 * so as to satisfy simultaneously your obligations under this License and 
 * any other pertinent obligations, then as a consequence you may not 
 * distribute the Font at all. For example, if a patent license would not 
 * permit royalty-free redistribution of the Font by all those who receive 
 * copies directly or indirectly through you, then the only way you could 
 * satisfy both it and this License would be to refrain entirely from 
 * distribution of the Font. 
 *  
 *    If any portion of this section is held invalid or unenforceable under 
 * any particular circumstance, the balance of the section is intended to 
 * apply and the section as a whole is intended to apply in other 
 * circumstances. 
 *  
 * 7. NO WARRANTY 
 *    BECAUSE THE FONT IS LICENSED FREE OF CHARGE, THERE IS NO WARRANTY FOR 
 * THE FONT, TO THE EXTENT PERMITTED BY APPLICABLE LAW. EXCEPT WHEN 
 * OTHERWISE STATED IN WRITING THE COPYRIGHT HOLDERS OR OTHER PARTIES 
 * PROVIDE THE FONT "AS IS" WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESSED 
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF 
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. THE ENTIRE RISK AS 
 * TO THE QUALITY AND PERFORMANCE OF THE FONT IS WITH YOU. SHOULD THE FONT 
 * PROVE DEFECTIVE, YOU ASSUME THE COST OF ALL NECESSARY SERVICING, REPAIR 
 * OR CORRECTION. 
 *  
 * 8. DAMAGES WAIVER 
 *    UNLESS REQUIRED BY APPLICABLE LAW OR AGREED TO IN WRITING, IN NO 
 * EVENT WILL ANY COPYRIGHTT HOLDERS, OR OTHER PARTIES WHO MAY COPY, MODIFY 
 * OR REDISTRIBUTE THE FONT AS PERMITTED ABOVE, BE LIABLE TO YOU FOR ANY 
 * DIRECT, INDIRECT, CONSEQUENTIAL, INCIDENTAL, SPECIAL OR EXEMPLARY 
 * DAMAGES ARISING OUT OF THE USE OR INABILITY TO USE THE FONT (INCLUDING 
 * BUT NOT LIMITED TO PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF 
 * USE, DATA OR PROFITS; OR BUSINESS INTERRUPTION), EVEN IF SUCH HOLDERS OR 
 * OTHER PARTIES HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES. 
 *  
 * ====================================================================== 
 ***********************************/
#if defined(GFX_CONFIG_FONT_PROG_SPACE_ENABLE) && (__XC16_VERSION__ >= 1011)
#ifndef GFX_CONFIG_FONT_PACKED_ENABLE
extern GFX_FONT_SPACE char __ChineseFont[] __attribute__((space(prog), section("grc_text"), aligned(2)));
#else
extern GFX_FONT_SPACE char __ChineseFont[] __attribute__((space(prog), section("grc_text")));
#endif
#else
extern GFX_FONT_SPACE char __ChineseFont[] __attribute__((aligned(2)));
#endif

const GFX_RESOURCE_HDR ChineseFont = 
{
    .type = (GFX_RESOURCE_MEMORY_FLASH | GFX_RESOURCE_TYPE_FONT | GFX_RESOURCE_COMP_NONE),
    .ID = 0x0000,
    .resource.font.location.progByteAddress = (GFX_FONT_SPACE char *) __ChineseFont,
    .resource.font.header.fontID = 0,
    .resource.font.header.extendedGlyphEntry = 0,
    .resource.font.header.res1 = 0,
    .resource.font.header.bpp = 1,
    .resource.font.header.orientation = 0,
    .resource.font.header.res2 = 0,
    .resource.font.header.firstChar = 0x0020,
    .resource.font.header.lastChar = 0x0024,
    .resource.font.header.height = 0x0016,
};
/*********************************
 * TTF Font File Structure
 * Label: JapaneseFont
 * Description:  Height: 25 pixels, 2 bits per pixel, Range: '!' to 'ん'
 * Comment Block:
 * ========================================================================== 
 *  
 * Created from AR PL New Sung font (fireflysung.ttf). 
 *  
 * AR PL New Sung font is 
 *     Copyright (c) 1994-1999, Arphic Technology Co., Ltd. 
 *     Copyright (C) 1999-2004, Firefly and Arphic Technology Co., Ltd. 
 * All Rights Reserved. 
 *  
 * NOTICE: This Truetype font contains embedded bitmap fonts made 
 *     by firefly and is released as a whole 
 *     under the ARPHIC PUBLIC LICENSE. 
 *     There are also seperate bitmap fonts made by Firefly and 
 *     released under the GENERAL PUBLIC LICENSE (GPL): 
 *     http://www.study-area.org/apt/firefly-font/bitmaps/ 
 *  
 * Copyright: 
 *     (Copied from 'license/english/ARPHICPL.TXT'. 
 *     See 'license/big5/ARPHICPL.TXT' or 'license/gb/ARPHICPL.TXT' 
 *     for the Chinese version.) 
 *  
 * ========================================================================== 
 *  
 * ARPHIC PUBLIC LICENSE 
 *  
 * Copyright (C) 1999 Arphic Technology Co., Ltd. 
 * 11Fl. No.168, Yung Chi Rd., Taipei, 110 Taiwan 
 * All rights reserved except as specified below. 
 *  
 * Everyone is permitted to copy and distribute verbatim copies of this 
 * license document, but changing it is forbidden. 
 *  
 * Preamble 
 *  
 *    The licenses for most software are designed to take away your freedom 
 * to share and change it. By contrast, the ARPHIC PUBLIC LICENSE 
 * specifically permits and encourages you to use this software, provided 
 * that you give the recipients all the rights that we gave you and make 
 * sure they can get the modifications of this software. 
 *  
 * Legal Terms 
 *  
 * 0. Definitions: 
 *    Throughout this License, "Font" means the TrueType fonts "AR PL 
 * Mingti2L Big5", "AR PL KaitiM Big5" (BIG-5 character set) and "AR PL 
 * SungtiL GB", "AR PL KaitiM GB" (GB character set) which are originally 
 * distributed by Arphic, and the derivatives of those fonts created 
 * through any modification including modifying glyph, reordering glyph, 
 * converting format, changing font name, or adding/deleting some 
 * characters in/from glyph table. 
 *  
 *    "PL" means "Public License". 
 *  
 *    "Copyright Holder" means whoever is named in the copyright or 
 * copyrights for the Font. 
 *  
 *    "You" means the licensee, or person copying, redistributing or 
 * modifying the Font. 
 *  
 *    "Freely Available" means that you have the freedom to copy or modify 
 * the Font as well as redistribute copies of the Font under the same 
 * conditions you received, not price. If you wish, you can charge for this 
 * service. 
 *  
 * 1. Copying & Distribution 
 *    You may copy and distribute verbatim copies of this Font in any 
 * medium, without restriction, provided that you retain this license file 
 * (ARPHICPL.TXT) unaltered in all copies. 
 *  
 * 2. Modification 
 *    You may otherwise modify your copy of this Font in any way, including 
 * modifying glyph, reordering glyph, converting format, changing font 
 * name, or adding/deleting some characters in/from glyph table, and copy 
 * and distribute such modifications under the terms of Section 1 above, 
 * provided that the following conditions are met: 
 *  
 *    a) You must insert a prominent notice in each modified file stating 
 * how and when you changed that file. 
 *  
 *    b) You must make such modifications Freely Available as a whole to 
 * all third parties under the terms of this License, such as by offering 
 * access to copy the modifications from a designated place, or 
 * distributing the modifications on a medium customarily used for software 
 * interchange. 
 *  
 *    c) If the modified fonts normally reads commands interactively when 
 * run, you must cause it, when started running for such interactive use in 
 * the most ordinary way, to print or display an announcement including an 
 * appropriate copyright notice and a notice that there is no warranty (or 
 * else, saying that you provide a warranty) and that users may 
 * redistribute the Font under these conditions, and telling the user how 
 * to view a copy of this License. 
 *  
 *    These requirements apply to the modified work as a whole. If 
 * identifiable sections of that work are not derived from the Font, and 
 * can be reasonably considered independent and separate works in 
 * themselves, then this License and its terms, do not apply to those 
 * sections when you distribute them as separate works. Therefore, mere 
 * aggregation of another work not based on the Font with the Font on a 
 * volume of a storage or distribution medium does not bring the other work 
 * under the scope of this License. 
 *  
 * 3. Condition Subsequent 
 *    You may not copy, modify, sublicense, or distribute the Font except 
 * as expressly provided under this License. Any attempt otherwise to copy, 
 * modify, sublicense or distribute the Font will automatically 
 * retroactively void your rights under this License. However, parties who 
 * have received copies or rights from you under this License will keep 
 * their licenses valid so long as such parties remain in full compliance. 
 *  
 * 4. Acceptance 
 *    You are not required to accept this License, since you have not 
 * signed it. However, nothing else grants you permission to copy, modify, 
 * sublicense or distribute the Font. These actions are prohibited by law 
 * if you do not accept this License. Therefore, by copying, modifying, 
 * sublicensing or distributing the Font, you indicate your acceptance of 
 * this License and all its terms and conditions. 
 *  
 * 5. Automatic Receipt 
 *    Each time you redistribute the Font, the recipient automatically 
 * receives a license from the original licensor to copy, distribute or 
 * modify the Font subject to these terms and conditions. You may not 
 * impose any further restrictions on the recipients' exercise of the 
 * rights granted herein. You are not responsible for enforcing compliance 
 * by third parties to this License. 
 *  
 * 6. Contradiction 
 *    If, as a consequence of a court judgment or allegation of patent 
 * infringement or for any other reason (not limited to patent issues), 
 * conditions are imposed on you (whether by court order, agreement or 
 * otherwise) that contradict the conditions of this License, they do not 
 * excuse you from the conditions of this License. If you cannot distribute 
 * so as to satisfy simultaneously your obligations under this License and 
 * any other pertinent obligations, then as a consequence you may not 
 * distribute the Font at all. For example, if a patent license would not 
 * permit royalty-free redistribution of the Font by all those who receive 
 * copies directly or indirectly through you, then the only way you could 
 * satisfy both it and this License would be to refrain entirely from 
 * distribution of the Font. 
 *  
 *    If any portion of this section is held invalid or unenforceable under 
 * any particular circumstance, the balance of the section is intended to 
 * apply and the section as a whole is intended to apply in other 
 * circumstances. 
 *  
 * 7. NO WARRANTY 
 *    BECAUSE THE FONT IS LICENSED FREE OF CHARGE, THERE IS NO WARRANTY FOR 
 * THE FONT, TO THE EXTENT PERMITTED BY APPLICABLE LAW. EXCEPT WHEN 
 * OTHERWISE STATED IN WRITING THE COPYRIGHT HOLDERS OR OTHER PARTIES 
 * PROVIDE THE FONT "AS IS" WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESSED 
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF 
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. THE ENTIRE RISK AS 
 * TO THE QUALITY AND PERFORMANCE OF THE FONT IS WITH YOU. SHOULD THE FONT 
 * PROVE DEFECTIVE, YOU ASSUME THE COST OF ALL NECESSARY SERVICING, REPAIR 
 * OR CORRECTION. 
 *  
 * 8. DAMAGES WAIVER 
 *    UNLESS REQUIRED BY APPLICABLE LAW OR AGREED TO IN WRITING, IN NO 
 * EVENT WILL ANY COPYRIGHTT HOLDERS, OR OTHER PARTIES WHO MAY COPY, MODIFY 
 * OR REDISTRIBUTE THE FONT AS PERMITTED ABOVE, BE LIABLE TO YOU FOR ANY 
 * DIRECT, INDIRECT, CONSEQUENTIAL, INCIDENTAL, SPECIAL OR EXEMPLARY 
 * DAMAGES ARISING OUT OF THE USE OR INABILITY TO USE THE FONT (INCLUDING 
 * BUT NOT LIMITED TO PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF 
 * USE, DATA OR PROFITS; OR BUSINESS INTERRUPTION), EVEN IF SUCH HOLDERS OR 
 * OTHER PARTIES HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES. 
 *  
 * ====================================================================== 
 ***********************************/
#if defined(GFX_CONFIG_FONT_PROG_SPACE_ENABLE) && (__XC16_VERSION__ >= 1011)
#ifndef GFX_CONFIG_FONT_PACKED_ENABLE
extern GFX_FONT_SPACE char __JapaneseFont[] __attribute__((space(prog), section("grc_text"), aligned(2)));
#else
extern GFX_FONT_SPACE char __JapaneseFont[] __attribute__((space(prog), section("grc_text")));
#endif
#else
extern GFX_FONT_SPACE char __JapaneseFont[] __attribute__((aligned(2)));
#endif

const GFX_RESOURCE_HDR JapaneseFont = 
{
    .type = (GFX_RESOURCE_MEMORY_FLASH | GFX_RESOURCE_TYPE_FONT | GFX_RESOURCE_COMP_NONE),
    .ID = 0x0000,
    .resource.font.location.progByteAddress = (GFX_FONT_SPACE char *) __JapaneseFont,
    .resource.font.header.fontID = 0,
    .resource.font.header.extendedGlyphEntry = 0,
    .resource.font.header.res1 = 0,
    .resource.font.header.bpp = 1,
    .resource.font.header.orientation = 0,
    .resource.font.header.res2 = 0,
    .resource.font.header.firstChar = 0x0020,
    .resource.font.header.lastChar = 0x0028,
    .resource.font.header.height = 0x0019,
};
/*********************************
 * TTF Font File Structure
 * Label: HindiFont
 * Description:  Height: 31 pixels, 2 bits per pixel, Range: ' ' to '≈'
 ***********************************/
#if defined(GFX_CONFIG_FONT_PROG_SPACE_ENABLE) && (__XC16_VERSION__ >= 1011)
#ifndef GFX_CONFIG_FONT_PACKED_ENABLE
extern GFX_FONT_SPACE char __HindiFont[] __attribute__((space(prog), section("grc_text"), aligned(2)));
#else
extern GFX_FONT_SPACE char __HindiFont[] __attribute__((space(prog), section("grc_text")));
#endif
#else
extern GFX_FONT_SPACE char __HindiFont[] __attribute__((aligned(2)));
#endif

const GFX_RESOURCE_HDR HindiFont = 
{
    .type = (GFX_RESOURCE_MEMORY_FLASH | GFX_RESOURCE_TYPE_FONT | GFX_RESOURCE_COMP_NONE),
    .ID = 0x0000,
    .resource.font.location.progByteAddress = (GFX_FONT_SPACE char *) __HindiFont,
    .resource.font.header.fontID = 0,
    .resource.font.header.extendedGlyphEntry = 1,
    .resource.font.header.res1 = 0,
    .resource.font.header.bpp = 1,
    .resource.font.header.orientation = 0,
    .resource.font.header.res2 = 0,
    .resource.font.header.firstChar = 0x0020,
    .resource.font.header.lastChar = 0x0029,
    .resource.font.header.height = 0x001F,
};
/*********************************
 * TTF Font File Structure
 * Label: ThaiFont
 * Description:  Height: 29 pixels, 2 bits per pixel, Range: '!' to 'â'
 ***********************************/
#if defined(GFX_CONFIG_FONT_PROG_SPACE_ENABLE) && (__XC16_VERSION__ >= 1011)
#ifndef GFX_CONFIG_FONT_PACKED_ENABLE
extern GFX_FONT_SPACE char __ThaiFont[] __attribute__((space(prog), section("grc_text"), aligned(2)));
#else
extern GFX_FONT_SPACE char __ThaiFont[] __attribute__((space(prog), section("grc_text")));
#endif
#else
extern GFX_FONT_SPACE char __ThaiFont[] __attribute__((aligned(2)));
#endif

const GFX_RESOURCE_HDR ThaiFont = 
{
    .type = (GFX_RESOURCE_MEMORY_FLASH | GFX_RESOURCE_TYPE_FONT | GFX_RESOURCE_COMP_NONE),
    .ID = 0x0000,
    .resource.font.location.progByteAddress = (GFX_FONT_SPACE char *) __ThaiFont,
    .resource.font.header.fontID = 0,
    .resource.font.header.extendedGlyphEntry = 1,
    .resource.font.header.res1 = 0,
    .resource.font.header.bpp = 1,
    .resource.font.header.orientation = 0,
    .resource.font.header.res2 = 0,
    .resource.font.header.firstChar = 0x0020,
    .resource.font.header.lastChar = 0x002A,
    .resource.font.header.height = 0x001D,
};