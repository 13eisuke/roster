#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#define maxline_length 1024

struct data{
  int year;
  int month;
  int day;
};

struct profile{
  int id;
  char school_name[70];
  struct data p1;
  char address[70];
  char *remarks;
};

struct profile profile_data_store[10000];
int profile_data_nitems = 0;
int error_line = 0;
int p_order[10000];
char fn_store1[1024];
char fn_store2[1024];

int subst(char *str, char c1, char c2);
int get_line(FILE *fp, char *line);
int split(char *str, char *ret[], char sep, int max);
void parse_line(char *line);
void exec_command(char *cmd, char *param);
void new_profile(struct profile *p, char *line);
void cmd_quit();
void cmd_check();
void cmd_print(int nitems);
void cmd_read(char *filename);
void cmd_write(char *filename);
void cmd_find(char *word);
void cmd_delite(int n);
void csv_write(FILE *fp, struct profile *w);
void print_profile(struct profile p);
char *date_to_string(char buf[], struct data *date);
void cmd_sort(int n);
void swap(int *a, int *b);
int compare_profile(struct profile *p1, struct profile *p2, int col);
int compare_date(struct data *d1, struct data *d2);
void bubblesort(int n);
int partition(int l, int r, int col);
void quicksort(int l, int r, int col);
void cmd_bread(char *filename);
void cmd_bwrite(char *filename);
void binary_read(FILE *fp, struct profile *w);
void binary_write(FILE *fp, struct profile *w);

int main()
{
  char line[maxline_length+1];
  while(get_line(stdin,line)){
    parse_line(line);
  }
  return 0;
}

int subst(char *str, char c1, char c2)
{
  int i=0;
  for(;*str != '\0'; str++ ){
    if(*str == c1){
      *str = c2;
      i++;
    }
  }
  return i;
}

int get_line(FILE *fp, char *line)
{
  int i=0, n=0;
  if (fgets(line, maxline_length+1, fp) == NULL){
    return 0;
  }
  else{
    for(i=0; i < maxline_length; i++){
      if(*(line + i) == '\n') n++;
    }
    if(n==0) while(fgetc(fp) != '\n');
  }
  subst(line,'\n','\0');
  return 1;
}

int split(char *str, char *ret[], char sep, int max)
{
  int n=0;
  ret[n] = str;
  while(*str != '\0' && n<max){
    if(*str == sep) {
      *str = '\0';
      n++;
      ret[n] = str + 1;
    }
    str++;
  }
  return n;
}

 void parse_line(char *line)
{
  error_line++;
  int a;
  char *ret[2];
  if(*line == '%'){
    a = split(line + 1, ret, ' ', 2);
    if(a == 0){
      *ret[1] = '\0';
    }
    exec_command(ret[0], ret[1]);
  }
  else{
    if(profile_data_nitems == 10000){
      printf("You can't add data any more.\n");
      return;
    }
    p_order[profile_data_nitems] = profile_data_nitems;
    new_profile(&profile_data_store[profile_data_nitems++],line);
  }
}

void exec_command(char *cmd, char *param)
{
  int i;
  char list[10][3] = {{"Q"}, {"C"}, {"P"}, {"R"},
              {"W"}, {"F"}, {"S"}, {"D"}, {"BR"}, {"BW"}};
 
  for(i = 0; i < 10; i++){
    if(strcmp(cmd, list[i]) == 0)break;
  }
    switch(i){
    case 0: cmd_quit(); break;
    case 1: cmd_check(); break;
    case 2: cmd_print(atoi(param)); break;
    case 3: cmd_read(param); break;
    case 4: cmd_write(param); break;
    case 5: cmd_find(param); break;
    case 6: cmd_sort(atoi(param)); break;
    case 7: cmd_delite(atoi(param)); break;
    case 8: cmd_bread(param); break;
    case 9: cmd_bwrite(param); break;
    default:
      fprintf(stderr, "Invalid command : %s.\n", cmd);
      break;
  }
}

void new_profile(struct profile *p,char *line){
  int a,b,c,d;
  char *ret[5], *bret[3];
  a =  split(line, ret, ',', 5);
  if(a<=3){
    printf("Error line : %d\n", error_line);
    profile_data_nitems--;
    return;
  }
  b = split(ret[2], bret, '-', 3);
  if(b<=1){
    printf("Error line : %d\n",error_line);
    profile_data_nitems--;
    return;
  }
  c = strlen(ret[1]);
  if(c > 70){
    printf("Error line : %d\n",error_line);
    printf("Write school name in 70 characters or less.\n");
    profile_data_nitems--;
    return;
  }
  d = strlen(ret[3]);
  if(d > 70){
    printf("Error line : %d\n",error_line);
    printf("Write address in 70 characters or less.\n");
    profile_data_nitems--;
    return;
  }

  p->p1.year = atoi(bret[0]);
  p->p1.month = atoi(bret[1]);
  p->p1.day = atoi(bret[2]);
  p->remarks = (char *)malloc(sizeof(char) *(strlen(ret[4])+1));

  p->id = atoi(ret[0]);
  strncpy(p->school_name,ret[1],70);
  strncpy(p->address,ret[3],70);
  strcpy(p->remarks,ret[4]);
}

void cmd_quit()
{
  exit(0);
}

void cmd_check()
{
  printf("%d profile(s)\n", profile_data_nitems);
}

void cmd_print(int nitems)
{
  int i;
  if(nitems > 0){
    for(i=0; i < nitems && i < profile_data_nitems; i++){
      print_profile(profile_data_store[p_order[i]]);
    }
  }
  if(nitems < 0){
    if(profile_data_nitems == 0)return;
    for(i = profile_data_nitems + nitems; i < profile_data_nitems; i++){
      if(i < 0) i = 0;
      print_profile(profile_data_store[p_order[i]]);
    }
  }
  if(nitems == 0){
    for(i = 0; i < profile_data_nitems; i++){
      print_profile(profile_data_store[p_order[i]]);
    }
  }
}

void cmd_read(char *filename)
{
  char line[maxline_length + 1];
  FILE *fp;
  fp = fopen(filename,"r");
  if(fp == NULL){
    fprintf(stderr, "Could not open file : %s\n", filename);
    return;
  }
  while(get_line(fp, line)){
    parse_line(line);
  }
  fclose(fp);
}

void cmd_write(char *filename)
{
  int i;
  FILE *fp;
  if(*fn_store1 == '\0'  && *filename == '\0'){
    fprintf(stderr, "Type filename\n");
    return;
  }

  if(*filename != '\0'){
    strncpy(fn_store1, filename, 1024);
  }
 
  fp = fopen(fn_store1, "w");
  if(fp == NULL){
    fprintf(stderr, "Could not open file : %s\n", fn_store1);
    return;
  }
  for(i = 0; i < profile_data_nitems; i++){
    csv_write(fp,&profile_data_store[p_order[i]]);
  }
  fclose(fp);
}

void csv_write(FILE *fp, struct profile *w)
{
  fprintf(fp, "%d,%s,%04d-%02d-%02d,%s,%s\n",
      w->id, w->school_name,w->p1.year,w->p1.month,w->p1.day,
      w->address,w->remarks);
}
void print_profile(struct profile p)
{
      printf("Id    : %d\n", p.id);
      printf("Name  : %s\n", p.school_name);
      printf("Birth : %d-%02d-%02d\n",p.p1.year,p.p1.month,p.p1.day);
      printf("Addr  : %s\n", p.address);
      printf("Com.  : %s\n", p.remarks);
      printf("\n");
}
void cmd_find(char *word)
{
  int i;
  char id_str[15],birth_str[11];
  struct profile *p;

  for (i = 0; i < profile_data_nitems; i++) {
    p = &profile_data_store[i];
    sprintf(id_str, "%d",p->id);
    date_to_string(birth_str, &p->p1);
    if (
    strcmp(id_str,word) == 0 ||
    strcmp(p->school_name, word) == 0 ||
    strcmp(birth_str,word) == 0 ||
    strcmp(p->address, word) == 0 ||
    strcmp(p->remarks, word) == 0
    ) {
      print_profile(*p);
    }
  }
}
char *date_to_string(char buf[], struct data *date)
{
  sprintf(buf, "%04d-%02d-%02d", date->year, date->month, date->day);
  return buf;
}

void cmd_sort(int col)
{
    if(abs(col) > 5 || col == 0){
    printf("Column(%d) is invalid\n", col);
    return;
  }
    quicksort(0, profile_data_nitems - 1, col);
}

void swap(int *a, int *b)
{
  int tmp;
 
  tmp = *a;
  *a = *b;
  *b = tmp;
}

void bubblesort(int col)
{
  int i,j,k;
 
  for(i = 1; i < profile_data_nitems; i++){
    for(j= i; j > 0; j--){
      k = compare_profile(&profile_data_store[p_order[j]],
              &profile_data_store[p_order[j-1]],col);
      if(k < 0) swap(&p_order[j],&p_order[j-1]);
    }
  }
}

int compare_profile(struct profile *px, struct profile *py, int col)
{
  int x = col/abs(col);
  switch (abs(col)){
  case 1:
    return x*(px->id - py->id);
  case 2:
    return x*(strcmp(px->school_name, py->school_name));
  case 3:
    return x*(compare_date(&px->p1,&py->p1));
  case 4:
    return x*(strcmp(px->address,py->address));
  case 5:
    return x*(strcmp(px->remarks,py->remarks));
  }
}

int compare_date(struct data *d1, struct data *d2)
{
  if(d1->year != d2->year) return d1->year - d2->year;
  if(d1->month != d2->month) return d1->month - d2->month;
  return d1->day - d2->day;
}

void quicksort(int l, int r, int col)
{
  int n;
  if(l > r) return;
  n = partition(l, r, col);
  quicksort(l, n - 1, col);
  quicksort(n + 1, r, col);
}

int partition(int l, int r, int col)
{
  int i = l - 1, j = r;
  while(i < j){

  while(compare_profile(&profile_data_store[p_order[++i]],
             &profile_data_store[p_order[r]],col) < 0);
 
  while(compare_profile(&profile_data_store[p_order[r]],
             &profile_data_store[p_order[--j]],col) < 0)
  if(i >= j) break;
  if (i < j) swap(&p_order[i],&p_order[j]);
  }
  swap(&p_order[i],&p_order[r]);
  return i;
}

void cmd_delite(int n)
{
  if(n <= 0 || profile_data_nitems < n){
    printf("There is no data.\n");
    return;
  }
  int i, k = p_order[--n];
  for(i = k; i < profile_data_nitems - 1; i++){
    profile_data_store[i] = profile_data_store[i + 1];
  }
  for(i = 0; i < profile_data_nitems - 1; i++){
    if(n <= i) p_order[i] = p_order[i + 1];
    if(p_order[i] > k) p_order[i]--;
  }
  profile_data_nitems--;
}

void cmd_bread(char *filename)
{
  FILE *fp;
  int nitems, i;
  fp = fopen(filename, "rb");
  if(fp == NULL){
    fprintf(stderr, "Could not open file : %s\n", filename);
    return;
  }
  fread(&nitems, sizeof(nitems), 1, fp);
  for(i = 1; i <= nitems; i++){
    p_order[profile_data_nitems - 1 + i] = profile_data_nitems - 1 + i;
    binary_read(fp, &(profile_data_store[p_order[profile_data_nitems - 1 + i]]));
  if(profile_data_nitems + i == 10000){
      printf("You can't add data any more.\n");
      i++;
      break;
    } 
  }
  profile_data_nitems = profile_data_nitems + i - 1;
  fclose(fp);
}

void binary_read(FILE *fp, struct profile *w)
{
  int i;
  fread(&(w->id), sizeof(w->id), 1, fp);
  fread(&(w->school_name), sizeof(w->school_name), 1, fp);
  fread(&(w->p1), sizeof(w->p1), 1, fp);
  fread(&(w->address), sizeof(w->address), 1, fp);
  fread(&i, sizeof(i), 1, fp);
  w->remarks = (char *)malloc(i);
  fread(w->remarks, i, 1, fp);
}

void cmd_bwrite(char *filename)
{
  FILE *fp;
  int i;
  if(*fn_store2 == '\0'  && *filename == '\0'){
    fprintf(stderr, "Type filename\n");
    return;
  }

  if(*filename != '\0'){
    strncpy(fn_store2, filename, 1024);
  }
 
  if(fp == NULL){
    fprintf(stderr, "Could not open file : %s\n", fn_store2);
    return;
  }
  fp = fopen(filename, "wb");
  fwrite(&profile_data_nitems, sizeof(profile_data_nitems), 1, fp);
  for(i = 0; i < profile_data_nitems; i++){
    binary_write(fp, &(profile_data_store[p_order[i]]));
  }
  fclose(fp);
}

void binary_write(FILE *fp, struct profile *w)
{
  int i = strlen(w->remarks) + 1;
  fwrite(&(w->id), sizeof(w->id), 1, fp);
  fwrite(&(w->school_name), sizeof(w->school_name), 1, fp);
  fwrite(&(w->p1), sizeof(w->p1), 1, fp);
  fwrite(&(w->address), sizeof(w->address), 1, fp);
  fwrite(&i, sizeof(i), 1, fp);
  fwrite(w->remarks, i, 1, fp);
}
