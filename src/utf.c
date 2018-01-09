#include "utf.h"
#include "debug.h"
#include "wrappers.h"
#include <sys/sendfile.h>
#include <unistd.h>

void
get_encoding_function(int infile, int outfile)
{
  encoding_from_to_t translate =
    (program_state->encoding_from - program_state->encoding_to);
  switch (translate) {
    case utf8_to_utf16le:
      from_utf8_to_utf16le(infile, outfile);
      break;
    case utf8_to_utf16be:
      from_utf8_to_utf16be(infile, outfile);
      break;
    case utf16le_to_utf16be:
      from_utf16le_to_utf16be(infile, outfile);
      break;
    case utf16be_to_utf16le:
      from_utf16be_to_utf16le(infile, outfile);
      break;
    case utf16be_to_utf8:
      from_utf16be_to_utf8(infile, outfile);
      break;
    case utf16le_to_utf8:
      from_utf16le_to_utf8(infile, outfile);
      break;
    case transcribe_file:
      transcribe(infile, outfile);
      break;
    default:
      break;
  }
}

void
check_bom()
{
  int fd;
  ssize_t bytes_read;

  uint32_t bom = 0;
  uint32_t test = 0;
  uint32_t temp = 0;
  uint32_t temp2 = 0;
  if (program_state->in_file == NULL) {
    error("%s\n", "In file not specified");
    exit(EXIT_FAILURE);
  }
  fd = Open(program_state->in_file, O_RDONLY);
  //bytes_read = read_to_bigendian(fd, &bom, 3);
  if ((bytes_read = read_to_bigendian(fd, &bom, 3)) < 3) {
    fprintf(stderr, "%s\n", "File contains invalid BOM or is empty beyond BOM");
    exit(EXIT_FAILURE);
  }
  //bom = 0;
  //read_to_bigendian(fd, &bom, 1);

  //exit(1);

  //temp2 = bom;
  //temp2 = temp2 << 16;

  //temp2 = temp2 >> 24;
  //temp2 = temp2 << 8;

  //temp = temp | (bom >> 16);

  //temp = temp | temp2;

  //temp2 = bom << 24;
  //temp2 = temp2 >> 8;
  //temp = temp | temp2;

  debug("BOM: %x", bom);
  if(bom == UTF8)
  {
    bytes_read = read_to_bigendian(fd, &test, 1);
    if(bytes_read < 1){
      printf("File empty after BOM\n");
      exit(EXIT_FAILURE);
    }
    info("Source BOM: %s", STR_UTF8);
    program_state->encoding_from = UTF8;
    program_state->bom_length = 3;
    close(fd);
    return;
  }
//#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
  //reverse_bytes(&bom, 2);
//#endif
  //debug("BOM AFTER SWAP: %x", bom);
  //bom = bom >> 8;

  bom = bom << 16;
  bom = bom >> 16;

  temp = bom >> 8;
  temp2 = bom << 24;
  temp2 = temp2 >> 16;
  temp = temp | temp2;
  bom = temp;


  if (bom == UTF16LE) {
    info("Source BOM: %s", STR_UTF16LE);
    program_state->encoding_from = UTF16LE;
    program_state->bom_length = 2;
  }
  elsif(bom == UTF16BE)
  {
    info("Source BOM: %s", STR_UTF16BE);
    program_state->encoding_from = UTF16BE;
    program_state->bom_length = 2;
  }
  else
  {
    fprintf(stderr, "%s\n", "Unrecognized BOM");
    exit(EXIT_FAILURE);
  }
  close(fd);
}

void
transcribe(int infile, int outfile)
{
  //program_state->encoding_from = UTF8;
    //program_state->bom_length = 3;
  int bom = program_state->encoding_from;
  ssize_t bytes_read;


  if(bom == UTF8){
    bytes_read = write_to_bigendian(outfile, &bom, 3);
    if(bytes_read != 3){
      exit(EXIT_FAILURE);
    }
  }
  else if(bom == UTF16BE){
    bom = UTF16LE;
    bytes_read = write_to_bigendian(outfile, &bom, 2);
    if(bytes_read != 2){
      exit(EXIT_FAILURE);
    }
  }
  else if(bom == UTF16LE){
    bom = UTF16BE;
    bytes_read = write_to_bigendian(outfile, &bom, 2);
    if(bytes_read != 2){
      exit(EXIT_FAILURE);
    }
  }
  //uint8_t buf = 0;
  //while(read_to_bigendian(infile, &buf, 1) > 0){
    //write_to_bigendian(outfile, &buf, 1);
    //buf = 0;
  //}
  //int ret = 0;

  struct stat infile_stat;
  fstat(infile, &infile_stat);

    //perror("Could not stat infile");
    //exit(EXIT_FAILURE);
  //}
  sendfile(outfile, infile, 0, infile_stat.st_size);

  //return ret;
}

bool
is_upper_surrogate_pair(utf16_glyph_t glyph)
{
  return ((glyph.upper_bytes > 0xD800) && (glyph.upper_bytes < 0xDBFF));
}

bool
is_lower_surrogate_pair(utf16_glyph_t glyph)
{
  return ((glyph.lower_bytes > 0xDC00) && (glyph.lower_bytes < 0xDFFF));
}

code_point_t
utf16_glyph_to_code_point(utf16_glyph_t *glyph)
{
  code_point_t ret = 0;
  if(!is_upper_surrogate_pair(*glyph)) {
    ret = glyph->upper_bytes;
  }
  else {
   // ret = (((glyph->upper_bytes - 0xD800) << 100) |
      //    ((glyph->lower_bytes - 0xDC00) & 0x3FF)) +
      //    0x10000;
  }
  return ret;
}

bool
is_code_point_surrogate(code_point_t code_point)
{
  return (code_point >= 10000);
}
