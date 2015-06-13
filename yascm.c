#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "yascm.h"

int yyparse(struct object_s *env);

static object *Nil;
static object *Symbol_table;

static object *create_object(int type)
{
	object *obj = calloc(1, sizeof(*obj));
	if (obj == NULL) {
		fprintf(stderr, "calloc fail!\n");
		exit(1);
	}
	obj->type = type;
	return obj;
}

static object *cons(object *car, object *cdr)
{
	object *pair = create_object(PAIR);
	pair->car = car;
	pair->cdr = cdr;
	return pair;
}

/* return ((x . y) . a) */
static object *acons(object *x, object *y, object *a)
{
	return cons(cons(x, y), a);
}

static void add_variable(object *env, object *sym, object *val)
{
	env->vars = acons(sym, val, env->vars);
}

object *make_fixnum(int64_t val)
{
	object *obj = create_object(FIXNUM);
	obj->int_val = val;
	return obj;
}

object *_make_symbol(const char *name)
{
	object *obj = create_object(SYMBOL);
	obj->string_val = strdup(name);
	return obj;
}

object *make_symbol(const char *name)
{
	object *p;
	for (p = Symbol_table; p != Nil; p = p->cdr)
		if (strcmp(name, p->car->string_val) == 0)
			return p->car;
	object *sym = _make_symbol(name);
	Symbol_table = cons(sym, Symbol_table);
	return sym;
}

void object_print(const object *obj)
{
	/* TODO */
	if (obj->type == FIXNUM)
		printf("FIXNUM: %ld\n", obj->int_val);
}

static void add_primitive(object *env, char *name, Primitive *func)
{
	object *sym = make_symbol(name);
	object *prim = create_object(PRIM);
	prim->func = func;
	add_variable(env, sym, prim);
}

object *make_env(object *var, object *up)
{
	object *env = create_object(ENV);
	env->vars = var;
	env->up = up;
	return env;
}

int main(int argc, char **argv)
{
	object *env = make_env(Nil, NULL);
	Symbol_table = Nil;
	printf("welcome\n> ");
	yyparse(env);
	return 0;
}
