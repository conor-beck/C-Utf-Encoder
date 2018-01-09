#include "utf.h"
#include "debug.h"
#include "wrappers.h"
#include <unistd.h>

void
from_utf16be_to_utf16le(int infile, int outfile)
{
  int bom;
  utf16_glyph_t a;
  utf16_glyph_t* buf;
  ssize_t bytes_read;
  ssize_t bytes_read2;
  //size_t bytes_to_write;
  //int ret = 0;
  buf = malloc(sizeof a);
  bom = UTF16LE;
//#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
  //reverse_bytes(&bom, 2);
//#endif
  bytes_read = write_to_bigendian(outfile, &bom, 2);
  if(bytes_read < 2){
    printf("can't write BOM\n");
    exit(EXIT_FAILURE);
  }

  while ((bytes_read = read_to_bigendian(infile, &buf->upper_bytes, 2)) > 0) {
    if(bytes_read < 2){
      printf("Can't read 2 bytes improper formatting\n");
      exit(EXIT_FAILURE);
    }
    buf->upper_bytes = reverse_bytes(buf->upper_bytes);
    if((buf->upper_bytes >= 0xD800) && (buf->upper_bytes <= 0xDBFF)){
      bytes_read2 = read_to_bigendian(infile, &buf->lower_bytes, 2);
      if(bytes_read2 < 2){
        printf("Can't read rest of surrogate\n");
        exit(EXIT_FAILURE);
      }
      buf->lower_bytes = reverse_bytes(buf->lower_bytes);
      if((buf->lower_bytes < 0xDC00) || (buf->lower_bytes > 0xDFFF)){
        printf("Not a full surrogate\n");
        exit(EXIT_FAILURE);
      }
      bytes_read2 = write_to_bigendian(outfile, &buf->upper_bytes, 2);
      if(bytes_read2 < 2){
        printf("Can't write to file\n");
        exit(EXIT_FAILURE);
      }
      bytes_read2 = write_to_bigendian(outfile, &buf->lower_bytes, 2);
      if(bytes_read2 < 2){
        printf("Can't write to file\n");
        exit(EXIT_FAILURE);
      }
    }
    else{
      bytes_read2 = write_to_bigendian(outfile, &buf->upper_bytes, 2);
      if(bytes_read2 < 2){
        printf("Can't write to file\n");
        exit(EXIT_FAILURE);
      }
    }
  }
  free(buf);
  //ret = bytes_read;
  //return ret;
}



void
from_utf16be_to_utf8(int infile, int outfile)
{
  utf8_glyph_t ret;
  ssize_t bytes_read;
  code_point_t code_point;
  code_point_t temp;
  utf16_glyph_t* utf16_buf;
  ssize_t bytes_read2;
  utf16_glyph_t a;
  utf8_glyph_t* utf8_buf;
  utf8_glyph_t b;
  utf16_buf = malloc(sizeof a);
  utf8_buf = malloc(sizeof b);
  int bom = UTF8;
  int check = 0;
  check = write_to_bigendian(outfile, &bom, 3);
  if(check != 3){
    printf("can't write to file in utf16BE to utf8\n");
  }
  while((bytes_read = read_to_bigendian(infile, &utf16_buf->upper_bytes, 2)) > 0){
    if(bytes_read < 2){
      printf("not enought bytes\n");
      exit(EXIT_FAILURE);
    }
    utf16_buf->upper_bytes = reverse_bytes(utf16_buf->upper_bytes);
    if((utf16_buf->upper_bytes >= 0xD800) && (utf16_buf->upper_bytes <= 0xDBFF)){
      bytes_read2 = read_to_bigendian(infile, &utf16_buf->lower_bytes, 2);
      if(bytes_read2 < 2){
        exit(EXIT_FAILURE);
      }
      utf16_buf->lower_bytes = reverse_bytes(utf16_buf->lower_bytes);
      if((utf16_buf->lower_bytes < 0xDC00) || (utf16_buf->lower_bytes > 0xDFFF)){
        exit(EXIT_FAILURE);
      }
      code_point = beSurrToPoint(utf16_buf);
      if(code_point <= 0x7F){
        ret.bytes[0].byte = (uint8_t)code_point;
        bytes_read2 = write_to_bigendian(outfile, &ret.bytes[0], 1);
        if(bytes_read2 < 1){
          printf("Couldn't write to file\n");
          exit(EXIT_FAILURE);
        }
      }
      else if((code_point > 0x7F) && (code_point <= 0x7FF)){
        ret.bytes[0].byte = 192;
        temp = code_point >> 6;
        ret.bytes[0].byte = ret.bytes[0].byte | temp;
        ret.bytes[1].byte = 128;
        temp = code_point << 26;
        temp = temp >> 26;
        ret.bytes[1].byte = ret.bytes[1].byte | temp;
        bytes_read2 = write_to_bigendian(outfile, &ret.bytes[0], 1);
        if(bytes_read2 < 1){
          printf("Couldn't write to file\n");
          exit(EXIT_FAILURE);
        }
        bytes_read2 = write_to_bigendian(outfile, &ret.bytes[1], 1);
        if(bytes_read2 < 1){
          printf("Couldn't write to file\n");
          exit(EXIT_FAILURE);
        }
      }
      else if((code_point > 0x7FF) && (code_point <= 0xFFFF)){
        ret.bytes[0].byte = 224;
        temp = code_point >> 12;
        ret.bytes[0].byte = ret.bytes[0].byte | temp;
        ret.bytes[1].byte = 128;
        temp = code_point << 20;
        temp = temp >> 26;
        ret.bytes[1].byte = ret.bytes[1].byte | temp;
        ret.bytes[2].byte = 128;
        temp = code_point << 26;
        temp = temp >> 26;
        ret.bytes[2].byte = ret.bytes[2].byte | temp;
        bytes_read2 = write_to_bigendian(outfile, &ret.bytes[0], 1);
        if(bytes_read2 < 1){
          printf("Couldn't write to file\n");
          exit(EXIT_FAILURE);
        }
        bytes_read2 = write_to_bigendian(outfile, &ret.bytes[1], 1);
        if(bytes_read2 < 1){
          printf("Couldn't write to file\n");
          exit(EXIT_FAILURE);
        }
        bytes_read2 = write_to_bigendian(outfile, &ret.bytes[2], 1);
        if(bytes_read2 < 1){
          printf("Couldn't write to file\n");
          exit(EXIT_FAILURE);
        }
      }
      else if((code_point > 0xFFFF) && (code_point <= 0x1FFFFF)){
        ret.bytes[0].byte = 240;
        temp = code_point >> 18;
        ret.bytes[0].byte = ret.bytes[0].byte | temp;
        ret.bytes[1].byte = 128;
        temp = code_point << 14;
        temp = temp >> 26;
        ret.bytes[1].byte = ret.bytes[1].byte | temp;
        ret.bytes[2].byte = 128;
        temp = code_point << 20;
        temp = temp >> 26;
        ret.bytes[2].byte = ret.bytes[2].byte | temp;
        ret.bytes[3].byte = 128;
        temp = code_point << 26;
        temp = temp >> 26;
        ret.bytes[3].byte = ret.bytes[3].byte | temp;

        bytes_read2 = write_to_bigendian(outfile, &ret.bytes[0], 1);
        if(bytes_read2 < 1){
          printf("Couldn't write to file\n");
          exit(EXIT_FAILURE);
        }
        bytes_read2 = write_to_bigendian(outfile, &ret.bytes[1], 1);
        if(bytes_read2 < 1){
          printf("Couldn't write to file\n");
          exit(EXIT_FAILURE);
        }
        bytes_read2 = write_to_bigendian(outfile, &ret.bytes[2], 1);
        if(bytes_read2 < 1){
          printf("Couldn't write to file\n");
          exit(EXIT_FAILURE);
        }
        bytes_read2 = write_to_bigendian(outfile, &ret.bytes[3], 1);
        if(bytes_read2 < 1){
          printf("Couldn't write to file\n");
          exit(EXIT_FAILURE);
        }
      }
      else{
        printf("not valid code point %d\n", code_point);
        exit(EXIT_FAILURE);
      }

    }
    else{
      code_point = utf16_buf->upper_bytes;
      if(code_point <= 0x7F){
        ret.bytes[0].byte = (uint8_t)code_point;
        bytes_read2 = write_to_bigendian(outfile, &ret.bytes[0], 1);
        if(bytes_read2 < 1){
          printf("Couldn't write to file\n");
          exit(EXIT_FAILURE);
        }
      }
      else if((code_point > 0x7F) && (code_point <= 0x7FF)){
        ret.bytes[0].byte = 192;
        temp = code_point >> 6;
        ret.bytes[0].byte = ret.bytes[0].byte | temp;
        ret.bytes[1].byte = 128;
        temp = code_point << 26;
        temp = temp >> 26;
        ret.bytes[1].byte = ret.bytes[1].byte | temp;
        bytes_read2 = write_to_bigendian(outfile, &ret.bytes[0], 1);
        if(bytes_read2 < 1){
          printf("Couldn't write to file\n");
          exit(EXIT_FAILURE);
        }
        bytes_read2 = write_to_bigendian(outfile, &ret.bytes[1], 1);
        if(bytes_read2 < 1){
          printf("Couldn't write to file\n");
          exit(EXIT_FAILURE);
        }
      }
      else if((code_point > 0x7FF) && (code_point <= 0xFFFF)){
        ret.bytes[0].byte = 224;
        temp = code_point >> 12;
        ret.bytes[0].byte = ret.bytes[0].byte | temp;
        ret.bytes[1].byte = 128;
        temp = code_point << 20;
        temp = temp >> 26;
        ret.bytes[1].byte = ret.bytes[1].byte | temp;
        ret.bytes[2].byte = 128;
        temp = code_point << 26;
        temp = temp >> 26;
        ret.bytes[2].byte = ret.bytes[2].byte | temp;
        bytes_read2 = write_to_bigendian(outfile, &ret.bytes[0], 1);
        if(bytes_read2 < 1){
          printf("Couldn't write to file\n");
          exit(EXIT_FAILURE);
        }
        bytes_read2 = write_to_bigendian(outfile, &ret.bytes[1], 1);
        if(bytes_read2 < 1){
          printf("Couldn't write to file\n");
          exit(EXIT_FAILURE);
        }
        bytes_read2 = write_to_bigendian(outfile, &ret.bytes[2], 1);
        if(bytes_read2 < 1){
          printf("Couldn't write to file\n");
          exit(EXIT_FAILURE);
        }
      }
      else if((code_point > 0xFFFF) && (code_point <= 0x1FFFFF)){
        ret.bytes[0].byte = 240;
        temp = code_point >> 18;
        ret.bytes[0].byte = ret.bytes[0].byte | temp;
        ret.bytes[1].byte = 128;
        temp = code_point << 14;
        temp = temp >> 26;
        ret.bytes[1].byte = ret.bytes[1].byte | temp;
        ret.bytes[2].byte = 128;
        temp = code_point << 20;
        temp = temp >> 26;
        ret.bytes[2].byte = ret.bytes[2].byte | temp;
        ret.bytes[3].byte = 128;
        temp = code_point << 26;
        temp = temp >> 26;
        ret.bytes[3].byte = ret.bytes[3].byte | temp;

        bytes_read2 = write_to_bigendian(outfile, &ret.bytes[0], 1);
        if(bytes_read2 < 1){
          printf("Couldn't write to file\n");
          exit(EXIT_FAILURE);
        }
        bytes_read2 = write_to_bigendian(outfile, &ret.bytes[1], 1);
        if(bytes_read2 < 1){
          printf("Couldn't write to file\n");
          exit(EXIT_FAILURE);
        }
        bytes_read2 = write_to_bigendian(outfile, &ret.bytes[2], 1);
        if(bytes_read2 < 1){
          printf("Couldn't write to file\n");
          exit(EXIT_FAILURE);
        }
        bytes_read2 = write_to_bigendian(outfile, &ret.bytes[3], 1);
        if(bytes_read2 < 1){
          printf("Couldn't write to file\n");
          exit(EXIT_FAILURE);
        }
      }
      else{
        printf("not valid code point %d\n", code_point);
        exit(EXIT_FAILURE);
      }
    }
  }
  /* TODO */
  free(utf16_buf);
  free(utf8_buf);
  //return -1;
}

utf16_glyph_t*
code_point_to_utf16be_glyph(code_point_t code_point, utf16_glyph_t* ptr)
{
 // utf16_glyph_t ret;
  //utf16_glyph_t* ptr;
  ptr->lower_bytes = 0;
  ptr->upper_bytes = 0;
  //ptr = memeset(ptr, 0, sizeof ret, 0);
  if(code_point >= 0x10000){
    code_point -= 0x10000;
    ptr->upper_bytes = (code_point >> 10) + 0xD800;
    ptr->upper_bytes = reverse_bytes(ptr->upper_bytes);
    ptr->lower_bytes = (code_point & 0x3FF) + 0xDC00;
    ptr->lower_bytes = reverse_bytes(ptr->lower_bytes);
  //#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
    //reverse_bytes(&ret.upper_bytes, 2);
    //reverse_bytes(&ret.lower_bytes, 2);
  //#endif
    //*size_of_glyph = 4;
  }
  else {
    ptr->lower_bytes = 0;
    ptr->lower_bytes |= code_point;
    ptr->lower_bytes = reverse_bytes(ptr->lower_bytes);
  //#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
    //reverse_bytes(&ret.upper_bytes, 2);
  //#endif
    //*size_of_glyph = 2;
  }
  return ptr;
}

code_point_t beSurrToPoint(utf16_glyph_t* buf){
  code_point_t ret = 0;
  uint32_t uBytes = 0;
  uint32_t lBytes = 0;
  uBytes = buf->upper_bytes - 0xD800;
  uBytes = uBytes << 10;
  ret = ret | uBytes;
  lBytes = buf->lower_bytes - 0xDC00;
  ret = ret | lBytes;
  ret = ret + 0x10000;
  return ret;
}