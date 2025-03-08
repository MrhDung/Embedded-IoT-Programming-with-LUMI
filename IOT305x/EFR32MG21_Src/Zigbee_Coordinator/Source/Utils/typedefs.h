/*******************************************************************************
 *
 * Copyright (c) 2016
 * Lumi, JSC.
 * All Rights Reserved
 *
 *
 * Description: Include file for application
 *
 * Author: Thonv
 *
 * Last Changed By:  $Author: trungtq $
 * Revision:         $Revision: 1.0.0.1 $
 * Last Changed:     $Date: 2016-05-16 11:45:00 (Tue, 16 May 2016) $
 *
 ******************************************************************************/
#ifndef _TYPEDEFS_H_
#define _TYPEDEFS_H_
/******************************************************************************/
/*                              INCLUDE FILES                                 */
/******************************************************************************/

/******************************************************************************/
/*                     EXPORTED TYPES and DEFINITIONS                         */
/******************************************************************************/
typedef unsigned char           byte_t;       // 1 byte
typedef unsigned short          word_t;       // 2 byte
typedef unsigned long           dword_t;      // 4 byte
typedef unsigned int            uint_t;       // 4 byte
typedef signed int              int_t;
typedef unsigned char           bool_t;
typedef char                    char_t;       // 1 byte
typedef void                    void_t;

typedef byte_t*                 byte_p;
typedef word_t*                 word_p;
typedef dword_t*                dword_p;
typedef uint_t*                 uint_p;
typedef char_t*                 char_p;
typedef void_t*                 void_p;

typedef unsigned char           u8_t;       // 1 byte
typedef unsigned short          u16_t;      // 2 byte
typedef unsigned long           u32_t;      // 4 byte
typedef unsigned long long      u64_t;

typedef signed char             i8_t;       // 1 byte
typedef signed short            i16_t;      // 2 byte
typedef signed long             i32_t;      // 4 byte
typedef signed long long        i64_t;

typedef unsigned char*          u8_p;       // 1 byte
typedef unsigned short*         u16_p;      // 2 byte
typedef unsigned long*          u32_p;      // 4 byte
typedef unsigned long long*     u64_p;

typedef signed char*            i8_p;       // 1 byte
typedef signed short*           i16_p;      // 2 byte
typedef signed long*            i32_p;      // 4 byte
typedef signed long long*       i64_p;

#ifndef TRUE
#define FALSE                   0
#define TRUE                    1
#endif // TRUE

#ifndef NULL
#define NULL                    (0)
#endif // NULL

#ifndef MAX_U8
#define MAX_U8                  255
#define MAX_U16                 65535
#endif // MAX_U8

typedef void_t (*timer_callback)(void_p);
/******************************************************************************/
/*                              PRIVATE DATA                                  */
/******************************************************************************/

/******************************************************************************/
/*                              EXPORTED DATA                                 */
/******************************************************************************/

/******************************************************************************/
/*                            EXPORTED FUNCTIONS                              */
/******************************************************************************/


#endif /* _TYPEDEFS_H_ */
