#include "debug.h"
#include "utf.h"
#include "wrappers.h"
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int opterr;
int optopt;
int optind;
char *optarg;

state_t *program_state;

void
parse_args(int argc, char *argv[])
{
  int hFlag = 0;
  int eFlag = 0;
  int eFlag2 = 0;
  if(argc < 2){
    USAGE(argv[0]);
    exit(EXIT_FAILURE);
  }

  //int i;
  char option;
  //char *joined_argv;

  //joined_argv = join_string_array(argc, argv);
  //info("argc: %d argv: %s", argc, joined_argv);
  //free(joined_argv);

  program_state = Calloc(1, sizeof(state_t));
  //for (i = 0; optind < argc; ++i) {
    //printf("yo\n");
    //if(i > 1 && argc > 5){
     // USAGE(argv[0]);
     // exit(EXIT_FAILURE);
    //}
    //if(i > 1 && argc < 6){
    //  USAGE(argv[0]);
     // exit(EXIT_FAILURE);
    //}
    debug("%d opterr: %d", i, opterr);
    debug("%d optind: %d", i, optind);
    debug("%d optopt: %d", i, optopt);
    debug("%d argv[optind]: %s", i, argv[optind]);

    while((option = getopt(argc, argv, "h")) != -1) {

      switch (option) {
        case 'e': {
          //eFlag = eFlag + 1;
          //info("Encoding Argument: %s", optarg);
          //if ((program_state->encoding_to = determine_format(optarg)) == 0){
            //eFlag2 = 1;

          //}
          break;
        }
        case 'h': {

          //hFlag = 1;
          //if (optopt != 'h'){
            //fprintf(stderr, KRED "-%c is not a supported argument\n" KNRM,
                    //optopt);
            //USAGE(argv[0]);
            //exit(EXIT_FAILURE);}
          //else{
            USAGE(argv[0]);
            exit(EXIT_SUCCESS);
          //}
        }
        default: {
          break;
        }
      }
    }
    hFlag = 0;
    eFlag = 0;
    eFlag2 = 0;
    optind = 0;
    while((option = getopt(argc, argv, "e:h")) != -1) {

      switch (option) {
        case 'e': {
          eFlag = eFlag + 1;
          //info("Encoding Argument: %s", optarg);
          if ((program_state->encoding_to = determine_format(optarg)) == 0){
            eFlag2 = 1;

          }
          break;
        }
        case 'h': {

          hFlag = 1;
          //if (optopt != 'h'){
            //fprintf(stderr, KRED "-%c is not a supported argument\n" KNRM,
                    //optopt);
            //USAGE(argv[0]);
            //exit(EXIT_FAILURE);}
          //else{
            //USAGE(argv[0]);
            //exit(EXIT_SUCCESS);
          //}
        }
        default: {
          break;
        }
      }
    }
    //elsif(argv[optind] != NULL)
    //{
      //if (program_state->in_file == NULL) {
      //  program_state->in_file = argv[optind];
      //}
      //elsif(program_state->out_file == NULL)
     // {
      //  program_state->out_file = argv[optind];
     // }
     // optind++;
    //}
  //}
  if(hFlag == 1){
    USAGE(argv[0]);
    exit(EXIT_SUCCESS);
  }
  if(eFlag2 == 1){
    USAGE(argv[0]);

    exit(EXIT_FAILURE);
  }
  if(eFlag != 1){
    printf("%d\n", eFlag);
    printf("aaa\n");
    USAGE(argv[0]);
    exit(EXIT_FAILURE);
  }
  if(argc > 5){
    USAGE(argv[0]);
    exit(EXIT_FAILURE);
  }
  if(argc < 5){
    USAGE(argv[0]);
    exit(EXIT_FAILURE);
  }
  //printf("here\n");
  program_state->in_file = argv[3];
  program_state->out_file = argv[4];
  //free(joined_argv);

}

format_t
determine_format(char *argument)
{
  if (strcmp(argument, STR_UTF16LE) == 0)
    return UTF16LE;
  if (strcmp(argument, STR_UTF16BE) == 0)
    return UTF16BE;
  if (strcmp(argument, STR_UTF8) == 0)
    return UTF8;
  return 0;
}

char*
bom_to_string(format_t bom){
  switch(bom){
    case UTF8: return STR_UTF8;
    case UTF16BE: return STR_UTF16BE;
    case UTF16LE: return STR_UTF16LE;
  }
  return "UNKNOWN";
}

char*
join_string_array(int count, char *array[])
{
  char *ret = (char*)malloc(1);
  //char charArray[count];
  int i;
  //int len = 0, str_len, cur_str_len;

  //str_len = array_size(count, array);
  //ret = &charArray;

  for (i = 0; i < count; ++i) {
    memecpy(ret, array[i], strlen(array[i]) + 1, strlen(ret));
    memecpy(ret, " ", 2, strlen(ret));
    //cur_str_len = strlen(array[i]);
    //memecpy(ret + len, array[i], cur_str_len);
    //len += cur_str_len;
    //memecpy(ret + len, " ", 1);
    //len += 1;
  }

  return ret;
}

int
array_size(int count, char *array[])
{
  int i, sum = 1; /* NULL terminator */
  for (i = 0; i < count; ++i) {
    sum += strlen(array[i]);
    ++sum; /* For the spaces */
  }
  return sum+1;
}

void
print_state()
{

  if (program_state == NULL) {
    error("program_state is %p", (void*)program_state);
    exit(EXIT_FAILURE);
  }
  info("program_state {\n"
         "  format_t encoding_to = 0x%X;\n"
         "  format_t encoding_from = 0x%X;\n"
         "  char *in_file = '%s';\n"
         "  char *out_file = '%s';\n"
         "};\n",
         program_state->encoding_to, program_state->encoding_from,
         program_state->in_file, program_state->out_file);
}
