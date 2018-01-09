#include "debug.h"
#include "utf.h"
#include "wrappers.h"
#include <stdlib.h>
//#include <sys/stat.h>


int
main(int argc, char *argv[])
{
  struct stat st;
  struct stat s;
  //struct stat* ptr = &st;
  //struct stat* ptr2 = &s;
  //ptr = malloc(sizeof(st));
  //ptr2 = malloc(sizeof(s));
  int infile, outfile, in_flags, out_flags;
  parse_args(argc, argv);
  check_bom();
  print_state();
  in_flags = O_RDONLY;
  out_flags = O_WRONLY | O_CREAT; //| O_TRUNC;
  infile = Open(program_state->in_file, in_flags);
  outfile = Open(program_state->out_file, out_flags);
uint8_t buf;
while(read(infile, &buf, 1) > 0){
printf("%d\n", buf);
}
exit(1);
  fstat(infile, &st);
  fstat(outfile, &s);
  int i1 = st.st_ino;
  int i2 = s.st_ino;
  if(i1 == i2){
    exit(EXIT_FAILURE);
  }
  close(infile);
  close(outfile);
  out_flags = O_WRONLY | O_CREAT | O_TRUNC;
  infile = Open(program_state->in_file, in_flags);
  outfile = Open(program_state->out_file, out_flags);
  lseek(infile, program_state->bom_length, SEEK_SET); /* Discard BOM */
  get_encoding_function(infile, outfile);
  if(program_state != NULL) {
    close(infile);
    close(outfile);
  }
  //I think this is how this works
  //free((void*)program_state->in_file);
  //free((void*)program_state->out_file);
  free(program_state);
  return EXIT_SUCCESS;
}
