// 本文件 137 行前系旧代码, 故与本项目整体编码风格不统一.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "game-def.h"
#include "math.h"

enum mJsonType {
  JFALSE, JTRUE, JNULL, JNUM, JSTR, JARRAY, JOBJ
};

int hasChild(mJson *c) {
  return JARRAY == c->type || JOBJ == c->type;
}

mJson *mJsonParse(const char *value) {
  mJson *c = NULL;
  char *raw1 = NULL, *raw2 = NULL;
  while(!isgraph(*value)) ++value;
  if('/' == *value)
    if('*' == value[1])
      while(*value && !('/' == *value || '*' == value[-1])) ++value;
    else if('/' == value[1])
      while(*value && '\n' != *value) ++value;
  while(!isgraph(*value)) ++value;

  if('\"' == *value) {
    char buf[255];
    int len = 0; 
    for(++value; *value && '\"' != *value; ++value) {
      if('\\' != *value)    buf[len++] = *value; else {
        ++value;
        if('\\' == *value)  buf[len++] = '\\';
        if('n' == *value)   buf[len++] = '\n';
        if('t' == *value)   buf[len++] = '\t';
        if('\"' == *value)  buf[len++] = '\"';
        else       --value, buf[len++] = '\\';
        if(256 & len) break;
      }
    } 
    ++value;
    raw1 = malloc(1 + len);
    memcpy(raw1, buf, len);
    raw1[len] = '\0';
  }
  while(!isgraph(*value)) ++value;

  if(':' == *value) { c = malloc(sizeof(mJson)); c->key = raw1; ++value; }
  while(*value && !isgraph(*value)) ++value;

  if(isdigit(*value) || '.' == *value || '-' == *value) {
    int getInt = 0, neg = ('-' == *value), point = ('.' == *value);
    double getReal = 0;
    if(neg) ++value;
    while(isdigit(*value) || '.' == *value) {
      if('.' == *value) point = 1, getReal = getInt;
      else if(point) getReal += (double)(*value - '0') / point++;
      else {
        getInt = 10 * getInt - '0' + *value;      }
      ++value;
    }
    if(neg) getInt = -getInt, getReal = -getReal;
    if(!getReal) getReal = getInt;
    if(NULL == c) { c = malloc(sizeof(mJson)); c->key = NULL; }
    c->type = JNUM; c->data.num.integer = getInt; c->data.num.real = getReal;
    goto here;
  }
  
  if('[' == *value) {
    if(NULL == c) { c = malloc(sizeof(mJson)); c->key = NULL; }
    c->type = JARRAY, c->data.child = mJsonParse(1 + value);
    goto here;
  }

  if('{' == *value) {
    if(NULL == c) { c = malloc(sizeof(mJson)); c->key = NULL; }
    c->type = JOBJ, c->data.child = mJsonParse(1 + value);
    goto here;
  }

  if('t' == *value || 'n' == *value || 'f' == *value) {
    if(NULL == c) { c = malloc(sizeof(mJson)); c->key = NULL; }
    c->type = 't' == *value? JTRUE: 'n' == *value? JNULL: JFALSE;
    while(isalpha(*value)) ++value;
    goto here;
  }

  if('\"' == *value) {
    char buf[255];
    int len = 0;
    for(++value; *value && '\"' != *value; ++value) {
      if('\\' != *value)    buf[len++] = *value; else {
        ++value;
        if('\\' == *value)  buf[len++] = '\\';
        if('n' == *value)   buf[len++] = '\n';
        if('t' == *value)   buf[len++] = '\t';
        if('\"' == *value)  buf[len++] = '\"';
        else       --value, buf[len++] = '\\';
        if(256 & len) break;
      }
    }
    ++value;
    if ((raw2 = malloc(1 + len))) { memcpy(raw2, buf, len); raw2[len] = '\0'; }
    if(NULL == c) { c = malloc(sizeof(mJson)); c->key = NULL; }
    c->type = JSTR, c->data.str = raw2;
  }

  if(NULL != raw1 && NULL == c) { 
    c = malloc(sizeof(mJson)); c->key = NULL, c->type = JSTR, c->data.str=raw1;
  }
  
  here: while(*value && !isgraph(*value)) ++value;
  if(NULL != c)
    if(',' == *value) {
      c->next = mJsonParse(1 + value);
      if(NULL != c->next) c->next->prev = c;
    }
    else c->next = NULL;

  return c;
}

mJson *mJsonGetObj(mJson *c, const char *key) {
  if(NULL == c || !hasChild(c)) return NULL;
  for(c = c->data.child; NULL != c; c = c->next)
    if(!strcmp(c->key, key)) return c;
  return NULL;
}

void mJsonDel(mJson *c) {
  if(NULL == c) return;
  if(hasChild(c)) mJsonDel(c->data.child);
  mJsonDel(c->next);
  free(c);
}

// 以下内容为新增.
#define max_json_length 4096

mJson *load_json(VOID)
{
    FILE *json_file;
    CHAR buffer[max_json_length] = "{\n"
        "\t\"description\": [\n"
        "\t\t\"Please modify Settings of this Game here.\",\n"
        "\t\t\"Sorry but it is too complicated to write a Setting Window.\",\n"
        "}\n", *cur = buffer;

    if (!fopen_s(&json_file, config_filename, "r")) {
        INT32 next_char;
        while (EOF != (next_char = fgetc(json_file)))
            *cur++ = next_char;
        *cur = '\0';
    }
    return mJsonParse(buffer);
}

BOOL mjson_write_unguarded(const mJson *json_obj, FILE *file_out, INT32 indent)
{
    switch (json_obj->type) {
    case JFALSE: fprintf_s(file_out, "false"); break;
    case JTRUE: fprintf_s(file_out, "true"); break;
    case JNUM:
        if (json_obj->data.num.integer - json_obj->data.num.real < 1e-3)
            fprintf_s(file_out, "%d", json_obj->data.num.integer);
        else fprintf_s(file_out, "%.2lf", json_obj->data.num.real);
        break;
    case JSTR:
    {
        CHAR *cur;
        putchar('"');
        for (cur = json_obj->data.str; *cur; ++cur)
            switch (*cur) {
            case '\\': fprintf_s(file_out, "\\\\"); break;
            case '\n': fprintf_s(file_out, "\\n"); break;
            case '\t': fprintf_s(file_out, "\\t"); break;
            case '\"': fprintf_s(file_out, "\\\""); break;
            default: fputc(*cur, file_out);
            }
        putchar('"');
        break;
    }
    case JARRAY:
    {
        fputs("[", file_out);
        for (mJson *child_item = json_obj->data.child; child_item;
            child_item = child_item->next && fprintf_s(file_out, ",\n")) {
            for (int i = 0; i <= indent; ++i) fputc('\t', file_out);
            mjson_write_unguarded(child_item, file_out, indent + 1);
        }
        fputs("", file_out);
        for (int i = 0; i < indent; ++i) fputc('\t', file_out);
        fputc(']', file_out);
        break;
    }
    case JOBJ:
    {
        fputs("{", file_out);
        for (mJson *child_item = json_obj->data.child; child_item;
            child_item = child_item->next && fprintf_s(file_out, ",\n")) {
            for (int i = 0; i < indent; ++i) fputc('\t', file_out);
            fprintf_s(file_out, "%s: ", child_item->key);
            mjson_write_unguarded(child_item, file_out, indent + 1);
        }
        fputs("", file_out);
        for (int i = 0; i < indent; ++i) fputc('\t', file_out);
        fputc('}', file_out);
        break;
    }
    case JNULL: default:
        fprintf_s(file_out, "null");
    }
    return TRUE;
}

BOOL mjson_write(mJson *json_obj)
{
    FILE *json_file;
    return fopen_s(&json_file, config_filename, "w") ? FALSE :
        mjson_write_unguarded(json_obj, json_file, 0);
}

struct int32_maybe mjson_get_int(const mJson *json_obj, const char *key)
{
    mJson *target = mJsonGetObj(json_obj, key);
    struct int32_maybe result = {
        .data = target ? target->data.num.integer : 0,
        .exce = NULL != target
    };
    return result;
}

double mjson_get_real(const mJson *json_obj, const char *key)
{
    mJson *target = mJsonGetObj(json_obj, key);
    return target ? target->data.num.real : INFINITY;
}

BOOL mjson_add_int(mJson *json_obj, const char *key, INT32 value)
{
    INT32 len = strlen(key) + 1;
    mJson *child_item = json_obj->data.child;
    while (json_obj->next) json_obj = json_obj->next;
    json_obj->next = (mJson *)malloc(sizeof(mJson));
    if (!json_obj->next) return FALSE;
    json_obj->next->next = NULL;
    json_obj->next->prev = json_obj;
    json_obj->next->type = JNUM;
    json_obj->next->data.num.integer = value;
    json_obj->next->data.num.real = value;
    json_obj->next->key = malloc(len);
    return !strcpy_s(json_obj->next->key, len, key);
}

BOOL mjson_add_real(mJson *json_obj, const char *key, double value)
{
    INT32 len = strlen(key) + 1;
    mJson *child_item = json_obj->data.child;
    while (json_obj->next) json_obj = json_obj->next;
    json_obj->next = (mJson *)malloc(sizeof(mJson));
    if (!json_obj->next) return FALSE;
    json_obj->next->next = NULL;
    json_obj->next->prev = json_obj;
    json_obj->next->type = JNUM;
    json_obj->next->data.num.integer = value;
    json_obj->next->data.num.real = value;
    json_obj->next->key = malloc(len);
    return !strcpy_s(json_obj->next->key, len, key);
}

INT32 mjson_get_int_or_default(mJson *json_obj, const char *key, INT32 value)
{
    struct int32_maybe result = mjson_get_int(json_obj, key);
    if (result.exce) return result.data;
    else {
        mjson_add_int(json_obj, key, value);
        return value;
    }
}

double mjson_get_real_or_default(mJson *json_obj, const char *key, double value)
{
    double result = mjson_get_real(json_obj, key);
    if (result != INFINITY) return result;
    else {
        mjson_add_int(json_obj, key, value);
        return value;
    }
}
