#include <stdio.h>
#include<string.h>
#include "bitwise.c"
int string_to_int(int *number,char* str){
   for (int i = 0; str[i] != '\0'; i++) {
        if(str[i]-48>9 || str[i]-48<0 ) return 0;
        *number = (int)*number * 10 + (str[i] - 48);
    }
    return 1;
}
int main(int argc, char *argv[]) {
  if(argc!=4) perror("Invalid number of arguments");
  else{
    int number=0;
    int index=0;
    
    if(!string_to_int(&number,argv[2])) perror("The second argument must be a integer");
    else if(!string_to_int(&index,argv[3])) perror("The third argument must be a integer");
    else if(index>7 || index<0) perror("Third argument must range between 0 and 7.");
    else if(strlen(argv[1])!=1)perror("First argument is invalid");
    else{
      int n=number;
      char *str;
    byte2bin(number,str);
    printf("\n %s\n",str);
      switch (argv[1][0])
      {
      case 'h':
      n=number;
      number|=(1<<index);
      if(n==number)printf("True");
      else printf("False");
        break;
      case '1':
        n=number;
        number|=(1<<index);
          if(n==number)printf("False");
          else printf("True");
          break;
      case 's':
        number|=1<<index;
        printf("%d",number);
        break;
      case 'r':
        number&=~(1<<index);
        printf("%d",number);
        break;
      case 't':
        number^=(1<<index);
        printf("%d",number);
        break;     
      
      default:
        perror("First argument is invalid");
        break;
      }
   
    byte2bin(number,str);
    printf("\n %s\n",str);
    }
  }
  return 0;
}